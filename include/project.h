#ifndef PROJECT_H
#define PROJECT_H

#include <stdlib.h>

#include "parser.h"

// project command handler
int handle_project(const command_t* command_data);

// Gets the name of a project by taking the last name of the path
void get_project_name(char* buffer, size_t buffer_size, const char* path);

// Creates a project by copying template structure to project path
int create_project_from_template(const char* path, const char* template, const char* lang, int use_git);

// Initializes git for the project
int init_git(const char* project_root);

#endif // PROJECT_H
