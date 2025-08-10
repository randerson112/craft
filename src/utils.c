#include "utils.h"
#include <unistd.h>

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

int fileExists(const char* path)
{
    return access(path, F_OK) == 0;
}