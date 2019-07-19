#include "vendor/unity.h"
#include "../src/hashtable.h"

void setUp()
{

}

void tearDown()
{

}

static void noop_destructor(void *ptr)
{
}

extern size_t next_prime(size_t);

void test_next_prime()
{

    size_t i;
    struct {
        size_t input;
        size_t result;
    } tt[] = {
            {.input = 0, .result = 1},
            {.input = 1, .result = 1},
            {.input = 4, .result = 5},
            {.input = 6, .result = 7},
            {.input = 8, .result = 11},
            {.input = 9, .result = 11},
            {.input = 12, .result = 13},
            {.input = 24, .result = 29},
            {.input = 45, .result = 47},
            {.input = 60, .result = 61},
            {.input = 100, .result = 101},
            {.input = 112, .result = 113},
            {.input = 203, .result = 211},
            {.input = 260, .result = 263},
            {.input = 1743, .result = 1747}
    };

    for (i = 0; i < sizeof(tt) / sizeof(tt[0]); i++)
    {
        TEST_ASSERT_EQUAL_UINT(tt[i].result, next_prime(tt[i].input));
    }
}

/*
 * Begin white-box tests.
 */

void test_hashtable_new_table()
{
    struct hashtable_table *table;

    table = hashtable_new_table(50, noop_destructor);

    TEST_ASSERT_EQUAL_UINT(table->cap, 53);
    TEST_ASSERT_EQUAL_UINT(table->size, 0);
    TEST_ASSERT_EQUAL_PTR(noop_destructor, table->destructor);

    hashtable_delete_table(table);
}

/*
 * End white-box tests.
 */

/*
 * Begin black-box tests.
 */

void test_hashtable_insert()
{
    struct hashtable_table *table;
    int result;

    table = hashtable_new_table(7, noop_destructor);

    result = hashtable_insert(table, "key1", (void *) 2); /* "key1" hashes to index 2 */

    TEST_ASSERT_EQUAL_INT(0, result);

    TEST_ASSERT_EQUAL_UINT(table->cap, 7);
    TEST_ASSERT_EQUAL_UINT(table->size, 1);
    TEST_ASSERT_NOT_NULL(table->items[2])
    TEST_ASSERT_EQUAL_PTR((void *) 2, table->items[2]->value);

    hashtable_delete_table(table);
}

void test_hashtable_insert_no_collision()
{
    struct hashtable_table *table;
    int result1, result2;

    table = hashtable_new_table(7, noop_destructor);

    result1 = hashtable_insert(table, "key1", (void *) 2); /* "key1" hashes to index 2 */
    result2 = hashtable_insert(table, "key2", (void *) 3); /* "key2" hashes to index 3 */


    TEST_ASSERT_EQUAL_INT(0, result1);
    TEST_ASSERT_EQUAL_INT(0, result2);

    TEST_ASSERT_EQUAL_UINT(table->cap, 7);
    TEST_ASSERT_EQUAL_UINT(table->size, 2);

    TEST_ASSERT_NOT_NULL(table->items[2])
    TEST_ASSERT_EQUAL_PTR((void *) 2, table->items[2]->value);

    TEST_ASSERT_NOT_NULL(table->items[3])
    TEST_ASSERT_EQUAL_PTR((void *) 3, table->items[3]->value);

    hashtable_delete_table(table);
}

void test_hashtable_insert_collision()
{
    struct hashtable_table *table;
    int result1, result2;

    table = hashtable_new_table(7, noop_destructor);

    // djb2("key1") % 7 = 2; expected index = 2
    result1 = hashtable_insert(table, "key1", (void *) 2); /* "key1" hashes to index 2 */

    // djb2("key8") % 7 = 2, djb2a("key8") % 7 = 1; expected index = 2 + (1 + 1) = 4
    result2 = hashtable_insert(table, "key8", (void *) 8);

    TEST_ASSERT_EQUAL_INT(0, result1);
    TEST_ASSERT_EQUAL_INT(0, result2);

    TEST_ASSERT_EQUAL_UINT(table->cap, 7);
    TEST_ASSERT_EQUAL_UINT(table->size, 2);

    TEST_ASSERT_NOT_NULL(table->items[2])
    TEST_ASSERT_EQUAL_STRING("key1", table->items[2]->key);
    TEST_ASSERT_EQUAL_PTR((void *) 2, table->items[2]->value);

    TEST_ASSERT_NOT_NULL(table->items[4])
    TEST_ASSERT_EQUAL_STRING("key8", table->items[4]->key);
    TEST_ASSERT_EQUAL_PTR((void *) 8, table->items[4]->value);

    hashtable_delete_table(table);
}

void test_hashtable_delete()
{
    struct hashtable_table *table;
    int result;

    table = hashtable_new_table(7, noop_destructor);

    hashtable_insert(table, "key1", (void *) 2); /* "key1" hashes to index 2 */
    result = hashtable_remove(table, "key1");

    TEST_ASSERT_EQUAL_INT(0, result);

    TEST_ASSERT_EQUAL_UINT(table->cap, 7);
    TEST_ASSERT_EQUAL_UINT(table->size, 0);

    TEST_ASSERT_NOT_NULL(table->items[2])
    TEST_ASSERT_EQUAL_STRING("key1", table->items[2]->key);
    TEST_ASSERT_EQUAL_PTR((void *) 2, table->items[2]->value);
    TEST_ASSERT_EQUAL_INT(true, table->items[2]->deleted);

    hashtable_delete_table(table);
}

void test_hashtable_delete_and_reinsert_at_same_key()
{
    struct hashtable_table *table;
    int result;

    table = hashtable_new_table(7, noop_destructor);

    hashtable_insert(table, "key1", (void *) 2); /* "key1" hashes to index 2 */
    hashtable_remove(table, "key1");
    result = hashtable_insert(table, "key8", (void *) 3);

    TEST_ASSERT_EQUAL_INT(0, result);

    TEST_ASSERT_EQUAL_UINT(table->cap, 7);
    TEST_ASSERT_EQUAL_UINT(table->size, 1);

    TEST_ASSERT_NOT_NULL(table->items[2])
    TEST_ASSERT_EQUAL_STRING("key8", table->items[2]->key);
    TEST_ASSERT_EQUAL_PTR((void *) 3, table->items[2]->value);
    TEST_ASSERT_EQUAL_INT(false, table->items[2]->deleted);

    hashtable_delete_table(table);
}

void test_hashtable_search()
{
    struct hashtable_table *table;
    void *result;

    table = hashtable_new_table(7, noop_destructor);

    hashtable_insert(table, "key1", (void *) 2);
    result = hashtable_search(table, "key1");

    TEST_ASSERT_EQUAL_PTR((void *) 2, result);

    hashtable_delete_table(table);
}



/*
 * End black-box tests.
 */

int main()
{
    UnityBegin("test_hashtable.c");
    RUN_TEST(test_next_prime);
    RUN_TEST(test_hashtable_new_table);
    RUN_TEST(test_hashtable_insert);
    RUN_TEST(test_hashtable_insert_no_collision);
    RUN_TEST(test_hashtable_insert_collision);
    RUN_TEST(test_hashtable_search);
    RUN_TEST(test_hashtable_delete);
    RUN_TEST(test_hashtable_delete_and_reinsert_at_same_key);
    return UnityEnd();
}
