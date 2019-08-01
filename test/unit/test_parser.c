#include "vendor/unity.h"
#include "../src/circ.h"

void setUp()
{

}

void tearDown()
{

}

static void test_parse_msg_nick()
{
    char msg[] = "NICK ben\r\n";
    struct circ_msg parsedMsg;
    parse_msg(msg, &parsedMsg);

    TEST_ASSERT_EQUAL_INT(MSGNICK, parsedMsg.msgType);
    TEST_ASSERT_EQUAL_STRING("ben", parsedMsg.msgNick.nick);
}

static void test_parse_msg_nick_no_nickname_given()
{
    char msg[] = "NICK\r\n";
    struct circ_msg parsedMsg;
    int result = parse_msg(msg, &parsedMsg);

    TEST_ASSERT_EQUAL_INT(ERR_NONICKNAMEGIVEN, result);
}

static void test_parse_msg_nick_erroneus_nickname()
{
    char msg[] = "NICK abcdefghijklmnop\r\n";
    struct circ_msg parsedMsg;
    int result = parse_msg(msg, &parsedMsg);

    TEST_ASSERT_EQUAL_INT(ERR_ERRONEUSNICKNAME, result);
}

static void test_parse_msg_user()
{
    char msg[] = "USER ben * * :Ben Llanes\r\n";
    struct circ_msg parsedMsg;
    parse_msg(msg, &parsedMsg);

    TEST_ASSERT_EQUAL_INT(MSGUSER, parsedMsg.msgType);
    TEST_ASSERT_EQUAL_STRING("ben", parsedMsg.msgUser.username);
    TEST_ASSERT_EQUAL_STRING("Ben Llanes", parsedMsg.msgUser.fullname);
}

int main() {
    UnityBegin("test_parser.c");
    RUN_TEST(test_parse_msg_nick);
    RUN_TEST(test_parse_msg_nick_no_nickname_given);
    RUN_TEST(test_parse_msg_nick_erroneus_nickname);
    RUN_TEST(test_parse_msg_user);
    return UnityEnd();
}