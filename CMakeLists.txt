# define minimum cmake version required
cmake_minimum_required(VERSION 3.20)

# define project name
project(craft)

# define c standard
set(CMAKE_C_STANDARD 99)

#define executable name
set(PROJECT_EXE craft)

# create executable with source files
add_executable(${PROJECT_EXE}
src/main.c
src/utils.c
src/gen.c
src/project.c
src/build.c
src/run.c
src/compile.c
src/init.c
src/help.c
src/clean.c
src/parser.c
src/dispatcher.c
src/template.c
src/config.c)

# define include directories
target_include_directories(${PROJECT_EXE} PUBLIC include PRIVATE lib/tomlc17)
target_link_libraries(${PROJECT_EXE} PRIVATE ${CMAKE_SOURCE_DIR}/lib/tomlc17/libtomlc17.a)