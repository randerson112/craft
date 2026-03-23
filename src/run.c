#include "run.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "config.h"
#include <string.h>
#include "platform.h"

// Runs an executable relative from the current directory
int run_executable_relative(const char* cwd, const char* executable_path)
{
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", cwd, executable_path);
    if (!file_exists(full_path)) {
        fprintf(stderr, "Error: No executable found at '%s'\n", executable_path);
        return -1;
    }

    // Run executable
    char command[512];
    snprintf(command, sizeof(command), "%s", full_path);

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
    char project_root[512];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    // Get path to build directory
    char build_dir[512];
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_root);
    if (!dir_exists(build_dir)) {
        fprintf(stderr, "Error: No build directory found\n\n");
        fprintf(stderr, "Run 'craft build' to build the project first\n");
        return -1;
    }

    // Get path to executable in build
    char project_name[32];
    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return -1;
    }
    strncpy(project_name, config.name, sizeof(project_name));

    char executable_path[512];
    snprintf(executable_path, sizeof(executable_path), "%s/%s", build_dir, project_name);
    if (!file_exists(executable_path)) {
        fprintf(stderr, "Error: Executable '%s' not found in build directory\n\n", project_name);
        fprintf(stderr, "Try running 'craft build' to re-build the project first\n");
        return -1;
    }

    // Run executable
    char command[512];
    snprintf(command, sizeof(command), "%s", executable_path);

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
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
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