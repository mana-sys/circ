//
// Created by mana on 10/2/19.
//

#include <stdio.h>

#include "unity.h"
#include "oper.h"

static void test_parse_oper_passwd();

static void destroy_func(gpointer data);

int main(int argc, char *argv[])
{
    UnityBegin("test_oper.c");

    RUN_TEST(test_parse_oper_passwd);

    return UnityEnd();
}


static void test_parse_oper_passwd()
{
    GHashTable *oper;
    FILE *f;

    char temp_path[] = "/tmp/oper-XXXXXX";

    oper = g_hash_table_new_full((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal,
            destroy_func, destroy_func);

    mkstemp(temp_path);

    f = fopen(temp_path, "w");
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "unable to open temporary oper passwd file");

    fputs("user1,password123!@#\nuser2,password321", f);
    fclose(f);

    parse_oper_passwd(temp_path, oper);

    TEST_ASSERT_EQUAL_STRING("password123!@#", g_hash_table_lookup(oper, "user1"));
    TEST_ASSERT_EQUAL_STRING("password321", g_hash_table_lookup(oper, "user2"));

    g_hash_table_destroy(oper);

}

static void destroy_func(gpointer data)
{
    g_free(data);
}

void setUp()
{
}

void tearDown()
{
}
