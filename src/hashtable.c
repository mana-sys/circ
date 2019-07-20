#include <math.h>
#include <string.h>
#include <stdio.h>

#include "hashtable.h"

#define HT_LOAD_FACTOR_UPPER_BOUND 0.75

int is_prime(size_t x)
{
    size_t i;
    if (x == 0)
        return 0;
    if (x == 1)
        return 1;

    for (i = 2; i <= (size_t) floor(sqrt(x)); i++) {
        if (x % i == 0)
            return 0;
    }

    return 1;
}

size_t next_prime(size_t start)
{
    while (!is_prime(start))
        start++;

    return start;
}

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
    item->value = value;
    item->deleted = false;

    return item;
}

static int *delete_hashtable_item(struct hashtable_item *item, destructor_t destructor)
{
    free(item->key);

    if (destructor == NULL) {
        free(item->value);
    } else {
        destructor(item->value);
    }

    free(item);

    return 0;
}

struct hashtable_table *hashtable_new_table(size_t cap_hint, destructor_t destructor)
{
    struct hashtable_table *ht = malloc(sizeof(struct hashtable_table));

    ht->size = ht->occupied = 0;
    ht->cap = next_prime(cap_hint);
    ht->items = calloc(ht->cap, sizeof(struct hashtable_item *));
    ht->destructor = destructor;

    return ht;
}

int hashtable_insert(struct hashtable_table *table, char *key, void *value)
{
    size_t loc, initLoc;

    if (table->size == table->cap)
        return 1;

    loc = initLoc = djb2((unsigned char *) key) % table->cap;

    while (table->items[loc] != NULL) {

        if (!table->items[loc]->deleted) {
            if (strcmp(key, table->items[loc]->key) == 0) {
                table->destructor(table->items[loc]->value);
                table->items[loc]->value = strdup(value);
                return 0;
            }
        } else {
            delete_hashtable_item(table->items[loc], table->destructor);
            table->occupied--;
            break;
        }
        
        loc = (loc + djb2a((unsigned char *) key) + 1) % table->cap;
        if (loc == initLoc)
            return 1;
    }

    table->items[loc] = new_hashtable_item(key, value);
    table->size++;
    table->occupied++;

    return 0;
}

void *hashtable_search(struct hashtable_table *table, char *key)
{
    size_t loc;
    struct hashtable_item *item;

    loc = djb2((unsigned char *) key) % table->cap;
    item = table->items[loc];

    while (item != NULL) {
        if (!item->deleted) {
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }

        loc = (loc + djb2a((unsigned char *) key) + 1) % table->cap;
        item = table->items[loc];
    }

    return NULL;
}

int hashtable_remove(struct hashtable_table *table, char *key)
{
    size_t loc;
    struct hashtable_item *item;

    loc = djb2((unsigned char *) key) % table->cap;
    item = table->items[loc];

    while (item != NULL) {
        if (!item->deleted) {
            if (strcmp(item->key, key) == 0) {
                item->deleted = true;
                table->size--;
                return 0;
            }
        }

        loc = (loc + djb2a((unsigned char *) key) + 1) % table->cap;
        item = table->items[loc];
    }

    return 1;
}

int hashtable_delete_table(struct hashtable_table *table)
{
    size_t i;
    for (i = 0; i < table->cap; i++) {
        if (table->items[i] != NULL) {
            delete_hashtable_item(table->items[i], table->destructor);
        }
    }

    free(table->items);
    free(table);
    return 0;
}



