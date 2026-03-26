#include "init.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "utils.h"
#include "project.h"
#include "config.h"
#include "cmake.h"
#include "platform.h"

// Checks if a Craft project exists at the path
static int project_exists(const char* path) {
    char toml_path[512];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", path);

    return file_exists(toml_path);
}

// Gets the full path to directory to initialize from the cwd and relative path
static int get_path_to_init(char* buffer, size_t buffer_size, const char* cwd, const char* rel_path) {
    // Get full path to specified directory
    char full_path[256];
    if (rel_path) {
        snprintf(full_path, sizeof(full_path), "%s/%s", cwd, rel_path);
    }
    else {
        snprintf(full_path, sizeof(full_path), "%s", cwd);
    }

    // Make sure directory exists
    if (!dir_exists(full_path))
    {
        fprintf(stderr, "Error: Directory does not exist\n");
        return -1;
    }

    // Copy path to buffer
    snprintf(buffer, buffer_size, "%s", full_path);
    return 0;
}

// Checks if directory at the path is empty
static int dir_is_empty(const char* path) {

    // Open directory for reading
    dir_t* dir = open_dir(path);
    if (!dir) {
        return 1;
    }

    // If only entries are . and .. the directory is empty
    dir_entry_t entry;
    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // Something is in directory
        close_dir(dir);
        return 0;
    }

    // Empty
    close_dir(dir);
    return 1;
}

// Detects language by recursively scanning directory and seeing what most of the source files are
static void get_language_counts(const char* path, int* cpp_count, int* c_count) {

    // Open directory for reading
    dir_t* dir = open_dir(path);
    if (!dir) {
        return;
    }

    // Read directory entries
    dir_entry_t entry;
    while(read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // Get path to current entry
        char full_path[PATH_SIZE];
        snprintf(full_path, PATH_SIZE, "%s/%s", path, entry.name);
        
        // If directory, recurse into directory
        if (entry.is_dir) {
            // Skip .craft and build directories
            if (strcmp(entry.name, "build") == 0) {
                continue;
            }
            get_language_counts(full_path, cpp_count, c_count);
        }

        // If file, check extension
        else {
            char extension[8];
            get_extension(entry.name, extension, sizeof(extension));
            if (strcmp(extension, "cpp") == 0 || strcmp(extension, "cc") == 0 || strcmp(extension, "cxx") == 0) {
                (*cpp_count)++;
            }
            else if (strcmp(extension, "c") == 0) {
                (*c_count)++;
            }
        }
    }

    close_dir(dir);
}

// Detects which directories have source files in them and adds them to config source dirs
static void detect_source_dirs(const char* path, project_config_t* config) {

    // Open directory for reading
    dir_t* dir = open_dir(path);
    if (!dir) {
        return;
    }

    // Read entries in directory
    dir_entry_t entry;
    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // Skip if entry is not a directory
        if (!entry.is_dir) {
            continue;
        }

        // Skip .craft and build directories
        if (strcmp(entry.name, "build") == 0) {
            continue;
        }

        char full_path[PATH_SIZE];
        snprintf(full_path, PATH_SIZE, "%s/%s", path, entry.name);

        // Read entries of subdirectory
        dir_t* sub_dir = open_dir(full_path);
        if (!sub_dir) {
            return;
        }

        // Read entries in sub-directory and check if it contains source files
        int has_sources = 0;

        dir_entry_t sub_entry;
        while(read_dir(sub_dir, &sub_entry)) {
            char extension[8];
            get_extension(sub_entry.name, extension, sizeof(extension));
            if (strcmp(extension, "cpp") == 0 || strcmp(extension, "cc") == 0 ||
                strcmp(extension, "cxx") == 0 || strcmp(extension, "c") == 0) {
                has_sources = 1;
                break;
            }
        }

        close_dir(sub_dir);

        // If directory contains sources, add to config source directories
        if (has_sources) {
            int count = config->source_dir_count;
            snprintf(config->source_dirs[count], sizeof(config->source_dirs[count]), "%s", entry.name);
            config->source_dir_count++;
        }
    }

    close_dir(dir);
}

