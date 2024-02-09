#define PCRE2_CODE_UNIT_WIDTH 8

#include "args.h"
#include <errno.h>
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Regex for a date string. The pattern is compiled once.
static pcre2_code* date_str_regex;

// Initializes date string regex
void init_date_str_regex(void)
{
	if (date_str_regex != NULL)
	{
		fprintf(stderr, "FATAL: date_str_regex not null when initializing");
		exit(1);
	}

	PCRE2_SPTR pattern = (PCRE2_SPTR) "^\\d{4}-\\d{2}-\\d{2}$";
	int errornumber;
	PCRE2_SIZE erroroffset;
	date_str_regex = pcre2_compile(pattern,
	                               PCRE2_ZERO_TERMINATED,
	                               0,
	                               &errornumber,
	                               &erroroffset,
	                               NULL);

	if (date_str_regex == NULL)
	{
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		fprintf(stderr,
		        "FATAL: PCRE2 compilation failed for date_str_regex pattern at offset "
		        "%d: %s\n",
		        (int)erroroffset,
		        buffer);
		exit(1);
	}
}

// Returns true if string is a (sort of) valid date string
int validate_date_str(char* date_str)
{
	if (!date_str_regex)
	{
		init_date_str_regex();
	}

	pcre2_match_data* match_data =
		pcre2_match_data_create_from_pattern(date_str_regex, NULL);

	int rc = pcre2_match(date_str_regex,
	                     (PCRE2_SPTR)date_str,
	                     strlen(date_str),
	                     0,
	                     0,
	                     match_data,
	                     NULL);

	pcre2_match_data_free(match_data);

	return rc >= 0;
}

// Validate arguments
bool validate_args(Arguments* args)
{
	if (args->date_arg == NULL)
	{
		fprintf(stderr, "FATAL: date_arg is null\n");
		return false;
	}
	if (!validate_date_str(args->date_arg))
	{
		fprintf(stderr, "ERROR: date is not valid\n");
		return false;
	}
	if ((args->use_csv || args->from_date != NULL || args->to_date != NULL) &&
	    !(args->use_csv && args->from_date != NULL && args->to_date != NULL))
	{
		fprintf(stderr,
		        "ERROR: If any of use_csv, from_date and to_date are defined, all must "
		        "be defined\n");
		return false;
	}
	if (args->from_date != NULL && !validate_date_str(args->from_date))
	{
		fprintf(stderr, "ERROR: from_date is not valid\n");
		return false;
	}
	if (args->to_date != NULL && !validate_date_str(args->to_date))
	{
		fprintf(stderr, "ERROR: to_date is not valid\n");
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

int main(int argc, char** argv)
{
	Arguments arguments;

	// Default values.
	arguments.date_arg  = get_today_date();
	arguments.verbose   = false;
	arguments.use_csv   = false;
	arguments.from_date = NULL;
	arguments.to_date   = NULL;

	parse_args(argc, argv, &arguments);
	// printf("[ARGS]\nDate: %s\nUse CSV: %d\nFrom: %s\nTo: %s\n",
	// 	   arguments.date_arg,
	// 	   arguments.use_csv,
	// 	   arguments.from_date,
	// 	   arguments.to_date);

	if (!validate_args(&arguments))
	{
		return EINVAL;
	}

	free(date_str_regex);

	return 0;
}