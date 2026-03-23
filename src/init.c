#include "init.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "project.h"
#include "config.h"
#include "cmake.h"
#include "platform.h"

// Checks if a Craft project exists at the path
int project_exists(const char* path) {
    char toml_path[512];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", path);

    if (file_exists(toml_path)) {
        return 1;
    }

    return 0;
}

// Gets the full path to directory to initialize from the cwd and relative path
int get_path_to_init(char* buffer, size_t buffer_size, const char* cwd, const char* rel_path) {
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

int handle_init(const command_t* command_data) {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
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
        fprintf(stderr, "Error: Craft project already exists at %s\n", rel_path);
        return -1;
    }

    // Create the project
    return create_project_from_template(init_path, template, language);
}