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
