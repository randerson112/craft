#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <stdlib.h>

#include "parser.h"

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

// Generates craft.toml for a workspace based on config values
int generate_workspace_toml(const char* workspace_root, const workspace_config_t* config);

// Scans subdirectories in workspace for craft.toml containing project section
int detect_workspace_members(workspace_config_t* config, const char* workspace_root, const char* path);

// Iterates through members and fetches dependencies for all members into workspace .craft/deps
int fetch_workspace_dependencies(const char* workspace_root, const workspace_config_t* config);

// Initializes a workspace at the given path
int init_workspace_at_path(const char* path);

// Returns the number of executable members in the workspace
int get_executable_member_count(const char* workspace_root, const workspace_config_t* config);

// workspace command handler
int handle_workspace(const command_t* command_data);

#endif // WORKSPACE_H
