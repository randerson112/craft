#include "help.h"
#include <stdio.h>
#include <string.h>

void printCraftInfo() {
    fprintf(stdout, "Craft - A lightweight C/C++ build tool\n\n");
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft <command> [args] [options]\n\n");

    fprintf(stdout, "Commands:\n");
    fprintf(stdout, "    project <path>               Create a new project at the given path\n");
    fprintf(stdout, "    init [path]                  Initialize a project in the current or given directory\n");
    fprintf(stdout, "    build                        Build the project in the current directory\n");
    fprintf(stdout, "    compile <src> [out]          Compile a single source file\n");
    fprintf(stdout, "    run [path]                   Run a compiled executable\n");
    fprintf(stdout, "    gen <file>                   Generate a file with boilerplate\n");
    fprintf(stdout, "    clean                        Remove the build directory\n");
    fprintf(stdout, "    template <subcommand>        Manage project templates\n");
    fprintf(stdout, "    config <subcommand>          Manage global Craft configuration\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft project MyApp\n");
    fprintf(stdout, "    craft init\n");
    fprintf(stdout, "    craft build\n");
    fprintf(stdout, "    craft compile main.cpp\n");
    fprintf(stdout, "    craft run\n");
    fprintf(stdout, "    craft clean\n");
    fprintf(stdout, "    craft template save MyTemplate\n");
    fprintf(stdout, "    craft config set lang cpp\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    Run 'craft help <command>' for details on a specific command.\n");
    fprintf(stdout, "    Run 'craft help <command> <subcommand>' for details on a subcommand.\n");
    fprintf(stdout, "    Run 'craft help craft.toml' for details on the project config file.\n");
}

static void printProjectHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft project - Create a new project at the given path.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Creates a new directory at the given path and initializes a Craft project\n");
    fprintf(stdout, "    inside it. Generates a craft.toml, CMakeLists.txt, src/, include/, and a\n");
    fprintf(stdout, "    starter main file. Use 'craft init' if the directory already exists.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft project <path> [options]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    <path>   Path where the new project will be created.\n\n");

    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    --template, -t <name>   Template to use for the project (defaults to global default)\n");
    fprintf(stdout, "    --lang, -l <language>   Language to use: c or cpp (defaults to global default)\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft project MyApp\n");
    fprintf(stdout, "    craft project MyApp --lang c\n");
    fprintf(stdout, "    craft project MyLib --template static-library\n");
}

static void printInitHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft init - Initialize a project in the current or specified directory.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Sets up a Craft project inside an existing directory. Unlike 'project',\n");
    fprintf(stdout, "    it does not create the directory. Generates craft.toml, CMakeLists.txt,\n");
    fprintf(stdout, "    src/, include/, and a starter main file.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft init [path] [options]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    [path]   Optional path to initialize. Defaults to current directory.\n\n");

    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    --template, -t <name>   Template to use for the project (defaults to global default)\n");
    fprintf(stdout, "    --lang, -l <language>   Language to use: c or cpp (defaults to global default)\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft init\n");
    fprintf(stdout, "    craft init MyApp --lang c\n");
    fprintf(stdout, "    craft init --template static-library\n");
}

static void printBuildHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft build - Build the project in the current directory.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Reads craft.toml, regenerates CMakeLists.txt if needed, then runs CMake\n");
    fprintf(stdout, "    and make to compile the project. Craft walks up the directory tree to\n");
    fprintf(stdout, "    find the project root so you can run this from any subdirectory.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft build\n\n");

    fprintf(stdout, "Example:\n");
    fprintf(stdout, "    craft build\n");
}

static void printCompileHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft compile - Compile a single source file into an executable.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Quickly compiles a single .c or .cpp file without a full project\n");
    fprintf(stdout, "    structure. Useful for small projects and testing single files. The\n");
    fprintf(stdout, "    output executable defaults to the source file name without its extension.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft compile <source> [output]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    <source>   Source file to compile.\n");
    fprintf(stdout, "    [output]   Name for the output executable. Defaults to source name.\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft compile main.cpp\n");
    fprintf(stdout, "    craft compile main.cpp app\n");
}

static void printRunHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft run - Run a compiled executable.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Executes the compiled binary of the project or a different binary if specified.\n");
    fprintf(stdout, "    With no arguments, it looks in the build directory for an executable with the same\n");
    fprintf(stdout, "    name as the project. If a path is specified it will look there from the current directory instead.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft run [path]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    [path]   Optional path to the executable. Defaults to build directory executable.\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft run\n");
    fprintf(stdout, "    craft run MyApp\n");
}

