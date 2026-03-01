#ifndef PARSER_H
#define PARSER_H

#define NUM_COMMANDS 8
#define NUM_OPTIONS 2

typedef enum {
    PARSE_SUCCESS,
    PARSE_NO_COMMAND,
    PARSE_INVALID_COMMAND,
    PARSE_MISSING_ARGS,
    PARSE_TOO_MANY_ARGS,
    PARSE_INVALID_OPTION,
    PARSE_DUPLICATE_OPTION,
    PARSE_MISSING_OPTION_ARG,
	PARSE_INVALID_OPTION_ARG
} parse_result_t;

// Option struct to store name of option and its arg if applicable
typedef struct {
	char name[16];
	char arg[32];
} option_t;

// Stores info about an option
typedef struct {
	const char* name;
	const char shorthand;
	const int has_arg;
	const char** valid_args;
	const int valid_args_count;
} option_info_t;

// Command struct to store command name, args, and options with their args
typedef struct {
	char name[16];
	char args[8][32];
	int arg_count;
	option_t options[8];
	int option_count;
} command_t;

// Stores info about a command
typedef struct {
	const char* name;
	const char** valid_options;
	const int valid_options_count;
	const int min_args;
	const int max_args;
} command_info_t;

// Info for all valid commands
extern const command_info_t commands_info[];

// Info for all valid options
extern const option_info_t options_info[];

// Parses the command line arguments into a command struct
// Returns an enum describing the parse result
parse_result_t parse(int argc, char** argv, command_t* command_data);

#endif