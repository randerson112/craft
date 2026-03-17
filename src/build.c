#include "build.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"
#include "config.h"
#include "cmake.h"
#include <string.h>
#include "deps.h"

// Builds a project by creating a build directory and running cmake
int build_project(const char* cwd)
{
    // Get path to root of the project
    char project_root[512];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    // Check if CMakeLists.txt exists in the project root
    char cmake_path[256];
    snprintf(cmake_path, sizeof(cmake_path), "%s/CMakeLists.txt", project_root);
    if (!file_exists(cmake_path))
    {
        fprintf(stderr, "Error: Could not find CMakeLists.txt in this directory\n");
        return -1;
    }

    // Regenerate CMakeLists.txt from craft.toml if needed
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }

    if (validate_project_config(&config) != 0) {
        return -1;
    }

    // Fetch git dependencies into .craft/deps
    for (int i = 0; i < config.dependencies_count; i++) {
        dependency_t* dep = &config.dependencies[i];

        if (dep->type == DEP_GIT) {
            if (fetch_git_dependency(project_root, dep) != 0) {
                return -1;
            }
        }
    }

    if (cmake_needs_regeneration(project_root)) {
        generate_cmake(project_root, &config);
    }

    // Create a build directory if it does not exist
    char build_dir[256];
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
    else {
        fprintf(stdout, "Build directory found\n");
    }

    // Run cmake to build project
    fprintf(stdout, "Building project\n");
    
    char command[512];
    snprintf(command, sizeof(command), "cd %s && cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5 && cmake --build .", build_dir);
    if (system(command) != 0)
    {
        fprintf(stderr, "Error: Failed to build project\n");
        return -1;
    }

    // Build successful
    fprintf(stdout, "Project built successfully\n");
    return 0;
}

int handle_build() {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    return build_project(cwd);
}