#include "help.h"
#include <stdio.h>
#include <string.h>

int printCommandHelp(const char* command)
{
    if (strcmp(command, "project") == 0)
    {
        fprintf(stdout, "Command:\n");
        fprintf(stdout, "   craft project - Create a new project in the specified directory.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    The 'project' command generates a new project structure at the given path.\n");
        fprintf(stdout, "    It creates the directory and adds boilerplate files such as\n");
        fprintf(stdout, "    CMakeLists.txt, a source folder, an include folder, and a starter main.cpp.\n");
        fprintf(stdout, "    Use this when beginning a brand-new project from scratch.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft project <path>\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <path>   Path where the new project directory will be created.\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft project MyApp\n");
    }

    else if (strcmp(command, "init") == 0)
    {
        fprintf(stdout, "Command:\n");
        fprintf(stdout, "    craft init - Initialize a new project in the current or specified directory.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    The 'init' command sets up a project structure inside an existing directory.\n");
        fprintf(stdout, "    Unlike 'project', it does not create the directory itself, but instead\n");
        fprintf(stdout, "    assumes it already exists. Craft boilerplate files such as CMakeLists.txt,\n");
        fprintf(stdout, "    a source folder, an include folder, and main.cpp are generated inside the target folder.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft init [path]\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    [path]   Optional path where the project should be initialized.\n\n");

        fprintf(stdout, "Examples:\n");
        fprintf(stdout, "    craft init\n");
        fprintf(stdout, "    craft init projects/MyProject\n");
    }

    else if (strcmp(command, "build") == 0)
    {
        fprintf(stdout, "Command:\n");
        fprintf(stdout, "    craft build - Build the project in the current working directory.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    The 'build' command creates a build folder and compiles all source files in the\n");
        fprintf(stdout, "    current project using the CMake build system. It looks for a CMakeLists.txt and\n");
        fprintf(stdout, "    other configuration files in the current directory. Use this to build your\n");
        fprintf(stdout, "    entire project after initialization or modifications.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft build\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    (none)\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft build\n");
    }

    else if (strcmp(command, "compile") == 0)
    {
        fprintf(stdout, "Command:\n");
        fprintf(stdout, "    craft compile - Compile a single source file into an executable.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    The 'compile' command is useful for quickly compiling and testing a single\n");
        fprintf(stdout, "    source file without needing a full project structure. By default, the output\n");
        fprintf(stdout, "    executable will share the same name as the source file (without extension),\n");
        fprintf(stdout, "    but you can provide a custom output name.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft compile <source> [output]\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <source>   The source file to compile (required).\n");
        fprintf(stdout, "    [output]   Optional name for the output executable.\n");
        fprintf(stdout, "               Defaults to the source file name without extension.\n\n");

        fprintf(stdout, "Examples:\n");
        fprintf(stdout, "    craft compile main.cpp\n");
        fprintf(stdout, "    craft compile main.cpp app\n");
    }

    else if (strcmp(command, "run") == 0)
    {
        fprintf(stdout, "Command:\n");
        fprintf(stdout, "    craft run - Run an executable from the current directory.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    The 'run' command executes a previously compiled binary. It assumes the\n");
        fprintf(stdout, "    executable exists in the current working directory (or a build directory\n");
        fprintf(stdout, "    if that's where output is configured). Use this after 'build' or 'compile'\n");
        fprintf(stdout, "    to quickly run your program.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft run <executable>\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <executable>   The name of the executable to run (required).\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft run app\n");
    }

    else if (strcmp(command, "gen") == 0)
    {
        fprintf(stdout, "Command:\n");
        fprintf(stdout, "    craft gen - Generate boilerplate files (header, source, or CMakeLists).\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    The 'gen' command quickly creates common files with boilerplate needed in C++\n");
        fprintf(stdout, "    projects. Supported outputs include C++ header files (.hpp), C++ source\n");
        fprintf(stdout, "    files (.cpp), and CMakeLists.txt. Source and header files will also be placed\n");
        fprintf(stdout, "    in respective folders if they exist. If a file already exists, Craft will not overwrite it\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft gen <file>\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <file>   The file to generate. Must be .hpp, .cpp, or CMakeLists.txt.\n\n");

        fprintf(stdout, "Examples:\n");
        fprintf(stdout, "    craft gen utils.hpp\n");
        fprintf(stdout, "    craft gen utils.cpp\n");
        fprintf(stdout, "    craft gen CMakeLists.txt\n");
    }

    else if (strcmp(command, "clean") == 0) {
        fprintf(stdout, "Command:\n");
        fprintf(stdout, "    craft clean - Remove the build directory and all compiled artifacts.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    The 'clean' command deletes the build directory generated by 'craft build',\n");
        fprintf(stdout, "    removing all compiled objects, executables, and CMake cache files. If no\n");
        fprintf(stdout, "    build directory exists, Craft will notify you that there is nothing to clean.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft clean\n\n");
    }

    else
    {
        fprintf(stderr, "Error: Unknown command: %s\n", command);
        fprintf(stdout, "Run 'craft help' to see all available commands.\n");
        return -1;
    }

    return 0;
}