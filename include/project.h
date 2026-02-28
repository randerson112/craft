#ifndef PROJECT_H
#define PROJECT_H

// Project command handler
int project(const char* project_name_arg, const char* cwd);

// Creates a new directory at path and creates a new project inside
int createNewProject(const char* cwd, const char* path);

#endif // PROJECT_H
