#include "help.h"

#include <stdio.h>
#include <string.h>

void print_craft_info() {
    fprintf(stdout, "Craft - A lightweight C/C++ build tool\n\n");
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft <command> [args] [options]\n\n");

    fprintf(stdout, "Commands:\n");
    fprintf(stdout, "    project         Create a new project at the given path\n");
    fprintf(stdout, "    init            Initialize a project in the current or given directory\n");
    fprintf(stdout, "    build           Build the project in the current directory\n");
    fprintf(stdout, "    run             Run a compiled executable\n");
    fprintf(stdout, "    gen             Generate a file with boilerplate\n");
    fprintf(stdout, "    clean           Remove the build directory\n");
    fprintf(stdout, "    add             Add a dependency to the project\n");
    fprintf(stdout, "    remove          Remove a dependency from the project\n");
    fprintf(stdout, "    update          Update git dependencies to their latest version\n");
    fprintf(stdout, "    upgrade         Update Craft to the latest version\n");
    fprintf(stdout, "    workspace       Initialize a workspace in the current or given directory\n");
    fprintf(stdout, "    template        Manage project templates\n");
    fprintf(stdout, "    config          Manage global Craft configuration\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft project my_app\n");
    fprintf(stdout, "    craft init\n");
    fprintf(stdout, "    craft build\n");
    fprintf(stdout, "    craft run\n");
    fprintf(stdout, "    craft clean\n");
    fprintf(stdout, "    craft add --path ../my_lib\n");
    fprintf(stdout, "    craft remove my_lib\n");
    fprintf(stdout, "    craft update\n");
    fprintf(stdout, "    craft upgrade\n");
    fprintf(stdout, "    craft workspace my_workspace\n");
    fprintf(stdout, "    craft template save my_template\n");
    fprintf(stdout, "    craft config set lang cpp\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    Run 'craft help <command>' for details on a specific command.\n");
    fprintf(stdout, "    Run 'craft help <command> <subcommand>' for details on a subcommand.\n");
    fprintf(stdout, "    Run 'craft help craft.toml' for details on the project config file.\n");
}

static void print_project_help() {
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
    fprintf(stdout, "    --lang, -l <language>   Language to use: c or cpp (defaults to global default)\n");
    fprintf(stdout, "    --no-git                Disable git initialization for the project\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft project my_app\n");
    fprintf(stdout, "    craft project my_app --no-git\n");
    fprintf(stdout, "    craft project my_app --lang c\n");
    fprintf(stdout, "    craft project my_lib --template static-library\n");
}

static void print_init_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft init - Initialize a Craft project in the current or specified directory.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Sets up a Craft project inside an existing directory. Unlike 'craft project',\n");
    fprintf(stdout, "    it does not create the directory. Behavior depends on the directory contents.\n\n");
    fprintf(stdout, "    Empty directory:\n");
    fprintf(stdout, "        Generates craft.toml, CMakeLists.txt, src/, include/, and starter\n");
    fprintf(stdout, "        files based on the specified or default template.\n\n");
    fprintf(stdout, "    Existing project:\n");
    fprintf(stdout, "        Scans the directory to detect language, source dirs, include dirs,\n");
    fprintf(stdout, "        and libraries. Generates craft.toml and CMakeLists.txt from what\n");
    fprintf(stdout, "        was found. Existing source files are never modified. If a\n");
    fprintf(stdout, "        CMakeLists.txt already exists it will be backed up to\n");
    fprintf(stdout, "        CMakeLists.backup.cmake before being replaced.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft init [path] [options]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    [path]   Optional path to initialize. Defaults to current directory.\n\n");

    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    --template, -t <name>   Template to use (empty directories only)\n");
    fprintf(stdout, "    --lang, -l <language>   Language override: c or cpp\n");
    fprintf(stdout, "                            Overrides auto-detection for existing projects\n");
    fprintf(stdout, "    --no-git                Disable git initialization for the project\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft init\n");
    fprintf(stdout, "    craft init my_app\n");
    fprintf(stdout, "    craft init my_app --no-git\n");
    fprintf(stdout, "    craft init --lang c\n");
    fprintf(stdout, "    craft init --template static-library\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    For existing projects, edit craft.toml after init to fine tune settings.\n");
    fprintf(stdout, "    Use CMakeLists.extra.cmake for any custom CMake beyond what Craft generates.\n");
    fprintf(stdout, "    Run 'craft help craft.toml' for details on the project config file.\n");
}

