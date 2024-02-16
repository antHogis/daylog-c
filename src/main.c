#include "regex.h"
#define PCRE2_CODE_UNIT_WIDTH 8

#include "args.h"
#include "daylog.h"
#include <errno.h>
#include <pcre2.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Regex for a date string. The pattern is compiled once.
static pcre2_code* date_str_regex;

// The pattern to define date_str_regex
static const char* date_str_pattern = "^\\d{4}-\\d{2}-\\d{2}$";

// Validate arguments
bool validate_args(Arguments* args)
{
	if (args->date_arg == NULL)
	{
		fprintf(stderr, "FATAL: date_arg is null\n");
		return false;
	}
	if (!match_regex(date_str_regex, args->date_arg))
	{
		fprintf(stderr, "ERROR: date is not valid\n");
		return false;
	}
	if ((args->use_csv || args->start_date != NULL || args->end_date != NULL) &&
	    !(args->use_csv && args->start_date != NULL && args->end_date != NULL))
	{
		fprintf(stderr,
		        "ERROR: If any of use_csv, start_date and end_date are defined, all must "
		        "be defined\n");
		return false;
	}
	if (args->start_date != NULL && !match_regex(date_str_regex, args->start_date))
	{
		fprintf(stderr, "ERROR: start_date is not valid\n");
		return false;
	}
	if (args->end_date != NULL && !match_regex(date_str_regex, args->end_date))
	{
		fprintf(stderr, "ERROR: end_date is not valid\n");
		return false;
	}
	if (args->file_path == NULL)
	{
		fprintf(stderr, "ERROR: file_path is null\n");
		return false;
	}

	return true;
}

// Get todays date as an YYYY-MM-DD string
char* get_today_date(void)
{
	time_t t     = time(NULL);
	struct tm tm = *localtime(&t);

	// need to have a larger buffer size than needed to avoid format-truncation warning.
	int buffer_size   = 36;
	char* date_string = malloc(sizeof(char) * buffer_size);
	snprintf(date_string,
	         buffer_size,
	         "%4d-%02d-%02d",
	         tm.tm_year + 1900,
	         tm.tm_mon + 1,
	         tm.tm_mday);

	return date_string;
}

// Get path to the daylog file (platform dependent)
char* get_default_daylog_path(void)
{
	char* homedir;

	if ((homedir = getenv("HOME")) == NULL)
	{
		homedir = getpwuid(geteuid())->pw_dir;
	}

	strcat(homedir, "/notes/day.log");

	return homedir;
}

int main(int argc, char** argv)
{
	int ret_val = 0;
	Arguments arguments;
	StringVector* read_result;
	DaySummary* summaries;

	date_str_regex = init_regex(date_str_pattern);

	// Default values.
	arguments.date_arg   = get_today_date();
	arguments.verbose    = false;
	arguments.use_csv    = false;
	arguments.start_date = NULL;
	arguments.end_date   = NULL;
	arguments.file_path  = get_default_daylog_path();

	parse_args(argc, argv, &arguments);

	if (!validate_args(&arguments))
	{
		ret_val = EINVAL;
		goto cleanup;
	}

	read_result = read_daylog(arguments.file_path, &ret_val);
	if (ret_val != 0)
	{
		goto cleanup;
	}

	summaries = parse_daylog(read_result, arguments.date_arg);
	for (size_t i = 0; i < MAX_DAYLOG_SIZE; ++i)
	{
		if (summaries[i].date != NULL) {
		printf("summary date %s\n", summaries[i].date);
		}
	}

cleanup:
	free(date_str_regex);
	free(summaries);
	destroy_string_vector(read_result);
	return ret_val;
}
