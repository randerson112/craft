#ifndef CMAKE_H
#define CMAKE_H

#include "craft_toml.h"
#include "workspace.h"

// Checks if the craft.toml has been modified and cmake needs to be regenerated
int cmake_needs_regeneration(const char* project_root);

// Generates a project CMakeLists.txt based on project configs in craft.toml
int generate_project_cmake(const char* project_root, project_config_t* config);

// Generates a workspace CMakeLists.txt based on workspace configs in craft.toml
int generate_workspace_cmake(const char* workspace_root, workspace_config_t* config);

// Backs up an existing CMakeLists.txt to CMakeLists.txt.bak when initializing projects
int backup_cmake(const char* project_root);

#endif // CMAKE_H
