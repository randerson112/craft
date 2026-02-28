#ifndef HELP_H
#define HELP_H

// Prints craft general usage
void printHelp();

// Prints a brief usage example of a command
int printBriefUsage(const char* command);

// Prints usage and detailed description of a specific command
int printCommandHelp(const char* command);

#endif