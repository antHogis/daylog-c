#ifndef DAYLOG_VECTOR_H
#define DAYLOG_VECTOR_H

#include <stddef.h>

// A dynamically allocated array of strings
typedef struct
{
	char** data;
	size_t size;

	// private members

	size_t _capacity;
	int _capacity_multiplier;
	int _capacity_addend;
} StringVector;

StringVector* init_string_vector(size_t capacity,
                                 unsigned int capacity_multiplier,
                                 unsigned int capacity_addend);

void destroy_string_vector(StringVector* vector);

void push_back(StringVector* vector, char* str);

#endif

