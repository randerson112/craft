#ifndef HELP_H
#define HELP_H

#include "parser.h"

// help command handler
int handle_help(const command_t* command_data);

// Prints craft general usage
void print_craft_info();

#endif