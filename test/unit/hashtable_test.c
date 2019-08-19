#include <stdlib.h>

#include "hashtable.h"
#include "unity_fixture.h"

TEST_GROUP(hash_table);

struct hashtable_table *table;

static void noop_destructor(void *ptr)
{
}

TEST_SETUP(hash_table)
{
    table = hashtable_new_table(7, noop_destructor);
}

TEST_TEAR_DOWN(hash_table)
{
    free(table);
}

TEST(hash_table, NewTable)
{
    TEST_ASSERT_EQUAL_UINT(table->cap, 53);
    TEST_ASSERT_EQUAL_UINT(table->size, 0);
    TEST_ASSERT_EQUAL_PTR(noop_destructor, table->destructor);
}
