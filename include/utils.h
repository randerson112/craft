#ifndef UTILS_H
#define UTILS_H

// Strips extension off a file and writes it to stripped file buffer
void stripExtension(const char* file, char* strippedFile);

// Gets the extension of a file and writes it to extension buffer
void getExtension(const char* file, char* extension, int extensionSize);

// Check if a file exists
int fileExists(const char* path);

// Check if a directory exists
int dirExists(const char* path);

// Gets name of base directory from a full path
const char* getBaseName(const char* path);

// Creates starter project files and directorys at the given path
int createCppProject(const char* path, const char* name);

#endif