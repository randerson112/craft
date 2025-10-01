#ifndef INIT_H
#define INIT_H

// Creates a new project in the current directory
int initProjectInCwd(const char* cwd);

// Creates a new project in an existing directory relative to the current directory
int initProjectAtPath(const char* cwd, const char* path);

#endif