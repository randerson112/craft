#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

void stripExtension(const char* file, char* strippedFile)
{
    int i = 0;
    while (file[i] != '\0' && file[i] != '.')
    {
        strippedFile[i] = file[i];
        i++;
    }
    strippedFile[i] = '\0';
}

void getExtension(const char* file, char* extension, int extensionSize)
{
    // Find where dot is
    char* dot = strrchr(file, '.');
    if (dot != NULL)
    {
        // Write contents after dot to extension buffer
        snprintf(extension, extensionSize, "%s", dot + 1);
    }
    else
    {
        // If no dot found then write null terminator
        *extension = '\0';
    }
}

// Check if a path exists and is a file
int fileExists(const char* path)
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
int dirExists(const char* path)
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
int removeDirRecursive(const char* path, unsigned int* count, unsigned int* bytes) {
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
            removeDirRecursive(buffer, count, bytes);
        }
        else {
            remove(buffer);
        }
    }

    closedir(dir);
    return rmdir(path);
}

// Removes a directory without keeping track of counts
int removeDir(const char* path) {
    return removeDirRecursive(path, NULL, NULL);
}

// Removes a directory and keeps track of counts
int removeDirCount(const char* path, unsigned int* count, unsigned int* bytes) {
    struct stat statbuff;
    stat(path, &statbuff);
    *bytes += statbuff.st_size;
    (*count)++;
    
    return removeDirRecursive(path, count, bytes);
}

const char* getBaseName(const char* path)
{
    const char* lastSlash = strrchr(path, '/');
    if (lastSlash)
    {
        return lastSlash + 1;
    }

    return path;
}

int createCppProject(const char* path, const char* name)
{
    // Make include directory if one does not exist
    char includeDir[256];
    snprintf(includeDir, sizeof(includeDir), "%s/include", path);
    if (!dirExists(includeDir))
    {
        if (mkdir(includeDir, 0755) == 0)
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
    char srcDir[256];
    snprintf(srcDir, sizeof(srcDir), "%s/src", path);
    if (!dirExists(srcDir))
    {
        if (mkdir(srcDir, 0755) == 0)
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
    char mainFilePath[256];
    snprintf(mainFilePath, sizeof(mainFilePath), "%s/src/main.cpp", path);

    if (!fileExists(mainFilePath))
    {
        FILE* mainFile = fopen(mainFilePath, "w");
        if (mainFile == NULL)
        {
            fprintf(stderr, "Error: Failed to generate main.cpp\n");
            return -1;
        }

        // Default source file contents if no corresponding header
        fprintf(mainFile, "#include <iostream>\n\n");
        fprintf(mainFile, "int main(int argc, char* argv[]) {\n");
        fprintf(mainFile, "   std::cout << \"This is my %s project!\" << std::endl;\n", name);
        fprintf(mainFile, "}");
        fprintf(stdout, "Generated main.cpp\n");
    }

    // Make default CMakeLists.txt if none exists
    char cmakePath[256];
    snprintf(cmakePath, sizeof(cmakePath), "%s/CMakeLists.txt", path);

    if (!fileExists(cmakePath))
    {
        // Project name
        char projectName[64];
        strcpy(projectName, name);
        projectName[0] = toupper(projectName[0]);

        // Executable name
        char executableName[64];
        strcpy(executableName, name);

        FILE* cmakeFile = fopen(cmakePath, "w");
        if (cmakeFile == NULL)
        {
            fprintf(stderr, "Error: Failed to generate CMakeLists.txt\n");
            return -1;
        }

        // Write default content
        fprintf(cmakeFile, "cmake_minimum_required(VERSION 3.10)\n");
        fprintf(cmakeFile, "project(%s)\n\n", projectName);
        fprintf(cmakeFile, "set(CMAKE_CXX_STANDARD 17)\n\n");
        fprintf(cmakeFile, "include_directories(include)\n\n");
        fprintf(cmakeFile, "add_executable(%s src/main.cpp)", executableName);
        fprintf(stdout, "Generated CMakeLists.txt\n");
    }

    return 0;
}

// Formats a number of bytes into a string with appropriate conversion
void formatBytes(unsigned int bytes, char* buffer, unsigned int bufferSize) {
    // Gigabytes
    if (bytes >= 1073741824) {
        snprintf(buffer, bufferSize, "%.1f GiB", bytes / 1073741824.0);
    }
    // Megabytes
    else if (bytes >= 1048576) {
        snprintf(buffer, bufferSize, "%.1f MiB", bytes / 1048576.0);
    }
    // Kilobytes
    else if (bytes >= 1024) {
        snprintf(buffer, bufferSize, "%.1f KiB", bytes / 1024.0);
    }
    // Bytes
    else {
        snprintf(buffer, bufferSize, "%u B", bytes);
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
        if (fileExists(toml_path)) {
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