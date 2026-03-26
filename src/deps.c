#include "deps.h"
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>
#include "platform.h"
#include <sys/stat.h>

int fetch_git_dependency(const char* project_root, const dependency_t* dep) {
    
    // Create .craft/deps if not already there
    char craft_directory[512];
    char craft_deps_directory[512];
    snprintf(craft_directory, sizeof(craft_directory), "%s/.craft", project_root);
    snprintf(craft_deps_directory, sizeof(craft_deps_directory), "%s/deps", craft_directory);

    if (!dir_exists(craft_directory)) {
        mkdir(craft_directory, 0755);
    }
    if (!dir_exists(craft_deps_directory)) {
        mkdir(craft_deps_directory, 0755);
    }

    char dep_path[512];
    snprintf(dep_path, sizeof(dep_path), "%s/.craft/deps/%s", project_root, dep->name);

    // Skip if dependency is already cloned
    if (dir_exists(dep_path)) {
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

dependency_t* get_dependency(dependency_t* dependencies, const int count, const char* dep_name) {
    // Find dependency by name
    for (int i = 0; i < count; i++) {
        dependency_t* dep = &dependencies[i];

        if (strcmp(dep->name, dep_name) == 0) {
            return dep;
        }
    }

    return NULL;
}

void delete_dependency_dir(const char* project_root, const char* dep_name) {
    char dep_dir[512];
    snprintf(dep_dir, sizeof(dep_dir), "%s/.craft/deps/%s", project_root, dep_name);

    if (dir_exists(dep_dir)) {
        remove_dir(dep_dir);
    }
}

const char* get_dependency_suggestion(const dependency_t* dependencies, const int count, const char* unknown) {
    // Make list of current dependency names
    const char* dep_names[32];
    for (int i = 0; i < count; i++) {
        dep_names[i] = dependencies[i].name;
    }

    // Get suggestion
    return suggest(unknown, dep_names, count);
}