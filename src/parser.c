#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "utils.h"

static const char* project_options[] = {"template", "lang"};
static const char* init_options[] = {"template", "lang"};
static const char* delete_options[] = {"lang"};
static const char* list_options[] = {"lang", "all"};
static const char* where_options[] = {"lang"};
static const char* add_options[] = {"path", "git", "tag", "branch", "links"};

static const subcommand_info_t template_subcommands[] = {
	{"save",   "craft template save <name>",             NULL,           0, 1, 1},
	{"delete", "craft template delete <name> [options]", delete_options, 1, 1, 1},
	{"update", "craft template update <name>",           NULL,           0, 1, 1},
	{"list",   "craft template list [options]",          list_options,   2, 0, 0},
	{"where",  "craft template where <name> [options]",  where_options,  1, 1, 1}
};

static const subcommand_info_t config_subcommands[] = {
	{"set",  "craft config set <key> <value>", NULL, 0, 2, 2},
	{"get",  "craft config get <key>",         NULL, 0, 1, 1},
	{"list", "craft config list",              NULL, 0, 0, 0}
};

const command_info_t commands_info[] = {
	{"project",  "craft project <path> [options]",     NULL,                 0, project_options, 2, 1, 1},
	{"init",     "craft init [path] [options]",        NULL,                 0, init_options,    2, 0, 1},
	{"build",    "craft build",                        NULL,                 0, NULL,            0, 0, 0},
	{"run",      "craft run [path]",                   NULL,                 0, NULL,            0, 0, 1},
	{"clean",    "craft clean",                        NULL,                 0, NULL,            0, 0, 0},
	{"compile",  "craft compile <src> [out]",          NULL,                 0, NULL,            0, 1, 2},
	{"gen",      "craft gen <file>",                   NULL,                 0, NULL,            0, 1, 1},
	{"help",     "craft help [command] [subcommand]",  NULL,                 0, NULL,            0, 0, 2},
	{"template", "craft template <subcommand> [args]", template_subcommands, 5, NULL,            0, 0, 0},
	{"config",   "craft config <subcommand> [args]",   config_subcommands,   3, NULL,            0, 0, 0},
	{"add",      "craft add <options>",                NULL,                 0, add_options,     5, 0, 0},
	{"remove",   "craft remove <dep name>",            NULL,                 0, NULL,            0, 1, 1}
};

static const char* lang_args[] = {"c", "cpp"};

const option_info_t options_info[] = {
	{"template",  't', "--template <name>\n\t-t <name>",     1, NULL,      0},
	{"lang",      'l', "--lang <language>\n\t-l <language>", 1, lang_args, 2},
	{"all",       'a', "--all\n\t-a",                        0, NULL,      0},
	{"path",       0,  "--path <local path>",                1, NULL,      0},
    {"git",        0,  "--git <url>",                        1, NULL,      0},
    {"tag",        0,  "--tag <tag>",                        1, NULL,      0},
    {"branch",     0,  "--branch <branch>",                  1, NULL,      0},
	{"links",     'l', "--links <list>\n\t-l <list>",        1, NULL,      0}
};

// Gets respective command info struct based on command name
const command_info_t* get_command_info(const char* command) {
	for (int i = 0; i < NUM_COMMANDS; i++) {
		if (strcmp(commands_info[i].name, command) == 0) {
			return &commands_info[i];
		}
	}

	return NULL;
}

// Gets respective info struct for a command subcommand
// Assumes command is a valid command
const subcommand_info_t* get_subcommand_info(const char* command, const char* subcommand) {
	const command_info_t* parent_info = get_command_info(command);

	for (int i = 0; i < parent_info->subcommands_count; i++) {
		if (strcmp(parent_info->subcommands[i].name, subcommand) == 0) {
			return &parent_info->subcommands[i];
		}
	}

	return NULL;
}

// Gets respective option info struct based on option name
const option_info_t* get_option_info(const char* option) {
	for (int i = 0; i < NUM_OPTIONS; i++) {
		if (strcmp(options_info[i].name, option) == 0) {
			return &options_info[i];
		}
	}

	return NULL;
}

