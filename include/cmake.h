#ifndef CMAKE_H
#define CMAKE_H

#include "config.h"

// Checks if the craft.toml has been modified and cmake needs to be regenerated
int cmake_needs_regeneration(const char* project_root);

// Generates a CMakeLists.txt based on project configs in craft.toml
int generate_cmake(const char* project_path, project_config_t* config);

#endif // CMAKE_H
