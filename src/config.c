#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "template.h"

const craft_config_t defaults = {
    .language = "cpp",
    .c_standard = 99,
    .cpp_standard = 17,
    .template = "executable"
};

int load_global_config(craft_config_t* config) {

    // Set defaults
    strncpy(config->language, defaults.language, sizeof(config->language));
    config->c_standard = defaults.c_standard;
    config->cpp_standard = defaults.cpp_standard;
    strncpy(config->template, defaults.template, sizeof(config->template));

    // Get path to config.toml
    char craft_home[512];
    char config_path[512];
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
        fprintf(stderr, "Error parsing global config file\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return -1;
    }

    // Get config values if valid
    toml_datum_t language = toml_seek(result.toptab, "defaults.language");
    toml_datum_t c_standard = toml_seek(result.toptab, "defaults.c_standard");
    toml_datum_t cpp_standard = toml_seek(result.toptab, "defaults.cpp_standard");
    toml_datum_t template = toml_seek(result.toptab, "defaults.template");

    if (language.type == TOML_STRING) {
        strncpy(config->language, language.u.s, sizeof(config->language));
    }
    if (c_standard.type == TOML_INT64) {
        config->c_standard = c_standard.u.int64;
    }
    if (cpp_standard.type == TOML_INT64) {
        config->cpp_standard = cpp_standard.u.int64;
    }
    if (template.type == TOML_STRING) {
        strncpy(config->template, template.u.s, sizeof(config->template));
    }

    toml_free(result);
    return 0;
}

