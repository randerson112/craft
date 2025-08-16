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

// Prints basic craft command usage
void printUsage()
{
    printf("Usage:\n");
    printf("craft project <project name>\n\n");
    printf("craft compile <source file>\n\n");
    printf("craft run <executable>\n\n");
    printf("craft gen <header>.hpp\n");
    printf("          <source>.cpp\n");
    printf("          CMakeLists.txt\n\n");
    printf("craft build\n");
}

int main(int argc, char* argv[])
{
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error getting current working directory\n");
        return -1;
    }

    // No command given
    if (argc < 2)
    {
        printf("Missing command.\n");
        printf("What would you like craft to do?\n");
        printUsage();
        return -1;
    }

    // Get command
    const char* command = argv[1];

    // Compile
    if (strcmp(command, "compile") == 0)
    {
        // No file specified for compile
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

    // Run
    if (strcmp(command, "run") == 0)
    {
        // No executable specified
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

    // Generate
    if (strcmp(command, "gen") == 0)
    {
        // No file specified to generate
        if (argc < 3)
        {
            printf("No file given to generate\n");
            printf("Usage:\n");
            printf("craft gen <header file>.hpp\n");
            printf("craft gen <source file>.cpp\n");
            printf("craft gen CMakeLists.txt\n");
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
        // No project name specified
        if (argc < 3)
        {
            printf("No project name specified\n");
            printf("Usage:\n");
            printf("craft project <project name>\n");
            return -1;
        }

        // Get project name
        const char* projectName = argv[2];

        // Create new project with given name
        // Returns 0 if successful, -1 if failed
        return createNewProject(cwd, projectName);
    }

    // Build
    if (strcmp(command, "build") == 0)
    {
        // Build project in current directory
        // Returns 0 if successful, -1 if failed
        return buildProject(cwd);
    }

    // If we get here, unknown command
    printf("Unknown command: %s\n", argv[1]);
    printUsage();
    return -1;
}
