#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdlib.h>

// Windows macro definitions
#ifdef _WIN32

#include <windows.h>
#include <direct.h>

#define PATH_SIZE MAX_PATH
#define PATH_SEP '\\'
#define DEVNULL "NUL"
#define rmdir _rmdir
#define mkdir(path, mode) _mkdir(path)
#define getcwd _getcwd

#else
// Mac/Linux macro definitions

#include <limits.h>
#include <unistd.h>

#define PATH_SIZE PATH_MAX
#define PATH_SEP '/'
#define DEVNULL "/dev/null"

#endif

// Struct to hold data about a directory entry
typedef struct {
    char name[256];
    int is_dir;
} dir_entry_t;

// Forward declaration of directory struct
// Platform specific implementations in source file
typedef struct dir_t dir_t;

// Gets the current working directory
char* get_cwd(char* buffer, size_t buffer_size);

// Opens a directory for iteration
dir_t* open_dir(const char* path);

// Gets the next entry, returns 1 if entry was filled, 0 if done
int read_dir(dir_t* dir, dir_entry_t* entry);

// Closes the directory
void close_dir(dir_t* dir);

// Platform abstract function to check if a command exists
int has_command(const char* command);

#endif // PLATFORM_H