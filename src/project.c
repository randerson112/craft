#include "project.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "utils.h"
#include "config.h"

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
    return copy_dir_contents(template_dir, path, NULL, 0);
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

void get_project_name(const char* path, char* buffer, size_t buffer_size) {
    // Get the last component of the path
    const char* last_slash = strrchr(path, '/');
    const char* name = last_slash ? last_slash + 1 : path;

    // Copy and capitalize first letter
    strncpy(buffer, name, buffer_size);
    if (buffer[0] != '\0')
        buffer[0] = toupper(buffer[0]);
}

const char* infer_build_type(const char* template) {
    if (strcmp(template, "executable") == 0)   return "executable";
    if (strcmp(template, "static-library") == 0) return "static-library";
    if (strcmp(template, "shared-library") == 0) return "shared-library";
    if (strcmp(template, "header-only") == 0)  return "header-only";
    return "executable";  // default for custom templates
}

int project(command_t* command_data) {
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
    const char* build_type = NULL;

    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "template") == 0) {
            template = command_data->options[i].arg;
        }
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
        if (strcmp(command_data->options[i].name, "type") == 0) {
            build_type = command_data->options[i].arg;
        }
    }

    if (!build_type) {
        build_type = infer_build_type(template);
    }

    // Create project files using template
    char project_path[256];
    if (get_project_path(cwd, rel_project_path, project_path) != 0) { 
        return -1;
    }

    if (create_project_from_template(project_path, template, language) != 0) {
        return -1;
    }

    // Populate project config with values
    project_config_t project_config;

    char project_name[32];
    get_project_name(rel_project_path, project_name, sizeof(project_name));
    strncpy(project_config.name, project_name, sizeof(project_config.name));
    strncpy(project_config.version, "0.1.0", sizeof(project_config.version));
    strncpy(project_config.language, language, sizeof(project_config.language));
    project_config.standard = strcmp(language, "cpp") == 0 ? global_config.cpp_standard : global_config.c_standard;
    strncpy(project_config.build_type, build_type, sizeof(project_config.build_type));

    // Generate craft.toml
    return generate_craft_toml(project_path, &project_config);
}