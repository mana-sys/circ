//
// Created by mana on 9/19/19.
//

#include "unity.h"
#include "vector.h"

vector_s *vec;

typedef struct vector_props_s {
    size_t  size, cap;
    void ** sp;
} vector_props_s;

static void __expect_vector_properties(vector_props_s);
static void      __expect_vector_items(vector_props_s);

void setUp()
{
    vec = vector_new(10);
}


void tearDown()
{
    free(vec);
}


static void test_vector_new()
{
    TEST_ASSERT_EQUAL_UINT(0, vec->size);
    TEST_ASSERT_EQUAL_UINT(10, vec->cap);
    TEST_ASSERT_NOT_NULL(vec->items);
    TEST_ASSERT_EQUAL(vec->items, vec->sp);
}


static void test_vector_push_back() {
    vector_push_back(vec, INT_TO_POINTER(1));

    TEST_ASSERT_EQUAL_UINT(1, vec->size);
    TEST_ASSERT_EQUAL_UINT(10, vec->cap);

    TEST_ASSERT_EQUAL_PTR(vec->items + 1, vec->sp);
    TEST_ASSERT_EQUAL_PTR(*(vec->sp - 1), INT_TO_POINTER(1));
}


static void test_vector_push_back_full_cap()
{
    vec->size = 10;
    vec->sp = vec->items + 10;

    vector_push_back(vec, INT_TO_POINTER(1));

    TEST_ASSERT_EQUAL_UINT(11, vec->size);
    TEST_ASSERT_EQUAL_UINT(20, vec->cap);

    TEST_ASSERT_EQUAL_PTR(vec->items + 11, vec->sp);
    TEST_ASSERT_EQUAL_PTR(*(vec->sp - 1), INT_TO_POINTER(1));
}


static void test_vector_push_back_adjust_cap()
{
    vector_s *vec2 = vector_new(0);

    TEST_ASSERT_EQUAL_PTR(vec2->items, NULL);

    vector_push_back(vec2, INT_TO_POINTER(1));

    TEST_ASSERT_EQUAL_UINT(1, vec2->size);
    TEST_ASSERT_EQUAL_UINT(1, vec2->cap);

    TEST_ASSERT_EQUAL_PTR(vec2->items + 1, vec2->sp);
    TEST_ASSERT_EQUAL_PTR(*(vec2->sp - 1), INT_TO_POINTER(1));

    vector_push_back(vec2, INT_TO_POINTER(2));

    TEST_ASSERT_EQUAL_UINT(2, vec2->size);
    TEST_ASSERT_EQUAL_UINT(2, vec2->cap);

    TEST_ASSERT_EQUAL_PTR(vec2->items + 2, vec2->sp);
    TEST_ASSERT_EQUAL_PTR(*(vec2->sp - 1), INT_TO_POINTER(2));
    TEST_ASSERT_EQUAL_PTR(*(vec2->sp - 2), INT_TO_POINTER(1));

    vector_push_back(vec2, INT_TO_POINTER(3));

    TEST_ASSERT_EQUAL_UINT(3, vec2->size);
    TEST_ASSERT_EQUAL_UINT(4, vec2->cap);

    TEST_ASSERT_EQUAL_PTR(vec2->items + 3, vec2->sp);
    TEST_ASSERT_EQUAL_PTR(*(vec2->sp - 1), INT_TO_POINTER(3));
    TEST_ASSERT_EQUAL_PTR(*(vec2->sp - 2), INT_TO_POINTER(2));
    TEST_ASSERT_EQUAL_PTR(*(vec2->sp - 3), INT_TO_POINTER(1));
}



static void test_vector_peek_back()
{
    int data;

    vector_push_back(vec, INT_TO_POINTER(1));
    data = POINTER_TO_INT(vector_peek_back(vec));

    TEST_ASSERT_EQUAL_UINT(1, vec->size);
    TEST_ASSERT_EQUAL_UINT(10, vec->cap);

    TEST_ASSERT_EQUAL_PTR(vec->items + 1, vec->sp);
    TEST_ASSERT_EQUAL_PTR(*(vec->sp - 1), INT_TO_POINTER(1));

    TEST_ASSERT_EQUAL_INT(1, data);
}


static void test_vector_peek_back_empty()
{
    int data;

    data = POINTER_TO_INT(vector_peek_back(vec));

    TEST_ASSERT_EQUAL_UINT(0, vec->size);
    TEST_ASSERT_EQUAL_UINT(10, vec->cap);

    TEST_ASSERT_EQUAL_PTR(vec->items, vec->sp);

    TEST_ASSERT_EQUAL_INT(NULL, data);
}


static void test_vector_pop_back()
{
    int data;

    vector_push_back(vec, INT_TO_POINTER(1));
    data = POINTER_TO_INT(vector_pop_back(vec));

    TEST_ASSERT_EQUAL_UINT(0, vec->size);
    TEST_ASSERT_EQUAL_UINT(10, vec->cap);

    TEST_ASSERT_EQUAL_PTR(vec->items, vec->sp);

    TEST_ASSERT_EQUAL_INT(1, data);
}


static void test_vector_pop_back_empty()
{
    int data;

    data = POINTER_TO_INT(vector_pop_back(vec));

    TEST_ASSERT_EQUAL_UINT(0, vec->size);
    TEST_ASSERT_EQUAL_UINT(10, vec->cap);

    TEST_ASSERT_EQUAL_PTR(vec->items, vec->sp);

    TEST_ASSERT_EQUAL_INT(NULL, data);
}


static void test_vector_push_and_pop()
{
    int j;

    for (j = 0; j < 11; j++) {
        vector_push_back(vec, INT_TO_POINTER(j));
    }

    for (j = 0; j < 11; j++) {
        TEST_ASSERT_EQUAL_PTR(INT_TO_POINTER(10 - j), vector_peek_back(vec));
        TEST_ASSERT_EQUAL_PTR(INT_TO_POINTER(10 - j), vector_pop_back(vec));
    }

    TEST_ASSERT_EQUAL_UINT(0, vec->size);
    TEST_ASSERT_EQUAL_UINT(20, vec->cap);
    TEST_ASSERT_EQUAL_PTR(vec->items, vec->sp);

}



int main()
{
    UnityBegin("test_vector.c");

    RUN_TEST(test_vector_new);
    RUN_TEST(test_vector_push_back);
    RUN_TEST(test_vector_push_back_full_cap);
    RUN_TEST(test_vector_push_back_adjust_cap);
    RUN_TEST(test_vector_peek_back);
    RUN_TEST(test_vector_peek_back_empty);
    RUN_TEST(test_vector_pop_back);
    RUN_TEST(test_vector_pop_back_empty);

    RUN_TEST(test_vector_push_and_pop);

    return UnityEnd();
}


static void __expect_vector_properties(vector_props_s props)
{
    TEST_ASSERT_EQUAL_UINT(props.size, vec->size);
    TEST_ASSERT_EQUAL_UINT(props.cap, vec->cap);
}
