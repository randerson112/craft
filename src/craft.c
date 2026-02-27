#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "gen.h"
#include "project.h"
#include "build.h"
#include "run.h"
#include "compile.h"
#include "init.h"
#include "help.h"
#include "clean.h"

// Prints craft info and usage
void printInfo()
{
    fprintf(stdout, "Craft - A lightweight C++ development tool\n\n");
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft <COMMAND> [ARGS]\n\n");

    fprintf(stdout, "Available Commands:\n");
    fprintf(stdout, "    project <path>       Create a new project at the given path\n");
    fprintf(stdout, "    init [path]          Initialize a new project in the current or specified directory\n");
    fprintf(stdout, "    build                Build the project in the current directory\n");
    fprintf(stdout, "    compile <src> [out]  Compile a single source file into an executable\n");
    fprintf(stdout, "    run <exe>            Run a compiled executable\n");
    fprintf(stdout, "    gen <file>           Generate files with starter boilerplate (.hpp, .cpp, CMakeLists.txt)\n");
    fprintf(stdout, "    clean                Remove the build directory and all compiled artifacts\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft project MyApp\n");
    fprintf(stdout, "    craft init\n");
    fprintf(stdout, "    craft build\n");
    fprintf(stdout, "    craft compile main.cpp\n");
    fprintf(stdout, "    craft run main\n");
    fprintf(stdout, "    craft gen CMakeLists.txt\n");
    fprintf(stdout, "    craft clean\n\n");

    fprintf(stdout, "Tip: Run 'craft help <command>' for more info on a specific command.\n");
    fprintf(stdout, "     Run 'craft help' to see this message again.\n");
}

int main(int argc, char* argv[])
{
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to getting current working directory\n");
        return -1;
    }

    // No command given
    if (argc < 2)
    {
        printInfo();
        return 0;
    }

    // Get command
    const char* command = argv[1];

    // Help
    if (strcmp(command, "help") == 0)
    {
        // No command specified
        if (argc < 3)
        {
            printInfo();
            return 0;
        }

        // Get command to display help for
        const char* helpCommand = argv[2];

        // print details of command
        return printCommandHelp(helpCommand);
    }

    // Clean
    if (strcmp(command, "clean") == 0) {
        
        // To many arguments
        if (argc > 2) {
            fprintf(stderr, "Error: Too many arguments.\n\n");
            fprintf(stdout, "Usage: craft clean\n\n");
            fprintf(stdout, "Run 'craft help clean' for more information.\n");
            return -1;
        }

        // Remove build directory
        // Returns 0 if successful, -1 if failed
        return clean(cwd);
    }

    // Compile
    if (strcmp(command, "compile") == 0)
    {
        // No file specified for compile
        if (argc < 3)
        {
            fprintf(stderr, "Error: No source file specified.\n\n");
            fprintf(stdout, "Usage: craft compile <source> [output]\n\n");
            fprintf(stdout, "Run 'craft help compile' for more information.\n");
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

    // Run
    if (strcmp(command, "run") == 0)
    {
        // No executable specified
        if (argc < 3)
        {
            fprintf(stderr, "Error: No executable specified.\n\n");
            fprintf(stdout, "Usage: craft run <executable>\n\n");
            fprintf(stdout, "Run 'craft help run' for more information.\n");
            return -1;
        }

        // Get name of executable
        const char* executableName = argv[2];

        // Attempt to run the executable
        // Returns 0 if successful and -1 if failed
        return runExecutable(cwd, executableName);
    }

    // Generate
    if (strcmp(command, "gen") == 0)
    {
        // No file specified to generate
        if (argc < 3)
        {
            fprintf(stderr, "Error: No file specified to generate.\n\n");
            fprintf(stdout, "Usage: craft gen <file>\n\n");
            fprintf(stdout, "Run 'craft help gen' for more information.\n");
            return -1;   
        }

        // Get file to generate
        const char* file = argv[2];

        // Get file name
        char filename[64];
        stripExtension(file, filename);

        // Get file extension
        char extension[8];
        getExtension(file, extension, sizeof(extension));

        // Generate file
        // Returns 0 if successful, -1 if failed
        return gen(filename, extension, cwd);
    }

    // Project
    if (strcmp(command, "project") == 0)
    {
        // No project path specified
        if (argc < 3)
        {
            fprintf(stderr, "Error: No project path specified.\n\n");
            fprintf(stdout, "Usage: craft project <path>\n\n");
            fprintf(stdout, "Run 'craft help project' for more information.\n");
            return -1;
        }

        // Get project path
        const char* path = argv[2];

        // Create new directory at path and project inside
        // Returns 0 if successful, -1 if failed
        return createNewProject(cwd, path);
    }

    // Init
    if (strcmp(command, "init") == 0)
    {
        // No path specified, initialize in current directory
        if (argc < 3)
        {
            return initProjectInCwd(cwd);
        }

        // Get path argument
        const char* path = argv[2];

        // Init in specified directory
        return initProjectAtPath(cwd, path);
    }

    // Build
    if (strcmp(command, "build") == 0)
    {
        // Build project in current directory
        // Returns 0 if successful, -1 if failed
        return buildProject(cwd);
    }

    // If we get here, unknown command
    fprintf(stderr, "Error: Unknown command: %s\n\n", argv[1]);
    fprintf(stdout, "Tip: Run 'craft help' for command info and examples\n");
    return -1;
}