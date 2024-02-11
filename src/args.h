#ifndef DAYLOG_ARGS_H
#define DAYLOG_ARGS_H

#include <stdbool.h>

// The arguments and options of daylog
typedef struct
{
	char* date_arg;
	bool verbose;
	bool use_csv;
	char* start_date;
	char* end_date;
	char* file_path;
} Arguments;

// Parse CLI arguments.
void parse_args(int argc, char** argv, Arguments* args);

#endif
