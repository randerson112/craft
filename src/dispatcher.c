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

int dispatch(command_t* command_data) {
    // Get command
    const char* command = command_data->name;

    if (strcmp(command, "project") == 0) {
        return project(command_data);
    }
    
    if (strcmp(command, "init") == 0) {
        return init(command_data);
    }
    
    if (strcmp(command, "build") == 0) {
        return build();
    }
    
    if (strcmp(command, "compile") == 0) {
        return compile(command_data);
    }
    
    if (strcmp(command, "run") == 0) {
        return run(command_data);
    }
    
    if (strcmp(command, "gen") == 0) {
        return gen(command_data);
    }
    
    if (strcmp(command, "clean") == 0) {
        return clean();
    }
    
    if (strcmp(command, "help") == 0) {
        return help(command_data);
    }

    if (strcmp(command, "template") == 0) {
        return handle_template(command_data);
    }

    if (strcmp(command, "config") == 0) {
        return handle_config(command_data);
    }

    if (strcmp(command, "add") == 0) {
        return add(command_data);
    }

    return -1;
}