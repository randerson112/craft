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

int dispatch(command_t* command_data, const char* cwd) {
    const char* command = command_data->name;

    if (strcmp(command, "project") == 0) {
        const char* project_name = command_data->args[0];

        return project(project_name, cwd);
    }
    
    if (strcmp(command, "init") == 0) {
        const char* path = command_data->args[0];

        return init(path, cwd);
    }
    
    if (strcmp(command, "build") == 0) {
        return build(cwd);
    }
    
    if (strcmp(command, "compile") == 0) {
        const char* source = command_data->args[0];
        char* output = command_data->args[1];

        return compile(source, output);
    }
    
    if (strcmp(command, "run") == 0) {
        const char* executable_name = command_data->args[0];

        return run(executable_name, cwd);
    }
    
    if (strcmp(command, "gen") == 0) {
        const char* file = command_data->args[0];

        return gen(file, cwd);
    }
    
    if (strcmp(command, "clean") == 0) {
        return clean(cwd);
    }
    
    if (strcmp(command, "help") == 0) {
        const char* command = command_data->args[0];

        return help(command);
    }

    return -1;
}