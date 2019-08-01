#include <string.h>

#include "vendor/unity.h"
#include "../../src/read_message.h"

#define MOCK_FULL_MSG_TEST_CASE 1
#define MOCK_FULL_MSG "NICK ben\r\n"

#define MOCK_SPLIT_MSG_TEST_CASE 2
#define MOCK_SPLIT_MSG_PT_1 "NICK mich"
#define MOCK_SPLIT_MSG_PT_2 "ele\r\n"
#define MOCK_SPLIT_MESSAGE_FULL "NICK michele\r\n"

#define MOCK_MSG_PLUS_PARTIAL "NICK michele\r\nNICK ben"

#define MOCK_SPLIT_MSG_PLUS_PARTIAL_PT_1 "NICK mich"
#define MOCK_SPLIT_MSG_PLUS_PARTIAL_PT_2 "ele\r\nNICK ben"
#define MOCK_SPLIT_MSG_PLUS_PARTIAL_FIRST_MSG "NICK michele\r\n"
#define MOCK_SPLIT_MSG_PLUS_PARTIAL_PARTIAL_MSG "NICK ben"

#define DISCARD_NEXT_TEST_CASE 4
#define DISCARD_NEXT_TEST_CASE_INPUT "asdfa\r\nNICK ben\r\n"

#define READ_TWO_MESSAGES_TEST_CASE 5

void setUp()
{

}

void tearDown()
{

}

static int g_current_test_case;

ssize_t read_full_message_mock(int fd, void *buf, size_t count)
{
    memcpy((char *) buf, MOCK_FULL_MSG, strlen(MOCK_FULL_MSG));
    return strlen(MOCK_FULL_MSG);
}

ssize_t read_full_message_split(int fd, void *buf, size_t count)
{
    static int callnum = 0;
    switch (callnum) {
        case 0:
            memcpy((char *) buf, MOCK_SPLIT_MSG_PT_1, strlen(MOCK_SPLIT_MSG_PT_1));
            callnum++;
            return strlen(MOCK_SPLIT_MSG_PT_1);
        case 1:
            memcpy((char *) buf, MOCK_SPLIT_MSG_PT_2, strlen(MOCK_SPLIT_MSG_PT_2));
            callnum++;
            return strlen(MOCK_SPLIT_MSG_PT_2);
        default:
            return 0;
    }
}

ssize_t read_mock_msg_plus_partial(int fd, void *buf, size_t count)
{
    strcpy((char *) buf, "NICK michele\r\nNICK ben");
    return strlen("NICK michele\r\nNICK ben");
}

static void test_read_full_message() {
    g_current_test_case = MOCK_FULL_MSG_TEST_CASE;
    char msg[512], buf[512];
    ssize_t storeTotalRead = 0;
    bool storeDiscardNext = false;
    ssize_t msgLen = read_message(0, msg, buf, &storeTotalRead, &storeDiscardNext);

    TEST_ASSERT_EQUAL_UINT(msgLen, strlen(MOCK_FULL_MSG));
    TEST_ASSERT_EQUAL_STRING_LEN(MOCK_FULL_MSG, msg, strlen(MOCK_FULL_MSG));
    g_current_test_case = 0;
}
//
static void test_read_full_message_split() {
    g_current_test_case = MOCK_SPLIT_MSG_TEST_CASE;

    char msg[512], buf[512];
    ssize_t storeTotalRead = 0;
    bool storeDiscardNext = false;
    ssize_t msgLen = read_message(0, msg, buf, &storeTotalRead, &storeDiscardNext);

    TEST_ASSERT_EQUAL_UINT(msgLen, strlen(MOCK_SPLIT_MESSAGE_FULL));
    TEST_ASSERT_EQUAL_STRING_LEN(MOCK_SPLIT_MESSAGE_FULL, msg, strlen(MOCK_SPLIT_MESSAGE_FULL));

    g_current_test_case = 0;
}
//
//static void test_read_msg_plus_partial() {
//
//    char msg[512], buf[512];
//    ssize_t storeTotalRead = 0;
//    ssize_t msgLen = read_message(0, msg, buf, &storeTotalRead, read_mock_msg_plus_partial);
//
//    TEST_ASSERT_EQUAL_UINT(msgLen, strlen("NICK michele\r\n"));
//    TEST_ASSERT_EQUAL_STRING_LEN("NICK michele\r\n", msg, strlen("NICK michele\r\n"));
//
//    TEST_ASSERT_EQUAL_UINT(storeTotalRead - msgLen, strlen("NICK ben"));
//    TEST_ASSERT_EQUAL_STRING_LEN("NICK ben", buf, strlen("NICK ben"));
//}

