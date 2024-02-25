#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BaseVector init_base_vector(size_t unit_size,
                            size_t capacity,
                            unsigned int capacity_multiplier,
                            unsigned int capacity_addend)
{
	BaseVector base = {
		.unit_size           = unit_size,
		.capacity            = capacity,
		.size                = 0,
		.capacity_multiplier = capacity_multiplier,
		.capacity_addend     = capacity_addend,
	};

	return base;
}

StringVector* init_string_vector(size_t capacity,
                                 unsigned int capacity_multiplier,
                                 unsigned int capacity_addend)
{
	StringVector* vector = malloc(sizeof(StringVector));
	vector->data         = malloc(sizeof(char*) * capacity);

	vector->base =
		init_base_vector(sizeof(char*), capacity, capacity_multiplier, capacity_addend);

	return vector;
}

void destroy_string_vector(StringVector* vector)
{
	for (size_t i = 0; i < vector->base.size; ++i)
	{
		free(vector->data[i]);
	}
	free(vector->data);
	free(vector);
}

void* realloc_vector(BaseVector* vector_meta, void* vector_data)
{
	// Don't reallocate if not at full capacity
	if (vector_meta->size < vector_meta->capacity)
	{
		return vector_data;
	}

	size_t new_capacity = (vector_meta->capacity + vector_meta->capacity_addend) *
		vector_meta->capacity_multiplier;

	vector_data           = realloc(vector_data, vector_meta->unit_size * new_capacity);
	vector_meta->capacity = new_capacity;
	return vector_data;
}

void push_string_vector(StringVector* vector, char* val)
{
	vector->data = realloc_vector(&vector->base, vector->data);
	vector->data[vector->base.size] = val;
	vector->base.size++;
}