// Gets respective option info struct based on shorthand
const option_info_t* get_option_info_from_shorthand(const char shorthand) {
	for (int i = 0; i < NUM_OPTIONS; i++) {
		if (options_info[i].shorthand == shorthand) {
			return &options_info[i];
		}
	}

	return NULL;
}

// Checks if a command expects a subcommand
// Assumes command is a valid command
int command_expects_subcommand(const char* command) {
	const command_info_t* info = get_command_info(command);
	return info->subcommands_count != 0;
}

// Checks if a subcommand is valid for a command
// Assumes command is a valid command
int command_has_subcommand(const char* command, const char* subcommand) {
	const command_info_t* info = get_command_info(command);

	for (int i = 0; i < info->subcommands_count; i++) {
		if (strcmp(info->subcommands[i].name, subcommand) == 0) {
			return 1;
		}
	}

	return 0;
}

// Checks if an option is valid for a command
// Assumes the command is a valid command
int command_has_option(const char* command, const char* option) {
	const command_info_t* info = get_command_info(command);

	for (int i = 0; i < info->valid_options_count; i++) {
		if (strcmp(info->valid_options[i], option) == 0) {
			return 1;
		}
	}

	return 0;
}

// Checks if an option is valid for a command's subcommand
// Assumes the command is valid and the subcommand is valid for the command
int subcommand_has_option(const char* command, const char* subcommand, const char* option) {
	const subcommand_info_t* info = get_subcommand_info(command, subcommand);

	for (int i = 0; i < info->valid_options_count; i++) {
		if (strcmp(info->valid_options[i], option) == 0) {
			return 1;
		}
	}

	return 0;
}

// Returns the full name of an option from its shorthand if it has one
const char* get_option_name_from_shorthand(const char shorthand) {
    const option_info_t* info = get_option_info_from_shorthand(shorthand);
	if (info) {
		return info->name;
	}

    return NULL;
}

// Checks if an arg is valid for an option
// Assumes the option is a valid option
int option_has_arg(const char* option, const char* arg) {
	const option_info_t* info = get_option_info(option);

	// Any arg is valid
	if (info->valid_args_count == 0) {
		return 1;
	}

	// Check if arg is in valid args
	for (int i = 0; i < info->valid_args_count; i++) {
		if (strcmp(info->valid_args[i], arg) == 0) {
			return 1;
		}
	}

	return 0;
}

// Gets the minimum number of args a command takes
// Assumes the command is a valid command
int get_command_min_args(const char* command) {
	const command_info_t* info = get_command_info(command);
	return info->min_args;
}

// Gets the maximum number of args a command takes
// Assumes the command is a valid command
int get_command_max_args(const char* command) {
	const command_info_t* info = get_command_info(command);
	return info->max_args;	
}

// Gets the minimum number of args a subcommand takes
// Assumes the command is valid and the subcommand is valid for the command
int get_subcommand_min_args(const char* command, const char* subcommand) {
	const subcommand_info_t* info = get_subcommand_info(command, subcommand);
	return info->min_args;
}

// Gets the maximum number of args a subcommand takes
// Assumes the command is valid and the subcommand is valid for the command
int get_subcommand_max_args(const char* command, const char* subcommand) {
	const subcommand_info_t* info = get_subcommand_info(command, subcommand);
	return info->max_args;
}

// Checks if an option takes an argument or not
// Assumes the option is a valid option
int option_expects_arg(const char* option) {
	const option_info_t* info = get_option_info(option);
	return info->has_arg;
}

// Checks if a command string is one of the valid commands
int command_is_valid(const char* command) {
	const command_info_t* info = get_command_info(command);	
	return info != NULL;
}

// Gets usage information for a command
const char* get_command_usage(const char* command) {
	const command_info_t* info = get_command_info(command);
	return info->usage;
}

