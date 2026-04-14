#include "run.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "craft_toml.h"
#include "build.h"
#include "platform.h"

// Runs an executable relative from the current directory
int run_executable_relative(const char* cwd, const char* executable_path)
{
    char full_path[PATH_SIZE];

    // Combine current working directory with relative path, adding .exe for windows
    #ifdef _WIN32
    snprintf(full_path, sizeof(full_path), "%s/%s.exe", cwd, executable_path);
    #else
    snprintf(full_path, sizeof(full_path), "%s/%s", cwd, executable_path);
    #endif

    if (!file_exists(full_path)) {
        fprintf(stderr, "Error: No executable found at '%s'\n", executable_path);
        return -1;
    }

    // Run executable
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "\"%s\"", full_path);

    if (system(command) != 0)
    {
        fprintf(stderr, "Error: Failed to run executable\n");
        return -1;
    }

    // Run successful
    return 0;
}

// Runs the executable in the build directory
int run_executable_build(const char* cwd) {

    // Get path to project root
    char project_root[PATH_SIZE];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "Error: Could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }

    // Make sure project is an executable
    if (strcmp(config.build.type, "executable") != 0) {
        fprintf(stderr, "Error: '%s' is not an executable project\n", config.project.name);
        return -1;
    }

    // Build project before run
    if (build_project(project_root) != 0) {
        return -1;
    }

    // Get path to build directory
    char build_dir[PATH_SIZE];
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_root);
    if (!dir_exists(build_dir)) {
        fprintf(stderr, "Error: No build directory found\n\n");
        fprintf(stderr, "Run 'craft build' to build the project first\n");
        return -1;
    }

    // Get path to executable in build
    char executable_name[128];
    #ifdef _WIN32
    snprintf(executable_name, sizeof(executable_name), "%s.exe", config.name);
    #else
    snprintf(executable_name, sizeof(executable_name), "%s", config.project.name);
    #endif

    char executable_path[PATH_SIZE];
    if (!search_dir_for_file(executable_path, PATH_SIZE, build_dir, executable_name)) {
        fprintf(stderr, "Error: Executable '%s' not found in build directory\n\n", config.project.name);
        fprintf(stderr, "Try running 'craft build' to re-build the project first\n");
        return -1;
    }

    // Run executable
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "%s", executable_path);

    fprintf(stdout, "Running '%s'...\n\n", executable_name);
    if (system(command) != 0)
    {
        fprintf(stderr, "Error: Failed to run executable\n");
        return -1;
    }

    // Run successful
    return 0;
}

int handle_run(const command_t* command_data) {
    // Retrive path of current working directory where craft is being called
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    // If no arguments, run the executable in the build directory
    if (command_data->arg_count == 0) {
        return run_executable_build(cwd);
    }

    // If path was specified, run that executable instead
    const char* executable_path = command_data->args[0];
    return run_executable_relative(cwd, executable_path);
}