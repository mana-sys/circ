#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

struct circlist_node {
    char *key;
    void *value;
    struct circlist_node *prev, *next;
};

struct circlist {
    size_t len;
    struct circlist_node *head, *tail;
    void (*destructor)(void *);
};

struct circlist *circlist_new();
int circlist_append(struct circlist *list, char *key, void *value);
int circlist_delete(struct circlist *list, char *key);
void *circlist_search(struct circlist *list, char *key);
void circlist_free(struct circlist *list);

#endif
