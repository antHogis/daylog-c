#define PCRE2_CODE_UNIT_WIDTH 8

#include "daylog.h"
#include "regex.h"
#include "vector.h"
#include <errno.h>
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Regex for a line in the daylog that defines the date for logs below
static pcre2_code* date_begin_regex;

// Pattern for date_begin_regex
static const char* date_begin_pattern = "^-{4} (?<day>\\d{2})\\.(?<month>\\d{2}) -{40}$";

// Regex for a line in the daylog that contains a task entry
static pcre2_code* task_entry_regex;

// Pattern for date_begin_regex
static const char* task_entry_pattern =
	"^(?<start>\\d{4})-(?<end>\\d{4}) (?<task_id>.*)$";

// Read daylog file into an array of strings
StringVector* read_daylog(char* file_path, int* error)
{
	FILE* daylog_file = fopen(file_path, "r");

	if (!daylog_file)
	{
		fprintf(stderr, "ERROR: Could not open file %s\n", file_path);
		*error = ENOENT;
		return NULL;
	}

	const int MAX_LINE_LENGTH = 500;
	bool truncating           = false;

	StringVector* vector = init_string_vector(5000, 2, 0);

	while (true)
	{
		char buf[MAX_LINE_LENGTH];
		if (fgets(buf, MAX_LINE_LENGTH, daylog_file) == NULL)
		{
			break;
		}

		size_t buf_len            = strlen(buf);
		bool last_char_is_newline = buf[buf_len - 1] == '\n';
		// Truncate the rest of the string if it exceeds the buffer size. There's no use
		// case for having lines longer than the buffer size in the daylog file.
		if (!last_char_is_newline)
		{
			truncating = true;
			continue;
		}
		// End truncation ended if line contained newline
		else if (truncating)
		{
			truncating = false;
			continue;
		}
		// Ignore empty lines
		else if (buf_len == 1 && buf[0] == '\n')
		{
			continue;
		}

		// Copy characters and \0
		char* str_copy = malloc(sizeof(char) * (buf_len + 1));
		strncpy(str_copy, buf, buf_len + 1);
		push_string_vector(vector, str_copy);
	}

	*error = 0;
	return vector;
}

// Calculates index of summary array from date
size_t calc_summary_index(int day, int month)
{
	// A compact way to represent a day and month. Days need 5 bits (max value 31), so
	// shift months by 5. The maximum value with month 12 day 31 is 110011111 (=415).
	// The minimum value with month 1 day 1 is 100001 (=33), so we subtract 33.
	return ((month << 5) | day) - 33;
}

// Converts a HHMM time string to minutes
// (note: doesn't validate that the characters are actually numbers)
unsigned int time_str_to_minutes(const char* time_str)
{
	if (strlen(time_str) != 4)
	{
		fprintf(stderr, "incorrect length for time string: %s", time_str);
		exit(1);
	}
	int sum_minutes = 0;

	sum_minutes += (time_str[0] - '0') * 60 * 10;
	sum_minutes += (time_str[1] - '0') * 60;
	sum_minutes += (time_str[2] - '0') * 10;
	sum_minutes += (time_str[3] - '0');

	if (sum_minutes < 0)
	{
		fprintf(stderr, "invalid time string: %s", time_str);
		exit(1);
	}

	return (unsigned int)sum_minutes;
}

static bool check_task_entry_match(const char* line, DaySummary* summary)
{
	bool continue_looking_for_match = false;
	bool task_entry_is_match        = false;
	unsigned int start_minutes;
	unsigned int end_minutes;
	int sum_minutes;

	// pointers that need to be freed
	char* start_str = NULL;
	char* end_str   = NULL;
	char* task_id   = NULL;
	pcre2_match_data* match_data;

	// pointer to match data, no need to free
	PCRE2_SIZE* offset_vector;

	match_data = match_regex_with_data(task_entry_regex, line, &task_entry_is_match);

	// If no match, go to end.
	if (!task_entry_is_match)
	{
		// Tell caller to continue looking for match
		continue_looking_for_match = true;
		goto cleanup;
	}

	// Extract matching string from regex groups
	offset_vector = pcre2_get_ovector_pointer(match_data);
	start_str     = find_match_by_group("start", line, task_entry_regex, offset_vector);
	end_str       = find_match_by_group("end", line, task_entry_regex, offset_vector);
	task_id       = find_match_by_group("task_id", line, task_entry_regex, offset_vector);

	// Convert time strings to minutes
	start_minutes = time_str_to_minutes(start_str);
	end_minutes   = time_str_to_minutes(end_str);

	// Cannot allow negative time span
	if (start_minutes > end_minutes)
	{
		// TODO warn of issue
		goto cleanup;
	}

	sum_minutes = end_minutes - start_minutes;

	// 	printf("Task id %s Range %s-%s => %u end %u = %d\n",
	// 	       task_id,
	// 	       start_str,
	// 	       end_str,
	// 	       start_minutes,
	// 	       end_minutes,
	// 	       sum_minutes);
	// TODO add task entry to summary

cleanup:
	pcre2_match_data_free(match_data);
	free(start_str);
	free(end_str);
	free(task_id);

	return continue_looking_for_match;
}

