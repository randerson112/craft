#include "project.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "utils.h"
#include "config.h"
#include "cmake.h"
#include "platform.h"

int create_project_from_template(const char* path, const char* template, const char* lang) {
    char template_dir[256];

    // Check for template in custom templates
    get_template_directory(template_dir, sizeof(template_dir), "custom", lang, template);
    if (!dir_exists(template_dir)) {
        // Check for template in builtin templates
        get_template_directory(template_dir, sizeof(template_dir), "builtin", lang, template);
        if (!dir_exists(template_dir)) {
            // Template not found
            fprintf(stderr, "Template '%s' not found for language '%s'\n", template, lang);
            return -1;
        }
    }

    // Copy template over to project directory
    copy_dir_contents(template_dir, path, NULL, 0);

    // Load the project config from the craft.toml and set the project name and version
    project_config_t project_config;
    if (load_project_config(&project_config, path) != 0) {
        return -1;
    }

    char project_name[32];
    get_project_name(project_name, sizeof(project_name), path);
    strncpy(project_config.name, project_name, sizeof(project_config.name));
    strncpy(project_config.version, "0.1.0", sizeof(project_config.version));

    // Make sure project configs from the template are valid
    if (validate_project_config(&project_config) != 0) {
        return -1;
    }

    // Generate a new craft.toml with name and version
    if (generate_craft_toml(path, &project_config) != 0) {
        return -1;
    }

    // Generate starting CMakeLists.txt based on configs
    return generate_cmake(path, &project_config);

    // Generate .craft directory with deps directory
    char craft_directory[512];
    char craft_deps_directory[512];
    snprintf(craft_directory, sizeof(craft_directory), "%s/.craft", path);
    snprintf(craft_deps_directory, sizeof(craft_deps_directory), "%s/deps", craft_directory);

    mkdir(craft_directory, 0755);
    mkdir(craft_deps_directory, 0755);
}

// Gets the full path to the new project directory from the cwd and relative path
int get_project_path(const char* cwd, const char* rel_path, char* buffer)
{
    // Check if directory at path already exists
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s/%s", cwd, rel_path);

    if (dir_exists(full_path))
    {
        fprintf(stderr, "Error: Directory with that name already exists\n");
        return -1;
    }

    strcpy(buffer, full_path);
    return 0;
}

void get_project_name(char* buffer, size_t buffer_size, const char* path) {
    // Get the last component of the path
    const char* last_slash = strrchr(path, '/');
    const char* name = last_slash ? last_slash + 1 : path;

    // Copy to buffer
    strncpy(buffer, name, buffer_size);
}

int handle_project(const command_t* command_data) {
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

    const char* rel_project_path = command_data->args[0];
    const char* language = global_config.language;
    const char* template = global_config.template;

    // Override defaults with option arguments
    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "template") == 0) {
            template = command_data->options[i].arg;
        }
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
    }

    // Get path to project
    char project_path[256];
    if (get_project_path(cwd, rel_project_path, project_path) != 0) { 
        return -1;
    }

    // Create new directory and create the project
    mkdir(project_path, 0755);
    return create_project_from_template(project_path, template, language);
}