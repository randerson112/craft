#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "parser.h"

// Template command handler
int handle_template(const command_t* command_data);

// Checks if a template exists for a certain lenaguage, builtin or custom
int template_exists(const char* name, const char* language);

#endif