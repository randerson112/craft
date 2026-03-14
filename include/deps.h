#ifndef DEPS_H
#define DEPS_H

typedef enum {
    DEP_PATH,
    DEP_GIT,
    DEP_INVALID
} dep_type_t;

typedef struct {
    char name[64];
    dep_type_t type;
    char value[512];
    char tag[64];
    char branch[64];
    char links[16][128];
    int links_count;
} dependency_t;

// Clones a repo from github into .craft/deps of the project
int fetch_git_dependency(const char* project_root, const dependency_t* dep);

#endif