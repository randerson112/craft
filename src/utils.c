#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "platform.h"

void strip_extension(const char* file, char* stripped_file)
{
    int i = 0;
    while (file[i] != '\0' && file[i] != '.')
    {
        stripped_file[i] = file[i];
        i++;
    }
    stripped_file[i] = '\0';
}

void get_extension(const char* file, char* extension, int extension_size)
{
    // Find where dot is
    char* dot = strrchr(file, '.');
    if (dot != NULL)
    {
        // Write contents after dot to extension buffer
        snprintf(extension, extension_size, "%s", dot + 1);
    }
    else
    {
        // If no dot found then write null terminator
        *extension = '\0';
    }
}

// Check if a path exists and is a file
int file_exists(const char* path)
{
    struct stat statbuff;
    if (stat(path, &statbuff) != 0) return 0;

    #ifdef _WIN32
    return (statbuff.st_mode & _S_IFMT) == _S_IFREG;
    #else
    return S_ISREG(statbuff.st_mode);
    #endif
}

// Check if a path exists and is a directory
int dir_exists(const char* path)
{
    struct stat statbuff;
    if (stat(path, &statbuff) != 0) return 0;

    #ifdef _WIN32
    return (statbuff.st_mode & _S_IFMT) == _S_IFDIR;
    #else
    return S_ISDIR(statbuff.st_mode);
    #endif
}

// Removes a directory recursivley, keeps track of number of files deleted, and how many bytes
static int remove_dir_recursive(const char* path, unsigned int* count, unsigned int* bytes) {
    dir_t* dir = open_dir(path);
    if (!dir) {
        return -1;
    }

    dir_entry_t entry;
    char buffer[PATH_SIZE];

    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        snprintf(buffer, sizeof(buffer), "%s/%s", path, entry.name);

        struct stat statbuff;
        stat(buffer, &statbuff);

        if (bytes) *bytes += statbuff.st_size;
        if (count) (*count)++;
        if (entry.is_dir) {
            remove_dir_recursive(buffer, count, bytes);
        }
        else {
            remove(buffer);
        }
    }

    close_dir(dir);
    return rmdir(path);
}

// Removes a directory without keeping track of counts
int remove_dir(const char* path) {
    return remove_dir_recursive(path, NULL, NULL);
}

// Removes a directory and keeps track of counts
int remove_dir_count(const char* path, unsigned int* count, unsigned int* bytes) {
    struct stat statbuff;
    if (stat(path, &statbuff) == 0) {
        *bytes += statbuff.st_size;
    }
    (*count)++;

    return remove_dir_recursive(path, count, bytes);
}

// Formats a number of bytes into a string with appropriate conversion
void format_bytes(unsigned int bytes, char* buffer, unsigned int buffer_size) {
    // Gigabytes
    if (bytes >= 1073741824) {
        snprintf(buffer, buffer_size, "%.1f GiB", bytes / 1073741824.0);
    }
    // Megabytes
    else if (bytes >= 1048576) {
        snprintf(buffer, buffer_size, "%.1f MiB", bytes / 1048576.0);
    }
    // Kilobytes
    else if (bytes >= 1024) {
        snprintf(buffer, buffer_size, "%.1f KiB", bytes / 1024.0);
    }
    // Bytes
    else {
        snprintf(buffer, buffer_size, "%u B", bytes);
    }
}

int copy_file(const char* source, const char* dest) {

    // Open files
    FILE* in = fopen(source, "rb");
    if (!in) {
        fprintf(stderr, "Error: Failed to copy file.\n");
        return -1;
    }

    FILE* out = fopen(dest, "wb");
    if (!out) {
        fprintf(stderr, "Error: Failed to copy file.\n");
        fclose(in);
        return -1;
    }

    // Read bits from file and write to destination file
    char buffer[FILE_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        fwrite(buffer, 1, bytes, out);
    }

    // Close files
    fclose(in);
    fclose(out);
    return 0;
}

int copy_dir_contents(const char* source_dir, const char* dest_dir, const char** excludes, size_t exclude_count) {

    // Open source directory
    dir_t* dir = open_dir(source_dir);
    if (!dir) {
        fprintf(stderr, "Error: Failed to copy directory contents.\n");
        return -1;
    }

    // Read all entries in directory
    dir_entry_t entry;
    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // Check if file or dir should be excluded
        int should_exclude = 0;
        for (int i = 0; i < exclude_count; i++) {
            if (strcmp(entry.name, excludes[i]) == 0) {
                should_exclude = 1;
                break;
            }
        }
        if (should_exclude) continue;

        // Get the full path to source entry and destination entry
        char source_path[PATH_SIZE];
        char dest_path[PATH_SIZE];
        snprintf(source_path, sizeof(source_path), "%s/%s", source_dir, entry.name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry.name);

        struct stat statbuff;
        stat(source_path, &statbuff);

        // If entry is directory, copy contents recursively
        if (entry.is_dir) {
            mkdir(dest_path, 0755);
            if (copy_dir_contents(source_path, dest_path, excludes, exclude_count) != 0) {
                close_dir(dir);
                return -1;
            }
        }
        // Entry is a file, copy contents to destination
        else {
            copy_file(source_path, dest_path);
        }
    }

    // Close source directory
    close_dir(dir);
    return 0;
}