// Saves the entries in the craft config struct to the global config.toml
int save_global_config(craft_config_t* config) {

    // Get path to config.toml
    char craft_home[512];
    char config_path[512];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        return -1;
    }
    snprintf(config_path, sizeof(config_path), "%s/config.toml", craft_home);

    // Open config.toml for writing and write config data
    FILE* f = fopen(config_path, "w");
    if (!f) {
        fprintf(stderr, "Error: could not write to config file\n");
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

int generate_craft_toml(const char* project_path, project_config_t* config) {
    // Get path to toml file
    char toml_path[512];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", project_path);

    // Open the file and write config values
    FILE* file = fopen(toml_path, "w");
    if (!file) {
        fprintf(stderr, "Error: Failed to create craft.toml for project\n");
        return -1;
    }

    fprintf(file, "[project]\n");
    if (strlen(config->name) > 0) {
        fprintf(file, "name = \"%s\"\n", config->name);
    }
    if (strlen(config->version) > 0) {
        fprintf(file, "version = \"%s\"\n", config->version);
    }
    
    fprintf(file, "language = \"%s\"\n", config->language);

    if (config->c_standard != 0) {
        fprintf(file, "c_standard = %d\n", config->c_standard);
    }
    if (config->cpp_standard != 0) {
        fprintf(file, "cpp_standard = %d\n", config->cpp_standard);
    }

    // Built type
    fprintf(file, "\n[build]\n");
    fprintf(file, "type = \"%s\"\n", config->build_type);

    // Include dirs
    fprintf(file, "include_dirs = [");
    if (config->include_dir_count > 0) {
        for (int i = 0; i < config->include_dir_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->include_dirs[i]);
    } else {
        fprintf(file, "\"include\"");
    }
    fprintf(file, "]\n");

    // Source dirs
    fprintf(file, "source_dirs = [");
    if (config->source_dir_count > 0) {
        for (int i = 0; i < config->source_dir_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->source_dirs[i]);
    } else {
        fprintf(file, "\"src\"");
    }
    fprintf(file, "]\n");

    // Lib dirs (only write if present)
    if (config->lib_dir_count > 0) {
        fprintf(file, "lib_dirs = [");
        for (int i = 0; i < config->lib_dir_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->lib_dirs[i]);
        fprintf(file, "]\n");
    }

    // Libs (only write if present)
    if (config->lib_count > 0) {
        fprintf(file, "libs = [");
        for (int i = 0; i < config->lib_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->libs[i]);
        fprintf(file, "]\n");
    }

    fclose(file);
    return 0;

    fclose(file);
    return 0;
}

int load_project_config(project_config_t* config, const char* project_root) {

    // Set all values to 0
    memset(config, 0, sizeof(*config));

    // Get path to craft.toml
    char toml_path[1024];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", project_root);
    if (!fileExists(toml_path)) {
        fprintf(stderr, "Error: no craft.toml found in '%s'\n", project_root);
        return -1;
    }

    // Open craft.toml and read config data
    FILE* file = fopen(toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: could not open craft.toml\n");
        return -1;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error parsing craft.toml: %s\n", result.errmsg);
        return -1;
    }

    // Project fields
    toml_datum_t name = toml_seek(result.toptab, "project.name");
    toml_datum_t version = toml_seek(result.toptab, "project.version");
    toml_datum_t language = toml_seek(result.toptab, "project.language");
    toml_datum_t c_standard = toml_seek(result.toptab, "project.c_standard");
    toml_datum_t cpp_standard = toml_seek(result.toptab, "project.cpp_standard");

    // Build fields
    toml_datum_t build_type = toml_seek(result.toptab, "build.type");
    toml_datum_t include_dirs = toml_seek(result.toptab, "build.include_dirs");
    toml_datum_t source_dirs = toml_seek(result.toptab, "build.source_dirs");
    toml_datum_t lib_dirs = toml_seek(result.toptab, "build.lib_dirs");
    toml_datum_t libs = toml_seek(result.toptab, "build.libs");

    // Store project values
    if (name.type == TOML_STRING)
        strncpy(config->name, name.u.s, sizeof(config->name));
    if (version.type == TOML_STRING)
        strncpy(config->version, version.u.s, sizeof(config->version));
    if (language.type == TOML_STRING)
        strncpy(config->language, language.u.s, sizeof(config->language));
    if (c_standard.type == TOML_INT64) {
        config->c_standard = (int)c_standard.u.int64;
        config->has_c_standard = 1;
    }
    if (cpp_standard.type == TOML_INT64) {
        config->cpp_standard = (int)cpp_standard.u.int64;
        config->has_cpp_standard = 1;
    }

    // Store build values
    if (build_type.type == TOML_STRING)
        strncpy(config->build_type, build_type.u.s, sizeof(config->build_type));

    // Include directories
    if (include_dirs.type == TOML_ARRAY) {
        config->include_dir_count = include_dirs.u.arr.size;
        for (int i = 0; i < include_dirs.u.arr.size; i++) {
            toml_datum_t elem = include_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                strncpy(config->include_dirs[i], elem.u.s, sizeof(config->include_dirs[i]));
        }
    }

    // Source directories
    if (source_dirs.type == TOML_ARRAY) {
        config->source_dir_count = source_dirs.u.arr.size;
        for (int i = 0; i < source_dirs.u.arr.size; i++) {
            toml_datum_t elem = source_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                strncpy(config->source_dirs[i], elem.u.s, sizeof(config->source_dirs[i]));
        }
    }

    // Library directories
    if (lib_dirs.type == TOML_ARRAY) {
        config->lib_dir_count = lib_dirs.u.arr.size;
        for (int i = 0; i < lib_dirs.u.arr.size; i++) {
            toml_datum_t elem = lib_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                strncpy(config->lib_dirs[i], elem.u.s, sizeof(config->lib_dirs[i]));
        }
    }

    // Libraries
    if (libs.type == TOML_ARRAY) {
        config->lib_count = libs.u.arr.size;
        for (int i = 0; i < libs.u.arr.size; i++) {
            toml_datum_t elem = libs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                strncpy(config->libs[i], elem.u.s, sizeof(config->libs[i]));
        }
    }

    toml_free(result);
    return 0;
}

int validate_project_config(project_config_t* config) {
    // Check if there is a project name
    if (strlen(config->name) == 0) {
        fprintf(stderr, "[Config Error]: Project name is missing from craft.toml\n");
        return -1;
    }

    // Check if there is a language and it is "c" or "cpp"
    if (strlen(config->language) != 0) {
        if (strcmp(config->language, "c") != 0 && strcmp(config->language, "cpp") != 0) {
            fprintf(stderr, "[Config Error]: Invalid language '%s' in craft.toml\n", config->language);
            return -1;
        }
    }
    else {
        fprintf(stderr, "[Config Error]: Language is missing from craft.toml\n");
        return -1;
    }

    // If c standard is present make sure it is valid
    if (config->has_c_standard) {
        int c_standard = config->c_standard;
        if (c_standard != 89 && c_standard != 99 && c_standard != 11 && c_standard != 17 && c_standard != 23) {
            fprintf(stderr, "[Config Error]: Invalid c standard '%d' in craft.toml\n", c_standard);
            return -1;
        }
    }

    // If cpp standard is present make sure it is valid
    if (config->has_cpp_standard) {
        int cpp_standard = config->cpp_standard;
        if (cpp_standard != 11 && cpp_standard != 14 && cpp_standard != 17 && cpp_standard != 20 && cpp_standard != 23) {
            fprintf(stderr, "[Config Error]: Invalid cpp standard '%d' in craft.toml\n", cpp_standard);
            return -1;
        }
    }

    // Check if there is a build type and it is one of the valid types
    if (strlen(config->build_type) != 0) {
        if (strcmp(config->build_type, "executable") != 0 &&
            strcmp(config->build_type, "static-library") != 0 &&
            strcmp(config->build_type, "shared-library") != 0 &&
            strcmp(config->build_type, "header-only") != 0) {
            fprintf(stderr, "[Config Error]: Invalid build type '%s' in craft.toml\n", config->build_type);
            return -1;
        }
    }
    else {
        fprintf(stderr, "[Config Error]: Build type is missing from craft.toml\n");
        return -1;
    }

    // Everything is valid
    return 0;
}

// Subcommand handler for config set to set a global config value
int handle_set(command_t* command_data) {

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
    strncpy(section, lookup, dot - lookup);
    section[dot - lookup] = '\0';
    strncpy(key, dot + 1, sizeof(key));

    // Default language
    if (strcmp(lookup, "defaults.language") == 0) {
        if (strcmp(new_value, "c") != 0 && strcmp(new_value, "cpp") != 0) {
            fprintf(stderr, "Error: Invalid language '%s', must be 'c' or 'cpp'\n", new_value);
            return -1;
        }
        if (!template_exists(config.template, new_value)) {
            fprintf(stdout, "Warning: Current default template '%s' does not exist for language '%s'\n", config.template, new_value);
        }
        strncpy(config.language, new_value, sizeof(config.language));
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
        strncpy(config.template, new_value, sizeof(config.template));
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
int handle_get(command_t* command_data) {

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
    strncpy(section, lookup, dot - lookup);
    section[dot - lookup] = '\0';
    strncpy(key, dot + 1, sizeof(key));

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
int handle_list() {

    // Get path to config.toml
    char craft_home[512];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0)
        return -1;

    char config_path[512];
    snprintf(config_path, sizeof(config_path), "%s/config.toml", craft_home);

    // Open config.toml and write the data to stdout
    FILE* f = fopen(config_path, "r");
    if (!f) {
        fprintf(stderr, "Error: no config file found\n");
        return -1;
    }

    char buf[4096];
    size_t bytes;
    while ((bytes = fread(buf, 1, sizeof(buf), f)) > 0) {
        fwrite(buf, 1, bytes, stdout);
    }

    fclose(f);
    return 0;
}

int handle_config(command_t* command_data) {
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