static void print_build_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft build - Builds the current project or workspace.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Reads craft.toml, regenerates CMakeLists.txt if needed, then runs CMake\n");
    fprintf(stdout, "    and make to build the project or workspace. Craft walks up the directory\n");
    fprintf(stdout, "    tree to find the root so you can run this from any subdirectory.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft build\n\n");

    fprintf(stdout, "Example:\n");
    fprintf(stdout, "    craft build\n");
}

static void print_run_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft run - Run a compiled executable.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Executes a compiled binary of a project or workspace. With no arguments, it looks\n");
    fprintf(stdout, "    in the build directory for an executable with the same name as the project.\n");
    fprintf(stdout, "    A name needs to be specified if in a workspace with multiple executable members.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft run [name]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    [path]   Optional name of the executable. Must specify name if there are multiple executables.\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft run\n");
    fprintf(stdout, "    craft run my_app\n");
}

static void print_gen_help() {
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

static void print_clean_help() {
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

static void print_add_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft add - Add a dependency to the project.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Adds a dependency to craft.toml and regenerates CMakeLists.txt.\n");
    fprintf(stdout, "    Git dependencies are cloned to .craft/deps/ automatically.\n");
    fprintf(stdout, "    Local path dependencies must be Craft projects.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft add --path <path>\n");
    fprintf(stdout, "    craft add --git <url> [options]\n\n");

    fprintf(stdout, "Options:\n");
    fprintf(stdout, "    --path <path>         Add a local Craft project as a dependency\n");
    fprintf(stdout, "    --git <url>           Add a git dependency\n");
    fprintf(stdout, "    --tag <tag>           Pin to a specific tag (git only)\n");
    fprintf(stdout, "    --branch <branch>     Pin to a specific branch (git only)\n");
    fprintf(stdout, "    --links <targets>     Comma separated CMake link targets (git only)\n");
    fprintf(stdout, "                          Required for non-Craft CMake projects\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft add --path ../my_lib\n");
    fprintf(stdout, "    craft add --git https://github.com/user/mylib\n");
    fprintf(stdout, "    craft add --git https://github.com/user/mylib --tag v1.0.0\n");
    fprintf(stdout, "    craft add --git https://github.com/raysan5/raylib --tag 5.5\n");
    fprintf(stdout, "    craft add --git https://github.com/SFML/SFML --tag 3.0.0 --links SFML::Graphics,SFML::Window,SFML::System\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    If the git repo is a Craft project, --links is not needed.\n");
    fprintf(stdout, "    Use --tag to pin to a specific version for reproducible builds.\n");
    fprintf(stdout, "    Run 'craft build' after adding a dependency to compile it.\n");
}

static void print_remove_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft remove - Remove a dependency from the project.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Removes a dependency from craft.toml and regenerates CMakeLists.txt.\n");
    fprintf(stdout, "    For git dependencies, the cloned directory in .craft/deps/ is also deleted.\n");
    fprintf(stdout, "    Local path dependencies are removed from craft.toml only.\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft remove <name>\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    <name>   Name of the dependency to remove.\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft remove my_lib\n");
    fprintf(stdout, "    craft remove raylib\n\n");

    fprintf(stdout, "Tip:\n");
    fprintf(stdout, "    Run 'craft clean && craft build' to fully rebuild without the dependency.\n");
}

