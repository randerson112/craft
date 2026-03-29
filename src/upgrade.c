#include "upgrade.h"
#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "utils.h"
#include "config.h"
#include <string.h>

// Fetches the latest version of craft by parsing the craft.toml from github
static int fetch_latest_version(char* buffer, size_t buffer_size) {

    // Fetch latest craft.toml from GitHub
    #ifdef _WIN32
    system("curl -fsSL https://raw.githubusercontent.com/randerson112/craft/main/craft.toml > %TEMP%\\craft_latest.toml 2>NUL");
    char* temp = getenv("TEMP");
    char toml_path[512];
    snprintf(toml_path, sizeof(toml_path), "%s\\craft_latest.toml", temp);
    #else
    system("curl -fsSL https://raw.githubusercontent.com/randerson112/craft/main/craft.toml > /tmp/craft_latest.toml 2>/dev/null");
    char toml_path[] = "/tmp/craft_latest.toml";
    #endif

    // Parse toml file for version
    FILE* file = fopen(toml_path, "r");
    if (!file) return -1;

    toml_result_t result = toml_parse_file(file);
    fclose(file);
    if (!result.ok) return -1;

    toml_datum_t version = toml_seek(result.toptab, "project.version");
    if (version.type != TOML_STRING) {
        toml_free(result);
        return -1;
    }

    // Write version to buffer
    snprintf(buffer, buffer_size, "%s", version.u.s);
    toml_free(result);
    remove(toml_path);
    return 0;
}

int handle_upgrade() {
    fprintf(stdout, "Checking for updates...\n");

    // Get latest version of craft
    char latest_version[32];
    if (fetch_latest_version(latest_version, sizeof(latest_version)) != 0) {
        fprintf(stderr, "Failed to fetch latest data from GitHub\n");
        return -1;
    }

    // Compare latest version to current version
    if (strcmp(latest_version, VERSION) == 0) {
        fprintf(stdout, "Craft already up to date (%s)\n", VERSION);
        return 0;
    }

    // Update to latest version
    fprintf(stdout, "Current version: %s\n", VERSION);
    fprintf(stdout, "Latest version:  %s\n", latest_version);
    fprintf(stdout, "Updating Craft...\n");

    // Run install script
    #ifdef _WIN32
    int result = system("curl https://raw.githubusercontent.com/randerson112/craft/main/install.ps1 | powershell > NUL 2>&1");
    #else
    int result = system("curl https://raw.githubusercontent.com/randerson112/craft/main/install.sh | bash > /dev/null 2>&1");
    #endif

    if (result != 0) {
        fprintf(stderr, "Error: Update failed\n");
        return -1;
    }

    // Print success message
    fprintf(stdout, "Updated Craft successfully\n");
    fprintf(stdout, "Current version: %s", latest_version);
    fprintf(stdout, "Restart your terminal to use the latest version\n");
    return 0;
}
