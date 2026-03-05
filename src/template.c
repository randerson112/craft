#include "template.h"
#include <stdio.h>
#include "utils.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "config.h"

// Checks if a builtin template exists for a certain language
// Prevents the user from creating custom templates with same name as a builtin
int builtin_template_exists(const char* name, const char* language) {
    char builtin_path[512];
    get_template_directory(builtin_path, sizeof(builtin_path), "builtin", language, name);

    if (dirExists(builtin_path)) {
        return 1;
    }

    return 0;
}

// Saves the current project structure as a template with the given name
static int handle_save(command_t* command_data) {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Load global config for default language
    craft_config_t config;
    if (load_global_config(&config) != 0) {
        return -1;
    }

    const char* name = command_data->args[0];
    const char* language = config.language;

    // Check if there is a builtin template with same name
    if (builtin_template_exists(name, language)) {
        fprintf(stderr, "Naming Error: Builtin template '%s' already exists for language '%s'\n", name, language);
        return -1;
    }

    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
    }

    // Find project root
    char project_root[512];
    if (get_project_root(cwd, project_root, sizeof(project_root)) != 0) {
        return -1;
    }

    // Get the path to where the template will be saved
    char template_dir[512];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);

    // Copy project contents to template, excluding craft.toml and build
    mkdir(template_dir, 0755);
    const char* excludes[] = {"craft.toml", ".craft", "build"};
    return copy_dir_contents(project_root, template_dir, excludes, 3);
}

// Updates an existing template with the current project structure
static int handle_update(command_t* command_data) {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Load global config for default language
    craft_config_t config;
    if (load_global_config(&config) != 0) {
        return -1;
    }

    const char* name = command_data->args[0];
    const char* language = config.language;

    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
    }

    // Find project root
    char project_root[512];
    if (get_project_root(cwd, project_root, sizeof(project_root)) != 0) {
        return -1;
    }

    // Get the path to where the template is saved
    char template_dir[512];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);
    if (!dirExists(template_dir)) {
        fprintf(stderr, "[File Error]: '%s' custom template with the name '%s' does not exist\n", language, name);
        return -1;
    }

    // Delete old template contents and copy project contents to template, excluding craft.toml and build
    removeDir(template_dir);
    mkdir(template_dir, 0755);
    const char* excludes[] = {"craft.toml", ".craft", "build"};
    return copy_dir_contents(project_root, template_dir, excludes, 3);
}

// Deletes a template by name
static int handle_delete(command_t* command_data) {

    // Load global config for default language
    craft_config_t config;
    if (load_global_config(&config) != 0) {
        return -1;
    }

    const char* name = command_data->args[0];
    const char* language = config.language;

    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
    }

    // Get path to template
    char template_dir[512];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);
    if (!dirExists(template_dir)) {
        fprintf(stderr, "[File Error]: '%s' custom template with the name '%s' does not exist\n", language, name);
        return -1;
    }

    // Delete template contents
    return removeDir(template_dir);
}

static int handle_where(command_t* command_data) {
    const char* name = command_data->args[0];
    const char* language = "cpp";

    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
    }

    // Get the path to where the template is saved
    char template_dir[512];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);
    if (!dirExists(template_dir)) {
        fprintf(stderr, "[File Error]: '%s' custom template with the name '%s' does not exist\n", language, name);
        return -1;
    }

    // Print the path to the template
    fprintf(stdout, "%s\n", template_dir);
    return 0;
}

// Shows a listing of all builtin and custom templates
static int handle_list(command_t* command_data) {

    // Load global config for default language
    craft_config_t config;
    if (load_global_config(&config) != 0) {
        return -1;
    }

    const char* language = config.language;
    const char* other_language;
    int show_all = 0;

    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "lang") == 0) {
            language = command_data->options[i].arg;
        }
        if (strcmp(command_data->options[i].name, "all") == 0) {
            show_all = 1;
        }
    }

    other_language = strcmp(language, "cpp") == 0 ? "c" : "cpp";

    // Get the path to where the templates are saved
    char craft_home[512];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        return -1;
    }

    // Print builtin templates of specified language
    char builtin_templates_dir[512];
    get_template_directory(builtin_templates_dir, sizeof(builtin_templates_dir), "builtin", language, NULL);
    DIR* bi_dir = opendir(builtin_templates_dir);
    if (!bi_dir) {
        fprintf(stderr, "[Fatal Error]: Failed to open builtin templates for language '%s'\n", language);
        return -1;
    }

    fprintf(stdout, "Built-in templates (%s):\n", language);

    struct dirent* entry;
    while ((entry = readdir(bi_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        fprintf(stdout, "\t%s\n", entry->d_name);
    }
    fprintf(stdout, "\n");
    closedir(bi_dir);

    // Print builtin templates of other language if show all is on
    if (show_all) {
        get_template_directory(builtin_templates_dir, sizeof(builtin_templates_dir), "builtin", other_language, NULL);
        bi_dir = opendir(builtin_templates_dir);
        if (!bi_dir) {
            fprintf(stderr, "[Fatal Error]: Failed to open builtin templates for language '%s'\n", other_language);
            return -1;
        }

        fprintf(stdout, "Built-in templates (%s):\n", other_language);

        while ((entry = readdir(bi_dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            fprintf(stdout, "\t%s\n", entry->d_name);
        }
        fprintf(stdout, "\n");
        closedir(bi_dir);
    }

    // Print custom templates of specified language
    char custom_templates_dir[512];
    get_template_directory(custom_templates_dir, sizeof(custom_templates_dir), "custom", language, NULL);
    DIR* c_dir = opendir(custom_templates_dir);
    if (!c_dir) {
        fprintf(stderr, "[Fatal Error]: Failed to open custom templates for language '%s'\n", language);
        return -1;
    }

    fprintf(stdout, "Custom templates (%s):\n", language);

    while ((entry = readdir(c_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        fprintf(stdout, "\t%s\n", entry->d_name);
    }
    closedir(c_dir);

    // Print builtin templates of other language if show all is on
    if (show_all) {
        fprintf(stdout, "\n");
        get_template_directory(custom_templates_dir, sizeof(custom_templates_dir), "custom", other_language, NULL);

        c_dir = opendir(custom_templates_dir);
        if (!c_dir) {
            fprintf(stderr, "[Fatal Error]: Failed to open custom templates for language '%s'\n", other_language);
            return -1;
        }

        fprintf(stdout, "Custom templates (%s):\n", other_language);

        while ((entry = readdir(c_dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            fprintf(stdout, "\t%s\n", entry->d_name);
        }
        closedir(c_dir);
    }

    return 0;
}

int template_exists(const char* name, const char* language) {
    char builtin_path[512];
    char custom_path[512];
    get_template_directory(builtin_path, sizeof(builtin_path), "builtin", language, name);
    get_template_directory(custom_path, sizeof(custom_path), "custom", language, name);

    if (dirExists(builtin_path) || dirExists(custom_path)) {
        return 1;
    }

    return 0;
}

int handle_template(command_t* command_data) {
    const char* subcommand = command_data->subcommand;

    if (strcmp(subcommand, "save") == 0) {
        return handle_save(command_data);
    }
    else if (strcmp(subcommand, "update") == 0) {
        return handle_update(command_data);
    }
    else if (strcmp(subcommand, "delete") == 0) {
        return handle_delete(command_data);
    }
    else if (strcmp(subcommand, "where") == 0) {
        return handle_where(command_data);
    }
    else if (strcmp(subcommand, "list") == 0) {
        return handle_list(command_data);
    }

    return -1;
}