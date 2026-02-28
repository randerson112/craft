#include "run.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"

int runExecutable(const char* cwd, const char* executableName)
{
    // Check current and build directory for executable
    char executablePathCwd[256];
    char executablePathBuild[256];

    snprintf(executablePathCwd, sizeof(executablePathCwd), "%s/%s", cwd, executableName);
    snprintf(executablePathBuild, sizeof(executablePathBuild), "%s/build/%s", cwd, executableName);

    char* validExecutablePath = NULL;
    if (fileExists(executablePathCwd))
    {
        // Executable found in current directory
        validExecutablePath = executablePathCwd;
    }
    
    else if (fileExists(executablePathBuild))
    {
        // Executable found in build directory
        validExecutablePath = executablePathBuild;
    }

    else
    {
        // Executable not found
        fprintf(stderr, "Error: Could not find executable: %s\n", executableName);
        return -1;
    }

    // Run executable
    fprintf(stdout, "Running executable: %s\n", executableName);

    char command[512];
    snprintf(command, sizeof(command), "%s", validExecutablePath);

    if (system(command) != 0)
    {
        fprintf(stderr, "Error: Failed to run executable\n");
        return -1;
    }

    // Run successful
    fprintf(stdout, "Run successful\n");
    return 0;
}

int run(const char* executable_name_arg, const char* cwd) {
    return runExecutable(cwd, executable_name_arg);
}