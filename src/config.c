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

// Struct to hold data about a key in craft.toml
typedef struct {
    const char* name;
    int required;
} config_key_t;

// Information about the project section keys in craft.toml
static const config_key_t project_keys[] = {
    {"name",         1},
    {"version",      1},
    {"language",     1},
    {"c_standard",   0},
    {"cpp_standard", 0}
};
static const int num_project_keys = 5;

// Information about the build section keys in craft.toml
static const config_key_t build_keys[] = {
    {"type",         1},
    {"include_dirs", 0},
    {"source_dirs",  0},
    {"lib_dirs",     0},
    {"libs",         0}
};
static const int num_build_keys = 5;

// Checks a section for any unknown keys and prints an error or warning message
static int check_unknown_keys(toml_datum_t section, const char* section_name, const config_key_t* valid_keys, int num_keys) {
    for (int i = 0; i < section.u.tab.size; i++) {
        const char* key = section.u.tab.key[i];
        int found = 0;
        for (int j = 0; j < num_keys; j++) {
            if (strcmp(key, valid_keys[j].name) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            // Build array of just names for suggest()
            const char* names[16];
            for (int j = 0; j < num_keys; j++)
                names[j] = valid_keys[j].name;

            const char* suggestion = suggest(key, names, num_keys);

            // Check if suggestion is a required key
            int suggestion_required = 0;
            if (suggestion) {
                for (int j = 0; j < num_keys; j++) {
                    if (strcmp(suggestion, valid_keys[j].name) == 0) {
                        suggestion_required = valid_keys[j].required;
                        break;
                    }
                }
            }

            if (suggestion && suggestion_required) {
                fprintf(stderr, "Error: Unknown key '%s' in [%s], did you mean '%s'?\n", key, section_name, suggestion);
                return -1;
            }
            else if (suggestion) {
                fprintf(stderr, "Warning: Unknown key '%s' in [%s], did you mean '%s'?\n", key, section_name, suggestion);
            }
            else {
                fprintf(stderr, "Warning: Unknown key '%s' in [%s]\n", key, section_name);
            }
        }
    }

    return 0;
}

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
int save_global_config(craft_config_t* config) {

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

int generate_craft_toml(const char* project_path, project_config_t* config) {
    // Get path to toml file
    char toml_path[PATH_SIZE];
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

    // Write dependencies if present
    if (config->dependencies_count > 0) {
        fprintf(file, "\n[dependencies]\n");
        for (int i = 0; i < config->dependencies_count; i++) {
            dependency_t* dep = &config->dependencies[i];

            switch(dep->type) {
                case DEP_PATH:
                    fprintf(file, "%s = { path = \"%s\" }\n", dep->name, dep->value);
                    break;
                case DEP_GIT:
                    fprintf(file, "%s = { git = \"%s\"", dep->name, dep->value);

                    if (strlen(dep->tag) > 0) {
                        fprintf(file, ", tag = \"%s\"", dep->tag);
                    }
                    else if (strlen(dep->branch) > 0) {
                        fprintf(file, ", branch = \"%s\"", dep->branch);
                    }

                    if (dep->links_count > 0) {
                        fprintf(file, ", links = [");
                        for (int j = 0; j < dep->links_count; j++) {
                            fprintf(file, "%s\"%s\"", j > 0 ? ", " : "", dep->links[j]);
                        }
                        fprintf(file, "]");
                    }
                    fprintf(file, " }\n");
                    break;
                case DEP_INVALID:
                    return -1;
            }
        }
    }

    fclose(file);
    return 0;
}

int load_project_config(project_config_t* config, const char* project_root) {

    // Set all values to 0
    memset(config, 0, sizeof(*config));

    // Get path to craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", project_root);
    if (!file_exists(toml_path)) {
        fprintf(stderr, "Error: No craft.toml found in '%s'\n", project_root);
        return -1;
    }

    // Open craft.toml and read config data
    FILE* file = fopen(toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open craft.toml\n");
        return -1;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: Failed to parse craft.toml\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return -1;
    }

    // Check if required sections are present
    toml_datum_t project_section = toml_seek(result.toptab, "project");
    if (project_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: Missing required [project] section in craft.toml\n");
        toml_free(result);
        return -1;
    }

    toml_datum_t build_section = toml_seek(result.toptab, "build");
    if (build_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: Missing required [build] section in craft.toml\n");
        toml_free(result);
        return -1;
    }

    // Check for unknown keys in sections
    if (check_unknown_keys(project_section, "project", project_keys, num_project_keys) != 0) {
        toml_free(result);
        return -1;
    }
    if (check_unknown_keys(build_section, "build", build_keys, num_build_keys) != 0) {
        toml_free(result);
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
        snprintf(config->name, sizeof(config->name), "%s", name.u.s);
    if (version.type == TOML_STRING)
        snprintf(config->version, sizeof(config->version), "%s", version.u.s);
    if (language.type == TOML_STRING)
        snprintf(config->language, sizeof(config->language), "%s", language.u.s);
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
        snprintf(config->build_type, sizeof(config->build_type), "%s", build_type.u.s);

    // Include directories
    if (include_dirs.type == TOML_ARRAY) {
        config->include_dir_count = include_dirs.u.arr.size;
        for (int i = 0; i < include_dirs.u.arr.size; i++) {
            toml_datum_t elem = include_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                snprintf(config->include_dirs[i], sizeof(config->include_dirs[i]), "%s", elem.u.s);
        }
    }

    // Source directories
    if (source_dirs.type == TOML_ARRAY) {
        config->source_dir_count = source_dirs.u.arr.size;
        for (int i = 0; i < source_dirs.u.arr.size; i++) {
            toml_datum_t elem = source_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                snprintf(config->source_dirs[i], sizeof(config->source_dirs[i]), "%s", elem.u.s);
        }
    }

    // Library directories
    if (lib_dirs.type == TOML_ARRAY) {
        config->lib_dir_count = lib_dirs.u.arr.size;
        for (int i = 0; i < lib_dirs.u.arr.size; i++) {
            toml_datum_t elem = lib_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                snprintf(config->lib_dirs[i], sizeof(config->lib_dirs[i]), "%s", elem.u.s);
        }
    }

    // Libraries
    if (libs.type == TOML_ARRAY) {
        config->lib_count = libs.u.arr.size;
        for (int i = 0; i < libs.u.arr.size; i++) {
            toml_datum_t elem = libs.u.arr.elem[i];
            if (elem.type == TOML_STRING)
                snprintf(config->libs[i], sizeof(config->libs[i]), "%s", elem.u.s);
        }
    }

    // Dependencies section
    toml_datum_t deps_section = toml_seek(result.toptab, "dependencies");
    if (deps_section.type == TOML_TABLE) {
        config->dependencies_count = deps_section.u.tab.size;
        for (int i = 0; i < config->dependencies_count; i++) {
            dependency_t* dep = &config->dependencies[i];
            memset(dep, 0, sizeof(*dep));

            // Get name of dependency from the key
            snprintf(dep->name, sizeof(dep->name), "%s", deps_section.u.tab.key[i]);

            // Get the value table for this dependency
            toml_datum_t dep_table = deps_section.u.tab.value[i];
            if (dep_table.type != TOML_TABLE) {
                fprintf(stderr, "Error: Value for dependency '%s' must be a table\n", dep->name);
                toml_free(result);
                return -1;
            }

            // Detect dependency type from which keys are present
            int has_path = 0;
            int has_git = 0;

            for (int j = 0; j < dep_table.u.tab.size; j++) {
                if (strcmp(dep_table.u.tab.key[j], "path") == 0)   has_path = 1;
                if (strcmp(dep_table.u.tab.key[j], "git") == 0)    has_git = 1;
            }

            // Validate exactly one type key is present
            if (has_path + has_git != 1) {
                fprintf(stderr, "Error: Dependency '%s' must have exactly one of 'path' or 'git'\n", dep->name);
                toml_free(result);
                return -1;
            }

            // Local path dependency
            if (has_path) {
                dep->type = DEP_PATH;
                for (int j = 0; j < dep_table.u.tab.size; j++) {
                    if (strcmp(dep_table.u.tab.key[j], "path") == 0) {
                        if (dep_table.u.tab.value[j].type != TOML_STRING) {
                            fprintf(stderr, "Error: 'path' value for dependency '%s' must be a string\n", dep->name);
                            toml_free(result);
                            return -1;
                        }
                        snprintf(dep->value, sizeof(dep->value), "%s", dep_table.u.tab.value[j].u.s);
                        break;
                    }
                }
            }

            // Git dependency
            else if (has_git) {
                dep->type = DEP_GIT;

                int has_tag = 0;
                int has_branch = 0;

                for (int j = 0; j < dep_table.u.tab.size; j++) {
                    const char* key = dep_table.u.tab.key[j];
                    toml_datum_t val = dep_table.u.tab.value[j];

                    if (strcmp(key, "git") == 0) {
                        if (val.type != TOML_STRING) {
                            fprintf(stderr, "Error: 'git' value for dependency '%s' must be a string\n", dep->name);
                            toml_free(result);
                            return -1;
                        }
                        snprintf(dep->value, sizeof(dep->value), "%s", val.u.s);
                    }
                    else if (strcmp(key, "tag") == 0) {
                        if (val.type != TOML_STRING) {
                            fprintf(stderr, "Error: 'tag' value for dependency '%s' must be a string\n", dep->name);
                            toml_free(result);
                            return -1;
                        }
                        snprintf(dep->tag, sizeof(dep->tag), "%s", val.u.s);
                        has_tag = 1;
                    }
                    else if (strcmp(key, "branch") == 0) {
                        if (val.type != TOML_STRING) {
                            fprintf(stderr, "Error: 'branch' value for dependency '%s' must be a string\n", dep->name);
                            toml_free(result);
                            return -1;
                        }
                        snprintf(dep->branch, sizeof(dep->branch), "%s", val.u.s);
                        has_branch = 1;
                    }
                    else if (strcmp(key, "links") == 0) {
                        if (val.type != TOML_ARRAY) {
                            fprintf(stderr, "Error: 'links' value for dependency '%s' must be an array\n", dep->name);
                            toml_free(result);
                            return -1;
                        }
                        for (int k = 0; k < val.u.arr.size; k++) {
                            if (val.u.arr.elem[k].type != TOML_STRING) {
                                fprintf(stderr, "Error: 'links' values for dependency '%s' must be strings\n", dep->name);
                                toml_free(result);
                                return -1;
                            }
                            snprintf(dep->links[k], sizeof(dep->links[0]), "%s", val.u.arr.elem[k].u.s);
                            dep->links_count++;
                        }
                    }
                }

                // Validate tag and branch are not both present
                if (has_tag && has_branch) {
                    fprintf(stderr, "Error: Dependency '%s' cannot have both 'tag' and 'branch'\n", dep->name);
                    toml_free(result);
                    return -1;
                }

                // Validate git URL is present
                if (strlen(dep->value) == 0) {
                    fprintf(stderr, "Error: Dependency '%s' is missing a 'git' URL\n", dep->name);
                    toml_free(result);
                    return -1;
                }
            }
        }
    }

    toml_free(result);
    return 0;
}

int validate_project_config(project_config_t* config) {
    // Check if there is a project name
    if (strlen(config->name) == 0) {
        fprintf(stderr, "Error: Project name is missing from craft.toml\n");
        return -1;
    }

    // Check if there is a version and that it is valid
    if (strlen(config->version) == 0) {
        fprintf(stderr, "Error: Version is missing from craft.toml\n");
        return -1;
    }
    else {
        if (!is_valid_version(config->version)) {
            fprintf(stderr, "Error: Version '%s' is invalid\n", config->version);
            return -1;
        }
    }

    // Check if there is a language and it is "c" or "cpp"
    if (strlen(config->language) != 0) {
        if (strcmp(config->language, "c") != 0 && strcmp(config->language, "cpp") != 0) {
            fprintf(stderr, "Error: Invalid language '%s' in craft.toml\n", config->language);
            return -1;
        }
    }
    else {
        fprintf(stderr, "Error: Language is missing from craft.toml\n");
        return -1;
    }

    // If c standard is present make sure it is valid
    if (config->has_c_standard) {
        int c_standard = config->c_standard;
        if (c_standard != 89 && c_standard != 99 && c_standard != 11 && c_standard != 17 && c_standard != 23) {
            fprintf(stderr, "Error: Invalid c standard '%d' in craft.toml\n", c_standard);
            return -1;
        }
    }

    // If cpp standard is present make sure it is valid
    if (config->has_cpp_standard) {
        int cpp_standard = config->cpp_standard;
        if (cpp_standard != 11 && cpp_standard != 14 && cpp_standard != 17 && cpp_standard != 20 && cpp_standard != 23) {
            fprintf(stderr, "Error: Invalid cpp standard '%d' in craft.toml\n", cpp_standard);
            return -1;
        }
    }

    // Check if there is a build type and it is one of the valid types
    if (strlen(config->build_type) != 0) {
        if (strcmp(config->build_type, "executable") != 0 &&
            strcmp(config->build_type, "static-library") != 0 &&
            strcmp(config->build_type, "shared-library") != 0 &&
            strcmp(config->build_type, "header-only") != 0) {
            fprintf(stderr, "Error: Invalid build type '%s' in craft.toml\n", config->build_type);
            return -1;
        }
    }
    else {
        fprintf(stderr, "Error: Build type is missing from craft.toml\n");
        return -1;
    }

    // Everything is valid
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
