#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <stdlib.h>

typedef struct {
    char members[16][256];
    int member_count;
} workspace_config_t;

// Checks if given path is a Craft workspace
int is_craft_workspace(const char* path);

// Finds the root of the workspace where craft.toml is present with a workspace section
int get_workspace_root(char* buffer, size_t buffer_size, const char* cwd);

// Loads workspace configuration from craft.toml into config struct
int load_workspace_config(workspace_config_t* config, const char* workspace_root);

#endif // WORKSPACE_H
