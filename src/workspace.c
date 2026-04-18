#include "workspace.h"

#include <stdio.h>
#include <string.h>

#include "tomlc17.h"
#include "platform.h"
#include "utils.h"
#include "craft_toml.h"
#include "cmake.h"
#include "deps.h"

// Information about workspace section keys in craft.toml
static const config_key_t workspace_keys[] = {
    {"members", 1}
};
static const int num_workspace_keys = 1;

// Checks if given path is a Craft workspace
int is_craft_workspace(const char* path) {

    // Check if craft.toml exists at path
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", path);
    if (!file_exists(toml_path)) {
        return 0;
    }

    // Check if craft.toml contains workspace section and not a project section
    FILE* file = fopen(toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open craft.toml\n");
        return 0;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: Failed to parse craft.toml\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return 0;
    }

    toml_datum_t workspace_section = toml_seek(result.toptab, "workspace");
    toml_datum_t project_section = toml_seek(result.toptab, "project");
    if (workspace_section.type == TOML_TABLE && project_section.type != TOML_TABLE) {
        toml_free(result);
        return 1;
    }
    else {
        toml_free(result);
        return 0;
    }
}

int get_workspace_root(char* buffer, size_t buffer_size, const char* cwd) {
    char current_path[PATH_SIZE];
    snprintf(current_path, sizeof(current_path), "%s", cwd);

    while (1) {
        char toml_path[PATH_SIZE];
        snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", current_path);

        // Check if craft.toml file exists in this directory
        if (file_exists(toml_path) && is_craft_workspace(current_path)) {
            snprintf(buffer, buffer_size, "%s", current_path);
            return 0;
        }

        // Move up a directory
        char* last_slash = strrchr(current_path, '/');
        if (!last_slash || last_slash == current_path) {
            return -1;
        }
        
        *last_slash = '\0';
    }
}

int load_workspace_config(workspace_config_t* config, const char* workspace_root) {
    memset(config, 0, sizeof(*config));

    // Get path to craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", workspace_root);
    if (!file_exists(toml_path)) {
        fprintf(stderr, "Error: No craft.toml found in '%s'\n", workspace_root);
        return -1;
    }

    // Open craft.toml and parse
    FILE* file = fopen(toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open craft.toml\n");
        return -1;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: Failed to parse craft.toml\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return -1;
    }

    // Ensure workspace section is present
    toml_datum_t workspace_section = toml_seek(result.toptab, "workspace");
    if (workspace_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: Missing required [workspace] section in craft.toml\n");
        toml_free(result);
        return -1;
    }

    // Check for unknown keys
    if (check_unknown_keys(workspace_section, "workspace", workspace_keys, num_workspace_keys) != 0) {
        toml_free(result);
        return -1;
    }

    // Get workspace keys
    toml_datum_t members = toml_seek(workspace_section, "members");

    // Store values into config struct
    // Members
    if (members.type == TOML_ARRAY) {
        config->member_count = members.u.arr.size;
        for (int i = 0; i < members.u.arr.size; i++) {
            toml_datum_t elem = members.u.arr.elem[i];
            if (elem.type == TOML_STRING) {
                snprintf(config->members[i], sizeof(config->members[i]), "%s", elem.u.s);
            }
        }
    }

    toml_free(result);
    return 0;
}

int generate_workspace_toml(const char* workspace_root, const workspace_config_t* config) {

    // Get path to toml file
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", workspace_root);

    // Open the file and write config values
    FILE* file = fopen(toml_path, "w");
    if (!file) {
        fprintf(stderr, "Error: Failed to create craft.toml for workspace\n");
        return -1;
    }

    // Write contents
    fprintf(file, "[workspace]\n");
    fprintf(file, "members = [\n");

    for (int i = 0; i < config->member_count; i++) {
        if (i == config->member_count - 1) {
            fprintf(file, "    \"%s\"\n", config->members[i]);
        }
        else {
            fprintf(file, "    \"%s\",\n", config->members[i]);
        }
    }

    if (config->member_count == 0) {
        fprintf(file, "\n");
    }

    fprintf(file, "]\n");

    fclose(file);
    return 0;
}

