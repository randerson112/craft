#include "gen.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// TODO: Implement source file generation
// - Create .cpp file with given filename
// - Add include statement for corresponding header
// - Add basic function stubs or class implementation
int generateSource(const char* filename, const char* cwd)
{
    printf("Generating source file: %s\n", filename);
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