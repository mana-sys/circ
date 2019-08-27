#include "unity.h"

#include "read_message.h"

static conn_s conn;
static char store_buffer[IRC_MSG_SIZE];
static char expected_message[IRC_MSG_SIZE];

void clear_conn()
{
    memset(&conn, 0, sizeof(struct conn_s));
}

void clear_buffers()
{
    memset(store_buffer, 0, IRC_MSG_SIZE);
    memset(expected_message, 0, IRC_MSG_SIZE);
}

void setUp()
{
    clear_conn();
    clear_buffers();
}

void tearDown()
{

}

static void with_store_buffer(char *content)
{
    strncpy(conn.store, content, IRC_MSG_SIZE);
    conn.totalRead = strlen(content);
}

static void with_discard_next()
{
    conn.discardNext = true;
}

static void expect_discard_next()
{
    TEST_ASSERT_TRUE(conn.discardNext);
}

static void expect_no_discard_next()
{
    TEST_ASSERT_FALSE(conn.discardNext);
}

static void expect_message_buffer(char *message, char *result)
{
    if (message) {
        TEST_ASSERT_EQUAL_PTR(conn.message, result);
        TEST_ASSERT_EQUAL_STRING(message, conn.message);
    } else {
        TEST_ASSERT_EQUAL_PTR(NULL, result);
    }
}

static void expect_total_read(size_t totalRead)
{
    TEST_ASSERT_EQUAL_UINT(totalRead, conn.totalRead);
}

static void test_no_message()
{
    with_store_buffer("");
    expect_message_buffer(NULL, conn_read_message(&conn));
    expect_total_read(0);
}

static void test_one_message()
{
    char *message;

    message = "NICK ben\r\n";

    with_store_buffer(message);
    expect_message_buffer(message, conn_read_message(&conn));
    expect_total_read(0);
}

static void test_two_messages()
{
    with_store_buffer("NICK user1\r\nNICK user2\r\n");

    expect_message_buffer("NICK user1\r\n", conn_read_message(&conn));
    expect_total_read(strlen("NICK user1\r\nNICK user2\r\n") - strlen("NICK user1\r\n"));

    expect_message_buffer("NICK user2\r\n", conn_read_message(&conn));
    expect_total_read(0);

}

static void test_one_message_discard_next()
{
    with_store_buffer("NICK user1\r\n");
    with_discard_next();

    expect_message_buffer(NULL, conn_read_message(&conn));
    expect_total_read(0);
    expect_no_discard_next();
}

static void test_partial_message()
{
    with_store_buffer("NICK user1\r");

    expect_message_buffer(NULL, conn_read_message(&conn));
    expect_total_read(strlen("NICK user1\r"));
}

static void test_one_message_plus_partial()
{
    with_store_buffer("NICK user1\r\nNICK user2\r");

    expect_message_buffer("NICK user1\r\n", conn_read_message(&conn));
    expect_total_read(strlen("NICK user2\r"));

    expect_message_buffer(NULL, conn_read_message(&conn));
    expect_total_read(strlen("NICK user2\r"));
}

static void test_two_messages_discard_next()
{
    with_store_buffer("NICK user1\r\nNICK user2\r\n");
    with_discard_next();

    expect_message_buffer("NICK user2\r\n", conn_read_message(&conn));
    expect_total_read(0);
    expect_no_discard_next();
}

static void test_no_crlf_511_bytes()
{
    char *result;

    memset(store_buffer, 'a', IRC_MSG_SIZE - 2);
    store_buffer[IRC_MSG_SIZE - 2] = '\r';

    memset(expected_message, 'a', IRC_MSG_SIZE - 2);
    expected_message[IRC_MSG_SIZE - 2] = '\r';
    expected_message[IRC_MSG_SIZE - 1] = '\n';

    with_store_buffer(store_buffer);

    result = conn_read_message(&conn);

    TEST_ASSERT_EQUAL_STRING_LEN(expected_message, result, IRC_MSG_SIZE);
    expect_total_read(1);
}

static void test_no_crlf_511_bytes_with_discard_next()
{
    char *result;
    char message[IRC_MSG_SIZE];

    memset(message, 'a', IRC_MSG_SIZE - 2);
    message[IRC_MSG_SIZE - 2] = '\r';

    with_store_buffer(message);
    with_discard_next();

    result = conn_read_message(&conn);

    expect_message_buffer(NULL, result);
    expect_total_read(1);
    expect_no_discard_next();
}

static void test_no_crlf_512_bytes()
{
    char *result;
    char message[IRC_MSG_SIZE];
    char expected[IRC_MSG_SIZE];

    memset(message, 'a', IRC_MSG_SIZE);
    memset(expected, 'a', IRC_MSG_SIZE - 2);
    expected[IRC_MSG_SIZE - 2] = '\r';
    expected[IRC_MSG_SIZE - 1] = '\n';

    with_store_buffer(message);

    result = conn_read_message(&conn);
    TEST_ASSERT_EQUAL_STRING_LEN(expected, result, IRC_MSG_SIZE);
    expect_total_read(2);
}

static void test_no_crlf_512_bytes_with_discard_next()
{
    char message[IRC_MSG_SIZE];

    memset(message, 'a', IRC_MSG_SIZE);

    with_store_buffer(message);
    with_discard_next();

    expect_message_buffer(NULL, conn_read_message(&conn));
    expect_total_read(2);
    expect_no_discard_next();
}

int main(int argc, char *argv[])
{
    UnityBegin("test_read_message1.c");

    RUN_TEST(test_no_message);
    RUN_TEST(test_partial_message);
    RUN_TEST(test_one_message);
    RUN_TEST(test_one_message_discard_next);
    RUN_TEST(test_one_message_plus_partial);
    RUN_TEST(test_two_messages);
    RUN_TEST(test_two_messages_discard_next);
    RUN_TEST(test_no_crlf_511_bytes);
    RUN_TEST(test_no_crlf_511_bytes_with_discard_next);
    RUN_TEST(test_no_crlf_512_bytes);
    RUN_TEST(test_no_crlf_512_bytes_with_discard_next);

    return UnityEnd();
}