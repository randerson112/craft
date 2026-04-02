#include "add.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "config.h"
#include "cmake.h"
#include "deps.h"
#include "registry.h"
#include "platform.h"

// Detects the type of dependency based on present options
static dep_type_t get_dependency_type(const command_t* command_data) {
    int has_path = 0;
    int has_git = 0;

    // Detect which options are present
    if (get_option(command_data, "path") != NULL) has_path = 1;
    if (get_option(command_data, "git") != NULL) has_git = 1;

    // Make sure only one of these options is present
    if (has_path && has_git) {
        fprintf(stderr, "Error: Specify exactly one of --path or --git\n");
        return DEP_INVALID;
    }

    // Return the dependency type
    if (has_path) return DEP_PATH;
    else if (has_git) return DEP_GIT;
    else return DEP_INVALID;
}

// Gets the dependency name based on type
// For path deps: reads the project name from the dependency's craft.toml
// For git deps: extracts the repo name from the URL
static int get_dependency_name(char* buffer, size_t buffer_size, const char* project_root, const char* value, const dep_type_t type) {
    if (type == DEP_PATH) {

        // Get path to dependency project root and load config
        char dep_project_root[512];
        snprintf(dep_project_root, sizeof(dep_project_root), "%s/%s", project_root, value);

        project_config_t dep_config;
        if (load_project_config(&dep_config, dep_project_root) != 0) {
            fprintf(stderr, "Make sure the craft.toml in the dependency is valid\n");
            return -1;
        }

        // Copy project name to buffer
        snprintf(buffer, buffer_size, "%s", dep_config.name);
    }
    else if (type == DEP_GIT) {

        // Get the last part of the path and remove the .git
        const char* last_slash = strrchr(value, '/');
        if (!last_slash || *(last_slash + 1) == '\0') {
            fprintf(stderr, "Error: could not parse repository name from URL '%s'\n", value);
            return -1;
        }

        snprintf(buffer, buffer_size, "%s", last_slash + 1);
        char* dot_git = strstr(buffer, ".git");
        if (dot_git)
            *dot_git = '\0';
    }
    else {
        return -1;
    }

    return 0;
}

// Validates a path dependency
// Checks that the path exists, has a valid craft.toml, and is not an executable type
static int validate_path_dependency(const char* project_root, const char* path) {
    
    // Get path to dependency project root
    char dep_project_root[512];
    snprintf(dep_project_root, sizeof(dep_project_root), "%s/%s", project_root, path);
    if (!dir_exists(dep_project_root)) {
        fprintf(stderr, "Error: path '%s' does not exist or is not a directory\n", path);
        return -1;
    }

    // Make sure it is a Craft project
    if (!is_craft_project(dep_project_root)) {
        fprintf(stderr, "Error: '%s' is not a Craft project, no craft.toml found\n", path);
        return -1;
    }

    // Load the config of the dependency
    project_config_t dep_config;
    if (load_project_config(&dep_config, dep_project_root) != 0 ||
        validate_project_config(&dep_config) != 0) {
        fprintf(stderr, "Make sure the craft.toml in the dependency is valid\n");
        return -1;
    }

    // Make sure it's a library not an executable
    if (strcmp(dep_config.build_type, "executable") == 0) {
        fprintf(stderr, "Error: '%s' is an executable project and cannot be linked as a dependency\n", dep_config.name);
        fprintf(stderr, "       Only static-library, shared-library, and header-only projects can be dependencies\n");
        return -1;
    }

    return 0;
}

// Checks if a dependency with the given name already exists
static int dependency_already_exists(project_config_t* config, const char* name) {
    for (int i = 0; i < config->dependencies_count; i++) {
        if (strcmp(config->dependencies[i].name, name) == 0)
            return 1;
    }
    return 0;
}

// Splits the command seperated links into the links field of a dependency
static void get_links(dependency_t* dep, const char* links) {
    dep->links_count = 0;

    const char* start = links;
    while (*start && dep->links_count < 8) {
        // Find the next comma
        const char* comma = strchr(start, ',');
        size_t len = comma ? (size_t)(comma - start) : strlen(start);

        // Copy the token into dep->links
        snprintf(dep->links[dep->links_count], sizeof(dep->links[0]), "%.*s", (int)len, start);
        dep->links_count++;

        // Move start past the comma, or break if at the end
        if (!comma) break;
        start = comma + 1;
    }
}

