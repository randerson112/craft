#include "dispatcher.h"
#include "string.h"
#include "gen.h"
#include "project.h"
#include "build.h"
#include "run.h"
#include "compile.h"
#include "init.h"
#include "help.h"
#include "clean.h"
#include "template.h"
#include "config.h"
#include "add.h"
#include "remove.h"
#include "update.h"

int dispatch(const command_t* command_data) {
    
    // Get command name
    const char* command = command_data->name;

    // Change backslashes to forward slashes in paths
    for (int i = 0; i < command_data->arg_count; i++) {
        normalize_path(command_data->args[i]);
    }
    for (int i = 0; i < command_data->option_count; i++) {
        if (command_data->options[i].arg) {
            normalize_path(command_data->options[i].arg);
        }
    }

    // Dispatch to respective command handler
    if (strcmp(command, "project") == 0)  return handle_project(command_data);
    if (strcmp(command, "init") == 0)     return handle_init(command_data);
    if (strcmp(command, "build") == 0)    return handle_build();
    if (strcmp(command, "compile") == 0)  return handle_compile(command_data);
    if (strcmp(command, "run") == 0)      return handle_run(command_data);
    if (strcmp(command, "gen") == 0)      return handle_gen(command_data);
    if (strcmp(command, "clean") == 0)    return handle_clean();
    if (strcmp(command, "help") == 0)     return handle_help(command_data);
    if (strcmp(command, "template") == 0) return handle_template(command_data);
    if (strcmp(command, "config") == 0)   return handle_config(command_data);
    if (strcmp(command, "add") == 0)      return handle_add(command_data);
    if (strcmp(command, "remove") == 0)   return handle_remove(command_data);
    if (strcmp(command, "update") == 0)   return handle_update(command_data);

    return -1;
}