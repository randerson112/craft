#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <stdlib.h>

#include "parser.h"

// Struct to hold workspace member data
typedef struct {
    char members[16][256];
    int member_count;
} workspace_config_t;

// Finds the root of the workspace where craft.toml is present with a workspace section
int get_workspace_root(char* buffer, size_t buffer_size, const char* cwd);

// Loads workspace configuration from craft.toml into config struct
int load_workspace_config(workspace_config_t* config, const char* workspace_root);

// Iterates through members and fetches dependencies for all members into workspace .craft/deps
int fetch_workspace_dependencies(const char* workspace_root, const workspace_config_t* config);

// Returns the number of executable members in the workspace
int get_executable_member_count(const char* workspace_root, const workspace_config_t* config);

// workspace command handler
int handle_workspace(const command_t* command_data);

#endif // WORKSPACE_H
