#include <string.h>

#include "vendor/unity.h"
#include "../src/read_message.h"

#define MOCK_FULL_MSG "NICK ben\r\n"

#define MOCK_SPLIT_MSG_PT_1 "NICK mich"
#define MOCK_SPLIT_MSG_PT_2 "ele\r\n"
#define MOCK_SPLIT_MESSAGE_FULL "NICK michele\r\n"

#define MOCK_SPLIT_MSG_PLUS_PARTIAL_PT_1 "NICK mich"
#define MOCK_SPLIT_MSG_PLUS_PARTIAL_PT_2 "ele\r\nNICK ben"
#define MOCK_SPLIT_MSG_PLUS_PARTIAL_FIRST_MSG "NICK michele\r\n"
#define MOCK_SPLIT_MSG_PLUS_PARTIAL_PARTIAL_MSG "NICK ben"

void setUp()
{

}

void tearDown()
{

}

ssize_t read_full_message_mock(int fd, void *buf, size_t count)
{
    strcpy((char *) buf, MOCK_FULL_MSG);
    return strlen(MOCK_FULL_MSG);
}

ssize_t read_full_message_split(int fd, void *buf, size_t count)
{
    static int callnum = 0;
    switch (callnum) {
        case 0:
            strcpy((char *) buf, MOCK_SPLIT_MSG_PT_1);
            callnum++;
            return strlen(MOCK_SPLIT_MSG_PT_1);
        case 1:
            strcpy((char *) buf, MOCK_SPLIT_MSG_PT_2);
            callnum++;
            return strlen(MOCK_SPLIT_MSG_PT_2);
        default:
            return 0;
    }
}

static void test_read_full_message() {

    char msg[512], buf[512];
    ssize_t storeTotalRead = 0;
    ssize_t msgLen = read_message(0, msg, buf, &storeTotalRead, read_full_message_mock);

    TEST_ASSERT_EQUAL_UINT(msgLen, strlen(MOCK_FULL_MSG));
    TEST_ASSERT_EQUAL_STRING_LEN(MOCK_FULL_MSG, msg, strlen(MOCK_FULL_MSG));
}

static void test_read_full_message_split() {

    char msg[512], buf[512];
    ssize_t storeTotalRead = 0;
    ssize_t msgLen = read_message(0, msg, buf, &storeTotalRead, read_full_message_split);

    TEST_ASSERT_EQUAL_UINT(msgLen, strlen(MOCK_SPLIT_MESSAGE_FULL));
    TEST_ASSERT_EQUAL_STRING_LEN(MOCK_SPLIT_MESSAGE_FULL, msg, strlen(MOCK_SPLIT_MESSAGE_FULL));
}

int main()
{
    UnityBegin("test_read_message.c");
    RUN_TEST(test_read_full_message);
    RUN_TEST(test_read_full_message_split);
    return UnityEnd();
}