#ifndef HELP_H
#define HELP_H

// help command handler
int help(const char* command_arg);

// Prints craft general usage
void printCraftInfo();

// Prints a short help message
void printBriefHelp();

// Prints a brief usage example of a command
int printBriefCommandUsage(const char* command);

#endif