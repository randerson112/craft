#include "registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "utils.h"
#include "tomlc17.h"
#include "platform.h"

// Clones the Craft registry repo into the .craft home directory
static int fetch_registry_kit(const char* name, const char* cache_path) {
    // Build URL for the kit file
    char url[512];
    snprintf(url, sizeof(url), "https://raw.githubusercontent.com/randerson112/craft-registry/main/kits/%s.toml", name);

    // Fetch latest kit file from GitHub
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "curl -fsSL \"%s\" -o \"%s\" > %s 2>&1", url, cache_path, DEVNULL);

    if (system(cmd) != 0) {
        return -1;
    }

    // If curl succeeded but the file is empty or contains a 404 page it means kit doesn't exist
    if (!file_exists(cache_path)) {
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
    snprintf(registry_cache_path, sizeof(registry_cache_path), "%s/registry_cache", craft_home);
    
    // Create registry cache directory if it doesn't exist
    if (!dir_exists(registry_cache_path)) {
        mkdir(registry_cache_path, 0755);
    }

    // Build path to cached kit file
    char kit_cache_path[512];
    snprintf(kit_cache_path, sizeof(kit_cache_path), "%s/%s.toml", registry_cache_path, name);

    // Fetch the kit file from GitHub
    if (fetch_registry_kit(name, kit_cache_path) != 0) {
        if (file_exists(kit_cache_path)) {
            fprintf(stdout, "Could not reach registry, using cached version of '%s'\n", name);
        }
        else {
            fprintf(stderr, "Error: '%s' was not found in the registry\n\n", name);
            fprintf(stderr, "Tip: Use --git to add a dependency directly from GitHub\n");
            fprintf(stderr, "     Consider submitting '%s' to the registry at\n", name);
            fprintf(stderr, "     https://github.com/randerson112/craft-registry\n");
            return -1;
        }
    }

    // Parse the kit toml file
    FILE* file = fopen(kit_cache_path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open kit file for '%s'\n", name);
        return -1;
    }
    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) {
        fprintf(stderr, "Error: Could not parse kit file for '%s': %s\n", name, result.errmsg);
        return -1;
    }

    // Get the [kit] section
    toml_datum_t kit_section = toml_seek(result.toptab, "kit");
    if (kit_section.type != TOML_TABLE) {
        fprintf(stderr, "Error: Missing [kit] section in kit file for '%s'\n", name);
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