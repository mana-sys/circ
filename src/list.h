//
// Created by mana on 8/12/19.
//

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef int (*list_data_cmp_fn)(void *, void *);

struct node_s {
    void *data;
    struct node_s *prev, *next;
};

struct list_s {
    struct node_s *head;
};

struct list_s * list_new         ();
void *          list_find        (struct list_s *list);
void *          list_find_custom (struct list_s *list, void *data, list_data_cmp_fn cmpFn);
int             list_push        (struct list_s *list, void *data);
void *          list_pop         (struct list_s *list);
int             list_remove      (struct list_s *list, size_t loc);
int             list_insert      (struct list_s *list, size_t loc);

#endif //CIRC_LIST_H
