#ifndef DAYLOG_REGEX_H
#define DAYLOG_REGEX_H

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>
#include <stdbool.h>

pcre2_code* init_regex(const char* pattern);

bool match_regex(pcre2_code* regex, const char* str);

pcre2_match_data* match_regex_with_data(pcre2_code* regex, const char* str, bool* ok);

char* find_match_by_group(const char* group_name,
                          const char* input,
                          pcre2_code* regex,
                          PCRE2_SIZE* offset_vector);

#endif
