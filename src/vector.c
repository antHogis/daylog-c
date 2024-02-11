#include "vector.h"
#include <stdlib.h>
#include <string.h>

StringVector* init_string_vector(size_t capacity,
                                 unsigned int capacity_multiplier,
                                 unsigned int capacity_addend)
{
	StringVector* vector = malloc(sizeof(StringVector));
	vector->_capacity    = capacity;
	vector->size         = 0;
	vector->data         = malloc(sizeof(char*) * capacity);

	// TODO raise error if multiplier less than 1
	vector->_capacity_multiplier = capacity_multiplier;
	vector->_capacity_addend     = capacity_addend;

	return vector;
}

void push_back(StringVector* vector, char* str)
{
	if (vector->size == vector->_capacity)
	{
		size_t new_capacity =
			(vector->_capacity + vector->_capacity_addend) * vector->_capacity_multiplier;

		vector->data      = realloc(vector->data, sizeof(char*) * new_capacity);
		vector->_capacity = new_capacity;
	}

	vector->data[vector->size] = str;
	vector->size += 1;
}
