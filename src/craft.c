#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "gen.h"
#include "project.h"
#include "build.h"
#include "run.h"

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

// Compile a C++ source file
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

// Entry point
int main(int argc, char* argv[])
{
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error getting current working directory\n");
        return -1;
    }

    if (argc < 2)
    {
        printf("Missing command.\n");
        printf("What would you like craft to do?\n");
        printf("Usage:\n");
        printf("craft project <project name>\n\n");
        printf("craft compile <source file>\n\n");
        printf("craft run <executable>\n\n");
        printf("craft gen <header>.hpp\n");
        printf("          <source>.cpp\n");
        printf("          CMakeLists.txt\n\n");
        printf("craft build\n");
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
        return runExecutable(cwd, executableName);
    }

    if (strcmp(argv[1], "gen") == 0)
    {
        if (argc < 3)
        {
            printf("No file given to generate\n");
            printf("Usage:\n");
            printf("craft gen <header file>.hpp\n");
            printf("craft gen <source file>.cpp\n");
            printf("craft gen CMakeLists.txt\n");
            return -1;   
        }

        char* file = argv[2];

        // Get file name
        char filename[64];
        stripExtension(file, filename);

        // Get file extension
        char extension[8];
        getExtension(file, extension, sizeof(extension));

        // Generate file
        return gen(filename, extension, cwd);
    }

    if (strcmp(argv[1], "project") == 0)
    {
        if (argc < 3)
        {
            printf("No project name specified\n");
            printf("Usage:\n");
            printf("craft project <project name>\n");
            return -1;
        }

        char* projectName = argv[2];

        // Create new project with given name
        return createNewProject(cwd, projectName);
    }

    if (strcmp(argv[1], "build") == 0)
    {
        // Build project in current directory
        return buildProject(cwd);
    }

    // If we get here, unknown command
    printf("Unknown command: %s\n", argv[1]);
    printf("Usage:\n");
    printf("craft project <project name>\n\n");
    printf("craft compile <source file>\n\n");
    printf("craft run <executable>\n\n");
    printf("craft gen <header>.hpp\n");
    printf("          <source>.cpp\n");
    printf("          CMakeLists.txt\n\n");
    printf("craft build\n");

    return -1;
}
