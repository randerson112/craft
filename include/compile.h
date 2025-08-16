#ifndef COMPILE_H
#define COMPILE_H

// define compiler and run prefix/suffix for each platform
#ifdef __APPLE__
#define COMPILER "clang++ "
#define RUNPREFIX "./"
#define RUNSUFFIX ""
#endif

#ifdef _WIN32
#define COMPILER "g++ "
#define RUNPREFIX ""
#define RUNSUFFIX ".exe"
#endif

// Compile a C++ source file
int compileFile(const char* sourceFile, const char* outputFile);

#endif // COMPILE_H