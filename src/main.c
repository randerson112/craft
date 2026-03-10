#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "help.h"
#include "parser.h"
#include "dispatcher.h"

int main(int argc, char* argv[])
{
    // Parse command line arguments
    command_t command_data = {0};
    parse_result_t result = parse(argc, argv, &command_data);

    // Dispatch command to respective function or print error message
    switch (result) {
        case PARSE_SUCCESS:
            return dispatch(&command_data);
            
        case PARSE_MISSING_COMMAND:
            printCraftInfo();
            return 0;
        
        case PARSE_FAIL:
            // Parser will print helpful error message
            return -1;
    }
}