static void print_update_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft update - Update git dependencies to their latest version.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Re-clones git dependencies to get the latest version. Dependencies\n");
    fprintf(stdout, "    pinned to a tag are skipped since they are already at a fixed version.\n");
    fprintf(stdout, "    Dependencies pinned to a branch are re-cloned to get the latest commits\n");
    fprintf(stdout, "    on that branch. Local path dependencies are always skipped.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft update [name]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    [name]   Optional name of a specific dependency to update.\n");
    fprintf(stdout, "             If omitted all dependencies are updated.\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft update\n");
    fprintf(stdout, "    craft update raylib\n\n");

    fprintf(stdout, "Tip:\n");
    fprintf(stdout, "    Run 'craft build' after updating to rebuild with the new versions.\n");
}

static void print_help_help() {
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

static void print_workspace_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft workspace - Initialize a Craft workspace.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Sets up a Craft workspace by scanning the directory for existing\n");
    fprintf(stdout, "    Craft projects and generating a craft.toml with a [workspace] section\n");
    fprintf(stdout, "    listing all detected members. Also generates a root CMakeLists.txt\n");
    fprintf(stdout, "    that builds all members together in dependency order.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft workspace [path]\n\n");

    fprintf(stdout, "Arguments:\n");
    fprintf(stdout, "    [path]   Optional path to initialize. Defaults to current directory.\n\n");

    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "    craft workspace\n");
    fprintf(stdout, "    craft workspace my_workspace\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    Review the generated craft.toml to verify all members were detected.\n");
    fprintf(stdout, "    Add any missing members manually to the members array.\n");
    fprintf(stdout, "    Run 'craft build' from the workspace root to build all members.\n");
    fprintf(stdout, "    Members can still be built independently with 'craft build' inside\n");
    fprintf(stdout, "    the member directory.\n");
}

static void print_template_help() {
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
    fprintf(stdout, "    craft template save my_template\n");
    fprintf(stdout, "    craft template delete my_template\n");
    fprintf(stdout, "    craft template update my_template\n");
    fprintf(stdout, "    craft template list\n");
    fprintf(stdout, "    craft template list --lang c\n");
    fprintf(stdout, "    craft template where my_template\n\n");

    fprintf(stdout, "Tip:\n");
    fprintf(stdout, "    Run 'craft help template <subcommand>' for details on a subcommand.\n");
}

static void print_config_help() {
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

static int print_template_subcommand_help(const char* subcommand) {
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
        fprintf(stdout, "    craft template save my_template\n");
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
        fprintf(stdout, "    craft template delete my_template\n");
        fprintf(stdout, "    craft template delete my_template --lang c\n");
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
        fprintf(stdout, "    craft template update my_template\n");
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
        fprintf(stdout, "    craft template where my_template\n");
    }
    else {
        fprintf(stderr, "Error: '%s' is not a valid subcommand for 'template'\n", subcommand);
        fprintf(stdout, "Run 'craft help template' to see available subcommands.\n");
        return -1;
    }
    return 0;
}

static int print_config_subcommand_help(const char* subcommand) {
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

static void print_upgrade_help() {
    fprintf(stdout, "Command:\n");
    fprintf(stdout, "    craft upgrade - Update Craft to the latest version.\n\n");

    fprintf(stdout, "Description:\n");
    fprintf(stdout, "    Checks GitHub for the latest version of Craft and updates if a newer\n");
    fprintf(stdout, "    version is available. Your configuration, templates, and dependencies\n");
    fprintf(stdout, "    in ~/.craft/ are never touched — only the binary is replaced.\n\n");

    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "    craft upgrade\n\n");

    fprintf(stdout, "Example:\n");
    fprintf(stdout, "    craft craft upgrade\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    Restart your terminal after updating to use the new version.\n");
    fprintf(stdout, "    Run 'craft --version' to verify the update was successful.\n");
}

