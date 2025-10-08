#include "build.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"

int buildProject(const char* cwd)
{
    // Check if CMakeLists.txt exists in current directory
    char cmakePath[256];
    snprintf(cmakePath, sizeof(cmakePath), "%s/CMakeLists.txt", cwd);
    if (!fileExists(cmakePath))
    {
        fprintf(stderr, "Error: Could not find CMakeLists.txt in this directory\n");
        return -1;
    }

    // Create a build directory if it does not exist
    char buildDir[256];
    snprintf(buildDir, sizeof(buildDir), "%s/build", cwd);

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