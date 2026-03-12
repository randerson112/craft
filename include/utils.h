#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include "parser.h"

// Strips extension off a file and writes it to stripped file buffer
void stripExtension(const char* file, char* strippedFile);

// Gets the extension of a file and writes it to extension buffer
void getExtension(const char* file, char* extension, int extensionSize);

// Check if a file exists
int fileExists(const char* path);

// Check if a directory exists
int dirExists(const char* path);

// Removes a directory recursively, called by the two functions below
int removeDirRecursive(const char* path, unsigned int* count, unsigned int* bytes);

// Removes a directory without keeping track of counts
int removeDir(const char* path);

// Removes a directory and keeps track of counts
int removeDirCount(const char* path, unsigned int* count, unsigned int* bytes);

// Gets name of base directory from a full path
const char* getBaseName(const char* path);

// Creates starter project files and directorys at the given path
int createCppProject(const char* path, const char* name);

// Formats a number of bytes into a string with appropriate conversion
void formatBytes(unsigned int bytes, char* buffer, unsigned int bufferSize);

// Copy all contents of a directory into another directory recursively, excluding certain files if needed
int copy_dir_contents(const char* source_dir, const char* dest_dir, const char** excludes, size_t exclude_count);

// Copy file contents to another file
int copy_file(const char* source, const char* dest);

// Gets the path to craft config directory
int get_craft_home(char* buffer, size_t buffer_size);

// Gets the root path of a project by searching cwd and parent directories
int get_project_root(const char* cwd, char* buffer, size_t buffer_size);

// Gets the path to template directory
// type: builtin or custom
// language: c or cpp
// name: name of template
int get_template_directory(char* buffer, size_t buffer_size, const char* type, const char* language, const char* name);

// Given an unknown string, suggests a string from a list of valid strings that is closest to it
const char* suggest(const char* unknown, const char** valid, int valid_count);

// Checks if a version string follows the format "vMAJOR.MINOR.PATCH"
int is_valid_version(const char* version);

// Gets the option struct from a command data struct based on name, returns NULL if not found
const option_t* get_option(const command_t* command_data, const char* name);

#endif