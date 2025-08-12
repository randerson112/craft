#include "utils.h"
#include <unistd.h>
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

int fileExists(const char* path)
{
    return access(path, F_OK) == 0;
}