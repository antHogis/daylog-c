#ifndef DAYLOG_DAYLOG_H
#define DAYLOG_DAYLOG_H

#include "vector.h"

// Result from reading daylog
typedef struct
{
	StringVector* data;
	int error;
} ReadDayLogResult;

// Read daylog file into an array of strings
ReadDayLogResult read_daylog(char* file_path);

#endif
