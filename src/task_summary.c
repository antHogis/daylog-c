#include "task_summary.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void destroy_task_summary(TaskSummary summary)
{
	free(summary.task_name);
}

DEFINE_VECTOR(TaskSummary, TaskSummaryVector, task_summary_vector, destroy_task_summary)

void add_minutes_to_task(TaskSummaryVector* vector, char* task, int minutes)
{
	TaskSummary* summary = NULL;

	size_t task_len = strlen(task);

	for (size_t i = 0; i < vector->base.size && summary == NULL; i++)
	{
		bool is_equal = strncmp(vector->data[i].task_name, task, task_len) == 0;
		if (is_equal)
		{
			summary = &vector->data[i];
		}
	}

	if (summary == NULL)
	{

		TaskSummary new_summary;
		new_summary.task_name = malloc(sizeof(char) * task_len + 1);
		new_summary.minutes   = minutes;
		strncpy(new_summary.task_name, task, task_len);
		new_summary.task_name[task_len] = '\0';

		push_task_summary_vector(vector, new_summary);
	}
	else
	{
		summary->minutes += minutes;
	}
}
