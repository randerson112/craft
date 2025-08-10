#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define compiler and run prefix/suffix for each platform
#ifdef __APPLE__
#define COMPILER "clang++ "
#define RUNPREFIX "./"
#define RUNSUFFIX ""
#endif

#ifdef _WIN32
#define COMPILER "g++ "
#define RUNPREFIX ""
#define RUNSUFFIX ".exe"
#endif

// Returns a string file name without the extension
void stripExtension(const char* file, char* strippedFile)
{
    int i = 0;
    while (file[i] != '\0' && file[i] != '.')
    {
        strippedFile[i] = file[i];
        i++;
    }
    strippedFile[i] = '\0';
}

// Compile a C++ source file
int compileFile(const char* sourceFile, const char* outputFile)
{
    // Calculate required buffer size for command
    size_t commandSize = strlen(COMPILER) + strlen(sourceFile) + strlen(" -o ") + strlen(outputFile) + 1;
    char* command = malloc(commandSize);
    
    if (command == NULL) {
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

// Run a binary executable
int runFile(const char* exeFile)
{
    // Calculate required buffer size for command
    size_t commandSize = strlen(RUNPREFIX) + strlen(exeFile) + strlen(RUNSUFFIX) + 1;
    char* runCommand = malloc(commandSize);
    
    if (runCommand == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return -1;
    }
    
    snprintf(runCommand, commandSize, "%s%s%s", RUNPREFIX, exeFile, RUNSUFFIX);

    printf("Running...\n");
    if (system(runCommand) == 0)
    {
        // Run was successful
        printf("Run successful!\n");
        free(runCommand);
        return 0;
    }
    else
    {
        // Run failed
        fprintf(stderr, "Executable failed to run!\n");
        free(runCommand);
        return -1;
    }
}

// Entry point
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Missing command.\n");
        printf("What would you like craft to do?\n");
        printf("Usage:\n");
        printf("craft compile <source file>\n");
        printf("craft run <executable>\n");
        return -1;
    }

    if (strcmp(argv[1], "compile") == 0)
    {
        if (argc < 3)
        {
            printf("No source file specified.\n");
            printf("Usage:\n");
            printf("craft compile <source file>\n");
            printf("or\n");
            printf("craft compile <source file> <output file>\n");
            return -1;
        }

        // Get source file name
        const char* sourceFile = argv[2];

        // Get output file name
        char outputFile[256];
        if (argc < 4)
        {
            // Output file not specified, create one
            stripExtension(sourceFile, outputFile);
        }
        else
        {
            // Get specified output file
            strncpy(outputFile, argv[3], sizeof(outputFile) - 1);
            outputFile[sizeof(outputFile) - 1] = '\0';
        }

        // Compile
        // Returns 0 if successful and -1 if failed
        return compileFile(sourceFile, outputFile);
    }

    if (strcmp(argv[1], "run") == 0)
    {
        if (argc < 3)
        {
            printf("No executable specified.\n");
            printf("Usage:\n");
            printf("craft run <executable>\n");
            return -1;
        }

        // Get name of executable
        const char* executableName = argv[2];

        // Attempt to run the executable
        // Returns 0 if successful and -1 if failed
        return runFile(executableName);
    }

    // If we get here, unknown command
    printf("Unknown command: %s\n", argv[1]);
    printf("Usage:\n");
    printf("craft compile <source file>\n");
    printf("craft run <executable>\n");
    return -1;
}
