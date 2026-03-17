#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

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
    if (stat(path, &statbuff) == 0 && S_ISREG(statbuff.st_mode)) {
        // File exists
        return 1;
    }

    // File does not exist
    return 0;
}

// Check if a path exists and is a directory
int dir_exists(const char* path)
{
    struct stat statbuff;
    if (stat(path, &statbuff) == 0 && S_ISDIR(statbuff.st_mode)) {
        // Directory exists
        return 1;
    }
    
    // Directory does not exist
    return 0;
}

// Removes a directory recursivley, keeps track of number of files deleted, and how many bytes
int remove_dir_recursive(const char* path, unsigned int* count, unsigned int* bytes) {
    DIR* dir = opendir(path);
    if (!dir) {
        return -1;
    }

    struct dirent* entry;
    char buffer[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(buffer, sizeof(buffer), "%s/%s", path, entry->d_name);

        struct stat statbuff;
        stat(buffer, &statbuff);

        if (bytes) *bytes += statbuff.st_size;
        if (count) (*count)++;
        if (S_ISDIR(statbuff.st_mode)) {
            remove_dir_recursive(buffer, count, bytes);
        }
        else {
            remove(buffer);
        }
    }

    closedir(dir);
    return rmdir(path);
}

// Removes a directory without keeping track of counts
int remove_dir(const char* path) {
    return remove_dir_recursive(path, NULL, NULL);
}

// Removes a directory and keeps track of counts
int remove_dir_count(const char* path, unsigned int* count, unsigned int* bytes) {
    struct stat statbuff;
    stat(path, &statbuff);
    *bytes += statbuff.st_size;
    (*count)++;
    
    return remove_dir_recursive(path, count, bytes);
}

const char* get_base_name(const char* path)
{
    const char* last_slash = strrchr(path, '/');
    if (last_slash)
    {
        return last_slash + 1;
    }

    return path;
}

