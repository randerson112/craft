cmake_minimum_required(VERSION 3.10)
project(Craft)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(Craft ${SOURCES})

target_include_directories(Craft PRIVATE include lib/tomlc17)

target_link_directories(Craft PRIVATE ${CMAKE_SOURCE_DIR}/lib/tomlc17)

target_link_libraries(Craft PRIVATE tomlc17)

if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeLists.extra.cmake)
    include(${CMAKE_SOURCE_DIR}/CMakeLists.extra.cmake)
endif()
