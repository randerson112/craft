#ifndef CMAKE_H
#define CMAKE_H

#include "craft_toml.h"

// Checks if the craft.toml has been modified and cmake needs to be regenerated
int cmake_needs_regeneration(const char* project_root);

// Generates a CMakeLists.txt based on project configs in craft.toml
int generate_cmake(const char* project_root, project_config_t* config);

// Backs up an existing CMakeLists.txt to CMakeLists.txt.bak when initializing projects
int backup_cmake(const char* project_root);

#endif // CMAKE_H