int create_cpp_project(const char* path, const char* name)
{
    // Make include directory if one does not exist
    char include_dir[256];
    snprintf(include_dir, sizeof(include_dir), "%s/include", path);
    if (!dir_exists(include_dir))
    {
        if (mkdir(include_dir, 0755) == 0)
        {
            fprintf(stdout, "Generated include directory\n");
        }
        else
        {
            fprintf(stderr, "Error: Failed to create include directory\n");
            return -1;
        }
    }

    // Make src directory if it does not exist
    char src_dir[256];
    snprintf(src_dir, sizeof(src_dir), "%s/src", path);
    if (!dir_exists(src_dir))
    {
        if (mkdir(src_dir, 0755) == 0)
        {
            fprintf(stdout, "Generated src directory\n");
        }
        else
        {
            fprintf(stderr, "Error: Failed to create src directory\n");
            return -1;
        }
    }

    // Make default main.cpp if there is none
    char main_file_path[256];
    snprintf(main_file_path, sizeof(main_file_path), "%s/src/main.cpp", path);

    if (!file_exists(main_file_path))
    {
        FILE* main_file = fopen(main_file_path, "w");
        if (main_file == NULL)
        {
            fprintf(stderr, "Error: Failed to generate main.cpp\n");
            return -1;
        }

        // Default source file contents if no corresponding header
        fprintf(main_file, "#include <iostream>\n\n");
        fprintf(main_file, "int main(int argc, char* argv[]) {\n");
        fprintf(main_file, "   std::cout << \"This is my %s project!\" << std::endl;\n", name);
        fprintf(main_file, "}");
        fprintf(stdout, "Generated main.cpp\n");
    }

    // Make default CMakeLists.txt if none exists
    char cmake_path[256];
    snprintf(cmake_path, sizeof(cmake_path), "%s/CMakeLists.txt", path);

    if (!file_exists(cmake_path))
    {
        // Project name
        char project_name[64];
        strcpy(project_name, name);
        project_name[0] = toupper(project_name[0]);

        // Executable name
        char executable_name[64];
        strcpy(executable_name, name);

        FILE* cmake_file = fopen(cmake_path, "w");
        if (cmake_file == NULL)
        {
            fprintf(stderr, "Error: Failed to generate CMakeLists.txt\n");
            return -1;
        }

        // Write default content
        fprintf(cmake_file, "cmake_minimum_required(VERSION 3.10)\n");
        fprintf(cmake_file, "project(%s)\n\n", project_name);
        fprintf(cmake_file, "set(CMAKE_CXX_STANDARD 17)\n\n");
        fprintf(cmake_file, "include_directories(include)\n\n");
        fprintf(cmake_file, "add_executable(%s src/main.cpp)", executable_name);
        fprintf(stdout, "Generated CMakeLists.txt\n");
    }

    return 0;
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

int copy_dir_contents(const char* source_dir, const char* dest_dir, const char** excludes, size_t exclude_count) {

    // Open source directory
    DIR* dir = opendir(source_dir);
    if (!dir) {
        fprintf(stderr, "Error: Failed to copy directory contents.\n");
        return -1;
    }

    // Read all entries in directory
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if file or dir should be excluded
        int should_exclude = 0;
        for (int i = 0; i < exclude_count; i++) {
            if (strcmp(entry->d_name, excludes[i]) == 0) {
                should_exclude = 1;
                break;
            }
        }
        if (should_exclude) continue;

        // Get the full path to source entry and destination entry
        char source_path[1024];
        char dest_path[1024];
        snprintf(source_path, sizeof(source_path), "%s/%s", source_dir, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

        struct stat statbuff;
        stat(source_path, &statbuff);

        // If entry is directory, copy contents recursively
        if (S_ISDIR(statbuff.st_mode)) {
            mkdir(dest_path, 0755);
            copy_dir_contents(source_path, dest_path, excludes, exclude_count);
        }
        // Entry is a file, copy contents to destination
        else {
            copy_file(source_path, dest_path);
        }
    }

    // Close source directory
    closedir(dir);
    return 0;
}

int copy_file(const char* source, const char* dest) {

    // Open files
    FILE* in = fopen(source, "rb");
    FILE* out = fopen(dest, "wb");
    if (!in || !out) {
        fprintf(stderr, "Error: Failed to copy file.\n");
        return -1;
    }

    // Read bits from file and write to destination file
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        fwrite(buffer, 1, bytes, out);
    }

    // Close files
    fclose(in);
    fclose(out);
    return 0;
}

int get_craft_home(char* buffer, size_t buffer_size) {
    char* craft_home = getenv("CRAFT_HOME");
    if (craft_home) {
        snprintf(buffer, buffer_size, "%s", craft_home);
        return 0;
    }

    char* user_home = getenv("HOME");
    if (user_home) {
        snprintf(buffer, buffer_size, "%s/.craft", user_home);
        return 0;
    }

    fprintf(stderr, "Error: Could not locate craft directory.\n");
    return -1;
}

int get_project_root(const char* cwd, char* buffer, size_t buffer_size) {
    char current_path[512];
    strncpy(current_path, cwd, sizeof(current_path));

    while (1) {
        char toml_path[512];
        snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", current_path);

        // Check if craft.toml file exists in this directory
        if (file_exists(toml_path)) {
            strncpy(buffer, current_path, buffer_size);
            return 0;
        }

        // Move up a directory
        char* last_slash = strrchr(current_path, '/');
        if (!last_slash || last_slash == current_path) {
            return -1;
        }
        
        *last_slash = '\0';
    }
}

int get_template_directory(char* buffer, size_t buffer_size, const char* type, const char* language, const char* name) {
    // Get the path to craft home
    char craft_home[512];
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
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
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
    int major, minor, patch;
    if (sscanf(version, "%d.%d.%d", &major, &minor, &patch) != 3)
        return 0;
    if (major < 0 || minor < 0 || patch < 0)
        return 0;
    // Make sure there's nothing extra after the patch number
    char expected[32];
    snprintf(expected, sizeof(expected), "%d.%d.%d", major, minor, patch);
    return strcmp(version, expected) == 0;
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

const int is_craft_project(const char* path) {
    // Check if craft.toml exists
    char toml_path[512];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", path);
    if (file_exists(toml_path)) {
        return 1;
    }

    return 0;
}