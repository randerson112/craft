#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "template.h"
#include "utils.h"
#include "platform.h"

// Fallback global config values if config.toml is missing or corrupted
static const craft_config_t defaults = {
    .language = "cpp",
    .c_standard = 99,
    .cpp_standard = 17,
    .template = "executable"
};

int load_global_config(craft_config_t* config) {

    // Set all values to 0
    memset(config, 0, sizeof(*config));

    // Set defaults
    snprintf(config->language, sizeof(config->language), "%s", defaults.language);
    config->c_standard = defaults.c_standard;
    config->cpp_standard = defaults.cpp_standard;
    snprintf(config->template, sizeof(config->template), "%s", defaults.template);

    // Get path to config.toml
    char craft_home[PATH_SIZE];
    char config_path[PATH_SIZE];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        return -1;
    }
    snprintf(config_path, sizeof(config_path), "%s/config.toml", craft_home);

    // Open config.toml and parse
    FILE* file = fopen(config_path, "r");
    if (!file) {
        return 0;
        // Just use default values
    }

    toml_result_t result = toml_parse_file(file);

    if (!result.ok) {
        fprintf(stderr, "Error: Failed to parse global config file\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return -1;
    }

    // Get config values if valid
    toml_datum_t language = toml_seek(result.toptab, "defaults.language");
    toml_datum_t c_standard = toml_seek(result.toptab, "defaults.c_standard");
    toml_datum_t cpp_standard = toml_seek(result.toptab, "defaults.cpp_standard");
    toml_datum_t template = toml_seek(result.toptab, "defaults.template");

    if (language.type == TOML_STRING) {
        snprintf(config->language, sizeof(config->language), "%s", language.u.s);
    }
    if (c_standard.type == TOML_INT64) {
        config->c_standard = (int)c_standard.u.int64;
    }
    if (cpp_standard.type == TOML_INT64) {
        config->cpp_standard = (int)cpp_standard.u.int64;
    }
    if (template.type == TOML_STRING) {
        snprintf(config->template, sizeof(config->template), "%s", template.u.s);
    }

    toml_free(result);
    return 0;
}

// Saves the entries in the craft config struct to the global config.toml
static int save_global_config(craft_config_t* config) {

    // Get path to config.toml
    char craft_home[PATH_SIZE];
    char config_path[PATH_SIZE];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        return -1;
    }
    snprintf(config_path, sizeof(config_path), "%s/config.toml", craft_home);

    // Open config.toml for writing and write config data
    FILE* f = fopen(config_path, "w");
    if (!f) {
        fprintf(stderr, "Error: Failed to write to config file\n");
        return -1;
    }

    fprintf(f, "[defaults]\n");
    fprintf(f, "language = \"%s\"\n", config->language);
    fprintf(f, "c_standard = %d\n", config->c_standard);
    fprintf(f, "cpp_standard = %d\n", config->cpp_standard);
    fprintf(f, "template = \"%s\"\n", config->template);

    fclose(f);
    return 0;
}

