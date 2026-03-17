#ifndef PROJECT_H
#define PROJECT_H

#include "parser.h"
#include <stdlib.h>

// Project command handler
int handle_project(const command_t* command_data);

// Gets the name of a project by taking the last name of the path
void get_project_name(const char* path, char* buffer, size_t buffer_size);

// Creates a project by copying template structure to project path
int create_project_from_template(const char* path, const char* template, const char* lang);

#endif // PROJECT_H
