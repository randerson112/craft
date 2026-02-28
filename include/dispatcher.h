#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "parser.h"

// Sorts command into correct function call based on command data
int dispatch(command_t* command_data, const char* cwd);

#endif