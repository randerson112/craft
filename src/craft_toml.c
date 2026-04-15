#include "craft_toml.h"

#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "utils.h"

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

// Checks if given path is a Craft project
int is_craft_project(const char* path) {

    // Check if craft.toml exists at path
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", path);
    if (!file_exists(toml_path)) {
        return 0;
    }

    // Check if craft.toml contains project section and not a workspace section
    FILE* file = fopen(toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open craft.toml\n");
        return 0;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: Failed to parse craft.toml\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return 0;
    }

    toml_datum_t project_section = toml_seek(result.toptab, "project");
    toml_datum_t workspace_section = toml_seek(result.toptab, "workspace");
    if (project_section.type == TOML_TABLE && workspace_section.type != TOML_TABLE) {
        toml_free(result);
        return 1;
    }
    else {
        toml_free(result);
        return 0;
    }
}

int get_project_root(char* buffer, size_t buffer_size, const char* cwd) {
    char current_path[PATH_SIZE];
    snprintf(current_path, sizeof(current_path), "%s", cwd);

    while (1) {
        char toml_path[PATH_SIZE];
        snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", current_path);

        // Check if craft.toml file exists in this directory
        if (file_exists(toml_path) && is_craft_project(current_path)) {
            snprintf(buffer, buffer_size, "%s", current_path);
            return 0;
        }

        // Move up a directory
        char* last_slash = strrchr(current_path, '/');
        if (!last_slash || last_slash == current_path) {
            return -1;
        }
        
        *last_slash = '\0';
    }
}

int check_unknown_keys(toml_datum_t section, const char* section_name, const config_key_t* valid_keys, int num_keys) {
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

// Loads the project section values into config struct
static int load_project_section(project_section_t* project, toml_result_t* result) {

    // Ensure project section is present
    toml_datum_t project_section = toml_seek(result->toptab, "project");
    if (project_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: Missing required [project] section in craft.toml\n");
        return -1;
    }

    // Check for unknown keys
    if (check_unknown_keys(project_section, "project", project_keys, num_project_keys) != 0) {
        return -1;
    }

    // Get project keys
    toml_datum_t name = toml_seek(project_section, "name");
    toml_datum_t version = toml_seek(project_section, "version");
    toml_datum_t language = toml_seek(project_section, "language");
    toml_datum_t c_standard = toml_seek(project_section, "c_standard");
    toml_datum_t cpp_standard = toml_seek(project_section, "cpp_standard");

    // Store values into config struct
    // Project name
    if (name.type == TOML_STRING) {
        snprintf(project->name, sizeof(project->name), "%s", name.u.s);
    }

    // Version
    if (version.type == TOML_STRING) {
        snprintf(project->version, sizeof(project->version), "%s", version.u.s);
    }

    // Language
    if (language.type == TOML_STRING) {
        snprintf(project->language, sizeof(project->language), "%s", language.u.s);
    }

    // C standard
    if (c_standard.type == TOML_INT64) {
        project->c_standard = (int)c_standard.u.int64;
        project->has_c_standard = 1;
    }

    // C++ standard
    if (cpp_standard.type == TOML_INT64) {
        project->cpp_standard = (int)cpp_standard.u.int64;
        project->has_cpp_standard = 1;
    }

    return 0;
}

// Loads the build section values into config struct
static int load_build_section(build_section_t* build, toml_result_t* result) {

    // Ensure build section is present
    toml_datum_t build_section = toml_seek(result->toptab, "build");
    if (build_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: Missing required [build] section in craft.toml\n");
        return -1;
    }

    // Check for unknown keys
    if (check_unknown_keys(build_section, "build", build_keys, num_build_keys) != 0) {
        return -1;
    }

    // Get build keys
    toml_datum_t type = toml_seek(build_section, "type");
    toml_datum_t include_dirs = toml_seek(build_section, "include_dirs");
    toml_datum_t source_dirs = toml_seek(build_section, "source_dirs");
    toml_datum_t lib_dirs = toml_seek(build_section, "lib_dirs");
    toml_datum_t libs = toml_seek(build_section, "libs");

    // Store values into config struct
    // Build type
    if (type.type == TOML_STRING) {
        snprintf(build->type, sizeof(build->type), "%s", type.u.s);
    }

    // Include directories
    if (include_dirs.type == TOML_ARRAY) {
        build->include_dir_count = include_dirs.u.arr.size;
        for (int i = 0; i < include_dirs.u.arr.size; i++) {
            toml_datum_t elem = include_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING) {
                snprintf(build->include_dirs[i], sizeof(build->include_dirs[i]), "%s", elem.u.s);
            }
        }
    }

    // Source directories
    if (source_dirs.type == TOML_ARRAY) {
        build->source_dir_count = source_dirs.u.arr.size;
        for (int i = 0; i < source_dirs.u.arr.size; i++) {
            toml_datum_t elem = source_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING) {
                snprintf(build->source_dirs[i], sizeof(build->source_dirs[i]), "%s", elem.u.s);
            }
        }
    }

    // Library directories
    if (lib_dirs.type == TOML_ARRAY) {
        build->lib_dir_count = lib_dirs.u.arr.size;
        for (int i = 0; i < lib_dirs.u.arr.size; i++) {
            toml_datum_t elem = lib_dirs.u.arr.elem[i];
            if (elem.type == TOML_STRING) {
                snprintf(build->lib_dirs[i], sizeof(build->lib_dirs[i]), "%s", elem.u.s);
            }
        }
    }

    // Libraries
    if (libs.type == TOML_ARRAY) {
        build->lib_count = libs.u.arr.size;
        for (int i = 0; i < libs.u.arr.size; i++) {
            toml_datum_t elem = libs.u.arr.elem[i];
            if (elem.type == TOML_STRING) {
                snprintf(build->libs[i], sizeof(build->libs[i]), "%s", elem.u.s);
            }
        }
    }

    return 0;
}