// Detects which directories have header files in them and adds them to config include dirs
static void detect_include_dirs(const char* path, project_config_t* config) {

    // Open directory for reading
    dir_t* dir = open_dir(path);
    if (!dir) {
        return;
    }

    // Read entries in directory
    dir_entry_t entry;
    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // Skip if entry is not a directory
        if (!entry.is_dir) {
            continue;
        }

        // Skip .craft and build directories
        if (strcmp(entry.name, "build") == 0) {
            continue;
        }

        char full_path[PATH_SIZE];
        snprintf(full_path, PATH_SIZE, "%s/%s", path, entry.name);

        // Read entries of subdirectory
        dir_t* sub_dir = open_dir(full_path);
        if (!sub_dir) {
            return;
        }

        // Read entries in sub-directory and check if it contains header files
        int has_headers = 0;

        dir_entry_t sub_entry;
        while(read_dir(sub_dir, &sub_entry)) {
            char extension[8];
            get_extension(sub_entry.name, extension, sizeof(extension));
            if (strcmp(extension, "hpp") == 0 || strcmp(extension, "h") == 0) {
                has_headers = 1;
                break;
            }
        }

        close_dir(sub_dir);

        // If directory contains headers, add to config include directories
        if (has_headers) {
            int count = config->include_dir_count;
            snprintf(config->include_dirs[count], sizeof(config->include_dirs[count]), "%s", entry.name);
            config->include_dir_count++;
        }
    }

    close_dir(dir);
}

