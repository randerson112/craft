#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdlib.h>

#ifdef _WIN32

#include <windows.h>
#include <direct.h>

#define PATH_SIZE MAX_PATH
#define PATH_SEP '\\'
#define rmdir _rmdir
#define mkdir(path, mode) _mkdir(path)
#define getcwd _getcwd

#else

#include <limits.h>
#include <unistd.h>

#define PATH_SIZE PATH_MAX
#define PATH_SEP '/'

#endif

typedef enum {
    COMPILER_GCC,
    COMPILER_CLANG,
    COMPILER_MSVC,
    COMPILER_NONE
} compiler_t;

typedef struct {
    char name[256];
    int is_dir;
} dir_entry_t;

typedef struct dir_t dir_t;

// Opens a directory for iteration
dir_t* open_dir(const char* path);

// Gets the next entry, returns 1 if entry was filled, 0 if done
int read_dir(dir_t* dir, dir_entry_t* entry);

// Closes the directory
void close_dir(dir_t* dir);

// Platform abstract function to check if a command exists
int has_command(const char* command);

// Platform abstract function to detect what compiler is being used
compiler_t detect_compiler();

#endif // PLATFORM_H