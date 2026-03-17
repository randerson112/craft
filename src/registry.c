#include "registry.h"
#include "utils.h"
#include <stdio.h>
#include "tomlc17.h"
#include <stdlib.h>

// Clones the Craft registry repo into the .craft home directory
static int fetch_craft_registry(const char* dest_path) {
    // Build git clone command
    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "git clone --depth 1 https://github.com/randerson112/craft-registry.git %s", dest_path);

    // Fetch registry
    if (system(cmd) != 0) {
        fprintf(stderr, "Error: failed to clone registry to '%s'\n", dest_path);
        return -1;
    }

    return 0;
}

int registry_find(const char* name, registry_kit_t* kit) {

    // Get path to registry cache
    char craft_home[512];
    if (get_craft_home(craft_home, sizeof(craft_home)) != 0) {
        return -1;
    }
    char registry_cache_path[512];
    snprintf(registry_cache_path, sizeof(registry_cache_path), "%s/registry", craft_home);
    
    // Clone the registry to craft home
    if (dir_exists(registry_cache_path)) {
        remove_dir(registry_cache_path);
    }
    if (fetch_craft_registry(registry_cache_path) != 0) {
        return -1;
    }

    // Check if kit is in the registry
    char kit_toml_path[512];
    snprintf(kit_toml_path, sizeof(kit_toml_path), "%s/kits/%s.toml", registry_cache_path, name);
    if (!file_exists(kit_toml_path)) {
        fprintf(stderr, "Error: '%s' is not in the registry\n\n", name);
        fprintf(stderr, "Tip: Use --git to fetch a dependency from github.\n");
        fprintf(stderr, "     Also consider adding the desired kit to the registry\n");
        fprintf(stderr, "     at https://github.com/randerson112/craft-registry\n");
        return -1;
    }

    // Parse the kit toml file
    FILE* file = fopen(kit_toml_path, "r");
    if (!file) {
        fprintf(stderr, "Error: could not open kit file for '%s'\n", name);
        return -1;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: could not parse kit file for '%s': %s\n", name, result.errmsg);
        return -1;
    }

    // Get the [kit] section
    toml_datum_t kit_section = toml_seek(result.toptab, "kit");
    if (kit_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: missing [kit] section in kit file for '%s'\n", name);
        toml_free(result);
        return -1;
    }

    // Parse fields
    toml_datum_t kit_name = toml_seek(result.toptab, "kit.name");
    toml_datum_t kit_desc = toml_seek(result.toptab, "kit.description");
    toml_datum_t kit_url  = toml_seek(result.toptab, "kit.git_url");
    toml_datum_t kit_links = toml_seek(result.toptab, "kit.links");

    if (kit_name.type == TOML_STRING) {
        snprintf(kit->name, sizeof(kit->name), "%s", kit_name.u.s);
    }
    if (kit_desc.type == TOML_STRING) {
        snprintf(kit->description, sizeof(kit->description), "%s", kit_desc.u.s);
    }
    if (kit_url.type == TOML_STRING) {
        snprintf(kit->git_url, sizeof(kit->git_url), "%s", kit_url.u.s);
    }

    // Parse links array
    if (kit_links.type == TOML_ARRAY) {
        kit->links_count = kit_links.u.arr.size;
        for (int i = 0; i < kit_links.u.arr.size; i++) {
            if (kit_links.u.arr.elem[i].type == TOML_STRING) {
                snprintf(kit->links[i], sizeof(kit->links[0]), "%s", kit_links.u.arr.elem[i].u.s);
            }
        }
    }

    toml_free(result);
    return 0;
}