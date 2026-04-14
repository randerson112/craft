#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

#include "utils.h"
#include "tomlc17.h"
#include "parser.h"
#include "deps.h"

// Struct to hold global craft config data from config.toml
typedef struct {
    char language[8];
    int c_standard;
    int cpp_standard;
    char template[16];
} craft_config_t;

// Loads values from config.toml into a craft_config_t struct
int load_global_config(craft_config_t* config);

// Config command handler
int handle_config(const command_t* command_data);

#endif // CONFIG_H
