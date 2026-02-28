#ifndef HELP_H
#define HELP_H

// help command handler
int help(const char* command_arg);

// Prints craft general usage
void printHelp();

// Prints a brief usage example of a command
int printBriefUsage(const char* command);

#endif