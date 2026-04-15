#include "workspace.h"

#include <stdio.h>
#include <string.h>

#include "tomlc17.h"
#include "platform.h"
#include "utils.h"
#include "craft_toml.h"

// Information about workspace section keys in craft.toml
static const config_key_t workspace_keys[] = {
    {"members", 1}
};
static const int num_workspace_keys = 1;

// Checks if given path is a Craft workspace
int is_craft_workspace(const char* path) {

    // Check if craft.toml exists at path
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", path);
    if (!file_exists(toml_path)) {
        return 0;
    }

    // Check if craft.toml contains workspace section and not a project section
    FILE* file = fopen(toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open craft.toml\n");
        return 0;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: Failed to parse craft.toml\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return 0;
    }

    toml_datum_t workspace_section = toml_seek(result.toptab, "workspace");
    toml_datum_t project_section = toml_seek(result.toptab, "project");
    if (workspace_section.type == TOML_TABLE && project_section.type != TOML_TABLE) {
        toml_free(result);
        return 1;
    }
    else {
        toml_free(result);
        return 0;
    }
}

int get_workspace_root(char* buffer, size_t buffer_size, const char* cwd) {
    char current_path[PATH_SIZE];
    snprintf(current_path, sizeof(current_path), "%s", cwd);

    while (1) {
        char toml_path[PATH_SIZE];
        snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", current_path);

        // Check if craft.toml file exists in this directory
        if (file_exists(toml_path) && is_craft_workspace(current_path)) {
            snprintf(buffer, buffer_size, "%s", current_path);
            return 0;
        }

        // Move up a directory
        char* last_slash = strrchr(current_path, '/');
        if (!last_slash || last_slash == current_path) {
            return -1;
        }
        
        *last_slash = '\0';
    }
}

int load_workspace_config(workspace_config_t* config, const char* workspace_root) {
    memset(config, 0, sizeof(*config));

    // Get path to craft.toml
    char toml_path[PATH_SIZE];
    snprintf(toml_path, sizeof(toml_path), "%s/craft.toml", workspace_root);
    if (!file_exists(toml_path)) {
        fprintf(stderr, "Error: No craft.toml found in '%s'\n", workspace_root);
        return -1;
    }

    // Open craft.toml and parse
    FILE* file = fopen(toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open craft.toml\n");
        return -1;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: Failed to parse craft.toml\n");
        fprintf(stderr, "%s\n", result.errmsg);
        return -1;
    }

    // Ensure workspace section is present
    toml_datum_t workspace_section = toml_seek(result.toptab, "workspace");
    if (workspace_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: Missing required [workspace] section in craft.toml\n");
        toml_free(result);
        return -1;
    }

    // Check for unknown keys
    if (check_unknown_keys(workspace_section, "workspace", workspace_keys, num_workspace_keys) != 0) {
        toml_free(result);
        return -1;
    }

    // Get workspace keys
    toml_datum_t members = toml_seek(workspace_section, "members");

    // Store values into config struct
    // Members
    if (members.type == TOML_ARRAY) {
        config->member_count = members.u.arr.size;
        for (int i = 0; i < members.u.arr.size; i++) {
            toml_datum_t elem = members.u.arr.elem[i];
            if (elem.type == TOML_STRING) {
                snprintf(config->members[i], sizeof(config->members[i]), "%s", elem.u.s);
            }
        }
    }

    toml_free(result);
    return 0;
}