static void printGenHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft gen - Generate a file with boilerplate.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Creates a new file with starter boilerplate. Supported file types are\n");
    fprintf(stdout, "    .c, .cpp, .h, and .hpp. Files are placed in src/ or include/ if those\n");
    fprintf(stdout, "    directories exist. Will not overwrite an existing file.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft gen <file>\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    <file>   File to generate. Must have a .c, .cpp, .h, or .hpp extension.\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft gen utils.hpp\n");
    fprintf(stdout, "    craft gen utils.cpp\n");
    fprintf(stdout, "    craft gen math.h\n");
    fprintf(stdout, "    craft gen math.c\n");
}

static void printCleanHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft clean - Remove the build directory and all compiled artifacts.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Deletes the build/ directory generated by 'craft build', removing all\n");
    fprintf(stdout, "    compiled objects, executables, and CMake cache files. Reports how many\n");
    fprintf(stdout, "    files and bytes were removed.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft clean\n\n");

    fprintf(stdout, "Example:\n");
    fprintf(stdout, "    craft clean\n");
}

static void printHelpHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft help - Show help for Craft commands and subcommands.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft help [command]\n");
    fprintf(stdout, "    craft help <command> <subcommand>\n");
    fprintf(stdout, "    craft help craft.toml\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft help\n");
    fprintf(stdout, "    craft help build\n");
    fprintf(stdout, "    craft help template save\n");
    fprintf(stdout, "    craft help config set\n");
    fprintf(stdout, "    craft help craft.toml\n");
}

static void printTemplateHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft template - Manage project templates.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Templates let you snapshot a project structure and reuse it with\n");
    fprintf(stdout, "    'craft project' or 'craft init'. Craft ships with built-in templates\n");
    fprintf(stdout, "    for common project types. Custom templates are saved per-language\n");
    fprintf(stdout, "    in ~/.craft/templates/custom/\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft template <subcommand> [args] [options]\n\n");

    fprintf(stdout, "Subcommands:\n");
    fprintf(stdout, "    save <name>      Save the current project as a template\n");
    fprintf(stdout, "    delete <name>    Delete a custom template\n");
    fprintf(stdout, "    update <name>    Replace a template with the current project structure\n");
    fprintf(stdout, "    list             List all available templates\n");
    fprintf(stdout, "    where <name>     Show the path to a template on disk\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft template save MyTemplate\n");
    fprintf(stdout, "    craft template delete MyTemplate\n");
    fprintf(stdout, "    craft template update MyTemplate\n");
    fprintf(stdout, "    craft template list\n");
    fprintf(stdout, "    craft template list --lang c\n");
    fprintf(stdout, "    craft template where MyTemplate\n\n");

    fprintf(stdout, "Tip:\n");
    fprintf(stdout, "    Run 'craft help template <subcommand>' for details on a subcommand.\n");
}

static void printConfigHelp() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft config - Manage global Craft configuration.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Reads and writes global defaults stored in ~/.craft/config.toml.\n");
    fprintf(stdout, "    These defaults are used when creating new projects. Project-level\n");
    fprintf(stdout, "    settings in craft.toml always take priority over global config.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft config <subcommand> [args]\n\n");

    fprintf(stdout, "Subcommands:\n");
    fprintf(stdout, "    set <key> <value>   Set a global config value\n");
    fprintf(stdout, "    get <key>           Get a global config value\n");
    fprintf(stdout, "    list                Show all global config values\n\n");

    fprintf(stdout, "Keys:\n");
    fprintf(stdout, "    defaults.language      Default language: c or cpp\n");
    fprintf(stdout, "    defaults.c_standard    Default C standard: 89, 99, 11, 17, 23\n");
    fprintf(stdout, "    defaults.cpp_standard  Default C++ standard: 11, 14, 17, 20, 23\n");
    fprintf(stdout, "    defaults.template      Default template when creating projects\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft config set defaults.language cpp\n");
    fprintf(stdout, "    craft config set defaults.cpp_standard 17\n");
    fprintf(stdout, "    craft config get defaults.language\n");
    fprintf(stdout, "    craft config list\n\n");

    fprintf(stdout, "Tip:\n");
    fprintf(stdout, "    Run 'craft help config <subcommand>' for details on a subcommand.\n");
}

