#ifndef DAYLOG_TASK_SUMMARY_H
#define DAYLOG_TASK_SUMMARY_H

#include "vector.h"

// Summary of a task on one day
typedef struct
{
	// Name of the task
	char* task_name;
	// Sum of minutes used on task
	int minutes;
} TaskSummary;

DECLARE_VECTOR(TaskSummary, TaskSummaryVector, task_summary_vector)

void add_minutes_to_task(TaskSummaryVector* vector, char* task, int minutes);

#endif
