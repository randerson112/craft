#include "compile.h"
#include "utils.h"
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "platform.h"

// Compiles a source file and writes the executable to the output file
int compile_file(const char* cwd, const char* source_path, const char* output_path) 
{
    // Get full path to source and output files
    char full_source_path[512];
    char full_output_path[512];
    snprintf(full_source_path, sizeof(full_source_path), "%s/%s", cwd, source_path);
    snprintf(full_output_path, sizeof(full_output_path), "%s/%s", cwd, output_path);

    if (!file_exists(full_source_path)) {
        fprintf(stderr, "Error: Source file '%s' does not exist\n", source_path);
        return -1;
    }

    // Get extension of source file
    char extension[8];
    get_extension(source_path, extension, sizeof(extension));

    // Write compile command
    char command[1024];
    if (strcmp(extension, "c") == 0) {
        snprintf(command, sizeof(command), "%s %s -o %s", C_COMPILER, full_source_path, full_output_path);
    }
    else if (strcmp(extension, "cpp") == 0) {
        snprintf(command, sizeof(command), "%s %s -o %s", CPP_COMPILER, full_source_path, full_output_path);
    }
    else {
        fprintf(stderr, "Error: Cannot compile '%s'\n", source_path);
        fprintf(stderr, "File must have a .c or .cpp extension\n");
        return -1;
    }

    // Run the command to compile source file
    fprintf(stdout, "Compiling...\n");
    if (system(command) == 0)
    {
        // Compilation successful
        fprintf(stdout, "Source file compiled successfully!\n");
        return 0;
    }
    else
    {
        // Compilation failed
        fprintf(stderr, "Error: Compilation failed!\n");
        return -1;
    }
}

int handle_compile(const command_t* command_data) {
    
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Get source path argument
    const char* source_path = command_data->args[0];

    // Get optional output path argument
    if (command_data->arg_count == 2) {
        const char* output_path = command_data->args[1];

        // Compile
        return compile_file(cwd, source_path, output_path);
    }

    // No output path was specified, make default output path
    char output_path[512];
    strip_extension(source_path, output_path);

    // Compile
    return compile_file(cwd, source_path, output_path);
}