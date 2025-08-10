#ifndef UTILS_H
#define UTILS_H

// Strips extension off a file and writes it to stripped file buffer
void stripExtension(const char* file, char* strippedFile);

// Check if a file or directory exists
int fileExists(const char* path);

#endif