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

// Removes a directory recursively, called by the two functions below
int removeDirRecursive(const char* path, unsigned int* count, unsigned int* bytes);

// Removes a directory without keeping track of counts
int removeDir(const char* path);

// Removes a directory and keeps track of counts
int removeDirCount(const char* path, unsigned int* count, unsigned int* bytes);

// Gets name of base directory from a full path
const char* getBaseName(const char* path);

// Creates starter project files and directorys at the given path
int createCppProject(const char* path, const char* name);

// Formats a number of bytes into a string with appropriate conversion
void formatBytes(unsigned int bytes, char* buffer, unsigned int bufferSize);

#endif