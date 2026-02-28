#include "clean.h"
#include "utils.h"
#include <stdio.h>

// Removes the build directory in the current directory
int remove_build_dir(const char* cwd) {
    char buildDir[256];
    snprintf(buildDir, sizeof(buildDir), "%s/build", cwd);

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

int clean(const char* cwd) {
    return remove_build_dir(cwd);
}