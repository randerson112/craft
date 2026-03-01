#include "parser.h"
#include <stdio.h>
#include <string.h>

const command_info_t commands_info[] = {
	{"project", {"template", "lang"}, 2, 1, 1},
	{"init", {"template", "lang"}, 2, 0, 1},
	{"build", {{0}}, 0, 0, 0},
	{"run", {{0}}, 0, 1, 1},
	{"clean", {{0}}, 0, 0, 0},
	{"compile", {{0}}, 0, 1, 2},
	{"gen", {{0}}, 0, 1, 1},
	{"help", {{0}}, 0, 0, 1}
};

const option_info_t options_info[] = {
	{"template", 1},
	{"lang", 1}
};

// Checks if an option is valid for a command
// Assumes the command is a valid command
int command_has_option(const char* command, const char* option) {
	int command_index = 0;
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (strcmp(commands_info[i].name, command) == 0) {
			command_index = i;
			break;
		}
	}

	for (int i = 0; i < commands_info[command_index].valid_options_count; i++) {
		if (strcmp(commands_info[command_index].valid_options[i], option) == 0) {
			return 1;
		}
	}

	return 0;
}

// Gets the minimum number of args a command takes
int get_command_min_args(const char* command) {
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (strcmp(commands_info[i].name, command) == 0) {
			return commands_info[i].min_args;
		}
	}
	return 0;
}

// Gets the maximum number of args a command takes
int get_command_max_args(const char* command) {
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (strcmp(commands_info[i].name, command) == 0) {
			return commands_info[i].max_args;
		}
	}
	return 0;	
}

// Checks if an option takes an argument or not
int option_has_arg(const char* option) {
	for (int i = 0; i < NUM_OPTIONS; i++) {
		if (strcmp(options_info[i].name, option) == 0) {
			return options_info[i].has_arg;
		}
	}
	return 0;
}

// Checks if a command string is one of the valid commands
int command_is_valid(const char* command) {
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (strcmp(commands_info[i].name, command) == 0) {
			return 1;
		}
	}
	return 0;
}

// Parses the command line arguments into a command struct
// Returns 0 if successful, -1 if parser encounters and error
parse_result_t parse(int argc, char** argv, command_t* command_data) {
	if (argc < 2) {
		return PARSE_NO_COMMAND;
	}

	// Store command if valid
	char* command = argv[1];
	if (!command_is_valid(command)) {
		fprintf(stderr, "[Parse Error]: '%s' is not a valid command\n\n", command);
		return PARSE_INVALID_COMMAND;
	}
	strcpy(command_data->name, command);

	// Loop through remaining command line arguments
	int next_is_option_arg = 0;

	for (int i = 2; i < argc; i++) {
		char* current = argv[i];

		// Argument is an option
		if (current[0] == '-') {

			// Throw error if last argument was an option that expected an argument
			if (next_is_option_arg) {
				char* last_option = argv[i - 1];
				fprintf(stderr, "[Parse Error]: option '%s' expects an argument\n\n", last_option);
				return PARSE_MISSING_OPTION_ARG;
			}

			// Strip dashes
			char* option;
			if (strncmp(current, "--", 2) == 0) {
				option = current + 2;
			}
			else {
				option = current + 1;
			}

			// Check if command can have this option
			if (!command_has_option(command, option)) {
				fprintf(stderr, "[Parse Error]: '%s' is not a valid option for '%s' command\n\n", current, command);
				return PARSE_INVALID_OPTION;
			}

			// Check if this option was already parsed
			for (int i = 0; i < command_data->option_count; i++) {
				if (strcmp(command_data->options[i].name, option) == 0) {
					fprintf(stderr, "[Parse Error]: Duplicate option '%s'\n\n", current);
					return PARSE_DUPLICATE_OPTION;
				}
			}

			// Store option and check if it expects an arg
			strcpy(command_data->options[command_data->option_count].name, option);
			if (option_has_arg(option)) {
				next_is_option_arg = 1;
			}
			command_data->option_count++;
		}

		// Argument is an arg
		else {
			char* arg = current;

			// Option arg
			if (next_is_option_arg) {
				strcpy(command_data->options[command_data->option_count - 1].arg, arg);
				next_is_option_arg = 0;
			}
			// Command arg
			else {
				strcpy(command_data->args[command_data->arg_count], arg);

				// Check if command has too many arguments
				if (++command_data->arg_count > get_command_max_args(command)) {
					fprintf(stderr, "[Parse Error]: Too many arguments for '%s' command\n\n", command);
					return PARSE_TOO_MANY_ARGS;
				}	
			}
		}
	}

	// Check if last command line argument was an option that expected an argument
	if (next_is_option_arg) {
		char* last_option = argv[argc - 1];
		fprintf(stderr, "[Parse Error]: option '%s' expects an argument\n\n", last_option);
		return PARSE_MISSING_OPTION_ARG;
	}

	// Check if command has enough arguments
	if (command_data->arg_count < get_command_min_args(command)) {
		fprintf(stderr, "[Parse Error]: Missing arguments for '%s' command\n\n", command);
		return PARSE_MISSING_ARGS;
	}

	return PARSE_SUCCESS;
}