static int printTemplateSubcommandHelp(const char* subcommand) {
    if (strcmp(subcommand, "save") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft template save - Save the current project as a template.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    Snapshots the current project structure and saves it as a reusable\n");
        fprintf(stdout, "    template. The build/ directory is excluded. The template's craft.toml\n");
        fprintf(stdout, "    has its name and version stripped so they are set fresh on project\n");
        fprintf(stdout, "    creation. Cannot overwrite a built-in template.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft template save <name>\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <name>   Name to save the template as.\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft template save MyTemplate\n");
    }
    else if (strcmp(subcommand, "delete") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft template delete - Delete a custom template.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft template delete <name> [options]\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <name>   Name of the template to delete.\n\n");

        fprintf(stdout, "Options:\n");
        fprintf(stdout, "    --lang, -l <language>   Language of the template: c or cpp (defaults to global default)\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft template delete MyTemplate\n");
        fprintf(stdout, "    craft template delete MyTemplate --lang c\n");
    }
    else if (strcmp(subcommand, "update") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft template update - Replace a template with the current project.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    Deletes the existing template and re-saves it from the current\n");
        fprintf(stdout, "    project structure. Equivalent to delete followed by save.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft template update <name>\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <name>   Name of the template to update.\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft template update MyTemplate\n");
    }
    else if (strcmp(subcommand, "list") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft template list - List all available templates.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    Shows all built-in and custom templates grouped by language.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft template list [options]\n\n");

        fprintf(stdout, "Options:\n");
        fprintf(stdout, "    --lang, -l <language>   Filter by language: c or cpp (defaults to global default)\n");
        fprintf(stdout, "    --all, -a               Show all templates regardless of language\n\n");

        fprintf(stdout, "Examples:\n");
        fprintf(stdout, "    craft template list\n");
        fprintf(stdout, "    craft template list --lang c\n");
    }
    else if (strcmp(subcommand, "where") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft template where - Show the path to a template on disk.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft template where <name> [options]\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <name>   Name of the template.\n\n");

        fprintf(stdout, "Options:\n");
        fprintf(stdout, "    --lang, -l <language>   Language of the template: c or cpp (defaults to global default)\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft template where MyTemplate\n");
    }
    else {
        fprintf(stderr, "Error: '%s' is not a valid subcommand for 'template'\n", subcommand);
        fprintf(stdout, "Run 'craft help template' to see available subcommands.\n");
        return -1;
    }
    return 0;
}

static int printConfigSubcommandHelp(const char* subcommand) {
    if (strcmp(subcommand, "set") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft config set - Set a global config value.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft config set <key> <value>\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <key>     The config key to set (e.g. defaults.language)\n");
        fprintf(stdout, "    <value>   The value to assign\n\n");

        fprintf(stdout, "Keys and valid values:\n");
        fprintf(stdout, "    defaults.language      c, cpp\n");
        fprintf(stdout, "    defaults.c_standard    89, 99, 11, 17, 23\n");
        fprintf(stdout, "    defaults.cpp_standard  11, 14, 17, 20, 23\n");
        fprintf(stdout, "    defaults.template      any template name\n\n");

        fprintf(stdout, "Examples:\n");
        fprintf(stdout, "    craft config set defaults.language cpp\n");
        fprintf(stdout, "    craft config set defaults.cpp_standard 17\n");
    }
    else if (strcmp(subcommand, "get") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft config get - Get a global config value.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft config get <key>\n\n");

        fprintf(stdout, "Arguments:\n");
        fprintf(stdout, "    <key>   The config key to read (e.g. defaults.language)\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft config get defaults.lang\n");
    }
    else if (strcmp(subcommand, "list") == 0) {
        fprintf(stdout, "Subcommand:\n");
        fprintf(stdout, "    craft config list - Show all global config values.\n\n");

        fprintf(stdout, "Description:\n");
        fprintf(stdout, "    Prints the full contents of ~/.craft/config.toml.\n\n");

        fprintf(stdout, "Usage:\n");
        fprintf(stdout, "    craft config list\n\n");

        fprintf(stdout, "Example:\n");
        fprintf(stdout, "    craft config list\n");
    }
    else {
        fprintf(stderr, "Error: '%s' is not a valid subcommand for 'config'\n", subcommand);
        fprintf(stdout, "Run 'craft help config' to see available subcommands.\n");
        return -1;
    }
    return 0;
}