int get_craft_home(char* buffer, size_t buffer_size) {
    char* craft_home = getenv("CRAFT_HOME");
    if (craft_home) {
        normalize_path(craft_home);
        snprintf(buffer, buffer_size, "%s", craft_home);
        return 0;
    }

    #ifdef _WIN32
        char* user_home = getenv("USERPROFILE");
    #else
        char* user_home = getenv("HOME");
    #endif

    if (user_home) {
        normalize_path(user_home);
        snprintf(buffer, buffer_size, "%s/.craft", user_home);
        return 0;
    }

    fprintf(stderr, "Error: Could not locate home directory.\n");
    return -1;
}

int get_template_directory(char* buffer, size_t buffer_size, const char* type, const char* language, const char* name) {
    // Get the path to craft home
    char craft_home[PATH_SIZE];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        return -1;
    }

    // Get base templates directory
    if (!type) {
        snprintf(buffer, buffer_size, "%s/templates", craft_home);
        return 0;
    }

    // Get (builtin or custom) templates directory
    if (!language) {
        snprintf(buffer, buffer_size, "%s/templates/%s", craft_home, type);
        return 0;
    }

    // Get (builtin or custom) (c or cpp) templates directory
    if (!name) {
        snprintf(buffer, buffer_size, "%s/templates/%s/%s", craft_home, type, language);
        return 0;
    }

    // Get specific template directory
    snprintf(buffer, buffer_size, "%s/templates/%s/%s/%s", craft_home, type, language, name);
    return 0;
}

// Calculates the minimum number of changes needed to make one string equal another
// Used for suggestions when the user misspells a value
static int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = (int)strlen(s1);
    int len2 = (int)strlen(s2);

    // Clamp lengths to prevent buffer overflow (matrix is 64x64)
    if (len1 > 62) len1 = 62;
    if (len2 > 62) len2 = 62;

    int matrix[64][64];

    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = s1[i-1] == s2[j-1] ? 0 : 1;
            int del = matrix[i-1][j] + 1;
            int ins = matrix[i][j-1] + 1;
            int sub = matrix[i-1][j-1] + cost;
            matrix[i][j] = del < ins ? (del < sub ? del : sub) : (ins < sub ? ins : sub);
        }
    }

    return matrix[len1][len2];
}

const char* suggest(const char* unknown, const char** valid, int valid_count) {
    const char* best = NULL;
    int best_dist = 999;

    for (int i = 0; i < valid_count; i++) {
        int dist = levenshtein_distance(unknown, valid[i]);
        if (dist < best_dist) {
            best_dist = dist;
            best = valid[i];
        }
    }

    return best_dist <= 3 ? best : NULL;
}

int is_valid_version(const char* version) {
    char *endptr;
    int major = (int)strtol(version, &endptr, 10);
    if (major < 0 || *endptr != '.') return 0;

    int minor = (int)strtol(endptr + 1, &endptr, 10);
    if (minor < 0 || *endptr != '.') return 0;

    int patch = (int)strtol(endptr + 1, &endptr, 10);
    if (patch < 0 || *endptr != '\0') return 0;

    return 1;
}

const option_t* get_option(const command_t* command_data, const char* name) {
    for (int i = 0; i < command_data->option_count; i++) {
        const option_t* option = &command_data->options[i];
        if (strcmp(option->name, name) == 0) {
            return option;
        }
    }

    return NULL;
}

void get_dir_name(char* buffer, size_t buffer_size, const char* path) {
    // Get the last component of the path
    const char* last_slash = strrchr(path, '/');
    const char* name = last_slash ? last_slash + 1 : path;

    // Copy to buffer
    snprintf(buffer, buffer_size, "%s", name);
}

int search_dir_for_file(char* buffer, size_t buffer_size, const char* path, const char* filename) {

    // Open source directory
    dir_t* dir = open_dir(path);
    if (!dir) {
        return 0;
    }

    // Keep track of subdirectories to look into
    char sub_dirs[128][PATH_SIZE];
    int sub_dirs_count = 0;

    // Read only files in directory
    dir_entry_t entry;
    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // If entry is directory save to subdirectories list
        if (entry.is_dir) {
            snprintf(sub_dirs[sub_dirs_count], PATH_SIZE, "%s/%s", path, entry.name);
            sub_dirs_count++;
            continue;
        }

        // Check if file matches give file name
        if (strcmp(entry.name, filename) == 0) {
            snprintf(buffer, buffer_size, "%s/%s", path, entry.name);
            close_dir(dir);
            return 1;
        }
    }

    // Now recurse into subdirectories
    for (int i = 0; i < sub_dirs_count; i++) {
        if (search_dir_for_file(buffer, buffer_size, sub_dirs[i], filename)) {
            return 1;
        }
    }

    // File not found
    close_dir(dir);
    return 0;
}

void normalize_path(char* path) {
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == '\\') path[i] = '/';
    }
}