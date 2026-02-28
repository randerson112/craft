#include "gen.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

// Generates a header file with starter code, placing it in include directory or current directory
int generateHeader(const char* filename, const char* cwd)
{
    // Determine where to place the header file
    char includePath[256];
    snprintf(includePath, sizeof(includePath), "%s/include", cwd);
    
    char fullPath[256];
    if (dirExists(includePath))
    {
        // Include directory exists, place header there
        snprintf(fullPath, sizeof(fullPath), "%s/%s.hpp", includePath, filename);
        fprintf(stdout, "Placing header in include directory\n");
    }
    else
    {
        // Include directory doesn't exist, place in current directory
        snprintf(fullPath, sizeof(fullPath), "%s/%s.hpp", cwd, filename);
        fprintf(stdout, "Placing header in current directory\n");
    }

    // Check if header with same name already exists
    if (fileExists(fullPath))
    {
        fprintf(stderr, "Error: Header with that name already exists\n");
        return -1;
    }

    // Create header file with given file name
    fprintf(stdout, "Generating header file: %s.hpp\n", filename);

    FILE* header = fopen(fullPath, "w");
    if (header == NULL)
    {
        fprintf(stderr, "Error: Failed to create header file: %s.hpp\n", filename);
        return -1;
    }

    // Create header macro name
    char headerMacro[256];
    snprintf(headerMacro, sizeof(headerMacro), "%s_HPP", filename);
    
    // Convert to uppercase for macro name
    for (int i = 0; headerMacro[i] != '\0'; i++)
    {
        headerMacro[i] = toupper(headerMacro[i]);
    }

    // Write default contents to header file
    fprintf(header, "#ifndef %s\n", headerMacro);
    fprintf(header, "#define %s\n", headerMacro);
    fprintf(header, "\n// Code here\n\n");
    fprintf(header, "#endif // %s\n", headerMacro);

    fclose(header);
    fprintf(stdout, "%s.hpp successfully generated\n", filename);
    return 0;
}

// Generates a source file with starter code, placing it in src directory or current directory
int generateSource(const char* filename, const char* cwd)
{
    // Check if corresponding header file exists
    char includeDir[256];
    snprintf(includeDir, sizeof(includeDir), "%s/include", cwd);

    char headerPath[256];
    snprintf(headerPath, sizeof(headerPath), "%s/%s.hpp", cwd, filename);

    char includeHeaderPath[256];
    snprintf(includeHeaderPath, sizeof(includeHeaderPath), "%s/%s.hpp", includeDir, filename);
    
    // Determine which header to include
    char* headerToInclude = NULL;
    if (fileExists(headerPath))
    {
        headerToInclude = headerPath;
        fprintf(stdout, "Matching header found in current directory\n");
    }
    else if (dirExists(includeDir) && fileExists(includeHeaderPath))
    {
        headerToInclude = includeHeaderPath;
        fprintf(stdout, "Matching header found in include directory\n");
    }
    else
    {
        fprintf(stdout, "No matching header file found\n");
        // Still creates a default source file with no include
    }

    // Determine where to place source file
    char sourcePath[256];
    snprintf(sourcePath, sizeof(sourcePath), "%s/src", cwd);

    char fullPath[256];
    if (dirExists(sourcePath))
    {
        snprintf(fullPath, sizeof(fullPath), "%s/%s.cpp", sourcePath, filename);
        fprintf(stdout, "Placing source file in src directory\n");
    }
    else
    {
        snprintf(fullPath, sizeof(fullPath), "%s/%s.cpp", cwd, filename);
        fprintf(stdout, "Placing source file in current directory\n");
    }

    // Check if source file with same name already exists
    if (fileExists(fullPath))
    {
        fprintf(stderr, "Error: Source file with that name already exists\n");
        return -1;
    }
    
    // Create source file
    fprintf(stdout, "Generating source file: %s.cpp\n", filename);

    FILE* source = fopen(fullPath, "w");
    if (source == NULL)
    {
        fprintf(stderr, "Error: Failed to create source file: %s.cpp\n", filename);
        return -1;
    }

    // Write source file contents
    if (headerToInclude != NULL)
    {
        // Source file contents with corresponding include
        fprintf(source, "#include \"%s.hpp\"\n\n", filename);
        fprintf(source, "// Add your implementation here\n");
    }
    else
    {
        // Default source file contents if no corresponding header
        fprintf(source, "#include <iostream>\n\n");
        fprintf(source, "int main(int argc, char* argv[]) {\n");
        fprintf(source, "   std::cout << \"Hello World\" << std::endl;\n");
        fprintf(source, "}");
    }

    fclose(source);
    fprintf(stdout, "%s.cpp successfully generated\n", filename);
    return 0;
}

