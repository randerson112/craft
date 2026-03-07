#ifndef CONFIG_H
#define CONFIG_H

#include "utils.h"
#include "tomlc17.h"
#include <stdio.h>
#include "parser.h"

typedef struct {
    char language[8];
    int c_standard;
    int cpp_standard;
    char template[16];
} craft_config_t;

typedef struct {

    // [project]
    char name[32];
    char version[16];
    char language[8];
    int c_standard;
    int cpp_standard;
    
    // [build]
    char build_type[32];
    char source_dirs[8][256];
    int source_dir_count;
    char include_dirs[8][256];
    int include_dir_count;
    char lib_dirs[8][256];
    int lib_dir_count;
    char libs[8][256];
    int lib_count;
} project_config_t;

// Default values to populate struct with if config file is lost or has errors
extern const craft_config_t defaults;

// Loads values from config.toml into a craft_config_t struct
int load_global_config(craft_config_t* config);

// Loads values from a project craft.toml into a project_config_t struct
int load_project_config(project_config_t* config, const char* project_path);

// Generates a craft.toml file for a project based on project config values
int generate_craft_toml(const char* project_path, project_config_t* config);

// Checks if basic required keys and sections are present in a project craft.toml
int validate_project_config(project_config_t* config);

// Config command handler
int handle_config(command_t* command_data);

#endif // CONFIG_H
