#include "vector.h"
#include <stdlib.h>

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

void free_int(int data)
{
	// no-op
}

DEFINE_VECTOR(char*, StringVector, string_vector, free)
DEFINE_VECTOR(int, IntVector, int_vector, free_int)
