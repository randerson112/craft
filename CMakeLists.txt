# define minimum cmake version required
cmake_minimum_required(VERSION 3.20)

# define project name
project(craft)

# define c standard
set(CMAKE_C_STANDARD 99)

#define executable name
set(PROJECT_EXE craft)

# define include directories
include_directories(include)

# create executable with source files
add_executable(${PROJECT_EXE}
src/craft.c
src/utils.c
src/gen.c)