#include <glib.h>
#include <stdio.h>

static void key_destroy_func(gpointer data)
{
    g_free(data);
}

int main(int argc, char *argv[])
{
    gint i;
    GHashTable *g;
    gpointer result;

    g = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal, key_destroy_func, NULL);

    char key[20] = "uhh";
    g_hash_table_insert(g, g_strdup(key), GINT_TO_POINTER(1));

    strcpy(key, "hellooo");
    if (g_hash_table_contains(g, "uhh")) {
        printf("Contains helloo");
//    g = g_hash_table_new(g_direct_hash, g_direct_equal);
//
//    g_hash_table_insert(g, GINT_TO_POINTER(1), GINT_TO_POINTER(1));
//
//    result = g_hash_table_lookup(g, GINT_TO_POINTER(1));
//
//    if (result) {
//        printf("Key 1 exists in hash.\n");
    } else {
        printf("Doesnt contain hello\n");
    }

    g_hash_table_destroy(g);
//    for (i = 0; i < 10; i++) {
//        g_hash_table_insert(g, GINT_TO_POINTER(i), GINT_TO_POINTER(i * 10));
//    }
//
//    for (i = 0; i < 10; i++) {
//        printf("Value corresponding to key %d is %d.\n", i,
//                GPOINTER_TO_INT(g_hash_table_lookup(g, GINT_TO_POINTER(i))));
//    }

}