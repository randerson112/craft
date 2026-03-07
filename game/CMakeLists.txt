cmake_minimum_required(VERSION 3.10)
project(MyExecutable)

set(CMAKE_C_STANDARD 99)

add_executable(MyExecutable src/main.c)
target_include_directories(MyExecutable PRIVATE include/)