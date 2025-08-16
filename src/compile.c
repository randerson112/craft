#include "compile.h"
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"

int compileFile(const char* sourceFile, const char* outputFile)
{
    // Calculate required buffer size for command
    size_t commandSize = strlen(COMPILER) + strlen(sourceFile) + strlen(" -o ") + strlen(outputFile) + 1;
    char* command = malloc(commandSize);
    
    if (command == NULL)
    {
        fprintf(stderr, "Memory allocation failed!\n");
        return -1;
    }
    
    // Write compile command
    snprintf(command, commandSize, "%s%s -o %s", COMPILER, sourceFile, outputFile);

    // Run the command to compile source file
    printf("Compiling...\n");
    if (system(command) == 0)
    {
        // Compilation successful
        printf("Source file compiled successfully!\n");
        free(command);
        return 0;
    }
    else
    {
        // Compilation failed
        fprintf(stderr, "Compilation failed!\n");
        free(command);
        return -1;
    }
}