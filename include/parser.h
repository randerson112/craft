#ifndef PARSER_H
#define PARSER_H

// Enum to denote the parse status
typedef enum {
    PARSE_SUCCESS,
    PARSE_MISSING_COMMAND,
    PARSE_FAIL
} parse_result_t;

// Option struct to store name of option and its arg if applicable
typedef struct {
	char name[32];
	char arg[256];
} option_t;

// Command struct to store command name, args, and options with their args
typedef struct {
	char name[16];
	char subcommand[16];
	char args[8][32];
	int arg_count;
	option_t options[8];
	int option_count;
} command_t;

// Stores info about a subcommand
typedef struct {
	const char* name;
	const char* usage;
	const char** valid_options;
	const int valid_options_count;
	const int min_args;
	const int max_args;
} subcommand_info_t;

// Stores info about a command
typedef struct {
	const char* name;
	const char* usage;
	const subcommand_info_t* subcommands;
	const int subcommands_count;
	const char** valid_options;
	const int valid_options_count;
	const int min_args;
	const int max_args;
} command_info_t;

// Stores info about an option
typedef struct {
	const char* name;
	const char shorthand;
	const char* usage;
	const int has_arg;
	const char** valid_args;
	const int valid_args_count;
} option_info_t;

// Parses the command line arguments into a command struct
// Returns an enum describing the parse result
parse_result_t parse(command_t* command_data, int argc, char** argv);

#endif