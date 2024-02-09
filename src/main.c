#include "args.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Returns true if string is a (sort of) valid date string
bool validate_date_str(char* date_str)
{
	// TODO implement validation
	return true;
}

// Validate arguments
bool validate_args(Arguments* args)
{
	if (args->date_arg == NULL)
	{
		fprintf(stderr, "DEVELOPER ERROR: date_arg is null\n");
		return false;
	}
	if (!validate_date_str(args->date_arg))
	{
		fprintf(stderr, "ERROR: Date is not valid");
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

	return true;
}

// Get todays date as an YYYY-MM-DD string
char* get_today_date(void)
{
	time_t t	 = time(NULL);
	struct tm tm = *localtime(&t);

	// need to have a larger buffer size than needed to avoid format-truncation warning.
	int buffer_size	  = 36;
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
	arguments.date_arg	= get_today_date();
	arguments.verbose	= false;
	arguments.use_csv	= false;
	arguments.from_date = NULL;
	arguments.to_date	= NULL;

	parse_args(argc, argv, &arguments);
	printf("[ARGS]\nDate: %s\nUse CSV: %d\nFrom: %s\nTo: %s\n",
		   arguments.date_arg,
		   arguments.use_csv,
		   arguments.from_date,
		   arguments.to_date);
	if (!validate_args(&arguments))
	{
		return EINVAL;
	}

	return 0;
}