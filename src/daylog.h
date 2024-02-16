#ifndef DAYLOG_DAYLOG_H
#define DAYLOG_DAYLOG_H

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

// Summary of a task on one day
typedef struct
{
	// Name of the task
	char* task_name;
	// Sum of minutes used on task
	int minutes;
} TaskSummary;

// Summary of all tasks on one day
typedef struct
{
	// The date of the day, YYYY-MM-DD format
	char* date;
	// Summary ot the day's tasks
	TaskSummary* task_summaries;
} DaySummary;

// Read daylog file into an array of strings
StringVector* read_daylog(char* file_path, int* error);

// Parses daylog from file data
DaySummary* parse_daylog(StringVector* daylog_lines, char* ref_date);

#endif
