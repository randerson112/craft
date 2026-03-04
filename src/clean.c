#include "clean.h"
#include "utils.h"
#include <stdio.h>

// Removes the build directory in the current directory
int remove_build_dir(const char* cwd) {

    // Get the root of the project
    char project_root[512];
    if (get_project_root(cwd, project_root, sizeof(project_root)) != 0) {
        return -1;
    }

    char buildDir[256];
    snprintf(buildDir, sizeof(buildDir), "%s/build", project_root);

    // No build directory
    if (!dirExists(buildDir)) {
        fprintf(stdout, "Nothing to clean.\n");
        return 0;
    }

    // Keep track of how many files removed and sizes
    unsigned int filesRemoved = 0;
    unsigned int bytesRemoved = 0;

    // Remove the build directory recursively
    if (removeDirCount(buildDir, &filesRemoved, &bytesRemoved) != 0) {
        fprintf(stderr, "Failed to remove build directory.\n");
        return -1;
    }

    // Print how many files were removed and the total size
    char buff[32];
    formatBytes(bytesRemoved, buff, sizeof(buff));
    fprintf(stdout, "Removed %u files, %s total.\n", filesRemoved, buff);
    return 0;
}

int clean() {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    return remove_build_dir(cwd);
}