static void printCraftTomlHelp() {
    fprintf(stdout, "craft.toml - Project configuration file\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    craft.toml is the project configuration file generated by 'craft project'\n");
    fprintf(stdout, "    or 'craft init'. It defines how your project is built. Craft reads this\n");
    fprintf(stdout, "    file to generate CMakeLists.txt and validate your project structure.\n\n");

    fprintf(stdout, "Sections:\n\n");

    fprintf(stdout, "    [project]\n");
    fprintf(stdout, "        name          (required) Name of the project\n");
    fprintf(stdout, "        version       (required) Version string in semver format e.g. 1.0.0\n");
    fprintf(stdout, "        language      (required) Language: c or cpp\n");
    fprintf(stdout, "        c_standard    (optional) C standard: 89, 99, 11, 17, 23\n");
    fprintf(stdout, "        cpp_standard  (optional) C++ standard: 11, 14, 17, 20, 23\n\n");

    fprintf(stdout, "    [build]\n");
    fprintf(stdout, "        type          (required) Build type: executable, static-library,\n");
    fprintf(stdout, "                                 shared-library, or header-only\n");
    fprintf(stdout, "        include_dirs  (optional) Directories to add to the include path\n");
    fprintf(stdout, "                                 Default: [\"include\"]\n");
    fprintf(stdout, "        source_dirs   (optional) Directories containing source files\n");
    fprintf(stdout, "                                 Default: [\"src\"]\n");
    fprintf(stdout, "        lib_dirs      (optional) Directories to search for libraries\n");
    fprintf(stdout, "        libs          (optional) Libraries to link against\n\n");

    fprintf(stdout, "Example:\n\n");
    fprintf(stdout, "    [project]\n");
    fprintf(stdout, "    name = \"MyApp\"\n");
    fprintf(stdout, "    version = \"0.1.0\"\n");
    fprintf(stdout, "    language = \"cpp\"\n");
    fprintf(stdout, "    cpp_standard = 17\n\n");
    fprintf(stdout, "    [build]\n");
    fprintf(stdout, "    type = \"executable\"\n");
    fprintf(stdout, "    include_dirs = [\"include\"]\n");
    fprintf(stdout, "    source_dirs = [\"src\"]\n");
    fprintf(stdout, "    lib_dirs = [\"lib/mylib\"]\n");
    fprintf(stdout, "    libs = [\"mylib\"]\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    Craft regenerates CMakeLists.txt automatically when craft.toml is newer.\n");
    fprintf(stdout, "    For custom CMake beyond what Craft generates, add a CMakeLists.extra.cmake\n");
    fprintf(stdout, "    file. Craft will include it at the end of the generated CMakeLists.txt.\n");
}

int help(command_t* command_data) {
    // No arguments, print general info
    if (command_data->arg_count == 0) {
        printCraftInfo();
        return 0;
    }

    const char* topic = command_data->args[0];

    // craft.toml help
    if (strcmp(topic, "craft.toml") == 0) {
        printCraftTomlHelp();
        return 0;
    }

    // Subcommand help
    if (command_data->arg_count == 2) {
        const char* subcommand = command_data->args[1];
        if (strcmp(topic, "template") == 0)
            return printTemplateSubcommandHelp(subcommand);
        if (strcmp(topic, "config") == 0)
            return printConfigSubcommandHelp(subcommand);
        fprintf(stderr, "Error: '%s' does not have subcommands\n", topic);
        return -1;
    }

    // Command help
    if (strcmp(topic, "project") == 0)        { printProjectHelp();  return 0; }
    if (strcmp(topic, "init") == 0)           { printInitHelp();     return 0; }
    if (strcmp(topic, "build") == 0)          { printBuildHelp();    return 0; }
    if (strcmp(topic, "compile") == 0)        { printCompileHelp();  return 0; }
    if (strcmp(topic, "run") == 0)            { printRunHelp();      return 0; }
    if (strcmp(topic, "gen") == 0)            { printGenHelp();      return 0; }
    if (strcmp(topic, "clean") == 0)          { printCleanHelp();    return 0; }
    if (strcmp(topic, "help") == 0)           { printHelpHelp();     return 0; }
    if (strcmp(topic, "template") == 0)       { printTemplateHelp(); return 0; }
    if (strcmp(topic, "config") == 0)         { printConfigHelp();   return 0; }

    // Unknown topic
    fprintf(stderr, "Error: No help available for '%s'\n", topic);
    fprintf(stdout, "Run 'craft help' to see all available commands.\n");
    return -1;
}