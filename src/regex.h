#ifndef DAYLOG_REGEX_H
#define DAYLOG_REGEX_H

#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>
#include <stdbool.h>

pcre2_code* init_regex(const char* pattern);

bool match_regex(pcre2_code* regex, const char* str);

#endif
