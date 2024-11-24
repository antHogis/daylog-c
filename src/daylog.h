#ifndef DAYLOG_DAYLOG_H
#define DAYLOG_DAYLOG_H

#include "task_summary.h"
#include "vector.h"

// Maximum size of entries in a DaySummary array
#define MAX_DAYLOG_SIZE 383
// The length of a YYYY-MM-DD string
#define DATE_STRING_LENGTH 11
// The length of a YYYY string
#define YEAR_STRING_LENGTH 5

// Result from reading daylog
typedef struct
{
	// Pointer to vector of strings containing data
	StringVector* data;
	// Error code, non-zero value indicates failure
	int error;
} ReadDayLogResult;


// Summary of all tasks on one day
typedef struct
{
	// The date of the day, YYYY-MM-DD format
	char* date;
	// Summary of the day's tasks
	TaskSummaryVector* task_summaries;
	// Summary of total minutes
	int sum_minutes;
} DaySummary;

// Calculates index of summary array from date
size_t calc_summary_index(int day, int month);

// Read daylog file into an array of strings
StringVector* read_daylog(char* file_path, int* error);

// Parses daylog from file data
DaySummary* parse_daylog(StringVector* daylog_lines, char* ref_date);

#endif
