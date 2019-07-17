#include <string.h>
#include <stdio.h>

#include "hashtable.h"

#define HT_INIT_CAP 53

static unsigned long djb2(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << (unsigned long) 5) + hash) + c;

    return hash;
}

static unsigned long djb2a(unsigned char *str)
{
    unsigned long hash = 5381;
    unsigned int c;

    while ((c = *str++)) {
        hash = ((hash << (unsigned long) 5) + hash) ^ c;
    }

    return hash;
}

static struct hashtable_item *new_hashtable_item(char *key, char *value)
{
    struct hashtable_item *item = malloc(sizeof(struct hashtable_item));

    item->key = strdup(key);
    item->value = strdup(value);
    item->deleted = false;

    return item;
}

static int *delete_hashtable_item(struct hashtable_item *item)
{
    free(item->key);
    free(item->value);
    free(item);

    return 0;
}

struct hashtable_table *hashtable_new_table(int size_hint)
{
    struct hashtable_table *ht = malloc(sizeof(struct hashtable_table));

    ht->size = 0;
    ht->cap = HT_INIT_CAP;
    ht->items = calloc(HT_INIT_CAP, sizeof(struct hashtable_item *));

    return ht;
}

int hashtable_insert(struct hashtable_table *table, char *key, char *value)
{
    if (table->size == table->cap)
        return 1;

    size_t loc = djb2((unsigned char *) key) % table->cap;

    while (table->items[loc] != NULL && !table->items[loc]->deleted) {
        if (strcmp(key, table->items[loc]->key) == 0) {
            free(table->items[loc]->value);
            table->items[loc]->value = strdup(value);
            return 0;
        }
        loc = (loc + djb2a((unsigned char *) key) + 1) % table->cap;
    }

    table->items[loc] = new_hashtable_item(key, value);
    table->size++;

    return 0;
}

char *hashtable_search(struct hashtable_table *table, char *key)
{
    size_t loc = djb2((unsigned char *) key) % table->cap;
    while (table->items[loc] != NULL) {
        if (!table->items[loc]->deleted) {
            if (strcmp(table->items[loc]->key, key) == 0) {
                return table->items[loc]->value;
            }
        }

        loc = (loc + djb2a((unsigned char *) key) + 1) % table->cap;
    }

    return NULL;
}



int hashtable_delete_table(struct hashtable_table *table)
{
    size_t i;
    for (i = 0; i < table->cap; i++) {
        if (table->items[i] != NULL) {
            delete_hashtable_item(table->items[i]);
        }
    }
    free(table->items);
    free(table);
    return 0;
}