// Subcommand handler for config set to set a global config value
static int handle_set(const command_t* command_data) {

    // Load config values
    craft_config_t config;
    load_global_config(&config);

    // Get command arguments
    const char* lookup = command_data->args[0];
    const char* new_value = command_data->args[1];

    // Seperate section and key values
    char* dot = strchr(lookup, '.');
    if (!dot) {
        fprintf(stderr, "Error: '%s' is not a valid config key\n", lookup);
        return -1;
    }

    char section[32];
    char key[32];
    snprintf(section, sizeof(section), "%.*s", (int)(dot - lookup), lookup);
    snprintf(key, sizeof(key), "%s", dot + 1);

    // Default language
    if (strcmp(lookup, "defaults.language") == 0) {
        if (strcmp(new_value, "c") != 0 && strcmp(new_value, "cpp") != 0) {
            fprintf(stderr, "Error: Invalid language '%s', must be 'c' or 'cpp'\n", new_value);
            return -1;
        }
        if (!template_exists(config.template, new_value)) {
            fprintf(stdout, "Warning: Current default template '%s' does not exist for language '%s'\n", config.template, new_value);
        }
        snprintf(config.language, sizeof(config.language), "%s", new_value);
    }

    // Default C standard
    else if (strcmp(lookup, "defaults.c_standard") == 0) {
        int standard = atoi(new_value);
        if (standard != 89 && standard != 99 && standard != 11 && standard != 17 && standard != 23) {
            fprintf(stderr, "Error: Invalid C standard '%s'\n", new_value);
            return -1;
        }
        config.c_standard = standard;
    }

    // Default C++ standard
    else if (strcmp(lookup, "defaults.cpp_standard") == 0) {
        int standard = atoi(new_value);
        if (standard != 11 && standard != 14 && standard != 17 && standard != 20 && standard != 23) {
            fprintf(stderr, "Error: Invalid C++ standard '%s'\n", new_value);
            return -1;
        }
        config.cpp_standard = standard;
    }

    // Default template
    else if (strcmp(lookup, "defaults.template") == 0) {
        if (!template_exists(new_value, config.language)) {
            fprintf(stdout, "Warning: Template '%s' does not currently exist for default language '%s'\n", new_value, config.language);
        }
        snprintf(config.template, sizeof(config.template), "%s", new_value);
    }

    // Not a valid key
    else {
        fprintf(stderr, "Error: '%s' is not a valid config key\n", lookup);
        return -1;
    }

    // Save new config
    if (save_global_config(&config) != 0)
        return -1;

    if (strcmp(lookup, "defaults.c_standard") == 0 || strcmp(lookup, "defaults.cpp_standard") == 0) {
        fprintf(stdout, "Set %s = %s\n", lookup, new_value);
        fprintf(stdout, "\n[%s]\n", section);
        fprintf(stdout, "%s = %s\n", key, new_value);
    } else {
        fprintf(stdout, "Set %s = \"%s\"\n", lookup, new_value);
        fprintf(stdout, "\n[%s]\n", section);
        fprintf(stdout, "%s = \"%s\"\n", key, new_value);
    }

    return 0;
}

// Subcommand handler for config get to read a global config value
static int handle_get(const command_t* command_data) {

    // Get config key argument
    const char* lookup = command_data->args[0];

    // Seperate section and key values
    char* dot = strchr(lookup, '.');
    if (!dot) {
        fprintf(stderr, "Error: '%s' is not a valid config key\n", lookup);
        return -1;
    }

    char section[32];
    char key[32];
    snprintf(section, sizeof(section), "%.*s", (int)(dot - lookup), lookup);
    snprintf(key, sizeof(key), "%s", dot + 1);

    // Get global configs
    craft_config_t config;
    if (load_global_config(&config) != 0) {
        return -1;
    }

    // Print key value if it is a valid key
    if (strcmp(lookup, "defaults.language") == 0) {
        fprintf(stdout, "[%s]\n", section);
        fprintf(stdout, "%s = \"%s\"\n", key, config.language);
    }
    else if (strcmp(lookup, "defaults.c_standard") == 0) {
        fprintf(stdout, "[%s]\n", section);
        fprintf(stdout, "%s = %d\n", key, config.c_standard);
    }
    else if (strcmp(lookup, "defaults.cpp_standard") == 0) {
        fprintf(stdout, "[%s]\n", section);
        fprintf(stdout, "%s = %d\n", key, config.cpp_standard);
    }
    else if (strcmp(lookup, "defaults.template") == 0) {
        fprintf(stdout, "[%s]\n", section);
        fprintf(stdout, "%s = \"%s\"\n", key, config.template);
    }
    else {
        fprintf(stderr, "Error: '%s' is not a valid config key\n", lookup);
        return -1;
    }

    return 0;
}

// Subcommand handler for config list, lists all the configs in config.toml
static int handle_list() {

    // Get path to config.toml
    char craft_home[PATH_SIZE];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0)
        return -1;

    char config_path[PATH_SIZE];
    snprintf(config_path, sizeof(config_path), "%s/config.toml", craft_home);

    // Open config.toml and write the data to stdout
    FILE* f = fopen(config_path, "r");
    if (!f) {
        fprintf(stderr, "Error: No config file found\n");
        return -1;
    }

    char buf[FILE_SIZE];
    size_t bytes;
    while ((bytes = fread(buf, 1, sizeof(buf), f)) > 0) {
        fwrite(buf, 1, bytes, stdout);
    }

    fclose(f);
    return 0;
}

int handle_config(const command_t* command_data) {
    const char* subcommand = command_data->subcommand;

    if (strcmp(subcommand, "set") == 0) {
        return handle_set(command_data);
    }

    if (strcmp(subcommand, "get") == 0) {
        return handle_get(command_data);
    }

    if (strcmp(subcommand, "list") == 0) {
        return handle_list();
    }

    return -1;
}
