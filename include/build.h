#ifndef BUILD_H
#define BUILD_H

// Builds a project by creating a build directory and running cmake
int build_project(const char* project_root);

// build command handler
int handle_build();

#endif // BUILD_H