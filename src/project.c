#include "project.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "utils.h"

// Creates a project by copying template structure to project path
int create_project_from_template(const char* path, const char* template, const char* lang) {
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

    // Make new project directory and copy template over
    mkdir(path, 0755);
    return copy_dir_contents(template_dir, path);
}

// Gets the full path to the new project directory from the cwd and relative path
int get_project_path(const char* cwd, const char* rel_path, char* buffer)
{
    // Check if directory at path already exists
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, rel_path);

    if (dirExists(fullPath))
    {
        fprintf(stderr, "Error: Directory with that name already exists\n");
        return -1;
    }

    strcpy(buffer, fullPath);
    return 0;
}

int project(command_t* command_data, const char* cwd) {
    const char* rel_project_path = command_data->args[0];
    const char* template = "executable";
    const char* language = "cpp";

    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "template") == 0) {
            template = command_data->options[i].arg;
        }
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
    }

    char project_path[256];
    if (get_project_path(cwd, rel_project_path, project_path) == 0) { 
        return create_project_from_template(project_path, template, language);
    }
    
    return -1;
}