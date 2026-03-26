#include "gen.h"
#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "platform.h"

// Writes a cpp header file at the path with starting content
static int write_cpp_header(const char* path, const char* filename) {
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Error: Failed to create '%s.hpp'\n", filename);
        return -1;
    }

    fprintf(file, "#pragma once\n");
    fprintf(file, "\n// Code here\n");
    fclose(file);

    fprintf(stdout, "Generated '%s.hpp'\n", filename);
    return 0;
}

// Writes a c header file at the path with starting content
static int write_c_header(const char* path, const char* filename) {
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Error: Failed to create '%s.h'\n", filename);
        return -1;
    }

    char macro[256];
    snprintf(macro, sizeof(macro), "%s_H", filename);
    for (int i = 0; macro[i]; i++) {
        macro[i] = toupper(macro[i]);
    }

    fprintf(file, "#ifndef %s\n", macro);
    fprintf(file, "#define %s\n", macro);
    fprintf(file, "\n// Code here\n\n");
    fprintf(file, "#endif // %s\n", macro);
    fclose(file);

    fprintf(stdout, "Generated '%s.h'\n", filename);
    return 0;
}

// Writes a cpp source file at the path with starting content, includes respective header if it exists
static int write_cpp_source(const char* path, const char* filename, int has_header) {
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Error: Failed to create '%s.cpp'\n", filename);
        return -1;
    }

    if (has_header) {
        fprintf(file, "#include \"%s.hpp\"\n\n", filename);
        fprintf(file, "// Add your implementation here\n");
    }
    else {
        fprintf(file, "#include <iostream>\n\n");
        fprintf(file, "int main(int argc, char* argv[]) {\n");
        fprintf(file, "    std::cout << \"Hello, World!\" << std::endl;\n");
        fprintf(file, "    return 0;\n");
        fprintf(file, "}\n");
    }
    fclose(file);

    fprintf(stdout, "Generated '%s.cpp'\n", filename);
    return 0;
}

// Writes a c source file at the path with starting content, includes respective header if it exists
static int write_c_source(const char* path, const char* filename, int has_header) {
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Error: Failed to create '%s.c'\n", filename);
        return -1;
    }

    if (has_header) {
        fprintf(file, "#include \"%s.h\"\n\n", filename);
        fprintf(file, "// Add your implementation here\n");
    }
    else {
        fprintf(file, "#include <stdio.h>\n\n");
        fprintf(file, "int main(int argc, char** argv) {\n");
        fprintf(file, "    printf(\"Hello, World!\\n\");\n");
        fprintf(file, "    return 0;\n");
        fprintf(file, "}\n");
    }
    fclose(file);

    fprintf(stdout, "Generated '%s.c'\n", filename);
    return 0;
}

// Finds a matching header file for a source file by looking in the cwd or include_dirs listed in craft.toml
static int find_matching_header(const char* cwd, const char* filename, const char* extension) {
    char path[1024];

    // Check cwd first
    snprintf(path, sizeof(path), "%s/%s.%s", cwd, filename, extension);
    if (file_exists(path)) {
        fprintf(stdout, "Matching header found in current directory\n");
        return 1;
    }

    // If in a project, load the project config
    char project_root[1024];
    if (get_project_root(project_root, sizeof(project_root), cwd) != 0) {
        return 0;
    }

    project_config_t config;
    if (load_project_config(&config, project_root) != 0) {
        return 0;
    }

    // Look in include_dirs in craft.toml for header
    for (int i = 0; i < config.include_dir_count; i++) {
        snprintf(path, sizeof(path), "%s/%s/%s.%s", project_root, config.include_dirs[i], filename, extension);
        if (file_exists(path)) {
            fprintf(stdout, "Matching header found in '%s'\n", config.include_dirs[i]);
            return 1;
        }
    }

    return 0;
}

// Generates a header file in project include directory or current directory
static int generate_header(const char* cwd, const char* filename, const char* extension) {

    // Check if cwd is in a project
    int in_project = 0;
    char project_root[1024];
    project_config_t config;
    if (get_project_root(project_root, sizeof(project_root), cwd) == 0) {
        in_project = 1;
        if (load_project_config(&config, project_root) != 0) {
            return -1;
        }
    }

    // Get destination for header
    char dest[1024];
    if (in_project && config.include_dir_count > 0) {
        snprintf(dest, sizeof(dest), "%s/%s/%s.%s", project_root, config.include_dirs[0], filename, extension);

        if (config.include_dir_count > 1) {
            fprintf(stdout, "Note: placing in '%s' (first include_dirs in craft.toml)\n", config.include_dirs[0]);
        }
    }
    else {
        snprintf(dest, sizeof(dest), "%s/%s.%s", cwd, filename, extension);
    }

    // Make sure file doesn't already exist at the destination
    if (file_exists(dest)) {
        fprintf(stderr, "Error: '%s.%s' already exists\n", filename, extension);
        return -1;
    }

    // Generate the header
    if (strcmp(extension, "h") == 0) {
        return write_c_header(dest, filename);
    }
    else {
        return write_cpp_header(dest, filename);
    }
}

// Generates a header file in project source directory or current directory
static int generate_source(const char* cwd, const char* filename, const char* extension) {

    // Check if cwd is in a project
    int in_project = 0;
    char project_root[1024];
    project_config_t config;
    if (get_project_root(project_root, sizeof(project_root), cwd) == 0) {
        in_project = 1;
        if (load_project_config(&config, project_root) != 0) {
            return -1;
        }
    }

    // Find matching header
    const char* header_extension = strcmp(extension, "c") == 0 ? "h" : "hpp";
    int has_header = find_matching_header(cwd, filename, header_extension);

    // Get destination for source file
    char dest[1024];
    if (in_project && config.source_dir_count > 0) {
        snprintf(dest, sizeof(dest), "%s/%s/%s.%s", project_root, config.source_dirs[0], filename, extension);

        if (config.source_dir_count > 1) {
            fprintf(stdout, "Note: placing in '%s' (first source_dirs in craft.toml)\n", config.source_dirs[0]);
        }
    }
    else {
        snprintf(dest, sizeof(dest), "%s/%s.%s", cwd, filename, extension);
    }

    // Make sure file doesn't already exist
    if (file_exists(dest)) {
        fprintf(stderr, "Error: '%s.%s' already exists\n", filename, extension);
        return -1;
    }

    // Generate source file
    if (strcmp(extension, "c") == 0) {
        return write_c_source(dest, filename, has_header);
    }
    else {
        return write_cpp_source(dest, filename, has_header);
    }
}

int handle_gen(const command_t* command_data) {

    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fprintf(stderr, "[Fatal Error]: Failed to get current working directory\n");
        return -1;
    }

    // Split filename and extension
    const char* arg = command_data->args[0];
    char filename[256];
    strip_extension(arg, filename);

    char extension[8];
    get_extension(arg, extension, sizeof(extension));

    // Generate file based on extension
    if (strcmp(extension, "h") == 0 || strcmp(extension, "hpp") == 0) {
        return generate_header(cwd, filename, extension);
    }
    else if (strcmp(extension, "c") == 0 || strcmp(extension, "cpp") == 0) {
        return generate_source(cwd, filename, extension);
    }
    else {
        fprintf(stderr, "Error: unsupported extension '.%s'\n", extension);
        fprintf(stderr, "Supported extensions: .c, .cpp, .h, .hpp\n");
        return -1;
    }
}