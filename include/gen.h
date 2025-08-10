#ifndef GEN_H
#define GEN_H

// Generates a header file in the current working directory
int generateHeader(const char* filename, const char* cwd);

// Generates a source file in the current working directory
int generateSource(const char* filename, const char* cwd);

// Generates a CMakeLists.txt file in the current working directory
int generateCMakeLists(const char* cwd);

#endif // GEN_H
