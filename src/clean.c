#include "clean.h"

#include <stdio.h>

#include "utils.h"
#include "craft_toml.h"
#include "platform.h"
#include "workspace.h"

// Removes the build directory in the current project or workspace
static int remove_build_dir(const char* cwd) {

    // Get the root of the project or workspace
    char root[PATH_SIZE];
    if (get_project_root(root, sizeof(root), cwd) != 0 && get_workspace_root(root, sizeof(root), cwd) != 0) {
        fprintf(stderr, "Error: Could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    char build_dir[PATH_SIZE];
    snprintf(build_dir, sizeof(build_dir), "%s/build", root);

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
        fprintf(stderr, "Error: Failed to remove build directory\n");
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
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    return remove_build_dir(cwd);
}