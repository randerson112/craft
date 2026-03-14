#include "deps.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>

int fetch_git_dependency(const char* project_root, const dependency_t* dep) {
    char dep_path[512];
    snprintf(dep_path, sizeof(dep_path), "%s/.craft/deps/%s", project_root, dep->name);

    // Skip if dependency is already cloned
    if (dirExists(dep_path)) {
        return 0;
    }

    fprintf(stdout, "Fetching '%s'...\n", dep->name);

    // Build git clone command
    char cmd[2048];
    if (strlen(dep->tag) > 0) {
        snprintf(cmd, sizeof(cmd), "git clone --depth 1 --branch %s %s %s", dep->tag, dep->value, dep_path);
    }
    else if (strlen(dep->branch) > 0) {
        snprintf(cmd, sizeof(cmd), "git clone --depth 1 --branch %s %s %s", dep->branch, dep->value, dep_path);
    }
    else {
        snprintf(cmd, sizeof(cmd), "git clone --depth 1 %s %s", dep->value, dep_path);
    }

    if (system(cmd) != 0) {
        fprintf(stderr, "Error: failed to clone '%s' from '%s'\n", dep->name, dep->value);
        return -1;
    }

    fprintf(stdout, "Fetched '%s'\n", dep->name);
    return 0;
}