#include "platform.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "utils.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dirent.h>
#endif

char* get_cwd(char* buffer, size_t buffer_size) {
    if (getcwd(buffer, buffer_size) == NULL) {
        return NULL;
    }
    normalize_path(buffer);
    return buffer;
}

// Windows implementation
#ifdef _WIN32

struct dir_t {
    char path[PATH_SIZE];
    HANDLE handle;
    WIN32_FIND_DATA find_data;
    int first;
};

dir_t* open_dir(const char* path) {
    if (!dir_exists(path)) {
        fprintf(stderr, "Error: '%s' is not a directory or does not exist\n", path);
        return NULL;
    }

    dir_t* dir = malloc(sizeof(dir_t));

    char search[512];
    snprintf(search, sizeof(search), "%s/*", path);
    dir->handle = FindFirstFile(search, &dir->find_data);
    if (dir->handle == INVALID_HANDLE_VALUE) {
        free(dir);
        return NULL;
    }

    snprintf(dir->path, sizeof(dir->path), "%s", path);
    dir->first = 1;
    return dir;
}

int read_dir(dir_t* dir, dir_entry_t* entry) {

    // Get name of next entry
    if (dir->first) {
        dir->first = 0;
    }
    else {
        if (!FindNextFile(dir->handle, &dir->find_data)) {
            return 0;
        }
    }
    snprintf(entry->name, sizeof(entry->name), "%s", dir->find_data.cFileName);
    
    // Check if entry is a directory
    char full_path[PATH_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir->path, entry->name);
    entry->is_dir = dir_exists(full_path);
    return 1;
}

void close_dir(dir_t* dir) {
    FindClose(dir->handle);
    free(dir);
}

int has_command(const char* command) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "where %s >null 2>&1", command);
    
    return system(buffer) == 0;
}

// MacOS and Linux implementation
#else

struct dir_t {
    char path[PATH_SIZE];
    DIR* dir;
};

dir_t* open_dir(const char* path) {
    if (!dir_exists(path)) {
        fprintf(stderr, "Error: '%s' is not a directory or does not exist\n", path);
        return NULL;
    }
    
    dir_t* d = malloc(sizeof(dir_t));
    d->dir = opendir(path);
    if (!d->dir) {
        free(d);
        return NULL;
    }

    snprintf(d->path, sizeof(d->path), "%s", path);
    return d;
}

int read_dir(dir_t* dir, dir_entry_t* entry) {

    // Get the entry name
    struct dirent* e = readdir(dir->dir);
    if (!e) {
        return 0;
    }
    snprintf(entry->name, sizeof(entry->name), "%s", e->d_name);
    
    // Check if entry is a directory
    char full_path[PATH_SIZE];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir->path, entry->name);
    entry->is_dir = dir_exists(full_path);
    return 1;
}

void close_dir(dir_t* dir) {
    closedir(dir->dir);
    free(dir);
}

int has_command(const char* command) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "which %s > /dev/null 2>&1", command);

    return system(buffer) == 0;
}

#endif
