#ifndef COMPILE_H
#define COMPILE_H

// define compiler and run prefix/suffix for each platform
#ifdef __APPLE__
#define C_COMPILER "clang"
#define CPP_COMPILER "clang++"
#define RUNPREFIX "./"
#define RUNSUFFIX ""
#endif

#ifdef _WIN32
#define C_COMPILER "gcc"
#define CPP_COMPILER "g++"
#define RUNPREFIX ""
#define RUNSUFFIX ".exe"
#endif

#ifdef __linux__
#define C_COMPILER "gcc"
#define CPP_COMPILER "g++"
#define RUNPREFIX "./"
#define RUNSUFFIX ""
#endif

#include "parser.h"

// compile command handler
int compile(command_t* command_data);

#endif // COMPILE_H
