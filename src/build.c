#include "build.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"
#include "config.h"
#include "cmake.h"
#include <string.h>
#include "deps.h"
#include "platform.h"

// Builds a project by creating a build directory and running cmake
int build_project(const char* cwd)
{
    // Get path to root of the project
    char project_root[512];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    // Regenerate CMakeLists.txt from craft.toml if needed
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }

    if (validate_project_config(&config) != 0) {
        return -1;
    }

    if (cmake_needs_regeneration(project_root)) {
        generate_cmake(project_root, &config);
    }

    // Fetch git dependencies into .craft/deps
    for (int i = 0; i < config.dependencies_count; i++) {
        dependency_t* dep = &config.dependencies[i];

        if (dep->type == DEP_GIT) {
            if (fetch_git_dependency(project_root, dep) != 0) {
                return -1;
            }
        }
    }

    // Create a build directory if it does not exist
    char build_dir[256];
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_root);
    if (!dir_exists(build_dir))
    {
        if (mkdir(build_dir, 0755) == 0)
        {
            fprintf(stdout, "Generated build directory\n");
        }
        else
        {
            fprintf(stderr, "Error: Failed to create build directory\n");
            return -1;
        }
    }
    else {
        fprintf(stdout, "Build directory found\n");
    }

    // Run cmake to build project
    fprintf(stdout, "Building project\n");

    compiler_t compiler = detect_compiler();

    if (compiler == COMPILER_NONE) {
        fprintf(stderr, "Error: No compiler found (g++, clang++, or cl)\n");
        return -1;
    }
    
    char command[512];

    #ifdef _WIN32
    
    if (compiler == COMPILER_GCC) {
        fprintf(stdout, "Using GCC toolchain\n");
        snprintf(command, sizeof(command),
            "cmake -S \"%s\" -B \"%s\" "
            "-G \"MinGW Makefiles\" "
            "-DCMAKE_C_COMPILER=gcc "
            "-DCMAKE_CXX_COMPILER=g++ "
            "&& cmake --build \"%s\"",
            project_root, build_dir, build_dir);
    }
    else if (compiler == COMPILER_MSVC) {
        fprintf(stdout, "Using MSVC toolchain\n");
        snprintf(command, sizeof(command), 
            "cmake -S \"%s\" -B \"%s\" "
            "&& cmake --build \"%s\"",
            project_root, build_dir, build_dir);
    }
    else {
        fprintf(stderr, "Error: No supported compiler found\n");
        return -1;
    }

    #else

    if (compiler == COMPILER_GCC || compiler == COMPILER_CLANG) {
        fprintf(stdout, "Using Unix toolchain\n");
        snprintf(command, sizeof(command), 
            "cmake -S \"%s\" -B \"%s\" "
            "&& cmake --build \"%s\"",
            project_root, build_dir, build_dir);
    }
    else {
        fprintf(stderr, "Error: No supported compiler found\n");
        return -1;
    }

    #endif

    if (system(command) != 0)
    {
        fprintf(stderr, "Error: Failed to build project\n");
        return -1;
    }

    // Build successful
    fprintf(stdout, "Project built successfully\n");
    return 0;
}

int handle_build() {
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    return build_project(cwd);
}