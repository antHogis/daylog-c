#define PCRE2_CODE_UNIT_WIDTH 8

#include "regex.h"
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

pcre2_code* init_regex(const char* pattern)
{
	int errornumber;
	PCRE2_SIZE erroroffset;
	pcre2_code* regex = pcre2_compile((PCRE2_SPTR)pattern,
	                                  PCRE2_ZERO_TERMINATED,
	                                  0,
	                                  &errornumber,
	                                  &erroroffset,
	                                  NULL);

	if (regex == NULL)
	{
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		fprintf(stderr,
		        "FATAL: PCRE2 compilation failed for regex pattern %s at offset "
		        "%d: %s\n",
		        pattern,
		        (int)erroroffset,
		        buffer);
		exit(1);
	}

	return regex;
}

bool match_regex(pcre2_code* regex, const char* str)
{
	if (regex == NULL)
	{
		fprintf(stderr, "FATAL: regex is null\n");
		exit(1);
	}

	pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regex, NULL);
	int rc = pcre2_match(regex, (PCRE2_SPTR)str, strlen(str), 0, 0, match_data, NULL);

	pcre2_match_data_free(match_data);

	return rc >= 0;
}

pcre2_match_data* match_regex_with_data(pcre2_code* regex, const char* str, bool* ok)
{
	if (regex == NULL)
	{
		fprintf(stderr, "FATAL: regex is null\n");
		exit(1);
	}

	pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regex, NULL);
	int rc = pcre2_match(regex, (PCRE2_SPTR)str, strlen(str), 0, 0, match_data, NULL);

	*ok = rc >= 0;

	return match_data;
}

char* find_match_by_group(const char* group_name,
                          const char* input,
                          pcre2_code* regex,
                          PCRE2_SIZE* offset_vector)
{
	int index = pcre2_substring_number_from_name(regex, (PCRE2_SPTR)group_name);
	PCRE2_SPTR substring_start = (PCRE2_SPTR)input + offset_vector[2 * index];
	size_t substring_length    = offset_vector[2 * index + 1] - offset_vector[2 * index];

	char* match = malloc(sizeof(char) * (substring_length + 1));
	strncpy(match, (char*)substring_start, substring_length);
	match[substring_length] = '\0';

	return match;
}
