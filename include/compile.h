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

#ifdef __linux__
#define COMPILER "g++ "
#define RUNPREFIX "./"
#define RUNSUFFIX ""
#endif

// compile command handler
int compile(const char* source_arg, char* output_arg);

#endif // COMPILE_H
