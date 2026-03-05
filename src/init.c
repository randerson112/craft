#include "init.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "project.h"
#include "config.h"

// Creates a new project in an existing directory relative to the current directory
int init_project_from_template(const char* path, const char* template, const char* lang) {
    char template_dir[256];

    // Get craft config directory
    char craft_home[256];
    get_craft_home(craft_home, sizeof(craft_home));

    // Check for template in custom templates
    snprintf(template_dir, sizeof(template_dir), "%s/templates/custom/%s/%s", craft_home, lang, template);
    if (!dirExists(template_dir)) {
        // Check for template in builtin templates
        snprintf(template_dir, sizeof(template_dir), "%s/templates/builtin/%s/%s", craft_home, lang, template);
        if (!dirExists(template_dir)) {
            // Template not found
            fprintf(stderr, "Template '%s' not found for language '%s'\n", template, lang);
            return -1;
        }
    }

    // Copy template contents to init path
    return copy_dir_contents(template_dir, path, NULL, 0);
}

// Gets the full path to directory to initialize from the cwd and relative path
int get_path_to_init(const char* cwd, const char* rel_path, char* buffer, size_t buffer_size) {
    // Get full path to specified directory
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, rel_path);

    // Remove . and .. from path
    char resolvedPath[256];
    if (realpath(fullPath, resolvedPath) == NULL) {
        fprintf(stderr, "Error: Failed to resolve path\n");
        return -1;
    }

    // Check if directory exists
    if (!dirExists(resolvedPath))
    {
        fprintf(stderr, "Error: Directory does not exist\n");
        return -1;
    }

    // Copy path to buffer
    snprintf(buffer, buffer_size, "%s", resolvedPath);
    return 0;
}

int init(command_t* command_data) {
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
    
    const char* rel_path = command_data->args[0];
    const char* language = global_config.language;
    const char* template = global_config.template;
    const char* build_type = NULL;

    // Get option arguments if specified
    for (int i = 0; i < command_data->option_count; i++) {
        const option_t* option = &command_data->options[i];
        if (strcmp(option->name, "template") == 0) {
            template = option->arg;
        }
        if (strcmp(option->name, "lang") == 0) {
            language = option->arg;
        }
        if (strcmp(option->name, "type") == 0) {
            build_type = option->arg;
        }
    }

    if (!build_type) {
        build_type = infer_build_type(template);
    }

    // Init a project at the given path
    char init_path[256];
    if (get_path_to_init(cwd, rel_path, init_path, sizeof(init_path)) != 0) {
        return -1;
    }

    if (init_project_from_template(init_path, template, language) != 0) {
        return -1;
    }

    // Populate project config with values
    project_config_t project_config;

    char project_name[32];
    get_project_name(init_path, project_name, sizeof(project_name));

    strncpy(project_config.name, project_name, sizeof(project_config.name));
    strncpy(project_config.version, "0.1.0", sizeof(project_config.version));
    strncpy(project_config.language, language, sizeof(project_config.language));
    project_config.standard = strcmp(language, "cpp") == 0 ? global_config.cpp_standard : global_config.c_standard;
    strncpy(project_config.build_type, build_type, sizeof(project_config.build_type));

    // Generate craft.toml
    return generate_craft_toml(init_path, &project_config);
}