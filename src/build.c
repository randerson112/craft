#include "build.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"
#include "craft_toml.h"
#include "workspace.h"
#include "cmake.h"
#include "deps.h"
#include "platform.h"

int build_project(const char* project_root) {

    // Regenerate CMakeLists.txt from craft.toml if needed
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }

    if (validate_project_config(&config) != 0) {
        return -1;
    }

    if (cmake_needs_regeneration(project_root)) {
        fprintf(stdout, "Regenerating CMake...\n");
        generate_project_cmake(project_root, &config);
    }

    // Fetch git dependencies into .craft/deps
    for (int i = 0; i < config.dependencies.dependencies_count; i++) {
        dependency_t* dep = &config.dependencies.dependencies[i];

        if (dep->type == DEP_GIT) {
            if (fetch_git_dependency(project_root, dep) != 0) {
                return -1;
            }
        }
    }

    // Create a build directory if it does not exist
    char build_dir[PATH_SIZE];
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_root);
    if (!dir_exists(build_dir))
    {
        if (mkdir(build_dir, 0755) == 0)
        {
            fprintf(stdout, "Generated build directory\n");
        }
        else
        {
            fprintf(stderr, "Error: Failed to create build directory\n");
            return -1;
        }
    }

    // Run cmake to build project
    fprintf(stdout, "Configuring...\n");
    char configure_command[COMMAND_SIZE];
    snprintf(configure_command, sizeof(configure_command), "cmake -S \"%s\" -B \"%s\" -Wno-dev > %s", project_root, build_dir, DEVNULL);

    if (system(configure_command) != 0) {
        fprintf(stderr, "Error: Failed to configure CMake\n");
        return -1;
    }

    char build_command[COMMAND_SIZE];
    snprintf(build_command, sizeof(build_command), "cmake --build \"%s\" > %s", build_dir, DEVNULL);

    fprintf(stdout, "Building...\n");
    if (system(build_command) != 0) {
        fprintf(stderr, "Error: Failed to build project\n");
        return -1;
    }

    // Build successful
    fprintf(stdout, "Project built successfully\n");
    return 0;
}

int build_workspace(const char* workspace_root) {

    // Regenerate CMakeLists.txt from craft.toml if needed
    workspace_config_t config = {0};
    if (load_workspace_config(&config, workspace_root) != 0) {
        return -1;
    }

    if (cmake_needs_regeneration(workspace_root)) {
        fprintf(stdout, "Regenerating CMake...\n");
        generate_workspace_cmake(workspace_root, &config);
    }

    // Fetch member dependencies if not already
    if (fetch_workspace_dependencies(workspace_root, &config) != 0) {
        return -1;
    }

    // Create a build directory if it does not exist
    char build_dir[PATH_SIZE];
    snprintf(build_dir, sizeof(build_dir), "%s/build", workspace_root);
    if (!dir_exists(build_dir))
    {
        if (mkdir(build_dir, 0755) == 0)
        {
            fprintf(stdout, "Generated build directory\n");
        }
        else
        {
            fprintf(stderr, "Error: Failed to create build directory\n");
            return -1;
        }
    }

    // Run cmake to build project
    fprintf(stdout, "Configuring...\n");
    char configure_command[COMMAND_SIZE];
    snprintf(configure_command, sizeof(configure_command), "cmake -S \"%s\" -B \"%s\" -Wno-dev > %s", workspace_root, build_dir, DEVNULL);

    if (system(configure_command) != 0) {
        fprintf(stderr, "Error: Failed to configure CMake\n");
        return -1;
    }

    char build_command[COMMAND_SIZE];
    snprintf(build_command, sizeof(build_command), "cmake --build \"%s\" > %s", build_dir, DEVNULL);

    fprintf(stdout, "Building...\n");
    if (system(build_command) != 0) {
        fprintf(stderr, "Error: Failed to build workspace\n");
        return -1;
    }

    // Build successful
    fprintf(stdout, "Workspace built successfully\n");
    return 0;
}

int handle_build() {
    // Retrive path of current working directory where craft is being called
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    // Determine if in a workspace or a project and build
    char project_root[PATH_SIZE];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        char workspace_root[PATH_SIZE];
        if (get_workspace_root(workspace_root, sizeof(workspace_root), cwd) != 0) {
            fprintf(stderr, "Error: Could not find craft.toml in current directory or any parent directory\n");
            return -1;
        }
        return build_workspace(workspace_root);
    }

    return build_project(project_root);
}