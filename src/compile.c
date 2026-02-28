#include "compile.h"
#include "utils.h"
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"

// Compiles a source file and writes the executable to the output file
int compileFile(const char* sourceFile, const char* outputFile)
{
    // Calculate required buffer size for command
    size_t commandSize = strlen(COMPILER) + strlen(sourceFile) + strlen(" -o ") + strlen(outputFile) + 1;
    char* command = malloc(commandSize);
    
    if (command == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        return -1;
    }
    
    // Write compile command
    snprintf(command, commandSize, "%s%s -o %s", COMPILER, sourceFile, outputFile);

    // Run the command to compile source file
    fprintf(stdout, "Compiling...\n");
    if (system(command) == 0)
    {
        // Compilation successful
        fprintf(stdout, "Source file compiled successfully!\n");
        free(command);
        return 0;
    }
    else
    {
        // Compilation failed
        fprintf(stderr, "Error: Compilation failed!\n");
        free(command);
        return -1;
    }
}

int compile(const char* source_arg, char* output_arg) {
    // Get optional output file name
    char outputFile[256];
    if (strlen(output_arg) == 0)
    {
        // Output file not specified, create default one
        stripExtension(source_arg, output_arg);
    }
    else
    {
        // Get specified output file
        strncpy(outputFile, output_arg, sizeof(outputFile) - 1);
        outputFile[sizeof(outputFile) - 1] = '\0';
    }

    // Compile
    // Returns 0 if successful and -1 if failed
    return compileFile(source_arg, outputFile);
}