// Gets usage information for a subcommand
const char* get_subcommand_usage(const char* command, const char* subcommand) {
	const subcommand_info_t* info = get_subcommand_info(command, subcommand);
	return info->usage;
}

// Gets usage information for an option
const char* get_option_usage(const char* option) {

	// Strip dashes
	if (strncmp(option, "--", 2) == 0) {
		option = option + 2;	
	}
	else if (option[0] == '-') {
		char shorthand = option[1];
		option = get_option_name_from_shorthand(shorthand);
	}

	// Return the usage string
	const option_info_t* info = get_option_info(option);
	return info->usage;
}

// Gets a command suggestion if command was a few letters off
const char* get_command_suggestion(const char* unknown) {

	// Get array of command names
	const char* valid_commands[NUM_COMMANDS];
	for (int i = 0; i < NUM_COMMANDS; i++) {	
		valid_commands[i] = commands_info[i].name;
	}

	// Return suggestion if unkown command is close enough
	const char* suggestion = suggest(unknown, valid_commands, NUM_COMMANDS);
	return suggestion;
}

// Gets a subcommand suggestion for a command if subcommand was a few letters off
const char* get_subcommand_suggestion(const char* command, const char* unknown) {

	// Get array of subcommand names for command
	const command_info_t* command_info = get_command_info(command);
	const int count = command_info->subcommands_count;

	const char* valid_subcommands[count];
	for (int i = 0; i < count; i++) {
		valid_subcommands[i] = command_info->subcommands[i].name;
	}

	// Return suggestion if unknown subcommand is close enough
	const char* suggestion = suggest(unknown, valid_subcommands, count);
	return suggestion;
}

// Gets an option suggestion for a command if option was a few letters off
const char* get_command_option_suggestion(const char* command, const char* unknown) {

	// Get array of options for command
	const command_info_t* command_info = get_command_info(command);
	const int count = command_info->valid_options_count;

	const char* valid_options[count];
	for (int i = 0; i < count; i++) {
		valid_options[i] = command_info->valid_options[i];
	}

	// Return suggestion if unknown option is close enough
	const char* suggestion = suggest(unknown, valid_options, count);
	return suggestion;
}

// Gets an option suggestion for subcommand if option was a few letters off 
const char* get_subcommand_option_suggestion(const char* command, const char* subcommand, const char* unknown) {

	// Get array of options for subcommand
	const subcommand_info_t* subcommand_info = get_subcommand_info(command, subcommand);
	const int count = subcommand_info->valid_options_count;

	const char* valid_options[count];
	for (int i = 0; i < count; i++) {
		valid_options[i] = subcommand_info->valid_options[i];
	}

	// Return suggestion if unknown option is close enough
	const char* suggestion = suggest(unknown, valid_options, count);
	return suggestion;
}

