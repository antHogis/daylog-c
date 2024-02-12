#include "args.h"
#include "version.h"
#include <argp.h>
#include <stdlib.h>

#ifdef VERSION_PRERELEASE
const char* argp_program_version =
	VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "-" VERSION_PRERELEASE;
#else
const char* argp_program_version = VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH;
#endif

// Program documentation.
static char doc[] = "daylog - a time tracking summation utility";

// Description of arguments
static char args_doc[] = "date";

// The options of daylog
static struct argp_option options[] = {
	{"verbose", 'v', 0, 0, "Produce verbose output", 0},
	{"csv", 'c', 0, 0, "Produce daylog output as CSV", 0},
	{"start",
     's',
     "start_date",
     0,
     "The beginning date of output in YYYY-MM-DD format",
     0},
	{"end", 'e', "end_date", 0, "The end date of output in YYYY-MM-DD format", 0},
	{"file",
     'f',
     "file_path",
     0,
     "The path to the daylog file to read, $HOME/notes/daylog by default",
     0},
	{0}};

// Parse a single option.
static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
	// Get the input argument from argp_parse, which is a pointer to our arguments struct.
	Arguments* arguments = state->input;

	switch (key)
	{
		case 'c':
			arguments->use_csv = 1;
			break;
		case 's':
			arguments->start_date = arg;
			break;
		case 'e':
			arguments->end_date = arg;
			break;
		case 'f':
			arguments->file_path = arg;
			break;
		case 'v':
			arguments->verbose = true;
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num == 0)
			{
				free(arguments->date_arg);
				arguments->date_arg = arg;
			}
			else
			{
				// Too many arguments
				argp_usage(state);
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

// Argp parser.
static struct argp arg_parser = {options, parse_opt, args_doc, doc, 0, 0, 0};

// Parse CLI arguments.
void parse_args(int argc, char** argv, Arguments* args)
{
	argp_parse(&arg_parser, argc, argv, 0, 0, args);
}