int detect_workspace_members(workspace_config_t* config, const char* workspace_root, const char* path) {

    // Open directory
    dir_t* dir = open_dir(path);
    if (!dir) {
        fprintf(stderr, "Error: Failed to open workspace root directory\n");
        return -1;
    }

    // Read entries in directory
    dir_entry_t entry;
    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // Check if entry is a directory
        char full_path[PATH_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry.name);
        if (!dir_exists(full_path)) {
            continue;
        }

        // Check if subdirectory is a Craft project
        if (is_craft_project(full_path)) {
            const char* relative = full_path + strlen(workspace_root) + 1;
            snprintf(config->members[config->member_count++], sizeof(config->members[0]), "%s", relative);
            continue;
        }

        // Recurse into subdirectory
        detect_workspace_members(config, workspace_root, full_path);
    }

    close_dir(dir);
    return 0;
}

int fetch_workspace_dependencies(const char* workspace_root, const workspace_config_t* config) {

    // Loop through each member in workspace
    char current_member_path[PATH_SIZE];
    for (int i = 0; i < config->member_count; i++)  {
        snprintf(current_member_path, sizeof(current_member_path), "%s/%s", workspace_root, config->members[i]);

        // Load member project config
        project_config_t member_config = {0};
        if (load_project_config(&member_config, current_member_path) != 0) {
            return -1;
        }

        if (validate_project_config(&member_config) != 0) {
            return -1;
        }

        // Fetch member dependencies into workspace .craft/deps
        for (int j = 0; j < member_config.dependencies.dependencies_count; j++) {
            dependency_t* dep = &member_config.dependencies.dependencies[j];

            if (dep->type == DEP_GIT) {
                fetch_git_dependency(workspace_root, dep);
            }
        }
    }

    return 0;
}

int init_workspace_at_path(const char* path) {

    // Detect projects in workspace and create craft.toml
    workspace_config_t config = {0};

    fprintf(stdout, "Detecting members...\n");
    detect_workspace_members(&config, path, path);

    if (generate_workspace_toml(path, &config) != 0) {
        return -1;
    }

    // Generate CMake
    if (generate_workspace_cmake(path, &config) != 0) {
        return -1;
    }

    // Fetch member dependencies into workspace .craft/deps
    fprintf(stdout, "Fetching member dependencies...\n");
    if (fetch_workspace_dependencies(path, &config) != 0) {
        return -1;
    }

    fprintf(stdout, "\nInitialized Craft workspace at '%s'\n\n", path);
    fprintf(stdout, "Run 'craft build' to build the workspace\n");
    return 0;
}

int get_executable_member_count(const char* workspace_root, const workspace_config_t* config) {

    int count = 0;

    // Loop through members in workspace
    for (int i = 0; i < config->member_count; i++) {
        char path_to_member[PATH_SIZE];
        snprintf(path_to_member, sizeof(path_to_member), "%s/%s", workspace_root, config->members[i]);

        // Load member config and check if it is executable
        project_config_t member_config = {0};
        if (load_project_config(&member_config, path_to_member) != 0) {
            return -1;
        }
        if (validate_project_config(&member_config) != 0) {
            return -1;
        }

        if (strcmp(member_config.build.type, "executable") == 0) {
            count++;
        }
    }

    return count;
}

int handle_workspace(const command_t* command_data) {

    // Retrive path of current working directory where craft is being called
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    // Get relative path optional argument
    const char* rel_path = NULL;
    if (command_data->arg_count != 0) {
        rel_path = command_data->args[0];
    }

    // Get path to workspace
    char full_path[PATH_SIZE];
    if (rel_path) {
        snprintf(full_path, sizeof(full_path), "%s/%s", cwd, rel_path);
    }
    else {
        snprintf(full_path, sizeof(full_path), "%s", cwd);
    }

    // Ensure path is a directory
    if (!dir_exists(full_path)) {
        fprintf(stderr, "Error: Provided path is not a directory\n");
        return -1;
    }

    // Check if it is already a Craft workspace or project
    if (is_craft_project(full_path)) {
        fprintf(stderr, "Error: Craft project already exists at '%s'\n", full_path);
        return -1;
    }
    if (is_craft_workspace(full_path)) {
        fprintf(stderr, "Error: Craft workspace already exists at '%s'\n", full_path);
        return -1;
    }

    // Create workspace
    return init_workspace_at_path(full_path);
}