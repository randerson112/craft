#include "remove.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "config.h"
#include <string.h>
#include "cmake.h"

// Finds and returns a dependency object by name in a project config
static dependency_t* find_target_dependency(const char* dep_name, project_config_t* config) {

    // Look for dependency by name in dependencies
    for (int i = 0; i < config->dependencies_count; i++) {
        dependency_t* dep = &config->dependencies[i];

        if (strcmp(dep->name, dep_name) == 0) {
            return dep;
        }
    }

    // Not found
    return NULL;
}

// Removes a dependency directory from .craft/deps by name
static void remove_dep_directory(const char* project_root, const char* dep_name) {
    char dep_dir[512];
    snprintf(dep_dir, sizeof(dep_dir), "%s/.craft/deps/%s", project_root, dep_name);

    if (dirExists(dep_dir)) {
        removeDir(dep_dir);
    }
}

// Removes a dependency from a project's config by name
static void remove_dep_from_config(project_config_t* config, const char* dep_name) {
    for (int i = 0; i < config->dependencies_count; i++) {
        if (strcmp(config->dependencies[i].name, dep_name) == 0) {
            for (int j = i; j < config->dependencies_count - 1; j++) {
                config->dependencies[j] = config->dependencies[j + 1];
            }
            config->dependencies_count--;
            break;
        }
    }
}

// Gets a dependency name suggestion if user was a few letters off
static const char* get_dependency_suggestion(const project_config_t* config, const char* unknown) {
    // Make list of current dependency names
    const char* dep_names[32];
    for (int i = 0; i < config->dependencies_count; i++) {
        dep_names[i] = config->dependencies[i].name;
    }

    // Get suggestion
    return suggest(unknown, dep_names, config->dependencies_count);
}

int handle_remove(const command_t* command_data) {

    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Find project root and load config
    char project_root[512];
    if (get_project_root(cwd, project_root, sizeof(project_root)) != 0) {
        fprintf(stderr, "Error: Could not find craft.toml in current or any parent directory\n");
        return -1;
    }

    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }
    if (validate_project_config(&config) != 0) {
        return -1;
    }

    // Get dependency name from command argument
    const char* dep_name = command_data->args[0];

    dependency_t* removed_dep = find_target_dependency(dep_name, &config);

    // Dependency not found, give a suggestion if close enough
    if (!removed_dep) {
        fprintf(stderr, "Error: dependency '%s' not found in craft.toml\n", dep_name);

        // Print suggestion if close enough
        const char* suggestion = get_dependency_suggestion(&config, dep_name);
        if (suggestion) {
            fprintf(stderr, "\nDid you mean '%s'?\n", suggestion);
        }

        return -1;
    }

    // If dependency is a git dependency, remove it from .craft/deps
    if (removed_dep->type == DEP_GIT) {
        remove_dep_directory(project_root, dep_name);
    }

    // Remove the dependency from the craft.toml
    remove_dep_from_config(&config, dep_name);

    if (generate_craft_toml(project_root, &config) != 0) {
        return -1;
    }

    // Regnerate CMake file
    if (generate_cmake(project_root, &config) != 0) {
        return -1;
    }

    fprintf(stdout, "Removed '%s' from dependencies\n\n", dep_name);
    fprintf(stdout, "Tip: Run 'craft clean' to remove the dependency files from the build directory.\n");
    fprintf(stdout, "     Then run 'craft build' to rebuild the project.\n");
    return 0;
}