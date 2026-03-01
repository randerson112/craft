#ifndef PROJECT_H
#define PROJECT_H

#include "parser.h"

// Project command handler
int project(command_t* command_data, const char* cwd);

// Creates a new directory at path and creates a new project inside
int get_project_path(const char* cwd, const char* rel_path, char* buffer);

// Creates a project by copying template structure to project path
int create_project_from_template(const char* path, const char* template, const char* language);

#endif // PROJECT_H