// Loads the dependencies section values into config struct
static int load_dependencies_section(dependencies_section_t* dependencies, toml_result_t* result) {

    // Check if section is present
    toml_datum_t deps_section = toml_seek(result->toptab, "dependencies");
    if (deps_section.type != TOML_TABLE) {
        return 0;
    }

    // Loop through dependencies
    dependencies->dependencies_count = deps_section.u.tab.size;
    for (int i = 0; i < dependencies->dependencies_count; i++) {
        dependency_t* dep = &dependencies->dependencies[i];

        // Get name of dependency from the key
        snprintf(dep->name, sizeof(dep->name), "%s", deps_section.u.tab.key[i]);

        // Get the value table for this dependency
        toml_datum_t dep_table = deps_section.u.tab.value[i];
        if (dep_table.type != TOML_TABLE) {
            fprintf(stderr, "Error: Value for dependency '%s' must be a table\n", dep->name);
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
            return -1;
        }

        // Local path dependency
        if (has_path) {
            dep->type = DEP_PATH;
            for (int j = 0; j < dep_table.u.tab.size; j++) {
                if (strcmp(dep_table.u.tab.key[j], "path") == 0) {
                    if (dep_table.u.tab.value[j].type != TOML_STRING) {
                        fprintf(stderr, "Error: 'path' value for dependency '%s' must be a string\n", dep->name);
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
                        return -1;
                    }
                    snprintf(dep->value, sizeof(dep->value), "%s", val.u.s);
                }
                else if (strcmp(key, "tag") == 0) {
                    if (val.type != TOML_STRING) {
                        fprintf(stderr, "Error: 'tag' value for dependency '%s' must be a string\n", dep->name);
                        return -1;
                    }
                    snprintf(dep->tag, sizeof(dep->tag), "%s", val.u.s);
                    has_tag = 1;
                }
                else if (strcmp(key, "branch") == 0) {
                    if (val.type != TOML_STRING) {
                        fprintf(stderr, "Error: 'branch' value for dependency '%s' must be a string\n", dep->name);
                        return -1;
                    }
                    snprintf(dep->branch, sizeof(dep->branch), "%s", val.u.s);
                    has_branch = 1;
                }
                else if (strcmp(key, "links") == 0) {
                    if (val.type != TOML_ARRAY) {
                        fprintf(stderr, "Error: 'links' value for dependency '%s' must be an array\n", dep->name);
                        return -1;
                    }
                    for (int k = 0; k < val.u.arr.size; k++) {
                        if (val.u.arr.elem[k].type != TOML_STRING) {
                            fprintf(stderr, "Error: 'links' values for dependency '%s' must be strings\n", dep->name);
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
                return -1;
            }

            // Validate git URL is present
            if (strlen(dep->value) == 0) {
                fprintf(stderr, "Error: Dependency '%s' is missing a 'git' URL\n", dep->name);
                return -1;
            }
        }
    }

    return 0;
}

int load_project_config(project_config_t* config, const char* project_root) {
    memset(config, 0, sizeof(*config));

    // Get path to craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", project_root);
    if (!file_exists(toml_path)) {
        fprintf(stderr, "Error: No craft.toml found in '%s'\n", project_root);
        return -1;
    }

    // Open craft.toml and parse
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

    // Load sections
    if (load_project_section(&config->project, &result) != 0) {
        toml_free(result);
        return -1;
    }
    if (load_build_section(&config->build, &result) != 0) {
        toml_free(result);
        return -1;
    }
    if (load_dependencies_section(&config->dependencies, &result) != 0) {
        toml_free(result);
        return -1;
    }

    toml_free(result);
    return 0;
}

int validate_project_config(project_config_t* config) {
    // Check if there is a project name
    if (strlen(config->project.name) == 0) {
        fprintf(stderr, "Error: Project name is missing from craft.toml\n");
        return -1;
    }

    // Check if there is a version and that it is valid
    if (strlen(config->project.version) == 0) {
        fprintf(stderr, "Error: Version is missing from craft.toml\n");
        return -1;
    }
    else {
        if (!is_valid_version(config->project.version)) {
            fprintf(stderr, "Error: Version '%s' is invalid\n", config->project.version);
            return -1;
        }
    }

    // Check if there is a language and it is "c" or "cpp"
    if (strlen(config->project.language) != 0) {
        if (strcmp(config->project.language, "c") != 0 && strcmp(config->project.language, "cpp") != 0) {
            fprintf(stderr, "Error: Invalid language '%s' in craft.toml\n", config->project.language);
            return -1;
        }
    }
    else {
        fprintf(stderr, "Error: Language is missing from craft.toml\n");
        return -1;
    }

    // If c standard is present make sure it is valid
    if (config->project.has_c_standard) {
        int c_standard = config->project.c_standard;
        if (c_standard != 89 && c_standard != 99 && c_standard != 11 && c_standard != 17 && c_standard != 23) {
            fprintf(stderr, "Error: Invalid c standard '%d' in craft.toml\n", c_standard);
            return -1;
        }
    }

    // If cpp standard is present make sure it is valid
    if (config->project.has_cpp_standard) {
        int cpp_standard = config->project.cpp_standard;
        if (cpp_standard != 11 && cpp_standard != 14 && cpp_standard != 17 && cpp_standard != 20 && cpp_standard != 23) {
            fprintf(stderr, "Error: Invalid cpp standard '%d' in craft.toml\n", cpp_standard);
            return -1;
        }
    }

    // Check if there is a build type and it is one of the valid types
    if (strlen(config->build.type) != 0) {
        if (strcmp(config->build.type, "executable") != 0 &&
            strcmp(config->build.type, "static-library") != 0 &&
            strcmp(config->build.type, "shared-library") != 0 &&
            strcmp(config->build.type, "header-only") != 0) {
            fprintf(stderr, "Error: Invalid build type '%s' in craft.toml\n", config->build.type);
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
    if (strlen(config->project.name) > 0) {
        fprintf(file, "name = \"%s\"\n", config->project.name);
    }
    if (strlen(config->project.version) > 0) {
        fprintf(file, "version = \"%s\"\n", config->project.version);
    }
    
    fprintf(file, "language = \"%s\"\n", config->project.language);

    if (config->project.c_standard != 0) {
        fprintf(file, "c_standard = %d\n", config->project.c_standard);
    }
    if (config->project.cpp_standard != 0) {
        fprintf(file, "cpp_standard = %d\n", config->project.cpp_standard);
    }

    // Built type
    fprintf(file, "\n[build]\n");
    fprintf(file, "type = \"%s\"\n", config->build.type);

    // Include dirs
    fprintf(file, "include_dirs = [");
    if (config->build.include_dir_count > 0) {
        for (int i = 0; i < config->build.include_dir_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->build.include_dirs[i]);
    } else {
        fprintf(file, "\"include\"");
    }
    fprintf(file, "]\n");

    // Source dirs
    fprintf(file, "source_dirs = [");
    if (config->build.source_dir_count > 0) {
        for (int i = 0; i < config->build.source_dir_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->build.source_dirs[i]);
    } else {
        fprintf(file, "\"src\"");
    }
    fprintf(file, "]\n");

    // Lib dirs (only write if present)
    if (config->build.lib_dir_count > 0) {
        fprintf(file, "lib_dirs = [");
        for (int i = 0; i < config->build.lib_dir_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->build.lib_dirs[i]);
        fprintf(file, "]\n");
    }

    // Libs (only write if present)
    if (config->build.lib_count > 0) {
        fprintf(file, "libs = [");
        for (int i = 0; i < config->build.lib_count; i++)
            fprintf(file, "%s\"%s\"", i > 0 ? ", " : "", config->build.libs[i]);
        fprintf(file, "]\n");
    }

    // Write dependencies if present
    if (config->dependencies.dependencies_count > 0) {
        fprintf(file, "\n[dependencies]\n");
        for (int i = 0; i < config->dependencies.dependencies_count; i++) {
            dependency_t* dep = &config->dependencies.dependencies[i];

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
