#include "unity.h"
#include "channel.h"
#include "client.h"

channel_s *channel;
client_s   operator = {.clientId = 1};

static void expect_channel_members_internal(client_s **expected);

#define expect_channel_members(...) do {        \
    client_s *expected[] = {__VA_ARGS__, 0};    \
    expect_channel_members_internal(expected);  \
} while (0)                                     \

void setUp()
{

    client_s *expected[] = {0};
    channel = channel_new("#channel", &operator);
}

void tearDown()
{
    free(channel);
}

static void test_channel_new()
{
    expect_channel_members(&operator);
}


int main()
{
    UnityBegin("test_channel.c");

    RUN_TEST(test_channel_new);

    return UnityEnd();
}


static void expect_channel_members_internal(client_s **expected)
{
    client_s *got;
    size_t count;

    count = 0;

    while (*expected) {

        got = g_hash_table_lookup(channel->members, GINT_TO_POINTER((*expected)->clientId));

        TEST_ASSERT_NOT_NULL_MESSAGE(got, "Channel does not contain desired client.");
        TEST_ASSERT_EQUAL_PTR_MESSAGE(*expected, got, "Channel pointers not equal.");

        expected++;
        count++;
    }

    TEST_ASSERT_EQUAL_UINT(count, g_hash_table_size(channel->members));
}