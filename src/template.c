#include "template.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"
#include "config.h"
#include "platform.h"

// Files and directories to exclude when saving a template
static const char* excludes[] = {".git", "gitignore", ".craft", "build", "CMakeLists.txt", "CMakeLists.extra.cmake"};
static size_t excludes_count = 6;

// Checks if a builtin template exists for a certain language
// Prevents the user from creating custom templates with same name as a builtin
int builtin_template_exists(const char* name, const char* language) {
    char builtin_path[PATH_SIZE];
    get_template_directory(builtin_path, sizeof(builtin_path), "builtin", language, name);

    if (dir_exists(builtin_path)) {
        return 1;
    }

    return 0;
}

// Saves the current project structure as a template with the given name
static int handle_save(const command_t* command_data) {
    // Retrive path of current working directory where craft is being called
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    // Find project root
    char project_root[PATH_SIZE];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "Error: Could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    // Load project config and make sure it is valid
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }

    if (validate_project_config(&config) != 0) {
        return -1;
    }

    const char* name = command_data->args[0];
    const char* language = config.language;

    // Check if there is a builtin template with same name
    if (builtin_template_exists(name, language)) {
        fprintf(stderr, "Error: Builtin template '%s' already exists for language '%s'\n", name, language);
        return -1;
    }

    // Check if there is a custom template already with that name
    if (template_exists(name, language)) {
        fprintf(stderr, "Error: Custom template '%s' already exists for language '%s'\n", name, language);
        return -1;
    }

    // Get the path to where the template will be saved
    char template_dir[PATH_SIZE];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);

    // Copy project contents to template, excluding craft.toml and build
    mkdir(template_dir, 0755);
    if (copy_dir_contents(project_root, template_dir, excludes, excludes_count) != 0) {
        return -1;
    }

    // Rewrite the craft.toml in the template without name and version
    config.name[0] = '\0';
    config.version[0] = '\0';
    generate_craft_toml(template_dir, &config);

    // Print success message
    fprintf(stdout, "Saved template '%s' to %s custom templates\n", name, language);

    return 0;
}

// Updates an existing template with the current project structure
static int handle_update(const command_t* command_data) {
    // Retrive path of current working directory where craft is being called
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    // Find project root
    char project_root[PATH_SIZE];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "Error: Could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    // Load project config and check if it is valid
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }

    if (validate_project_config(&config) != 0) {
        return -1;
    }

    const char* name = command_data->args[0];
    const char* language = config.language;

    // Get the path to where the template is saved
    char template_dir[PATH_SIZE];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);
    if (!dir_exists(template_dir)) {
        fprintf(stderr, "Error: '%s' custom template with the name '%s' does not exist\n", language, name);
        return -1;
    }

    // Delete old template contents and copy project contents to template, excluding .craft and build
    remove_dir(template_dir);
    mkdir(template_dir, 0755);
    if (copy_dir_contents(project_root, template_dir, excludes, excludes_count) != 0) {
        return -1;
    }

    // Rewrite the craft.toml in the template without name and version
    config.name[0] = '\0';
    config.version[0] = '\0';
    generate_craft_toml(template_dir, &config);

    // Print success message
    fprintf(stdout, "Updated %s custom template '%s'\n", language, name);

    return 0;
}

// Deletes a template by name
static int handle_delete(const command_t* command_data) {

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
    char template_dir[PATH_SIZE];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);
    if (!dir_exists(template_dir)) {
        fprintf(stderr, "Error: '%s' custom template with the name '%s' does not exist\n", language, name);
        return -1;
    }

    // Delete template contents
    remove_dir(template_dir);

    // Print success message
    fprintf(stdout, "Deleted %s custom template '%s'\n", language, name);
    return 0;
}

static int handle_where(const command_t* command_data) {

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

    // Get the path to where the template is saved
    char template_dir[PATH_SIZE];
    get_template_directory(template_dir, sizeof(template_dir), "custom", language, name);
    if (!dir_exists(template_dir)) {
        fprintf(stderr, "Error: '%s' custom template with the name '%s' does not exist\n", language, name);
        return -1;
    }

    // Print the path to the template
    fprintf(stdout, "%s\n", template_dir);
    return 0;
}

// Shows a listing of all builtin and custom templates
static int handle_list(const command_t* command_data) {

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
    char craft_home[PATH_SIZE];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        return -1;
    }

    // Print builtin templates of specified language
    char builtin_templates_dir[PATH_SIZE];
    get_template_directory(builtin_templates_dir, sizeof(builtin_templates_dir), "builtin", language, NULL);
    dir_t* bi_dir = open_dir(builtin_templates_dir);
    if (!bi_dir) {
        fprintf(stderr, "Error: Failed to open builtin templates for language '%s'\n", language);
        return -1;
    }

    fprintf(stdout, "Built-in templates (%s):\n", language);

    dir_entry_t entry;
    while (read_dir(bi_dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        fprintf(stdout, "\t%s\n", entry.name);
    }
    fprintf(stdout, "\n");
    close_dir(bi_dir);

    // Print builtin templates of other language if show all is on
    if (show_all) {
        get_template_directory(builtin_templates_dir, sizeof(builtin_templates_dir), "builtin", other_language, NULL);
        bi_dir = open_dir(builtin_templates_dir);
        if (!bi_dir) {
            fprintf(stderr, "Error: Failed to open builtin templates for language '%s'\n", other_language);
            return -1;
        }

        fprintf(stdout, "Built-in templates (%s):\n", other_language);

        while (read_dir(bi_dir, &entry)) {
            if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
                continue;
            }

            fprintf(stdout, "\t%s\n", entry.name);
        }
        fprintf(stdout, "\n");
        close_dir(bi_dir);
    }

    // Print custom templates of specified language
    char custom_templates_dir[PATH_SIZE];
    get_template_directory(custom_templates_dir, sizeof(custom_templates_dir), "custom", language, NULL);
    dir_t* c_dir = open_dir(custom_templates_dir);
    if (!c_dir) {
        fprintf(stderr, "Error: Failed to open custom templates for language '%s'\n", language);
        return -1;
    }

    fprintf(stdout, "Custom templates (%s):\n", language);

    while (read_dir(c_dir, &entry)) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
            continue;
        }

        fprintf(stdout, "\t%s\n", entry.name);
    }
    close_dir(c_dir);

    // Print builtin templates of other language if show all is on
    if (show_all) {
        fprintf(stdout, "\n");
        get_template_directory(custom_templates_dir, sizeof(custom_templates_dir), "custom", other_language, NULL);

        c_dir = open_dir(custom_templates_dir);
        if (!c_dir) {
            fprintf(stderr, "Error: Failed to open custom templates for language '%s'\n", other_language);
            return -1;
        }

        fprintf(stdout, "Custom templates (%s):\n", other_language);

        while (read_dir(c_dir, &entry)) {
            if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
                continue;
            }

            fprintf(stdout, "\t%s\n", entry.name);
        }
        close_dir(c_dir);
    }

    return 0;
}

int template_exists(const char* name, const char* language) {
    char builtin_path[PATH_SIZE];
    char custom_path[PATH_SIZE];
    get_template_directory(builtin_path, sizeof(builtin_path), "builtin", language, name);
    get_template_directory(custom_path, sizeof(custom_path), "custom", language, name);

    if (dir_exists(builtin_path) || dir_exists(custom_path)) {
        return 1;
    }

    return 0;
}

int handle_template(const command_t* command_data) {
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