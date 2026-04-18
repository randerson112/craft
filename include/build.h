#ifndef BUILD_H
#define BUILD_H

// Builds a project by creating a build directory and running CMake
int build_project(const char* project_root);

// Builds a workspace by creating build directory and running CMake
int build_workspace(const char* workspace_root);

// build command handler
int handle_build();

#endif // BUILD_H