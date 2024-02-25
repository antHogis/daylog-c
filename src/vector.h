#ifndef DAYLOG_VECTOR_H
#define DAYLOG_VECTOR_H

#include <stddef.h>

typedef struct
{
	size_t size;
	size_t unit_size;
	size_t capacity;
	int capacity_multiplier;
	int capacity_addend;
} BaseVector;

typedef struct
{
	char** data;
	BaseVector base;
} StringVector;

StringVector* init_string_vector(size_t capacity,
                                 unsigned int capacity_multiplier,
                                 unsigned int capacity_addend);

void destroy_string_vector(StringVector* vector);

void push_string_vector(StringVector* vector, char* str);

#endif
