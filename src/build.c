#include "build.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"
#include "config.h"
#include "cmake.h"

// Builds a project by creating a build directory and running cmake
int buildProject(const char* cwd)
{
    // Get path to root of the project
    char project_root[512];
    if (get_project_root(cwd, project_root, sizeof(project_root)) != 0) {
        return -1;
    }

    // Check if CMakeLists.txt exists in the project root
    char cmakePath[256];
    snprintf(cmakePath, sizeof(cmakePath), "%s/CMakeLists.txt", project_root);
    if (!fileExists(cmakePath))
    {
        fprintf(stderr, "Error: Could not find CMakeLists.txt in this directory\n");
        return -1;
    }

    // Regenerate CMakeLists.txt from craft.toml if needed
    if (cmake_needs_regeneration(project_root)) {
        project_config_t config;
        if (load_project_config(&config, project_root) != 0) {
            return -1;
        }

        if (validate_project_config(&config) != 0) {
            return -1;
        }

        generate_cmake(project_root, &config);
    }

    // Create a build directory if it does not exist
    char buildDir[256];
    snprintf(buildDir, sizeof(buildDir), "%s/build", project_root);
    if (!dirExists(buildDir))
    {
        if (mkdir(buildDir, 0755) == 0)
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
    snprintf(command, sizeof(command), "cd %s && cmake .. && cmake --build .", buildDir);
    if (system(command) != 0)
    {
        fprintf(stderr, "Error: Failed to build project\n");
        return -1;
    }

    // Build successful
    fprintf(stdout, "Project built successfully\n");
    return 0;
}

int build() {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    return buildProject(cwd);
}