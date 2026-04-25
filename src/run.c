#include "run.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "craft_toml.h"
#include "build.h"
#include "platform.h"
#include "workspace.h"

// Runs the executable in the project build directory
int run_executable_project(const char* project_root, const char* executable_name, const char* profile) {

    // Load project config
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
    if (build_project(project_root, profile) != 0) {
        return -1;
    }

    // Get path to build directory
    char build_dir[PATH_SIZE];
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_root);
    if (!dir_exists(build_dir)) {
        fprintf(stderr, "Error: No build directory found\n\n");
        return -1;
    }

    // If executable name not specified get it from project name
    char final_executable_name[128];
    if (executable_name == NULL) {
        #ifdef _WIN32
        snprintf(final_executable_name, sizeof(final_executable_name), "%s.exe", config.project.name);
        #else
        snprintf(final_executable_name, sizeof(final_executable_name), "%s", config.project.name);
        #endif
    }
    else {
        snprintf(final_executable_name, sizeof(final_executable_name), "%s", executable_name);
    }

    // Get path to executable
    char executable_path[PATH_SIZE];
    if (!search_dir_for_file(executable_path, PATH_SIZE, build_dir, final_executable_name)) {
        fprintf(stderr, "Error: Executable '%s' not found in build directory\n", final_executable_name);
        return -1;
    }

    // Run executable
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "%s", executable_path);

    fprintf(stdout, "Running '%s'...\n\n", final_executable_name);
    if (system(command) != 0)
    {
        fprintf(stderr, "Error: Failed to run executable\n");
        return -1;
    }

    // Run successful
    return 0;
}

// Runs a specified executable in the workspace, runs the only executable present if name not specified
int run_executable_workspace(const char* workspace_root, const char* executable_name, const char* profile) {

    // Load workspace config
    workspace_config_t config = {0};
    if (load_workspace_config(&config, workspace_root) != 0) {
        return -1;
    }

    // Get number of executables in the workspace
    int executable_count = get_executable_member_count(workspace_root, &config);

    // Throw error if workspace does not have any executables
    if (executable_count == 0) {
        fprintf(stderr, "Error: Workspace does not contain any executables\n");
        return -1;
    }

    // If there are multiple executables and no executable was specified, throw error
    if (executable_count > 1 && executable_name == NULL) {
        fprintf(stderr, "Error: Workspace contains multiple executables\n");
        fprintf(stderr, "       Please specify the name of executable to run\n");
        return -1;
    }

    // Find executable member and get its name
    char final_executable_name[128];
    if (executable_name == NULL) {
        for (int i = 0; i < config.member_count; i++) {
            char path_to_member[PATH_SIZE];
            snprintf(path_to_member, sizeof(path_to_member), "%s/%s", workspace_root, config.members[i]);

            // Load member config and check if it is executable
            project_config_t member_config = {0};
            if (load_project_config(&member_config, path_to_member) != 0) {
                return -1;
            }
            if (validate_project_config(&member_config) != 0) {
                return -1;
            }

            // Get the name of the project if executable
            if (strcmp(member_config.build.type, "executable") == 0) {
                #ifdef _WIN32
                snprintf(final_executable_name, sizeof(final_executable_name), "%s.exe", member_config.project.name);
                #else
                snprintf(final_executable_name, sizeof(final_executable_name), "%s", member_config.project.name);
                #endif
            }
        }
    }
    else {
        snprintf(final_executable_name, sizeof(final_executable_name), "%s", executable_name);
    }

    // Build workspace before run
    if (build_workspace(workspace_root, profile) != 0) {
        return -1;
    }

    // Get path to build directory
    char build_dir[PATH_SIZE];
    snprintf(build_dir, sizeof(build_dir), "%s/build", workspace_root);
    if (!dir_exists(build_dir)) {
        fprintf(stderr, "Error: No build directory found\n\n");
        return -1;
    }

    char executable_path[PATH_SIZE];
    if (!search_dir_for_file(executable_path, PATH_SIZE, build_dir, final_executable_name)) {
        fprintf(stderr, "Error: Executable '%s' not found in build directory\n", final_executable_name);
        return -1;
    }

    // Run executable
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "%s", executable_path);

    fprintf(stdout, "Running '%s'...\n\n", final_executable_name);
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

    // Get executable name if specified
    const char* executable_name = NULL;
    if (command_data->arg_count > 0) {
        executable_name = command_data->args[0];
    }

    // Get release or profile option
    const option_t* release_option = get_option(command_data, "release");
    const option_t* profile_option = get_option(command_data, "profile");

    if (release_option && profile_option) {
        fprintf(stderr, "Error: --release and --profile cannot be used together\n");
        fprintf(stderr, "       --release is a shorthand for --profile release\n");
        return -1;
    }

    const char* profile = "dev";
    if (release_option) {
        profile = "release";
    }
    if (profile_option) {
        profile = profile_option->arg;
    }

    // Get path to project or workspace root and run executable
    char project_root[PATH_SIZE];
    if (get_project_root(project_root, sizeof(project_root), cwd) == 0) {
        return run_executable_project(project_root, executable_name, profile);
    }

    char workspace_root[PATH_SIZE];
    if (get_workspace_root(workspace_root, sizeof(workspace_root), cwd) != 0) {
        fprintf(stderr, "Error: No craft.toml found in current directory or any parent directory\n");
        return -1;
    }

    return run_executable_workspace(workspace_root, executable_name, profile);
}