#include "regex.h"
#include "task_summary.h"
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

// Regex for a JIRA task id
static pcre2_code* task_id_regex;

// The pattern to define task_id_regex;
static const char* task_id_pattern = "^\\w+-\\d+$";

// Validate arguments
bool validate_args(Arguments* args)
{
	if (args->date == NULL)
	{
		fprintf(stderr, "FATAL: date_arg is null\n");
		return false;
	}
	if (!match_regex(date_str_regex, args->date))
	{
		fprintf(stderr, "ERROR: date %s is not valid\n", args->date);
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

void print_minutes_as_hm(int total_minutes)
{
	int hours   = total_minutes / 60;
	int minutes = total_minutes % 60;
	printf("%dh", hours);
	if (minutes > 0)
	{
		printf("%dm", minutes);
	}
}

size_t date_to_summary_index(char* date)
{
	// Date is expected to be in format YYYY-MM-DD
	int month = (date[5] - '0') * 10 + (date[6] - '0');
	int day   = (date[8] - '0') * 10 + (date[9] - '0');

	return calc_summary_index(day, month);
}

// Print out the summary of a single date in the daylog
int display_day_summary(DaySummary* day_summaries, char* date)
{
	size_t index = date_to_summary_index(date);

	if (day_summaries[index].date == NULL)
	{
		fprintf(stderr, "No daylog entry found for %s\n", date);
		return 1;
	}

	if (day_summaries[index].task_summaries == NULL)
	{
		fprintf(stderr, "No task summaries for date %s", date);
		return 0;
	}

	int longest_task_name = 0;
	for (size_t i = 0; i < day_summaries[index].task_summaries->base.size; i++)
	{
		int len = strlen(day_summaries[index].task_summaries->data[i].task_name);

		if (len > longest_task_name)
		{
			longest_task_name = len;
		}
	}

	for (size_t i = 0; i < day_summaries[index].task_summaries->base.size; i++)
	{
		TaskSummary task_summary = day_summaries[index].task_summaries->data[i];
		int divider_len          = longest_task_name - strlen(task_summary.task_name) + 4;

		printf("%s%*c", task_summary.task_name, divider_len, ' ');
		print_minutes_as_hm(task_summary.minutes);
		printf("\n");
	}

	printf("-------------------------------------------\n");
	printf("TOTAL ");
	print_minutes_as_hm(day_summaries[index].sum_minutes);
	printf("\n");

	return 0;
}

// Output CSV from the daylog in the specified date range
int output_csv(DaySummary* day_summaries, char* start_date, char* end_date)
{
	size_t start_index = date_to_summary_index(start_date);
	size_t end_index   = date_to_summary_index(end_date);

	for (size_t day_i = start_index; day_i <= end_index; day_i++)
	{
		if (day_summaries[day_i].task_summaries == NULL)
		{
			continue;
		}

		DaySummary* day_summary = &day_summaries[day_i];

		char day[3];
		char month[3];
		strncpy(day, &day_summary->date[8], 2);
		strncpy(month, &day_summary->date[5], 2);
		day[2]   = '\0';
		month[2] = '\0';

		int other_minutes = 0;

		for (size_t task_i = 0; task_i < day_summary->task_summaries->base.size; task_i++)
		{
			TaskSummary* task_summary = &day_summary->task_summaries->data[task_i];

			if (!match_regex(task_id_regex, task_summary->task_name))
			{
				other_minutes += task_summary->minutes;
			}
			else
			{
				printf("%s.%s,%s,%.2f\n",
				       day,
				       month,
				       task_summary->task_name,
				       task_summary->minutes / 60.0);
			}
		}

		if (other_minutes > 0)
		{
			printf("%s.%s,%s,%.2f\n", day, month, "other", other_minutes / 60.0);
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	int ret_val = 0;
	Arguments arguments;
	StringVector* read_result;
	DaySummary* summaries;

	date_str_regex = init_regex(date_str_pattern);
	task_id_regex  = init_regex(task_id_pattern);

	// Default values.
	arguments.date       = get_today_date();
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

	summaries = parse_daylog(read_result, arguments.date);

	if (arguments.use_csv)
	{
		ret_val = output_csv(summaries, arguments.start_date, arguments.end_date);
	}
	else
	{
		ret_val = display_day_summary(summaries, arguments.date);
	}

cleanup:
	free(arguments.date);
	free(date_str_regex);
	free(task_id_regex);
	for (size_t i = 0; i < MAX_DAYLOG_SIZE; ++i)
	{
		free(summaries[i].date);
		free(summaries[i].task_summaries);
	}
	free(summaries);
	destroy_string_vector(read_result);
	return ret_val;
}
