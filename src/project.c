#include "project.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "sys/stat.h"

int createNewProject(const char* cwd, const char* name)
{
    // Make include directory
    char includeDir[256];
    snprintf(includeDir, sizeof(includeDir), "%s/include", cwd);
    if (mkdir(includeDir, 0755) == 0)
    {
        printf("Generated include directory\n");
    }
    else
    {
        fprintf(stderr, "Failed to create include directory\n");
        return -1;
    }

    // Make src directory
    char srcDir[256];
    snprintf(srcDir, sizeof(srcDir), "%s/src", cwd);
    if (mkdir(srcDir, 0755) == 0)
    {
        printf("Generated src directory\n");
    }
    else
    {
        fprintf(stderr, "Failed to create src directory\n");
        return -1;
    }

    // Make default main.cpp
    char mainFilePath[256];
    snprintf(mainFilePath, sizeof(mainFilePath), "%s/src/main.cpp", cwd);

    FILE* mainFile = fopen(mainFilePath, "w");
    if (mainFile == NULL)
    {
        fprintf(stderr, "Failed to generate main.cpp\n");
        return -1;
    }

    // Default source file contents if no corresponding header
    fprintf(mainFile, "#include <iostream>\n\n");
    fprintf(mainFile, "int main(int argc, char* argv[]) {\n");
    fprintf(mainFile, "   std::cout << \"This is my %s project!\" << std::endl;\n", name);
    fprintf(mainFile, "}");
    printf("Generated main.cpp\n");

    // Make default CMakeLists.txt
    // Project name
    char projectName[64];
    strcpy(projectName, name);
    projectName[0] = toupper(projectName[0]);

    // Executable name
    char executableName[64];
    strcpy(executableName, name);

    // Create CMakeLists.txt file
    char cmakePath[256];
    snprintf(cmakePath, sizeof(cmakePath), "%s/CMakeLists.txt", cwd);

    FILE* cmakeFile = fopen(cmakePath, "w");
    if (cmakeFile == NULL)
    {
        fprintf(stderr, "Failed to generate CMakeLists.txt\n");
        return -1;
    }

    // Write default content
    fprintf(cmakeFile, "cmake_minimum_required(VERSION 3.10)\n");
    fprintf(cmakeFile, "project(%s)\n\n", projectName);
    fprintf(cmakeFile, "set(CMAKE_CXX_STANDARD 17)\n\n");
    fprintf(cmakeFile, "include_directories(include)\n\n");
    fprintf(cmakeFile, "add_executable(%s src/main.cpp)", executableName);
    printf("Generated CMakeLists.txt\n");

    return 0;
}
