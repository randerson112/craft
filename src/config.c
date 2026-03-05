#include "config.h"
#include <string.h>

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
    toml_datum_t c_standard = toml_seek(result.toptab, "defaults.cstandard");
    toml_datum_t cpp_standard = toml_seek(result.toptab, "defaults.cppstandard");
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
    fprintf(file, "name = \"%s\"\n", config->name);
    fprintf(file, "version = \"%s\"\n", config->version);
    fprintf(file, "language = \"%s\"\n", config->language);
    fprintf(file, "standard = %d\n", config->standard);
    fprintf(file, "\n[build]\n");
    fprintf(file, "type = \"%s\"\n", config->build_type);

    fclose(file);
    return 0;
}