// Handles adding a kit from the registry
static int add_registry_dependency(const char* project_root, const command_t* command_data) {

    // Check for invalid options
    for (int i = 0; i < command_data->option_count; i++) {
        const char* option_name = command_data->options[i].name;
        if (strcmp(option_name, "tag") != 0 && strcmp(option_name, "branch") != 0) {
            fprintf(stderr, "Error: option '--%s' cannot be used when adding kits from registry\n", option_name);
            return -1;
        }
    }

    const char* kit_name = command_data->args[0];

    // Look up the kit in the registry
    registry_kit_t kit;
    memset(&kit, 0, sizeof(kit));
    if (registry_find(kit_name, &kit) != 0) {
        return -1;
    }

    // Build the dependency entry from registry info
    dependency_t dep;
    memset(&dep, 0, sizeof(dep));
    snprintf(dep.name, sizeof(dep.name), "%s", kit.name);
    dep.type = DEP_GIT;
    snprintf(dep.value, sizeof(dep.value), "%s", kit.git_url);
    dep.links_count = kit.links_count;
    for (int i = 0; i < kit.links_count; i++) {
        snprintf(dep.links[i], sizeof(dep.links[0]), "%s", kit.links[i]);
    }

    // Allow --tag or --branch to override the default
    const option_t* tag_option = get_option(command_data, "tag");
    const option_t* branch_option = get_option(command_data, "branch");
    if (tag_option && branch_option) {
        fprintf(stderr, "Error: --tag and --branch cannot be used together\n");
        return -1;
    }
    if (tag_option) {
        snprintf(dep.tag, sizeof(dep.tag), "%s", tag_option->arg);
    }
    if (branch_option) {
        snprintf(dep.branch, sizeof(dep.branch), "%s", branch_option->arg);
    }

    // Load config and check for duplicates
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }
    if (validate_project_config(&config) != 0) {
        return -1;
    }
    if (dependency_already_exists(&config, dep.name)) {
        fprintf(stderr, "Error: kit '%s' already exists in craft.toml\n", dep.name);
        return -1;
    }

    // Add dependency and regenerate files
    config.dependencies[config.dependencies_count++] = dep;

    if (generate_craft_toml(project_root, &config) != 0) return -1;
    if (fetch_git_dependency(project_root, &dep) != 0) return -1;
    if (generate_cmake(project_root, &config) != 0) return -1;

    fprintf(stdout, "Added kit '%s'\n\n", dep.name);
    fprintf(stdout, "Run 'craft build' to build with the new dependency\n");
    return 0;
}

int handle_add(const command_t* command_data) {

    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Get the root of the project
    char project_root[512];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    // If an argument was given, treat it as a registry kit name
    if (command_data->arg_count > 0) {
        return add_registry_dependency(project_root, command_data);
    }

    // Determine the type of dependency and get the main value from the option
    const dep_type_t type = get_dependency_type(command_data);

    const option_t* option = NULL;
    switch (type) {
        case DEP_PATH:
            option = get_option(command_data, "path");
            break;
        case DEP_GIT:
            option = get_option(command_data, "git");
            break;
        case DEP_INVALID:
            return -1;
    }

    if (!option) {
        fprintf(stderr, "Error: Failed to get option value\n");
        return -1;
    }

    const char* value = option->arg;

    // Throw error if incompatable options are present
    const option_t* tag_option = get_option(command_data, "tag");
    const option_t* branch_option = get_option(command_data, "branch");
    const option_t* links_option = get_option(command_data, "links");

    if ((tag_option || branch_option || links_option) && type != DEP_GIT) {
        fprintf(stderr, "Error: --tag, --branch, and --links can only be used with --git\n");
        return -1;
    }

    if (tag_option && branch_option) {
        fprintf(stderr, "Error: --tag and --branch cannot be used together\n");
        return -1;
    }

    // Validate that the dependency path is not an executable
    if (type == DEP_PATH) {
        if (validate_path_dependency(project_root, value) != 0) {
            return -1;
        }
    }

    char dep_name[64];
    if (get_dependency_name(dep_name, sizeof(dep_name), project_root, value, type) != 0) {
        return -1;
    }

    // Build the dependency entry
    dependency_t dep;
    memset(&dep, 0, sizeof(dep));
    snprintf(dep.name, sizeof(dep.name), "%s", dep_name);
    dep.type = type;
    snprintf(dep.value, sizeof(dep.value), "%s", value);
    if (tag_option) {
        snprintf(dep.tag, sizeof(dep.tag), "%s", tag_option->arg);
    }
    if (branch_option) {
        snprintf(dep.branch, sizeof(dep.branch), "%s", branch_option->arg);
    }
    if (links_option) {
        get_links(&dep, links_option->arg);
    }

    // Load current project config and add the dependency
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }
    if (validate_project_config(&config) != 0) {
        return -1;
    }
    if (dependency_already_exists(&config, dep_name)) {
        fprintf(stderr, "Error: Dependency '%s' already exists in craft.toml\n", dep_name);
        return -1;
    }

    config.dependencies[config.dependencies_count++] = dep;

    // Regenerate craft.toml with new dependency
    if (generate_craft_toml(project_root, &config) != 0) {
        return -1;
    }

    // Fetch git dependency into .craft/deps
    if (type == DEP_GIT) {
        if (fetch_git_dependency(project_root, &dep) != 0) {
            return -1;
        }
    }

    // Regenerate CMakeLists.txt with new dependency
    if (generate_cmake(project_root, &config) != 0) {
        return -1;
    }

    // Print success message
    switch (type) {
        case DEP_PATH:
            fprintf(stdout, "Added path dependency '%s' -> '%s'\n\n", dep_name, value);
            break;
        case DEP_GIT:
            fprintf(stdout, "Added git dependency '%s' -> '%s'", dep_name, value);
            if (strlen(dep.tag) > 0) {
                fprintf(stdout, " (tag: %s)", dep.tag);
            }
            else if (strlen(dep.branch) > 0) {
                fprintf(stdout, " (branch: %s)", dep.branch);
            }
            fprintf(stdout, "\n\n");
            break;
        default:
            return -1;
    }

    fprintf(stdout, "Run 'craft build' to build with the new dependency\n");
    return 0;
}
