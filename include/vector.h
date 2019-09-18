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


vector_s *  vector_new(size_t cap);
void        vector_free(vector_s *vector);
void        vector_push_back(vector_s *vector, void *data);
void *      vector_get_index(vector_s *vector, size_t index);
void        vector_remove_index(vector_s *vector, size_t index);


#endif