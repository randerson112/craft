#include "remove.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "craft_toml.h"
#include "cmake.h"
#include "deps.h"
#include "platform.h"

// Removes a dependency from a project's config by name
static void remove_dependency_from_config(project_config_t* config, const char* dep_name) {
    for (int i = 0; i < config->dependencies.dependencies_count; i++) {
        if (strcmp(config->dependencies.dependencies[i].name, dep_name) == 0) {
            for (int j = i; j < config->dependencies.dependencies_count - 1; j++) {
                config->dependencies.dependencies[j] = config->dependencies.dependencies[j + 1];
            }
            config->dependencies.dependencies_count--;
            break;
        }
    }
}

int handle_remove(const command_t* command_data) {

    // Retrive path of current working directory where craft is being called
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    // Find project root and load config
    char project_root[PATH_SIZE];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
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

    dependency_t* removed_dep = get_dependency(config.dependencies.dependencies, config.dependencies.dependencies_count, dep_name);

    // Dependency not found, give a suggestion if close enough
    if (!removed_dep) {
        fprintf(stderr, "Error: Dependency '%s' not found in craft.toml\n", dep_name);

        // Print suggestion if close enough
        const char* suggestion = get_dependency_suggestion(config.dependencies.dependencies, config.dependencies.dependencies_count, dep_name);
        if (suggestion) {
            fprintf(stderr, "\nDid you mean '%s'?\n", suggestion);
        }

        return -1;
    }

    // If dependency is a git dependency, remove it from .craft/deps
    if (removed_dep->type == DEP_GIT) {
        delete_dependency_dir(project_root, dep_name);
    }

    // Remove the dependency from the craft.toml
    remove_dependency_from_config(&config, dep_name);

    if (generate_craft_toml(project_root, &config) != 0) {
        return -1;
    }

    // Regnerate CMake file
    if (generate_project_cmake(project_root, &config) != 0) {
        return -1;
    }

    fprintf(stdout, "Removed '%s' from dependencies\n\n", dep_name);
    fprintf(stdout, "Tip: Run 'craft clean && craft build' to fully rebuild without this dependency\n");
    return 0;
}