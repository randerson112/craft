#include "cmake.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int cmake_needs_regeneration(const char* project_root) {
    char cmake_path[1024];
    char toml_path[1024];
    snprintf(cmake_path, sizeof(cmake_path), "%s/CMakeLists.txt", project_root);
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", project_root);

    struct stat cmake_stat, toml_stat;
    
    // If CMakeLists.txt doesn't exist, always regenerate
    if (stat(cmake_path, &cmake_stat) != 0)
        return 1;
    
    // If craft.toml doesn't exist something is wrong
    if (stat(toml_path, &toml_stat) != 0)
        return 0;

    // Regenerate if craft.toml is newer
    return toml_stat.st_mtime > cmake_stat.st_mtime;
}

int generate_cmake(const char* project_path, project_config_t* config) {
    char cmake_path[1024];
    snprintf(cmake_path, sizeof(cmake_path), "%s/CMakeLists.txt", project_path);

    FILE* file = fopen(cmake_path, "w");
    if (!file) {
        fprintf(stderr, "Error: Filed to create CMakeLists.txt\n");
        return -1;
    }

    // Header
    fprintf(file, "cmake_minimum_required(VERSION 3.10)\n");
    fprintf(file, "project(%s)\n\n", config->name);

    // Standards
    if (config->c_standard != 0)
        fprintf(file, "set(CMAKE_C_STANDARD %d)\n", config->c_standard);
    if (config->cpp_standard != 0)
        fprintf(file, "set(CMAKE_CXX_STANDARD %d)\n", config->cpp_standard);
    fprintf(file, "\n");

    // Source glob
    fprintf(file, "file(GLOB_RECURSE SOURCES");
    for (int i = 0; i < config->source_dir_count; i++) {
        if (strcmp(config->language, "c") == 0 || config->c_standard != 0)
            fprintf(file, " \"%s/*.c\"", config->source_dirs[i]);
        if (strcmp(config->language, "cpp") == 0 || config->cpp_standard != 0)
            fprintf(file, " \"%s/*.cpp\"", config->source_dirs[i]);
    }
    if (config->source_dir_count == 0) {
        if (strcmp(config->language, "c") == 0 || config->c_standard != 0)
            fprintf(file, " \"src/*.c\"");
        if (strcmp(config->language, "cpp") == 0 || config->cpp_standard != 0)
            fprintf(file, " \"src/*.cpp\"");
    }
    fprintf(file, ")\n\n");

    // Build target
    if (strcmp(config->build_type, "executable") == 0)
        fprintf(file, "add_executable(%s ${SOURCES})\n\n", config->name);
    else if (strcmp(config->build_type, "static-library") == 0)
        fprintf(file, "add_library(%s STATIC ${SOURCES})\n\n", config->name);
    else if (strcmp(config->build_type, "shared-library") == 0)
        fprintf(file, "add_library(%s SHARED ${SOURCES})\n\n", config->name);
    else if (strcmp(config->build_type, "header-only") == 0)
        fprintf(file, "add_library(%s INTERFACE)\n\n", config->name);

    // Include directories
    if (config->include_dir_count > 0) {
        const char* visibility = strcmp(config->build_type, "header-only") == 0 ? "INTERFACE" : "PRIVATE";
        fprintf(file, "target_include_directories(%s %s", config->name, visibility);
        for (int i = 0; i < config->include_dir_count; i++)
            fprintf(file, " %s", config->include_dirs[i]);
        if (config->include_dir_count == 0) {
            fprintf(file, "target_include_directories(%s %s include", config->name, visibility);
        }
        fprintf(file, ")\n\n");
    }

    // Library directories
    if (config->lib_dir_count > 0) {
        fprintf(file, "target_link_directories(%s PRIVATE", config->name);
        for (int i = 0; i < config->lib_dir_count; i++)
            fprintf(file, " ${CMAKE_SOURCE_DIR}/%s", config->lib_dirs[i]);
        fprintf(file, ")\n\n");
    }

    // Libraries
    if (config->lib_count > 0) {
        fprintf(file, "target_link_libraries(%s PRIVATE", config->name);
        for (int i = 0; i < config->lib_count; i++)
            fprintf(file, " %s", config->libs[i]);
        fprintf(file, ")\n\n");
    }

    // Escape hatch
    fprintf(file, "if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeLists.extra.cmake)\n");
    fprintf(file, "    include(${CMAKE_SOURCE_DIR}/CMakeLists.extra.cmake)\n");
    fprintf(file, "endif()\n");

    fclose(file);
    return 0;
}