#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "string.h"

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