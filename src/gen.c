#include "gen.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int gen(const char* filename, const char* extension, const char* cwd)
{
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

    // Extension not recognized
    else
    {
        fprintf(stderr, "File extension not supported\n");
        return -1;
    }
}

int generateHeader(const char* filename, const char* cwd)
{
    printf("Generating header file: %s.hpp\n", filename);

    // Create header macro name
    char headerMacro[256];
    snprintf(headerMacro, sizeof(headerMacro), "%s_HPP", filename);
    
    // Convert to uppercase for macro name
    for (int i = 0; headerMacro[i] != '\0'; i++)
    {
        headerMacro[i] = toupper(headerMacro[i]);
    }

    // Check if include directory exists
    char includePath[256];
    snprintf(includePath, sizeof(includePath), "%s/include", cwd);
    
    // Determine where to place the header file
    char fullPath[256];
    if (fileExists(includePath))
    {
        // Include directory exists, place header there
        snprintf(fullPath, sizeof(fullPath), "%s/%s.hpp", includePath, filename);
        printf("Placing header in include directory\n");
    }
    else
    {
        // Include directory doesn't exist, place in current directory
        snprintf(fullPath, sizeof(fullPath), "%s/%s.hpp", cwd, filename);
        printf("Placing header in current directory\n");
    }

    // Create header file with given file name
    FILE* header = fopen(fullPath, "w");
    if (header == NULL)
    {
        fprintf(stderr, "Error creating header file: %s.hpp\n", filename);
        return -1;
    }

    // Write default contents to header file
    fprintf(header, "#ifndef %s\n", headerMacro);
    fprintf(header, "#define %s\n", headerMacro);
    fprintf(header, "\n// Code here\n\n");
    fprintf(header, "#endif // %s\n", headerMacro);

    fclose(header);
    printf("%s.hpp successfully generated\n", filename);
    return 0;
}

int generateSource(const char* filename, const char* cwd)
{
    printf("Generating source file: %s.cpp\n", filename);

    // Check if corresponding header file exists
    char headerPath[256];
    char includeHeaderPath[256];
    
    // Check in current directory first
    snprintf(headerPath, sizeof(headerPath), "%s/%s.hpp", cwd, filename);
    
    // Check in include directory if it exists
    char includeDir[256];
    snprintf(includeDir, sizeof(includeDir), "%s/include", cwd);
    snprintf(includeHeaderPath, sizeof(includeHeaderPath), "%s/%s.hpp", includeDir, filename);
    
    // Determine which header to include
    char* headerToInclude = NULL;
    if (fileExists(headerPath))
    {
        headerToInclude = headerPath;
        printf("Matching header found in current directory\n");
    }
    else if (fileExists(includeDir) && fileExists(includeHeaderPath))
    {
        headerToInclude = includeHeaderPath;
        printf("Matching header found in include directory\n");
    }
    else
    {
        printf("No matching header file found\n");
        // Still creates a default source file with no include
    }

    // Check if src directory exists
    char sourcePath[256];
    snprintf(sourcePath, sizeof(sourcePath), "%s/src", cwd);

    // Determine where to place source file
    char fullPath[256];
    if (fileExists(sourcePath))
    {
        snprintf(fullPath, sizeof(fullPath), "%s/%s.cpp", sourcePath, filename);
        printf("Placing source file in src directory\n");
    }
    else
    {
        snprintf(fullPath, sizeof(fullPath), "%s/%s.cpp", cwd, filename);
        printf("Placing source file in current directory\n");
    }
    
    // Create source file
    FILE* source = fopen(fullPath, "w");
    if (source == NULL)
    {
        fprintf(stderr, "Error creating source file: %s.cpp\n", filename);
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
    printf("%s.cpp successfully generated\n", filename);
    return 0;
}

// TODO: Implement CMakeLists.txt generation
// - Create CMakeLists.txt file in current working directory
// - Add project name and version
// - Add executable target
// - Add source files
// - Add include directories
int generateCMakeLists(const char* cwd)
{
    printf("Generating CMakeLists.txt\n");
    return 0;
}