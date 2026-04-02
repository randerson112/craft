#ifndef REGISTRY_H
#define REGISTRY_H

// Struct to store data about a kit from the registry
typedef struct {
    char name[64];
    char description[256];
    char git_url[512];
    char links[8][128];
    int links_count;
} registry_kit_t;

// Fetches the registry and looks up a kit by name
// Returns 0 on success and fills out the kit struct
// Returns -1 if the kit is not found or the registry could not be fetched
int registry_find(const char* name, registry_kit_t* kit);

#endif // REGISTRY_H
