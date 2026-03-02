#include "init.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

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
    return copy_dir_contents(template_dir, path);
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

int init(command_t* command_data, const char* cwd) {
    const char* rel_path = command_data->args[0];

    // Default option arguments
    const char* template = "executable";
    const char* language = "cpp";

    // Get option arguments if specified
    for (int i = 0; i < command_data->option_count; i++) {
        const option_t* option = &command_data->options[i];
        if (strcmp(option->name, "template") == 0) {
            template = option->arg;
        }
        if (strcmp(option->name, "lang") == 0) {
            language = option->arg;
        }
    }

    // Init a project at the given path
    char init_path[256];
    if (get_path_to_init(cwd, rel_path, init_path, sizeof(init_path)) == 0) {
        return init_project_from_template(init_path, template, language);
    }

    fprintf(stderr, "Error: Failed to initialize project\n");
    return -1;
}