#ifndef DAYLOG_VECTOR_H
#define DAYLOG_VECTOR_H

#include <stddef.h>

// -----------------------------
// BaseVector
// -----------------------------

typedef struct
{
	size_t size;
	size_t unit_size;
	size_t capacity;
	int capacity_multiplier;
	int capacity_addend;
} BaseVector;


BaseVector init_base_vector(size_t unit_size,
                            size_t capacity,
                            unsigned int capacity_multiplier,
                            unsigned int capacity_addend);

void* realloc_vector(BaseVector* vector_meta, void* vector_data);

// -----------------------------
// StringVector
// -----------------------------

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
