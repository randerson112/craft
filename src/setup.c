#include "setup.h"

#include <stdio.h>
#include <sys/stat.h>

#include "utils.h"
#include "platform.h"

static int create_global_config_file(const char* craft_home) {

    // If config.toml already exists, return early
    char config_path[PATH_SIZE];
    snprintf(config_path, sizeof(config_path), "%s/config.toml", craft_home);
    if (file_exists(config_path)) {
        return 0;
    }

    // Create config.toml
    FILE* file = fopen(config_path, "w");
    if (!file) {
        fprintf(stderr, "Error: Failed to create global config.toml file\n");
        return -1;
    }

    fprintf(file, "[defaults]\n");
    fprintf(file, "language = \"cpp\"\n");
    fprintf(file, "c_standard = 99\n");
    fprintf(file, "cpp_standard = 17\n");
    fprintf(file, "template = \"executable\"\n");

    fclose(file);
    return 0;
}

static int create_c_executable_template(const char* path) {

    // Return early if executable template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/executable", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create c executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'c executable' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"c\"\n");
    fprintf(toml_file, "c_standard = 99\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"executable\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");
    fprintf(toml_file, "source_dirs = [\"src\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // Source directory
    char source_path[PATH_SIZE];
    snprintf(source_path, sizeof(source_path), "%s/src", template_path);
    mkdir(source_path, 0755);

    // main.c
    char main_c_path[PATH_SIZE];
    snprintf(main_c_path, sizeof(main_c_path), "%s/main.c", source_path);
    FILE* main_c_file = fopen(main_c_path, "w");
    if (!main_c_file) {
        fprintf(stderr, "Error: Failed to create 'c executable' builtin template\n");
        return -1;
    }

    fprintf(main_c_file, "#include <stdio.h>\n\n");
    fprintf(main_c_file, "int main(int argc, char** argv) {\n");
    fprintf(main_c_file, "\tprintf(\"Hello, World!\\n\");\n");
    fprintf(main_c_file, "\treturn 0;\n");
    fprintf(main_c_file, "}\n");

    fclose(main_c_file);
    return 0;
}

static int create_cpp_executable_template(const char* path) {

    // Return early if executable template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/executable", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create cpp executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'cpp executable' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"cpp\"\n");
    fprintf(toml_file, "cpp_standard = 17\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"executable\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");
    fprintf(toml_file, "source_dirs = [\"src\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // Source directory
    char source_path[PATH_SIZE];
    snprintf(source_path, sizeof(source_path), "%s/src", template_path);
    mkdir(source_path, 0755);

    // main.c
    char main_cpp_path[PATH_SIZE];
    snprintf(main_cpp_path, sizeof(main_cpp_path), "%s/main.cpp", source_path);
    FILE* main_cpp_file = fopen(main_cpp_path, "w");
    if (!main_cpp_file) {
        fprintf(stderr, "Error: Failed to create 'cpp executable' builtin template\n");
        return -1;
    }

    fprintf(main_cpp_file, "#include <iostream>\n\n");
    fprintf(main_cpp_file, "int main(int argc, char* argv[]) {\n");
    fprintf(main_cpp_file, "\tstd::cout << \"Hello, World!\" << std::endl;\n");
    fprintf(main_cpp_file, "}\n");

    fclose(main_cpp_file);
    return 0;
}

static int create_c_static_library_template(const char* path) {

    // Return early if static library template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/static-library", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create cpp executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'c static-library' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"c\"\n");
    fprintf(toml_file, "cpp_standard = 99\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"static-library\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");
    fprintf(toml_file, "source_dirs = [\"src\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // mylib.h
    char mylib_h_path[PATH_SIZE];
    snprintf(mylib_h_path, sizeof(mylib_h_path), "%s/mylib.h", include_path);
    FILE* mylib_h_file = fopen(mylib_h_path, "w");
    if (!mylib_h_file) {
        fprintf(stderr, "Error: Failed to create 'c static-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_h_file, "#ifndef MYLIB_H\n");
    fprintf(mylib_h_file, "#define MYLIB_H\n\n");
    fprintf(mylib_h_file, "void mylib_hello();\n\n");
    fprintf(mylib_h_file, "#endif\n");

    fclose(mylib_h_file);

    // Source directory
    char source_path[PATH_SIZE];
    snprintf(source_path, sizeof(source_path), "%s/src", template_path);
    mkdir(source_path, 0755);

    // mylib.c
    char mylib_c_path[PATH_SIZE];
    snprintf(mylib_c_path, sizeof(mylib_c_path), "%s/mylib.c", source_path);
    FILE* mylib_c_file = fopen(mylib_c_path, "w");
    if (!mylib_c_file) {
        fprintf(stderr, "Error: Failed to create 'c static-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_c_file, "#include \"mylib.h\"\n");
    fprintf(mylib_c_file, "#include <stdio.h>\n\n");
    fprintf(mylib_c_file, "void mylib_hello() {\n");
    fprintf(mylib_c_file, "\tprintf(\"Hello from mylib!\\n\");\n");
    fprintf(mylib_c_file, "}\n");

    fclose(mylib_c_file);
    return 0;
}

static int create_cpp_static_library_template(const char* path) {

    // Return early if static library template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/static-library", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create cpp executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'cpp static-library' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"cpp\"\n");
    fprintf(toml_file, "cpp_standard = 17\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"static-library\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");
    fprintf(toml_file, "source_dirs = [\"src\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // mylib.h
    char mylib_hpp_path[PATH_SIZE];
    snprintf(mylib_hpp_path, sizeof(mylib_hpp_path), "%s/mylib.hpp", include_path);
    FILE* mylib_hpp_file = fopen(mylib_hpp_path, "w");
    if (!mylib_hpp_file) {
        fprintf(stderr, "Error: Failed to create 'cpp static-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_hpp_file, "#pragma once\n\n");
    fprintf(mylib_hpp_file, "class MyLib {\n");
    fprintf(mylib_hpp_file, "public:\n");
    fprintf(mylib_hpp_file, "\tstatic void hello();\n");
    fprintf(mylib_hpp_file, "};\n");

    fclose(mylib_hpp_file);

    // Source directory
    char source_path[PATH_SIZE];
    snprintf(source_path, sizeof(source_path), "%s/src", template_path);
    mkdir(source_path, 0755);

    // mylib.c
    char mylib_cpp_path[PATH_SIZE];
    snprintf(mylib_cpp_path, sizeof(mylib_cpp_path), "%s/mylib.cpp", source_path);
    FILE* mylib_cpp_file = fopen(mylib_cpp_path, "w");
    if (!mylib_cpp_file) {
        fprintf(stderr, "Error: Failed to create 'cpp static-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_cpp_file, "#include \"mylib.hpp\"\n");
    fprintf(mylib_cpp_file, "#include <iostream>\n\n");
    fprintf(mylib_cpp_file, "void MyLib::hello() {\n");
    fprintf(mylib_cpp_file, "\tstd::cout << \"Hello from MyLib!\" << std::endl;\n");
    fprintf(mylib_cpp_file, "}\n");

    fclose(mylib_cpp_file);
    return 0;
}

static int create_c_shared_library_template(const char* path) {

    // Return early if shared library template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/shared-library", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create cpp executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'c shared-library' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"c\"\n");
    fprintf(toml_file, "cpp_standard = 99\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"shared-library\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");
    fprintf(toml_file, "source_dirs = [\"src\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // mylib.h
    char mylib_h_path[PATH_SIZE];
    snprintf(mylib_h_path, sizeof(mylib_h_path), "%s/mylib.h", include_path);
    FILE* mylib_h_file = fopen(mylib_h_path, "w");
    if (!mylib_h_file) {
        fprintf(stderr, "Error: Failed to create 'c shared-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_h_file, "#ifndef MYLIB_H\n");
    fprintf(mylib_h_file, "#define MYLIB_H\n\n");
    fprintf(mylib_h_file, "void mylib_hello();\n\n");
    fprintf(mylib_h_file, "#endif\n");

    fclose(mylib_h_file);

    // Source directory
    char source_path[PATH_SIZE];
    snprintf(source_path, sizeof(source_path), "%s/src", template_path);
    mkdir(source_path, 0755);

    // mylib.c
    char mylib_c_path[PATH_SIZE];
    snprintf(mylib_c_path, sizeof(mylib_c_path), "%s/mylib.c", source_path);
    FILE* mylib_c_file = fopen(mylib_c_path, "w");
    if (!mylib_c_file) {
        fprintf(stderr, "Error: Failed to create 'c shared-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_c_file, "#include \"mylib.h\"\n");
    fprintf(mylib_c_file, "#include <stdio.h>\n\n");
    fprintf(mylib_c_file, "void mylib_hello() {\n");
    fprintf(mylib_c_file, "\tprintf(\"Hello from mylib!\\n\");\n");
    fprintf(mylib_c_file, "}\n");

    fclose(mylib_c_file);
    return 0;
}

static int create_cpp_shared_library_template(const char* path) {

    // Return early if shared library template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/shared-library", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create cpp executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'cpp shared-library' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"cpp\"\n");
    fprintf(toml_file, "cpp_standard = 17\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"shared-library\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");
    fprintf(toml_file, "source_dirs = [\"src\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // mylib.h
    char mylib_hpp_path[PATH_SIZE];
    snprintf(mylib_hpp_path, sizeof(mylib_hpp_path), "%s/mylib.hpp", include_path);
    FILE* mylib_hpp_file = fopen(mylib_hpp_path, "w");
    if (!mylib_hpp_file) {
        fprintf(stderr, "Error: Failed to create 'cpp shared-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_hpp_file, "#pragma once\n\n");
    fprintf(mylib_hpp_file, "class MyLib {\n");
    fprintf(mylib_hpp_file, "public:\n");
    fprintf(mylib_hpp_file, "\tstatic void hello();\n");
    fprintf(mylib_hpp_file, "};\n");

    fclose(mylib_hpp_file);

    // Source directory
    char source_path[PATH_SIZE];
    snprintf(source_path, sizeof(source_path), "%s/src", template_path);
    mkdir(source_path, 0755);

    // mylib.c
    char mylib_cpp_path[PATH_SIZE];
    snprintf(mylib_cpp_path, sizeof(mylib_cpp_path), "%s/mylib.cpp", source_path);
    FILE* mylib_cpp_file = fopen(mylib_cpp_path, "w");
    if (!mylib_cpp_file) {
        fprintf(stderr, "Error: Failed to create 'cpp shared-library' builtin template\n");
        return -1;
    }

    fprintf(mylib_cpp_file, "#include \"mylib.hpp\"\n");
    fprintf(mylib_cpp_file, "#include <iostream>\n\n");
    fprintf(mylib_cpp_file, "void MyLib::hello() {\n");
    fprintf(mylib_cpp_file, "\tstd::cout << \"Hello from MyLib!\" << std::endl;\n");
    fprintf(mylib_cpp_file, "}\n");

    fclose(mylib_cpp_file);
    return 0;
}

static int create_c_header_only_template(const char* path) {

    // Return early if header only template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/header-only", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create cpp executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'c header-only' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"c\"\n");
    fprintf(toml_file, "cpp_standard = 99\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"header-only\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // mylib.h
    char mylib_h_path[PATH_SIZE];
    snprintf(mylib_h_path, sizeof(mylib_h_path), "%s/mylib.h", include_path);
    FILE* mylib_h_file = fopen(mylib_h_path, "w");
    if (!mylib_h_file) {
        fprintf(stderr, "Error: Failed to create 'c header-only' builtin template\n");
        return -1;
    }

    fprintf(mylib_h_file, "#ifndef MYLIB_H\n");
    fprintf(mylib_h_file, "#define MYLIB_H\n\n");
    fprintf(mylib_h_file, "void mylib_hello();\n\n");
    fprintf(mylib_h_file, "#ifdef MYLIB_IMPLEMENTATION\n");
    fprintf(mylib_h_file, "#include <stdio.h>\n\n");
    fprintf(mylib_h_file, "void mylib_hello(void) {\n");
    fprintf(mylib_h_file, "\tprintf(\"Hello from mylib!\\n\");\n");
    fprintf(mylib_h_file, "}\n");
    fprintf(mylib_h_file, "#endif\n\n");
    fprintf(mylib_h_file, "#endif\n");

    fclose(mylib_h_file);
    return 0;
}

static int create_cpp_header_only_template(const char* path) {

    // Return early if header only template already exists
    char template_path[PATH_SIZE];
    snprintf(template_path, sizeof(template_path), "%s/header-only", path);
    if (dir_exists(template_path)) {
        return 0;
    }

    // Create cpp executable template directory
    mkdir(template_path, 0755);

    // craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", template_path);
    FILE* toml_file = fopen(toml_path, "w");
    if (!toml_file) {
        fprintf(stderr, "Error: Failed to create 'cpp header-only' builtin template\n");
        return -1;
    }

    fprintf(toml_file, "[project]\n");
    fprintf(toml_file, "language = \"cpp\"\n");
    fprintf(toml_file, "cpp_standard = 17\n\n");
    fprintf(toml_file, "[build]\n");
    fprintf(toml_file, "type = \"header-only\"\n");
    fprintf(toml_file, "include_dirs = [\"include\"]\n");

    fclose(toml_file);

    // Include directory
    char include_path[PATH_SIZE];
    snprintf(include_path, sizeof(include_path), "%s/include", template_path);
    mkdir(include_path, 0755);

    // mylib.h
    char mylib_hpp_path[PATH_SIZE];
    snprintf(mylib_hpp_path, sizeof(mylib_hpp_path), "%s/mylib.hpp", include_path);
    FILE* mylib_hpp_file = fopen(mylib_hpp_path, "w");
    if (!mylib_hpp_file) {
        fprintf(stderr, "Error: Failed to create 'cpp header-only' builtin template\n");
        return -1;
    }

    fprintf(mylib_hpp_file, "#pragma once\n");
    fprintf(mylib_hpp_file, "#include <iostream>\n\n");
    fprintf(mylib_hpp_file, "inline void mylib_hello() {\n");
    fprintf(mylib_hpp_file, "\tstd::cout << \"Hello from mylib!\" << std::endl;\n");
    fprintf(mylib_hpp_file, "}\n");

    fclose(mylib_hpp_file);
    return 0;
}

static int create_templates(const char* craft_home) {

    // Create templates path if not already there
    char templates_path[PATH_SIZE];
    snprintf(templates_path, sizeof(templates_path), "%s/templates", craft_home);
    if (!dir_exists(templates_path)) {
        mkdir(templates_path, 0755);
    }

    // Create builtin templates directory if not already there
    char builtin_path[PATH_SIZE];
    snprintf(builtin_path, sizeof(builtin_path), "%s/builtin", templates_path);
    if (!dir_exists(builtin_path)) {
        mkdir(builtin_path, 0755);
    }

    char builtin_c_path[PATH_SIZE];
    snprintf(builtin_c_path, sizeof(builtin_c_path), "%s/c", builtin_path);
    if (!dir_exists(builtin_c_path)) {
        mkdir(builtin_c_path, 0755);
    }

    char builtin_cpp_path[PATH_SIZE];
    snprintf(builtin_cpp_path, sizeof(builtin_cpp_path), "%s/cpp", builtin_path);
    if (!dir_exists(builtin_cpp_path)) {
        mkdir(builtin_cpp_path, 0755);
    }

    // Create custom templates directory if not already there
    char custom_path[PATH_SIZE];
    snprintf(custom_path, sizeof(custom_path), "%s/custom", templates_path);
    if (!dir_exists(custom_path)) {
        mkdir(custom_path, 0755);
    }

    char custom_c_path[PATH_SIZE];
    snprintf(custom_c_path, sizeof(custom_c_path), "%s/c", custom_path);
    if (!dir_exists(custom_c_path)) {
        mkdir(custom_c_path, 0755);
    }

    char custom_cpp_path[PATH_SIZE];
    snprintf(custom_cpp_path, sizeof(custom_cpp_path), "%s/cpp", custom_path);
    if (!dir_exists(custom_cpp_path)) {
        mkdir(custom_cpp_path, 0755);
    }

    // Create templates if not already there
    create_c_executable_template(builtin_c_path);
    create_c_static_library_template(builtin_c_path);
    create_c_shared_library_template(builtin_c_path);
    create_c_header_only_template(builtin_c_path);

    create_cpp_executable_template(builtin_cpp_path);
    create_cpp_static_library_template(builtin_cpp_path);
    create_cpp_shared_library_template(builtin_cpp_path);
    create_cpp_header_only_template(builtin_cpp_path);
    return 0;
}

int setup_craft() {

    // Create craft home directory if it does not exist yet
    char craft_home[PATH_SIZE];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        fprintf(stderr, "Error: Failed to set up craft home directory\n");
        return -1;
    }

    // Create the templates directory with builtin templates if not already there
    create_global_config_file(craft_home);
    create_templates(craft_home);
    return 0;
}
