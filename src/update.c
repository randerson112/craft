#include "update.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deps.h"
#include "utils.h"
#include "config.h"
#include "platform.h"

// Updates a single dependency by fetching the latest version, unless a tag is present
static int update_dependency(const char* project_root, const dependency_t* dep) {

    // If tag is present, no need to fetch latest
    if (dep->type == DEP_PATH) {
        fprintf(stdout, "Skipping '%s' - local path dependency\n", dep->name);
        return 0;
    }

    // If it is a local craft project no need do anything
    if (strlen(dep->tag) > 0) {
        fprintf(stdout, "Skipping '%s' - pinned to tag '%s'\n", dep->name, dep->tag);
        return 0;
    }

    fprintf(stdout, "Updating '%s'\n", dep->name);

    // Delete current dep directory
    delete_dependency_dir(project_root, dep->name);

    // Fetch the latest version
    return fetch_git_dependency(project_root, dep);
}

// Updates all dependencies by fetching the latest version for each, unless a tag is present
static int update_all_dependencies(const char* project_root, const project_config_t* config) {
    
    // Call update_dependency on all dependencies
    for (int i = 0; i < config->dependencies_count; i++) {
        const dependency_t* dep = &config->dependencies[i];

        if (update_dependency(project_root, dep) != 0) {
            return -1;
        }
    }

    return 0;
}

int handle_update(const command_t* command_data) {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Find project root and load project config
    char project_root[512];
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

    // If argument was given, update that dependency only
    if (command_data->arg_count > 0) {
        const char* dep_name = command_data->args[0];

        // Check if dependency exists
        const dependency_t* dep = get_dependency(config.dependencies, config.dependencies_count, dep_name);
        if (!dep) {
            fprintf(stderr, "Error: Could not find dependency '%s' in craft.toml\n", dep_name);

            // Print suggestion if close enough
            const char* suggestion = get_dependency_suggestion(config.dependencies, config.dependencies_count, dep_name);
            if (suggestion) {
                fprintf(stderr, "\nDid you mean '%s'?\n", suggestion);
            }
            
            return -1;
        }

        if (update_dependency(project_root, dep) != 0) {
            return -1;
        }
    }

    // No arguments, update all dependencies
    else {
        if (update_all_dependencies(project_root, &config) != 0) {
            return -1;
        }
    }

    fprintf(stdout, "Tip: Run 'craft build' to rebuild the project with the updated dependencies\n");
    return 0;
}