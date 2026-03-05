#ifndef PROJECT_H
#define PROJECT_H

#include "parser.h"
#include <stdlib.h>

// Project command handler
int project(command_t* command_data);

// Gets the name of a project by capitalizing the directory name
void get_project_name(const char* path, char* buffer, size_t buffer_size);

// Infer a build type based on the template if it is builtin, otherwise returns executable
const char* infer_build_type(const char* template);

#endif // PROJECT_H
