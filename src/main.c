#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.h"
#include "parser.h"
#include "dispatcher.h"
#include "setup.h"

int main(int argc, char* argv[])
{
    // Setup craft home directory on first run
    setup_craft();

    // Print craft version if --version is specified
    if (argc > 1 && strcmp(argv[1], "--version") == 0) {
        fprintf(stdout, "craft %s\n", VERSION);
        return 0;
    }
    
    // Parse command line arguments
    command_t command_data = {0};
    parse_result_t result = parse(argc, argv, &command_data);

    // Dispatch command to respective function or print error message
    switch (result) {
        case PARSE_SUCCESS:
            return dispatch(&command_data);
            
        case PARSE_MISSING_COMMAND:
            print_craft_info();
            return 0;
        
        case PARSE_FAIL:
            // Parser will print helpful error message
            return -1;
    }
}