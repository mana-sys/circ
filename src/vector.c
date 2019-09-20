//
// Created by mana on 9/16/19.
//

#include "vector.h"

vector_s *vector_new(size_t cap)
{
    vector_s *vector;

    vector = calloc(1, sizeof(vector_s));

    vector->size = 0;
    vector->cap = cap;

    if (cap)
        vector->items = malloc(cap * sizeof(void *));
    else
        vector->items = NULL;

    vector->sp = vector->items;

    return vector;
}


void vector_free(vector_s *vector)
{
    free(vector);
}


void vector_push_back(vector_s *vector, void *data)
{
    if (vector->size == vector->cap) {

        if (vector->cap == 0)
            vector->cap = 1;
        else
            vector->cap *= 2;

        vector->items = realloc(vector->items, vector->cap * sizeof(void *));
        vector->sp = vector->items + vector->size;
    }

    vector->size++;
    *vector->sp++ = data;
}


void *vector_peek_back(vector_s * vector)
{
    if (vector->size)
        return *(vector->sp - 1);

    return NULL;
}


void * vector_pop_back(vector_s *vector)
{
    if (vector->size) {
        vector->size--;
        vector->sp--;
        return *vector->sp;
    }

    return NULL;
}


//void * vector_get_index(vector_s *vector, size_t index)
//{
//
//}
//
//void vector_remove_index(vector_s *vector, size_t index)
//{
//
//}