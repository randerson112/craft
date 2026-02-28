#include "project.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "utils.h"

int createNewProject(const char* cwd, const char* path)
{
    // Check if directory at path already exists
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, path);

    if (dirExists(fullPath))
    {
        fprintf(stderr, "Error: Directory with that name already exists\n");
        return -1;
    }

    // Create a new directory
    if (mkdir(fullPath, 0755) == 0)
    {
        fprintf(stdout, "Generated project directory\n");
    }
    else
    {
        fprintf(stderr, "Error: Failed to create project directory\n");
        return -1;
    }

    // Remove . and .. from path
    char resolvedPath[256];
    if (realpath(fullPath, resolvedPath) == NULL)
    {
        fprintf(stderr, "Error: Failed to resolve path\n");
        return -1;
    }

    // Get project name from directory name
    const char* projectName = getBaseName(resolvedPath);

    // Create project inside that directory
    return createCppProject(resolvedPath, projectName);
}

int project(const char* project_name_arg, const char* cwd) {
    return createNewProject(cwd, project_name_arg);
}