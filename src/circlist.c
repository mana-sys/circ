#include <string.h>
#include "circlist.h"

static struct circlist_node *circlist_new_node(char *key, void *value, struct circlist_node *prev,
        struct circlist_node *next)
{
    struct circlist_node *node = malloc(sizeof(struct circlist_node));
    node->key = strdup(key);
    node->value = value;
    node->prev = prev;
    node->next = next;

    return node;
}

struct circlist *circlist_new(void (*destructor)(void *))
{
    struct circlist *list = malloc(sizeof(struct circlist));

    list->len = 0;
    list->head = list->tail = NULL;
    list->destructor = destructor;

    return list;
}

int circlist_append(struct circlist *list, char *key, void *value)
{
    if (list->head == NULL) {

    }
    return 0;
}