static void test_discard_next() {
    g_current_test_case = DISCARD_NEXT_TEST_CASE;

    char msg[512], buf[512];
    ssize_t storeTotalRead = 0;
    bool storeDiscardNext = true;
    ssize_t msgLen = read_message(0, msg, buf, &storeTotalRead, &storeDiscardNext);

//    TEST_ASSERT_EQUAL_UINT(msgLen, strlen(MOCK_SPLIT_MESSAGE_FULL));
//    TEST_ASSERT_EQUAL_STRING_LEN(MOCK_SPLIT_MESSAGE_FULL, msg, strlen(MOCK_SPLIT_MESSAGE_FULL));

    g_current_test_case = 0;
}

static void test_read_two_messages()
{
    g_current_test_case = READ_TWO_MESSAGES_TEST_CASE;
    char msg[512], buf[512];
    ssize_t storeTotalRead = 0;
    bool storeDiscardNext = false;

    memset(msg, 0, 512);
    memset(buf, 0, 512);

    ssize_t msgLen = read_message(0, msg, buf, &storeTotalRead, &storeDiscardNext);

    TEST_ASSERT_EQUAL_INT(msgLen, strlen("NICK ben\r\n"));
    TEST_ASSERT_EQUAL_STRING_LEN("NICK ben\r\n", msg, msgLen);
    TEST_ASSERT_EQUAL_INT(strlen("NICK michele\r\n"), storeTotalRead);
    TEST_ASSERT_EQUAL_INT(storeDiscardNext, false);

    msgLen = read_message(0, msg, buf, &storeTotalRead, &storeDiscardNext);

    TEST_ASSERT_EQUAL_INT(msgLen, strlen("NICK michele\r\n"));
    TEST_ASSERT_EQUAL_STRING_LEN("NICK michele\r\n", msg, msgLen);
    TEST_ASSERT_EQUAL_INT(0, storeTotalRead);
    TEST_ASSERT_EQUAL_INT(storeDiscardNext, false);
}

ssize_t __wrap_read(int fd, char *buf, size_t count)
{
    switch (g_current_test_case) {
        case MOCK_FULL_MSG_TEST_CASE:
            return read_full_message_mock(fd, buf, count);
        case MOCK_SPLIT_MSG_TEST_CASE:
            return read_full_message_split(fd, buf, count);
        case DISCARD_NEXT_TEST_CASE:
            memcpy(buf, DISCARD_NEXT_TEST_CASE_INPUT, strlen(DISCARD_NEXT_TEST_CASE_INPUT));
            return strlen(DISCARD_NEXT_TEST_CASE_INPUT);
        case READ_TWO_MESSAGES_TEST_CASE:
            memcpy(buf, "NICK ben\r\nNICK michele\r\n", strlen("NICK ben\r\nNICK michele\r\n"));
            return strlen("NICK ben\r\nNICK michele\r\n");
        default:
            return 0;
    }
}

int main()
{
    UnityBegin("test_read_message.c");
    RUN_TEST(test_read_full_message);
    RUN_TEST(test_read_full_message_split);
    RUN_TEST(test_discard_next);
    RUN_TEST(test_read_two_messages);
    return UnityEnd();
}