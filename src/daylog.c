#define PCRE2_CODE_UNIT_WIDTH 8

#include "daylog.h"
#include "vector.h"
#include <errno.h>
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Read daylog file into an array of strings
ReadDayLogResult read_daylog(char* file_path)
{
	FILE* daylog_file = fopen(file_path, "r");

	if (!daylog_file)
	{
		fprintf(stderr, "ERROR: Could not open file %s\n", file_path);
		ReadDayLogResult result = {NULL, ENOENT};
		return result;
	}

	const int MAX_LINE_LENGTH = 500;
	int truncating            = 0;

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
			truncating = 1;
			continue;
		}
		// End truncation ended if line contained newline
		else if (truncating)
		{
			truncating = 0;
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
		push_back(vector, str_copy);
	}

	ReadDayLogResult result = {vector, 0};
	return result;
}

