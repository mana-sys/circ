#include <stdio.h>
#include <assert.h>
#include "hashtable.h"

int main()
{
    struct hashtable_table *table = hashtable_new_table(53);
    hashtable_insert(table, "ben", "llanes");
    hashtable_insert(table, "ben", "michele");
    hashtable_insert(table, "marc", "castillo");
    char *benval = hashtable_search(table, "ben");

    fprintf(stderr, "%s\n", benval);
    char *marcval = hashtable_search(table, "marc");

    fprintf(stderr, "%s\n", marcval);

    hashtable_delete_table(table);
}
