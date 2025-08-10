# define minimum cmake version required
cmake_minimum_required(VERSION 3.20)

# define project name
project(craft)

# define c standard
set(CMAKE_C_STANDARD 99)

# create executable
add_executable(craft src/craft.c)