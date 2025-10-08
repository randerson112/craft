#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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

void getExtension(const char* file, char* extension, int extensionSize)
{
    // Find where dot is
    char* dot = strrchr(file, '.');
    if (dot != NULL)
    {
        // Write contents after dot to extension buffer
        snprintf(extension, extensionSize, "%s", dot + 1);
    }
    else
    {
        // If no dot found then write null terminator
        *extension = '\0';
    }
}

// Check if a path exists and is a file
int fileExists(const char* path)
{
    struct stat statbuff;
    if (stat(path, &statbuff) == 0 && S_ISREG(statbuff.st_mode)) {
        // File exists
        return 1;
    }

    // File does not exist
    return 0;
}

// Check if a path exists and is a directory
int dirExists(const char* path)
{
    struct stat statbuff;
    if (stat(path, &statbuff) == 0 && S_ISDIR(statbuff.st_mode)) {
        // Directory exists
        return 1;
    }
    
    // Directory does not exist
    return 0;
}

const char* getBaseName(const char* path)
{
    const char* lastSlash = strrchr(path, '/');
    if (lastSlash)
    {
        return lastSlash + 1;
    }

    return path;
}

int createCppProject(const char* path, const char* name)
{
    // Make include directory if one does not exist
    char includeDir[256];
    snprintf(includeDir, sizeof(includeDir), "%s/include", path);
    if (!dirExists(includeDir))
    {
        if (mkdir(includeDir, 0755) == 0)
        {
            fprintf(stdout, "Generated include directory\n");
        }
        else
        {
            fprintf(stderr, "Error: Failed to create include directory\n");
            return -1;
        }
    }

    // Make src directory if it does not exist
    char srcDir[256];
    snprintf(srcDir, sizeof(srcDir), "%s/src", path);
    if (!dirExists(srcDir))
    {
        if (mkdir(srcDir, 0755) == 0)
        {
            fprintf(stdout, "Generated src directory\n");
        }
        else
        {
            fprintf(stderr, "Error: Failed to create src directory\n");
            return -1;
        }
    }

    // Make default main.cpp if there is none
    char mainFilePath[256];
    snprintf(mainFilePath, sizeof(mainFilePath), "%s/src/main.cpp", path);

    if (!fileExists(mainFilePath))
    {
        FILE* mainFile = fopen(mainFilePath, "w");
        if (mainFile == NULL)
        {
            fprintf(stderr, "Error: Failed to generate main.cpp\n");
            return -1;
        }

        // Default source file contents if no corresponding header
        fprintf(mainFile, "#include <iostream>\n\n");
        fprintf(mainFile, "int main(int argc, char* argv[]) {\n");
        fprintf(mainFile, "   std::cout << \"This is my %s project!\" << std::endl;\n", name);
        fprintf(mainFile, "}");
        fprintf(stdout, "Generated main.cpp\n");
    }

    // Make default CMakeLists.txt if none exists
    char cmakePath[256];
    snprintf(cmakePath, sizeof(cmakePath), "%s/CMakeLists.txt", path);

    if (!fileExists(cmakePath))
    {
        // Project name
        char projectName[64];
        strcpy(projectName, name);
        projectName[0] = toupper(projectName[0]);

        // Executable name
        char executableName[64];
        strcpy(executableName, name);

        FILE* cmakeFile = fopen(cmakePath, "w");
        if (cmakeFile == NULL)
        {
            fprintf(stderr, "Error: Failed to generate CMakeLists.txt\n");
            return -1;
        }

        // Write default content
        fprintf(cmakeFile, "cmake_minimum_required(VERSION 3.10)\n");
        fprintf(cmakeFile, "project(%s)\n\n", projectName);
        fprintf(cmakeFile, "set(CMAKE_CXX_STANDARD 17)\n\n");
        fprintf(cmakeFile, "include_directories(include)\n\n");
        fprintf(cmakeFile, "add_executable(%s src/main.cpp)", executableName);
        fprintf(stdout, "Generated CMakeLists.txt\n");
    }

    return 0;
}