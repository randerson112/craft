#ifndef UTILS_H
#define UTILS_H

#include "parser.h"
#include <stdlib.h>

// Strips extension off a file and writes it to stripped file buffer
void strip_extension(const char* file, char* stripped_file);

// Gets the extension of a file and writes it to extension buffer
void get_extension(const char* file, char* extension, int extension_size);

// Check if a file exists
int file_exists(const char* path);

// Check if a directory exists
int dir_exists(const char* path);

// Removes a directory without keeping track of counts
int remove_dir(const char* path);

// Removes a directory and keeps track of counts
int remove_dir_count(const char* path, unsigned int* count, unsigned int* bytes);

// Formats a number of bytes into a string with appropriate conversion
void format_bytes(unsigned int bytes, char* buffer, unsigned int buffer_size);

// Copies file contents to another file
int copy_file(const char* source, const char* dest);

// Copy all contents of a directory into another directory recursively, excluding certain files if needed
int copy_dir_contents(const char* source_dir, const char* dest_dir, const char** excludes, size_t exclude_count);

// Gets the path to craft config directory
int get_craft_home(char* buffer, size_t buffer_size);

// Gets the root path of a project by searching cwd and parent directories
int get_project_root(char* buffer, size_t buffer_size, const char* cwd);

// Gets the path to template directory
// type: builtin or custom
// language: c or cpp
// name: name of template
int get_template_directory(char* buffer, size_t buffer_size, const char* type, const char* language, const char* name);

// Given an unknown string, suggests a string from a list of valid strings that is closest to it
const char* suggest(const char* unknown, const char** valid, int valid_count);

// Checks if a version string follows the format "vMAJOR_MINOR_PATCH"
int is_valid_version(const char* version);

// Gets the option struct from a command data struct based on name, returns NULL if not found
const option_t* get_option(const command_t* command_data, const char* name);

// Checks if the given path is a Craft project
const int is_craft_project(const char* path);

// Gets the name of the current directory from a path
void get_dir_name(char* buffer, size_t buffer_size, const char* path);

// Searches a directory recursively for a file and writes the path to buffer
int search_dir_for_file(char* buffer, size_t buffer_size, const char* path, const char* filename);

// Replaces all backslashes in a path to forward slashes
void normalize_path(char* path);

#endif