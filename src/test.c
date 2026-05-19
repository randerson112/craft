#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "build.h"
#include "config.h"
#include "platform.h"
#include "utils.h"

#define MAX_TESTS 128

typedef struct {
    char name[128];
    char source_path[PATH_SIZE];
} test_info_t;

// Collects test sources (*.c, *.cpp) at the top level of tests_dir.
static int discover_tests(const char* tests_dir, test_info_t* tests_out, int max_tests) {
    dir_t* dir = open_dir(tests_dir);
    if (!dir) {
        return 0;
    }

    int count = 0;
    dir_entry_t entry;
    while (read_dir(dir, &entry) && count < max_tests) {
        if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) continue;
        if (entry.is_dir) continue;

        char ext[8];
        get_extension(entry.name, ext, sizeof(ext));
        if (strcmp(ext, "c") != 0 && strcmp(ext, "cpp") != 0) continue;

        strip_extension(entry.name, tests_out[count].name);
        snprintf(tests_out[count].source_path, PATH_SIZE, "%s/%s", tests_dir, entry.name);
        count++;
    }

    close_dir(dir);
    return count;
}

// Looks up the compiled test binary across the standard CMake generator output
// locations: build/<name> (single-config) and build/<config>/<name> (multi-config).
static int find_test_binary(const char* build_dir, const char* test_name,
                            char* buffer, size_t buffer_size) {
    static const char* const config_dirs[] = {
        "", "Debug", "Release", "MinSizeRel", "RelWithDebInfo"
    };

    char binary_name[128];
    #ifdef _WIN32
    snprintf(binary_name, sizeof(binary_name), "%s.exe", test_name);
    #else
    snprintf(binary_name, sizeof(binary_name), "%s", test_name);
    #endif

    for (size_t i = 0; i < sizeof(config_dirs) / sizeof(*config_dirs); i++) {
        if (config_dirs[i][0] == '\0') {
            snprintf(buffer, buffer_size, "%s/%s", build_dir, binary_name);
        } else {
            snprintf(buffer, buffer_size, "%s/%s/%s", build_dir, config_dirs[i], binary_name);
        }
        if (file_exists(buffer)) {
            return 1;
        }
    }
    return 0;
}

int handle_test(const command_t* command_data) {
    char cwd[PATH_SIZE];
    if (get_cwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "Error: Failed to get current working directory\n");
        return -1;
    }

    char project_root[PATH_SIZE];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        fprintf(stderr, "Error: Could not find craft.toml in current directory or any parent directory\n");
        return -1;
    }

    char tests_dir[PATH_SIZE];
    snprintf(tests_dir, sizeof(tests_dir), "%s/tests", project_root);
    if (!dir_exists(tests_dir)) {
        fprintf(stderr, "Error: No 'tests' directory found in project root\n\n");
        fprintf(stderr, "Create a 'tests' directory and add .c or .cpp test files to it.\n");
        return -1;
    }

    test_info_t tests[MAX_TESTS];
    int test_count = discover_tests(tests_dir, tests, MAX_TESTS);
    if (test_count == 0) {
        fprintf(stderr, "Error: No .c or .cpp files found in 'tests' directory\n");
        return -1;
    }

    const char* filter = command_data->arg_count > 0 ? command_data->args[0] : NULL;

    int no_run = 0;
    for (int i = 0; i < command_data->option_count; i++) {
        if (strcmp(command_data->options[i].name, "no-run") == 0) {
            no_run = 1;
            break;
        }
    }

    // Build the project; this also builds the test binaries because
    // generate_cmake emits them as additional targets when tests/ exists.
    if (handle_build() != 0) {
        return -1;
    }

    if (no_run) {
        fprintf(stdout, "\n%d test binar%s built (--no-run specified, skipping execution)\n",
                test_count, test_count == 1 ? "y" : "ies");
        return 0;
    }

    char build_dir[PATH_SIZE];
    snprintf(build_dir, sizeof(build_dir), "%s/build", project_root);

    int will_run = 0;
    for (int i = 0; i < test_count; i++) {
        if (!filter || strstr(tests[i].name, filter)) will_run++;
    }
    fprintf(stdout, "\nrunning %d test%s\n", will_run, will_run == 1 ? "" : "s");

    int passed = 0;
    int failed = 0;

    for (int i = 0; i < test_count; i++) {
        if (filter && strstr(tests[i].name, filter) == NULL) {
            continue;
        }

        char binary_path[PATH_SIZE];
        if (!find_test_binary(build_dir, tests[i].name, binary_path, PATH_SIZE)) {
            fprintf(stdout, "test %s ... FAILED (binary not found)\n", tests[i].name);
            failed++;
            continue;
        }

        char command[COMMAND_SIZE];
        snprintf(command, sizeof(command), "\"%s\" > %s 2>&1", binary_path, DEVNULL);
        int result = system(command);

        if (result == 0) {
            fprintf(stdout, "test %s ... ok\n", tests[i].name);
            passed++;
        } else {
            fprintf(stdout, "test %s ... FAILED\n", tests[i].name);
            failed++;
        }
    }

    fprintf(stdout, "\n");
    if (failed == 0) {
        fprintf(stdout, "test result: ok. %d passed; %d failed\n", passed, failed);
        return 0;
    } else {
        fprintf(stdout, "test result: FAILED. %d passed; %d failed\n", passed, failed);
        fprintf(stdout, "\nRun a failed test binary directly to see its output:\n");
        fprintf(stdout, "    %s/build/<config>/<test_name>%s\n",
                project_root,
                #ifdef _WIN32
                ".exe"
                #else
                ""
                #endif
        );
        return -1;
    }
}