static void print_craft_toml_help() {
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

    fprintf(stdout, "    [dependencies]\n");
    fprintf(stdout, "        Local path:  name = { path = \"../my_lib\" }\n");
    fprintf(stdout, "        Git:         name = { git = \"https://github.com/user/repo\" }\n");
    fprintf(stdout, "        Git + tag:   name = { git = \"https://...\", tag = \"v1.0.0\" }\n");
    fprintf(stdout, "        Git + links: name = { git = \"https://...\", links = [\"lib::target\"] }\n\n");

    fprintf(stdout, "Example:\n\n");
    fprintf(stdout, "    [project]\n");
    fprintf(stdout, "    name = \"my_app\"\n");
    fprintf(stdout, "    version = \"0.1.0\"\n");
    fprintf(stdout, "    language = \"cpp\"\n");
    fprintf(stdout, "    cpp_standard = 17\n\n");
    fprintf(stdout, "    [build]\n");
    fprintf(stdout, "    type = \"executable\"\n");
    fprintf(stdout, "    include_dirs = [\"include\"]\n");
    fprintf(stdout, "    source_dirs = [\"src\"]\n\n");
    fprintf(stdout, "    [dependencies]\n");
    fprintf(stdout, "    my_lib = { path = \"../my_lib\" }\n");
    fprintf(stdout, "    raylib = { git = \"https://github.com/raysan5/raylib\", tag = \"5.5\" }\n\n");

    fprintf(stdout, "Tips:\n");
    fprintf(stdout, "    Craft regenerates CMakeLists.txt automatically when craft.toml is newer.\n");
    fprintf(stdout, "    For custom CMake beyond what Craft generates, add a CMakeLists.extra.cmake\n");
    fprintf(stdout, "    file. Craft will include it at the end of the generated CMakeLists.txt.\n");
    fprintf(stdout, "    Run 'craft help add' for details on adding dependencies.\n");
}

int handle_help(const command_t* command_data) {
    // No arguments, print general info
    if (command_data->arg_count == 0) {
        print_craft_info();
        return 0;
    }

    const char* topic = command_data->args[0];

    // craft.toml help
    if (strcmp(topic, "craft.toml") == 0) {
        print_craft_toml_help();
        return 0;
    }

    // Subcommand help
    if (command_data->arg_count == 2) {
        const char* subcommand = command_data->args[1];
        if (strcmp(topic, "template") == 0)
            return print_template_subcommand_help(subcommand);
        if (strcmp(topic, "config") == 0)
            return print_config_subcommand_help(subcommand);
        fprintf(stderr, "Error: '%s' does not have subcommands\n", topic);
        return -1;
    }

    // Command help
    if (strcmp(topic, "project") == 0)        { print_project_help();   return 0; }
    if (strcmp(topic, "init") == 0)           { print_init_help();      return 0; }
    if (strcmp(topic, "build") == 0)          { print_build_help();     return 0; }
    if (strcmp(topic, "run") == 0)            { print_run_help();       return 0; }
    if (strcmp(topic, "gen") == 0)            { print_gen_help();       return 0; }
    if (strcmp(topic, "clean") == 0)          { print_clean_help();     return 0; }
    if (strcmp(topic, "add") == 0)            { print_add_help();       return 0; }
    if (strcmp(topic, "remove") == 0)         { print_remove_help();    return 0; }
    if (strcmp(topic, "update") == 0)         { print_update_help();    return 0; }
    if (strcmp(topic, "help") == 0)           { print_help_help();      return 0; }
    if (strcmp(topic, "template") == 0)       { print_template_help();  return 0; }
    if (strcmp(topic, "config") == 0)         { print_config_help();    return 0; }
    if (strcmp(topic, "upgrade") == 0)        { print_upgrade_help();   return 0; }
    if (strcmp(topic, "workspace") == 0)      { print_workspace_help(); return 0; }

    // Unknown topic
    fprintf(stderr, "Error: No help available for '%s'\n", topic);
    fprintf(stdout, "Run 'craft help' to see all available commands.\n");
    return -1;
}