// Detects which directories have library files in them and adds them to config lib dirs
static void detect_libs(const char* path, project_config_t* config) {

    // Open directory for reading
    dir_t* dir = open_dir(path);
    if (!dir) {
        return;
    }

    // Read entries in directory
    dir_entry_t entry;
    while (read_dir(dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        // Skip if entry is not a directory
        if (!entry.is_dir) {
            continue;
        }

        // Skip .craft and build directories
        if (strcmp(entry.name, "build") == 0) {
            continue;
        }

        // Read entries of subdirectory
        char full_path[PATH_SIZE];
        snprintf(full_path, PATH_SIZE, "%s/%s", path, entry.name);

        dir_t* sub_dir = open_dir(full_path);
        if (!sub_dir) {
            return;
        }

        dir_entry_t sub_entry;
        while (read_dir(sub_dir, &sub_entry)) {
            char extension[8];
            get_extension(sub_entry.name, extension, sizeof(extension));
            if (strcmp(extension, "a")     == 0 ||
                strcmp(extension, "dylib") == 0 ||
                strcmp(extension, "so")    == 0 ||
                strcmp(extension, "lib")   == 0 ||
                strcmp(extension, "dll")   == 0) {
                
                // Add lib dir if not already added
                int already_added = 0;
                for (int i = 0; i < config->lib_dir_count; i++) {
                    if (strcmp(config->lib_dirs[i], entry.name) == 0) {
                        already_added = 1;
                        break;
                    }
                }

                if (!already_added && config->lib_dir_count < 8) {
                    int count = config->lib_dir_count;
                    snprintf(config->lib_dirs[count], sizeof(config->lib_dirs[count]), "%s", entry.name);
                    config->lib_dir_count++;
                }

                // Strip lib prefix and extension to get library name
                char lib_name[256];
                snprintf(lib_name, sizeof(lib_name), "%s", sub_entry.name);

                // Strip extension
                char* dot = strrchr(lib_name, '.');
                if (dot) *dot = '\0';

                // Strip lib prefix
                char* name = lib_name;
                if (strncmp(lib_name, "lib", 3) == 0) {
                    name = lib_name + 3;
                }

                snprintf(config->libs[config->lib_count++], sizeof(config->libs[0]), "%s", name);
            }
        }

        close_dir(sub_dir);
    }

    close_dir(dir);
}

// Initializes a craft project structure in a directory with existing files
static int init_existing_project(const char* path, const char* language_option) {

    // Load global defaults for fallbacks
    craft_config_t global_config;
    if (load_global_config(&global_config) != 0) {
        return -1;
    }

    // Build config from detected structure
    project_config_t config = {0};

    // Get project name
    char project_name[32];
    get_dir_name(project_name, sizeof(project_name), path);
    snprintf(config.name, sizeof(config.name), "%s", project_name);

    // Write default version
    snprintf(config.version, sizeof(config.version), "0.1.0");

    // Get language
    if (language_option) {
        snprintf(config.language, sizeof(config.language), "%s", language_option);
    }
    else {
        int cpp_count = 0;
        int c_count = 0;
        get_language_counts(path, &cpp_count, &c_count);
        if (cpp_count > 0 || c_count > 0) {
            const char* language = cpp_count >= c_count ? "cpp" : "c";
            snprintf(config.language, sizeof(config.language), "%s", language);
        }
        else {
            snprintf(config.language, sizeof(config.language), "%s", global_config.language);
        }
    }

    // Get standard
    if (strcmp(config.language, "cpp") == 0) {
        config.cpp_standard = global_config.cpp_standard;
        config.has_cpp_standard = 1;
    }
    else {
        config.c_standard = global_config.c_standard;
        config.has_c_standard = 1;
    }

    // Set build type
    snprintf(config.build_type, sizeof(config.build_type), "%s", global_config.template);

    // Get include and source directories
    detect_include_dirs(path, &config);
    detect_source_dirs(path, &config);

    if (config.include_dir_count == 0) {
        snprintf(config.include_dirs[0], sizeof(config.include_dirs[0]), "%s", "include");
        config.include_dir_count++;
    }
    if (config.source_dir_count == 0) {
        snprintf(config.source_dirs[0], sizeof(config.source_dirs[0]), "%s", "src");
        config.source_dir_count++;
    }

    // Get library directories and library names
    detect_libs(path, &config);

    // Backup existing CMakeLists.txt if present
    backup_cmake(path);

    // Generate craft.toml and CMakeLists.txt
    if (generate_craft_toml(path, &config) != 0) {
        return -1;
    }
    if (generate_cmake(path, &config) != 0) {
        return -1;
    }

    // Generate .craft directory with deps directory
    char craft_directory[512];
    char craft_deps_directory[512];
    snprintf(craft_directory, sizeof(craft_directory), "%s/.craft", path);
    snprintf(craft_deps_directory, sizeof(craft_deps_directory), "%s/deps", craft_directory);

    mkdir(craft_directory, 0755);
    mkdir(craft_deps_directory, 0755);

    // Print success message
    fprintf(stdout, "Initialized Craft project at '%s'\n\n", path);
    fprintf(stdout, "Edit craft.toml to adjust the configuration.\n\n");

    char backup_path[PATH_SIZE];
    snprintf(backup_path, PATH_SIZE, "%s/CMakeLists.backup.cmake", path);

    if (file_exists(backup_path)) {
        fprintf(stdout, "Your existing CMakeLists.txt was backed up to CMakeLists.backup.cmake\n\n");
    }

    fprintf(stdout, "Run 'craft build' to build your project.\n");
    return 0;
}

int handle_init(const command_t* command_data) {

    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Load global configs and set defaults
    craft_config_t global_config;
    load_global_config(&global_config);
    
    const char* language = global_config.language;
    const char* template = global_config.template;

    // Override defaults with option arguments if specified
    for (int i = 0; i < command_data->option_count; i++) {
        const option_t* option = &command_data->options[i];
        if (strcmp(option->name, "template") == 0) {
            template = option->arg;
        }
        if (strcmp(option->name, "lang") == 0) {
            language = option->arg;
        }
    }

    const char* rel_path = NULL;
    if (command_data->arg_count != 0) {
        rel_path = command_data->args[0];
    }

    // Get path to where project is being initialized
    char init_path[256];
    if (get_path_to_init(init_path, sizeof(init_path), cwd, rel_path) != 0) {
        return -1;
    }

    // Check if a project already exists at the init path
    if (project_exists(init_path)) {
        fprintf(stderr, "Error: Craft project already exists at '%s'\n", init_path);
        return -1;
    }

    // If init directory is empty, create a new project
    if (dir_is_empty(init_path)) {
        return create_project_from_template(init_path, template, language);
    }

    // Otherwise init from existing project structure
    const char* language_option = NULL;
    for (int i = 0; i < command_data->option_count; i++) {
        const option_t* option = &command_data->options[i];
        if (strcmp(option->name, "lang") == 0) {
            language_option = option->arg;
        }
    }
    return init_existing_project(init_path, language_option);
}