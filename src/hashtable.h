//
// Created by manaxlalanes on 7/15/19.
//

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stdlib.h>

typedef void (*destructor_t)(void *);

struct hashtable_item {
    char *key;
    void *value;
    bool deleted;
};

struct hashtable_table {
    size_t size, cap;
    struct hashtable_item **items;
    destructor_t destructor;
};

struct hashtable_table *hashtable_new_table(size_t cap_hint, destructor_t destructor);
int hashtable_insert(struct hashtable_table *table, char *key, void *value);
void *hashtable_search(struct hashtable_table *table, char *key);
int hashtable_remove(struct hashtable_table *table, char *key);
int hashtable_delete_table(struct hashtable_table *table);

#endif //CIRC_HASHTABLE_H
