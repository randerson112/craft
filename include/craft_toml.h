#ifndef CRAFT_TOML_H
#define CRAFT_TOML_H

#include "deps.h"
#include "tomlc17.h"

// Struct to store project section data
typedef struct {
    char name[32];
    char version[16];
    char language[8];
    int c_standard;
    int has_c_standard;
    int cpp_standard;
    int has_cpp_standard;
} project_section_t;

// Struct to store build section data
typedef struct {
    char type[32];
    char source_dirs[8][256];
    int source_dir_count;
    char include_dirs[8][256];
    int include_dir_count;
    char lib_dirs[8][256];
    int lib_dir_count;
    char libs[8][256];
    int lib_count;
} build_section_t;

// Struct to store dependencies section data
typedef struct {
    dependency_t dependencies[32];
    int dependencies_count;
} dependencies_section_t;

// Struct to hold project config data from craft.toml
typedef struct {
    project_section_t project;
    build_section_t build;
    dependencies_section_t dependencies;
} project_config_t;

// Struct to hold data about a key in craft.toml
typedef struct {
    const char* name;
    int required;
} config_key_t;

// Checks if given path is a Craft project
int is_craft_project(const char* path);

// Gets the root path of a project where craft.toml is present with project section
int get_project_root(char* buffer, size_t buffer_size, const char* cwd);

// Checks a section for any unknown keys and prints an error or warning message
int check_unknown_keys(toml_datum_t section, const char* section_name, const config_key_t* valid_keys, int num_keys);

// Loads values from a project craft.toml into a project_config_t struct
int load_project_config(project_config_t* config, const char* project_root);

// Checks if basic required keys and sections are present in a project craft.toml
int validate_project_config(project_config_t* config);

// Generates a craft.toml file for a project based on project config values
int generate_craft_toml(const char* project_root, project_config_t* config);

#endif // CRAFT_TOML_H
