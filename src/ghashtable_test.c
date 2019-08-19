#include <glib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    gint i;
    GHashTable *g;

    g = g_hash_table_new(g_direct_hash, g_direct_equal);

    for (i = 0; i < 10; i++) {
        g_hash_table_insert(g, GINT_TO_POINTER(i), GINT_TO_POINTER(i * 10));
    }

    for (i = 0; i < 10; i++) {
        printf("Value corresponding to key %d is %d.\n", i,
                GPOINTER_TO_INT(g_hash_table_lookup(g, GINT_TO_POINTER(i))));
    }

}