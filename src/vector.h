#ifndef DAYLOG_VECTOR_H
#define DAYLOG_VECTOR_H

#include <stddef.h>

// --------------------------------------------------
// BaseVector
// --------------------------------------------------

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

// --------------------------------------------------
// Macro to Define Type-Specific Vectors
// --------------------------------------------------

#define DECLARE_VECTOR(ITEM_TYPE, VECTOR_TYPE, FUNC_POSTFIX)           \
	typedef struct                                                     \
	{                                                                  \
		ITEM_TYPE* data;                                               \
		BaseVector base;                                               \
	} VECTOR_TYPE;                                                     \
                                                                       \
	VECTOR_TYPE* init_##FUNC_POSTFIX(size_t capacity,                  \
	                                 unsigned int capacity_multiplier, \
	                                 unsigned int capacity_addend);    \
                                                                       \
	void push_##FUNC_POSTFIX(VECTOR_TYPE* vector, ITEM_TYPE val);      \
                                                                       \
	void destroy_##FUNC_POSTFIX(VECTOR_TYPE* vector);

#define DEFINE_VECTOR(ITEM_TYPE, VECTOR_TYPE, FUNC_POSTFIX, FREE_FUNC)                 \
                                                                                       \
	VECTOR_TYPE* init_##FUNC_POSTFIX(size_t capacity,                                  \
	                                 unsigned int capacity_multiplier,                 \
	                                 unsigned int capacity_addend)                     \
	{                                                                                  \
		VECTOR_TYPE* vector = malloc(sizeof(VECTOR_TYPE));                             \
		vector->data        = malloc(sizeof(ITEM_TYPE) * capacity);                    \
		vector->base        = init_base_vector(sizeof(ITEM_TYPE),                      \
                                        capacity,                               \
                                        capacity_multiplier,                    \
                                        capacity_addend);                       \
		return vector;                                                                 \
	}                                                                                  \
                                                                                       \
	void push_##FUNC_POSTFIX(VECTOR_TYPE* vector, ITEM_TYPE val)                       \
	{                                                                                  \
		vector->data                    = realloc_vector(&vector->base, vector->data); \
		vector->data[vector->base.size] = val;                                         \
		vector->base.size++;                                                           \
	}                                                                                  \
                                                                                       \
	void destroy_##FUNC_POSTFIX(VECTOR_TYPE* vector)                                   \
	{                                                                                  \
		if (FREE_FUNC != NULL)                                                         \
		{                                                                              \
			for (size_t i = 0; i < vector->base.size; ++i)                             \
			{                                                                          \
				FREE_FUNC(vector->data[i]);                                            \
			}                                                                          \
		}                                                                              \
		free(vector->data);                                                            \
		free(vector);                                                                  \
	}

// --------------------------------------------------
// Vector implementation type definitions
// --------------------------------------------------

DECLARE_VECTOR(char*, StringVector, string_vector)
DECLARE_VECTOR(int, IntVector, int_vector)

#endif
