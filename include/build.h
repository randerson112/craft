#ifndef BUILD_H
#define BUILD_H

#include "parser.h"

// Builds a project by creating a build directory and running CMake
int build_project(const char* project_root, const char* profile);

// Builds a workspace by creating build directory and running CMake
int build_workspace(const char* workspace_root, const char* profile);

// build command handler
int handle_build(const command_t* command_data);

#endif // BUILD_H