#include "clean.h"
#include "utils.h"
#include <stdio.h>

// Removes the build directory in the current directory
int remove_build_dir(const char* cwd) {

    // Get the root of the project
    char project_root[512];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    char build_dir[256];
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_root);

    // No build directory
    if (!dir_exists(build_dir)) {
        fprintf(stdout, "Nothing to clean.\n");
        return 0;
    }

    // Keep track of how many files removed and sizes
    unsigned int files_removed = 0;
    unsigned int bytes_removed = 0;

    // Remove the build directory recursively
    if (remove_dir_count(build_dir, &files_removed, &bytes_removed) != 0) {
        fprintf(stderr, "Failed to remove build directory.\n");
        return -1;
    }

    // Print how many files were removed and the total size
    char buff[32];
    format_bytes(bytes_removed, buff, sizeof(buff));
    fprintf(stdout, "Removed %u files, %s total.\n", files_removed, buff);
    return 0;
}

int handle_clean() {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    return remove_build_dir(cwd);
}