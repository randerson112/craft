#include "init.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int initProjectInCwd(const char* cwd)
{
    // Get name of base directory
    const char* projectName = getBaseName(cwd);

    // Create new project in current directory with that name
    return createCppProject(cwd, projectName);
}

int initProjectAtPath(const char* cwd, const char* path)
{
    // Get full path to specified directory
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, path);

    // Remove . and .. from path
    char resolvedPath[256];
    if (realpath(fullPath, resolvedPath) == NULL) {
        fprintf(stderr, "Failed to resolve path\n");
        return -1;
    }

    // Check if directory exists
    if (!dirExists(resolvedPath))
    {
        fprintf(stderr, "Directory does not exist\n");
        return -1;
    }

    // Get name of base directory
    const char* projectName = getBaseName(fullPath);
    
    // Create a new project in that directory with same name
    return createCppProject(fullPath, projectName);
}