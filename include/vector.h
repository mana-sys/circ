//
// Created by mana on 9/15/19.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

#ifndef INT_TO_POINTER
#define INT_TO_POINTER(p) ((void *) (long) p)
#endif

#ifndef POINTER_TO_INT
#define POINTER_TO_INT(p) ((int) (long) p)
#endif


typedef struct vector_s {
    size_t  size, cap;
    void ** items;
    void ** sp;
} vector_s;


typedef struct vector_iter_s {

} vector_iter_s;


/**
 * Creates a new vector with the initial capacity.
 *
 * @param cap The initial capacity of the new vector
 * @return A pointer to the new vector
 */
vector_s * vector_new(size_t cap);


/**
 * Returns the length of the vector.
 *
 * @param vector The vector to get the size of
 * @return The length of the vector.
 */
size_t vector_size(vector_s *vector);


/**
 * Frees the vector and any associated memory.
 *
 * @param vector The vector to free.
 */
void vector_free(vector_s *vector);


/**
 * Returns the last element of the vector.
 *
 * @param vector The vector to perform the peek on.
 * @return The last element of the vector, or NULL if the vector is of size 0.
 */
void * vector_peek_back(vector_s * vector);

/**
 * Pushes the given data to the back of the vector. This may trigger a resize if the capacity of
 * the vector is not large enough to hold the new element.
 *
 * @param vector The vector on which to perform the operation on.
 * @param data The data to push.
 */
void vector_push_back(vector_s *vector, void *data);


/**
 * Pops the last element off of the back of the vector, and returns it. The data is not actually removed;
 * rather, the stack pointer is adjusted to point to the previous element.
 *
 * @param vector The vector on which to perform the pop.
 * @return The data popped off of the vector
 */
void * vector_pop_back(vector_s *vector);


/**
 * Gets the element at the specified index.
 *
 * @param vector The vector on which to perform the get
 * @param index The index of the desired element
 * @return The data at the given index
 */
void * vector_get_index(vector_s *vector, size_t index);
void * vector_remove_index(vector_s *vector, size_t index);


#endif