// Generates a CMakeLists.txt with starter code, placing it in current directory
int generateCMakeLists(const char* cwd)
{
    char cmakePath[256];
    snprintf(cmakePath, sizeof(cmakePath), "%s/CMakeLists.txt", cwd);

    // Check if CMakeLists.txt already exists
    if (fileExists(cmakePath))
    {
        fprintf(stderr, "Error: CMakeLists.txt already exists in this directory\n");
        return -1;
    }

    // Generate file content
    fprintf(stdout, "Generating CMakeLists.txt\n");

    FILE* cmakeFile = fopen(cmakePath, "w");
    if (cmakeFile == NULL)
    {
        fprintf(stderr, "Error: Failed to create CMakeLists.txt\n");
        return -1;
    }

    // List of source files
    char* sourceFiles[64];
    int i = 0;
    
    // Check for source files in current directory
    DIR* dir = opendir(cwd);
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && i < 63)
    {
        if (strstr(entry->d_name, ".cpp") != NULL)
        {
            // Add source file to list
            sourceFiles[i] = strdup(entry->d_name);
            i++;
        }
    }
    closedir(dir);

    // Check for source files in src directory
    char srcDir[256];
    snprintf(srcDir, sizeof(srcDir), "%s/src", cwd);

    if (dirExists(srcDir))
    {
        DIR* dir = opendir(srcDir);
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL && i < 63)
        {
            if (strstr(entry->d_name, ".cpp") != NULL)
            {
                // Add src/ prefix to filename before adding to list
                char srcFile[256];
                snprintf(srcFile, sizeof(srcFile), "src/%s", entry->d_name);
                sourceFiles[i] = strdup(srcFile);
                i++;
            }
        }
        closedir(dir);
    }

    // Make project and executable name based on cwd
    char cwdName[64];
    char* name = strrchr(cwd, '/');
    snprintf(cwdName, sizeof(cwdName), "%s", name + 1);
    for (int i = 0; i < sizeof(cwdName); i++)
    {
        cwdName[i] = tolower(cwdName[i]);
    }
    
    char projectName[64];
    strcpy(projectName, cwdName);
    projectName[0] = toupper(projectName[0]);

    char executableName[64];
    strcpy(executableName, cwdName);

    // Project settings
    fprintf(cmakeFile, "cmake_minimum_required(VERSION 3.10)\n");
    fprintf(cmakeFile, "project(%s)\n\n", projectName);
    fprintf(cmakeFile, "set(CMAKE_CXX_STANDARD 17)\n\n");

    // Include directories
    char includePath[256];
    snprintf(includePath, sizeof(includePath), "%s/include", cwd);

    if (dirExists(includePath))
    {
        fprintf(cmakeFile, "include_directories(include)\n\n");
    }

    fprintf(cmakeFile, "add_executable(%s\n", executableName);

    // Add source files
    for (int j = 0; j < i; j++)
    {
        fprintf(cmakeFile, "    %s\n", sourceFiles[j]);
    }
    fprintf(cmakeFile, ")\n");
    
    fclose(cmakeFile);
    fprintf(stdout, "CMakeLists.txt successfully generated\n");

    return 0;
}

int gen(const char* file_arg, const char* cwd)
{
    // Get file name
    char filename[64];
    stripExtension(file_arg, filename);

    // Get file extension
    char extension[8];
    getExtension(file_arg, extension, sizeof(extension));

    // Generate hpp
    if (strcmp(extension, "hpp") == 0)
    {
        return generateHeader(filename, cwd);
    }

    // Generate cpp
    else if (strcmp(extension, "cpp") == 0)
    {
        return generateSource(filename, cwd);
    }

    // Generate CMakeLists.txt
    else if (strcmp(filename, "CMakeLists") == 0 && strcmp(extension, "txt") == 0)
    {
        return generateCMakeLists(cwd);
    }

    // Extension not recognized
    else
    {
        fprintf(stderr, "Error: File extension not supported\n");
        return -1;
    }
}