// Parses the command line arguments into a command struct
parse_result_t parse(int argc, char** argv, command_t* command_data) {
	if (argc < 2) {
		return PARSE_MISSING_COMMAND;
	}

	// Store command if valid
	const char* command = argv[1];
	if (!command_is_valid(command)) {
		fprintf(stderr, "[Parse Error]: '%s' is not a valid command\n", command);
		const char* suggestion = get_command_suggestion(command);
		if (suggestion) {
			fprintf(stderr, "\nDid you mean '%s'?\n\n", suggestion);
			fprintf(stderr, "Usage: %s\n\n", get_command_usage(suggestion));
			fprintf(stderr, "Run 'craft help %s' for more information\n", suggestion);
		}
		else {
			fprintf(stderr, "\nRun 'craft help' to see a list of all available commands\n");
		}
		return PARSE_FAIL;
	}
	strcpy(command_data->name, command);

	// Useful flags
	int next_is_subcommand = 0;
	int next_is_option_arg = 0;
	int subcommand_present = 0;

	// Check if command expects subcommands
	if (command_expects_subcommand(command)) {
		next_is_subcommand = 1;
	}

	// Loop through remaining command line arguments
	for (int i = 2; i < argc; i++) {
		char* current = argv[i];

		// Argument is an option
		if (current[0] == '-') {

			// Throw error if next should be a subcommand
			if (next_is_subcommand) {
				fprintf(stderr, "[Parse Error]: '%s' command expects a subcommand, but got an option '%s'\n\n", command, current);
				fprintf(stderr, "Usage: %s\n\n", get_command_usage(command));
				fprintf(stderr, "Run 'craft help %s' for more information\n", command);
				return PARSE_FAIL;
			}

			// Throw error if last argument was an option that expected an argument
			if (next_is_option_arg) {
				char* last_option = argv[i - 1];
				fprintf(stderr, "[Parse Error]: '%s' option expects an argument\n\n", last_option);
				fprintf(stderr, "Usage: %s\n\n", get_option_usage(last_option));
				fprintf(stderr, "Run 'craft help %s' for more information\n", last_option);
				return PARSE_FAIL;
			}

			// Strip dashes
			const char* option;
			if (strncmp(current, "--", 2) == 0) {
				option = current + 2;
				if (strlen(option) == 0) {
					fprintf(stderr, "[Parse Error]: Empty option '--'\n");
					return PARSE_FAIL;
				}
			}
			else {
				if (strlen(current) == 1) {
					fprintf(stderr, "[Parse Error]: Empty option '-'\n");
					return PARSE_FAIL;	
				}
				if (strlen(current) > 2) {
					fprintf(stderr, "[Parse Error]: '%s' is not a valid option\n\n", current);
					fprintf(stderr, "Use '--' for full option names and '-' for shorthands\n");
					return PARSE_FAIL;
				}
				option = get_option_name_from_shorthand(current[1]);
				if (!option) {
					fprintf(stderr, "[Parse Error]: '%s' is not a valid option\n", current);
					return PARSE_FAIL;
				}
			}

			// Check if command or subcommand can have this option
			if (subcommand_present) {
				if (!subcommand_has_option(command, command_data->subcommand, option)) {
					fprintf(stderr, "[Parse Error]: '%s' is not a valid option for '%s' subcommand\n", current, command_data->subcommand);
					const char* suggestion = get_subcommand_option_suggestion(command, command_data->subcommand, option);
					if (suggestion) {
						fprintf(stderr, "\nDid you mean '--%s'?\n\n", suggestion);
						fprintf(stderr, "Usage: %s\n\n", get_option_usage(suggestion));
						fprintf(stderr, "Run 'craft help --%s' for more information\n", suggestion);
					}
					return PARSE_FAIL;
				}
			}
			else {
				if (!command_has_option(command, option)) {
					fprintf(stderr, "[Parse Error]: '%s' is not a valid option for '%s' command\n", current, command);
					const char* suggestion = get_command_option_suggestion(command, option);
					if (suggestion) {
						fprintf(stderr, "\nDid you mean '--%s'?\n\n", suggestion);
						fprintf(stderr, "Usage: %s\n\n", get_option_usage(suggestion));
						fprintf(stderr, "Run 'craft help --%s' for more information\n", suggestion);	
					}
					return PARSE_FAIL;
				}
			}

			// Check if this option was already parsed
			for (int j = 0; j < command_data->option_count; j++) {
				if (strcmp(command_data->options[j].name, option) == 0) {
					fprintf(stderr, "[Parse Error]: '%s' option was already specified\n", current);
					return PARSE_FAIL;
				}
			}

			// Store option and check if it expects an arg
			strcpy(command_data->options[command_data->option_count].name, option);
			if (option_expects_arg(option)) {
				next_is_option_arg = 1;
			}
			command_data->option_count++;
		}

		// Argument is an arg or subcommand
		else {
			const char* arg = current;

			// Subcommand
			if (next_is_subcommand) {
				if (command_has_subcommand(command, arg)) {
					next_is_subcommand = 0;
					subcommand_present = 1;
					strcpy(command_data->subcommand, arg);
					continue;
				}
				else {
					fprintf(stderr, "'%s' is not a valid subcommand for '%s' command\n", arg, command);
					const char* suggestion = get_subcommand_suggestion(command, arg);
					if (suggestion) {
						fprintf(stderr, "\nDid you mean '%s'?\n\n", suggestion);
						fprintf(stderr, "Usage: %s\n\n", get_subcommand_usage(command, suggestion));
						fprintf(stderr, "Run 'craft help %s %s' for more information\n", command, suggestion);
					}
					return PARSE_FAIL;
				}
			}

			// Option arg
			if (next_is_option_arg) {
				const char* last_option = argv[i - 1];
				option_t* last_option_info = &command_data->options[command_data->option_count - 1];

				// Check if argument is valid for the option
				if (!option_has_arg(last_option_info->name, arg))  {
					fprintf(stderr, "[Parse Error]: '%s' is not a valid argument for '%s' option\n\n", arg, last_option);
					fprintf(stderr, "Usage: %s\n\n", get_option_usage(last_option_info->name));
					fprintf(stderr, "Run 'craft help %s' for more information\n", last_option);
					return PARSE_FAIL;
				}

				strcpy(last_option_info->arg, arg);
				next_is_option_arg = 0;
			}

			// Arg
			else {
				strcpy(command_data->args[command_data->arg_count], arg);
				command_data->arg_count++;

				// Check if command or subcommand has too many arguments
				if (subcommand_present) {
					if (command_data->arg_count > get_subcommand_max_args(command, command_data->subcommand)) {
						fprintf(stderr, "[Parse Error]: Too many arguments for %s subcommand '%s'\n\n", command, command_data->subcommand);
						fprintf(stderr, "Usage: %s\n\n", get_subcommand_usage(command, command_data->subcommand));
						fprintf(stderr, "Run 'craft help %s %s' for more information\n", command, command_data->subcommand);
						return PARSE_FAIL;
					}
				}
				else {
					if (command_data->arg_count > get_command_max_args(command)) {
						fprintf(stderr, "[Parse Error]: Too many arguments for '%s' command\n\n", command);
						fprintf(stderr, "Usage: %s\n\n", get_command_usage(command));
						fprintf(stderr, "Run 'craft help %s' for more information\n", command);
						return PARSE_FAIL;
					}
				}
			}
		}
	}

	// Check if command expects a subcommand
	if (next_is_subcommand) {
		fprintf(stderr, "[Parse Error]: '%s' command expects a subcommand\n\n", command);
		fprintf(stderr, "Usage: %s\n\n", get_command_usage(command));
		fprintf(stderr, "Run 'craft help %s' for more information\n", command);
		return PARSE_FAIL;
	}

	// Check if last command line argument was an option that expected an argument
	if (next_is_option_arg) {
		char* last_option = argv[argc - 1];
		fprintf(stderr, "[Parse Error]: option '%s' expects an argument\n\n", last_option);
		fprintf(stderr, "Usage: %s\n\n", get_option_usage(last_option));
		fprintf(stderr, "Run 'craft help %s' for more information\n", last_option);
		return PARSE_FAIL;
	}

	// Check if command or subcommand has enough arguments
	if (subcommand_present) {
		if (command_data->arg_count < get_subcommand_min_args(command, command_data->subcommand)) {
			fprintf(stderr, "[Parse Error]: Missing arguments for %s subcommand '%s'\n\n", command, command_data->subcommand);
			fprintf(stderr, "Usage: %s\n\n", get_subcommand_usage(command, command_data->subcommand));
			fprintf(stderr, "Run 'craft help %s %s' for more information\n", command, command_data->subcommand);
			return PARSE_FAIL;
		}
	}
	else {
		if (command_data->arg_count < get_command_min_args(command)) {
			fprintf(stderr, "[Parse Error]: Missing arguments for '%s' command\n\n", command);
			fprintf(stderr, "Usage: %s\n\n", get_command_usage(command));
			fprintf(stderr, "Run 'craft help %s' for more information\n", command);
			return PARSE_FAIL;
		}
	}

	return PARSE_SUCCESS;
}