#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "help.h"
#include "parser.h"
#include "dispatcher.h"

int main(int argc, char* argv[])
{
    // Retrive path of current working directory where craft is being called
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "[Main Error]: Failed to getting current working directory\n");
        return -1;
    }

    // Parse command line arguments
    command_t command_data = {0};
    parse_result_t result = parse(argc, argv, &command_data);

    // Dispatch command to respective function or print error message
    switch (result) {
        case PARSE_SUCCESS:
            return dispatch(&command_data, cwd);
            
        case PARSE_NO_COMMAND:
        case PARSE_INVALID_COMMAND:
            printHelp();
            return -1;

        case PARSE_MISSING_ARGS:
        case PARSE_TOO_MANY_ARGS:
        case PARSE_INVALID_OPTION:
        case PARSE_MISSING_OPTION_ARG:
        case PARSE_DUPLICATE_OPTION:
            printBriefUsage(command_data.name);
            return -1;
    }
}