static bool check_date_begin_match(const char* line,
                                   DaySummary* summaries,
                                   int* summary_index,
                                   char* ref_year)
{
	bool continue_looking_for_match = false;
	bool date_begin_is_match        = false;
	unsigned int day;
	unsigned int month;

	// pointers that need to be freed
	char* day_str                = NULL;
	char* month_str              = NULL;
	pcre2_match_data* match_data = NULL;

	// pointer to match data, no need to free
	PCRE2_SIZE* offset_vector = NULL;

	match_data = match_regex_with_data(date_begin_regex, line, &date_begin_is_match);

	// If no match, go to end.
	if (!date_begin_is_match)
	{
		// Tell caller to continue looking for match
		continue_looking_for_match = true;
		goto cleanup;
	}

	// Extract matching string from regex groups
	offset_vector = pcre2_get_ovector_pointer(match_data);
	day_str       = find_match_by_group("day", line, date_begin_regex, offset_vector);
	month_str     = find_match_by_group("month", line, date_begin_regex, offset_vector);

	day   = atoi(day_str);
	month = atoi(month_str);

	if (day > 31)
	{
		// TODO warn of issue
		goto cleanup;
	}
	if (month > 12)
	{
		// TODO warn of issue
		goto cleanup;
	}

	*summary_index = calc_summary_index(day, month);

	summaries[*summary_index].date = malloc(sizeof(char) * DATE_STRING_LENGTH);
	snprintf(summaries[*summary_index].date,
	         DATE_STRING_LENGTH,
	         "%s-%s-%s",
	         ref_year,
	         month_str,
	         day_str);

cleanup:
	pcre2_match_data_free(match_data);
	free(month_str);
	free(day_str);

	return continue_looking_for_match;
}

// Parses daylog from file data
DaySummary* parse_daylog(StringVector* daylog_lines, char* ref_date)
{
	if (daylog_lines == NULL || daylog_lines->size == 0)
	{
		// TODO error handling
		return NULL;
	}

	if (strlen(ref_date) < 5)
	{
		// TODO error handling
		return NULL;
	}

	char* ref_year = malloc(sizeof(char) * YEAR_STRING_LENGTH);
	strncpy(ref_year, ref_date, YEAR_STRING_LENGTH - 1);

	if (date_begin_regex == NULL)
	{
		date_begin_regex = init_regex(date_begin_pattern);
	}

	if (task_entry_regex == NULL)
	{
		task_entry_regex = init_regex(task_entry_pattern);
	}

	DaySummary* summaries = calloc(MAX_DAYLOG_SIZE, sizeof(DaySummary));
	int summary_index     = -1;

	for (char* line = daylog_lines->data[0];
	     line <= daylog_lines->data[daylog_lines->size - 1];
	     ++line)
	{
		// if (match_regex(date_begin_regex, line))
		// {
		// 	// TODO extract month and day
		// 	// TODO check they don't exceed 12 and 31
		// 	int month = 12;
		// 	int day   = 31;

		// 	summary_index                 = calc_summary_index(day, month);
		// 	summaries[summary_index].date = (char*)"2024-12-31";
		// 	continue;
		// }

		if (summary_index >= 0 &&
		    !check_task_entry_match(line, &summaries[summary_index]))
		{
			continue;
		}

		if (!check_date_begin_match(line, summaries, &summary_index, ref_year))
		{
			continue;
		}
	}

	free(ref_year);

	return summaries;
}
