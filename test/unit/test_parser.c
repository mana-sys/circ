#include <string.h> /* For memset() */
#include <irc.h>

#include "unity.h"
#include "codes.h"
#include "parser.h"

static irc_message_s parsed;

#define expect_message_type(expected) TEST_ASSERT_EQUAL_INT(expected, parsed.type)
#define expect_message_parse_error(expected) TEST_ASSERT_EQUAL_INT(expected, parsed.parse_err)
#define expect_message_parse_error_none() TEST_ASSERT_EQUAL_INT(0, parsed.parse_err)

void setUp()
{
    memset(&parsed, 0, sizeof(irc_message_s));
}

void tearDown()
{
}

static void TestParser_Invalid()
{
    char message[] = "\r\n";

    TEST_ASSERT_EQUAL_INT(-1, parse_message(message, &parsed));
}

static void TestParser_Unknown()
{
    char message[] = "UNKNOWNMESSAGE param1\r\n";

    parse_message(message, &parsed);

    expect_message_type(UNKNOWN);
}


static void test_parse_msg_nick()
{
    char msg[] = "NICK ben\r\n";

    parse_message(msg, &parsed);

    expect_message_type(NICK);
    TEST_ASSERT_EQUAL_STRING("ben", parsed.message.nick.nick);
}

static void TestParser_NickNoNicknameGiven()
{
    char message[] = "NICK\r\n";

    parse_message(message, &parsed);

    expect_message_type(NICK);
    expect_message_parse_error(ERR_NONICKNAMEGIVEN);

}

static void TestParser_NickErroneusNickname()
{
    char message[] = "NICK waytoolongnicknamehere\r\n";

    parse_message(message, &parsed);

    expect_message_type(NICK);
    expect_message_parse_error(ERR_ERRONEUSNICKNAME);
}

static void TestParser_User()
{
    char message[] = "USER user * * :Full Name\r\n";

    parse_message(message, &parsed);

    expect_message_type(USER);

    expect_message_parse_error_none();

    TEST_ASSERT_EQUAL_STRING("user", parsed.message.user.username);
    TEST_ASSERT_EQUAL_STRING("Full Name", parsed.message.user.fullname);

}


static void TestParser_Privmsg()
{
    char message[] = "PRIVMSG target :Hello there!\r\n";

    parse_message(message, &parsed);

    expect_message_type(PRIVMSG);
    expect_message_parse_error_none();

    TEST_ASSERT_EQUAL_STRING("target", parsed.message.privmsg.msgtarget);
    TEST_ASSERT_EQUAL_STRING("Hello there!", parsed.message.privmsg.contents);
}


static void TestParser_PrivmsgNoRecipient()
{
    char message[] = "PRIVMSG\r\n";

    parse_message(message, &parsed);

    expect_message_type(PRIVMSG);
    expect_message_parse_error(ERR_NORECIPIENT);
}

static void TestParser_PrivmsgNoTextToSend()
{
    char message[] = "PRIVMSG target\r\n";

    parse_message(message, &parsed);

    expect_message_type(PRIVMSG);
    expect_message_parse_error(ERR_NOTEXTTOSEND);
}


static void TestParser_UserNeedMoreParams1()
{
    char message[] = "USER\r\n";

    parse_message(message, &parsed);

    expect_message_type(USER);
    expect_message_parse_error(ERR_NEEDMOREPARAMS);
}


static void TestParser_UserNeedMoreParams2()
{
    char message[] = "USER user\r\n";

    parse_message(message, &parsed);

    expect_message_type(USER);
    expect_message_parse_error(ERR_NEEDMOREPARAMS);
}

static void TestParser_UserNeedMoreParams3()
{
    char message[] = "USER user *\r\n";

    parse_message(message, &parsed);

    expect_message_type(USER);
    expect_message_parse_error(ERR_NEEDMOREPARAMS);
}

static void TestParser_UserNeedMoreParams4()
{
    char message[] = "USER user * *\r\n";

    parse_message(message, &parsed);

    expect_message_type(USER);
    expect_message_parse_error(ERR_NEEDMOREPARAMS);
}

static void TestParser_Ping()
{
    char message[] = "PING localhost\r\n";

    parse_message(message, &parsed);

    expect_message_type(PING);
}

static void TestParser_Pong()
{
    char message[] = "PONG localhost\r\n";

    parse_message(message, &parsed);

    expect_message_type(PONG);
}

static void TestParser_Motd()
{
    char message[] = "MOTD\r\n";

    parse_message(message, &parsed);

    expect_message_type(MOTD);
}

static void TestParser_LUsers()
{
    char message[] = "LUSERS\r\n";

    parse_message(message, &parsed);

    expect_message_type(LUSERS);
}

static void TestParser_Join()
{
    char message[] = "JOIN channel\r\n";

    parse_message(message, &parsed);

    expect_message_type(JOIN);
    expect_message_parse_error_none();

    TEST_ASSERT_EQUAL_STRING("channel", parsed.message.join.channels);
}


static void TestParser_JoinNeedMoreParams()
{
    char message[] = "JOIN\r\n";

    parse_message(message, &parsed);

    expect_message_type(JOIN);
    expect_message_parse_error(ERR_NEEDMOREPARAMS);
}


static void TestParser_Part()
{
    char message[] = "PART #channel1,#channel2\r\n";

    parse_message(message, &parsed);

    expect_message_type(PART);
    expect_message_parse_error_none();

    TEST_ASSERT_EQUAL_STRING("#channel1,#channel2", parsed.message.part.channels);
}


static void TestParser_PartNeedMoreParams()
{
    char message[] = "PART\r\n";

    parse_message(message, &parsed);

    expect_message_type(PART);
    expect_message_parse_error(ERR_NEEDMOREPARAMS);
}


static void TestParser_List()
{
    char message[] = "LIST\r\n";

    parse_message(message, &parsed);

    expect_message_type(LIST);
    expect_message_parse_error_none();

    TEST_ASSERT_EQUAL_PTR(NULL, parsed.message.list.channels);
    TEST_ASSERT_EQUAL_PTR(NULL, parsed.message.list.server);

}


static void TestParser_ListChannels()
{
    char message[] = "LIST #channel1,#channel2\r\n";

    parse_message(message, &parsed);

    expect_message_type(LIST);
    expect_message_parse_error_none();

    TEST_ASSERT_EQUAL_STRING("#channel1,#channel2", parsed.message.list.channels);
    TEST_ASSERT_EQUAL_PTR(NULL, parsed.message.list.server);
}



int main() {
    UnityBegin("test_parser.c");

    RUN_TEST(TestParser_Invalid);

    RUN_TEST(TestParser_Unknown);

    RUN_TEST(test_parse_msg_nick);
    RUN_TEST(TestParser_NickNoNicknameGiven);
    RUN_TEST(TestParser_NickErroneusNickname);

    RUN_TEST(TestParser_User);
    RUN_TEST(TestParser_UserNeedMoreParams1);
    RUN_TEST(TestParser_UserNeedMoreParams2);
    RUN_TEST(TestParser_UserNeedMoreParams3);
    RUN_TEST(TestParser_UserNeedMoreParams4);

    RUN_TEST(TestParser_Ping);

    RUN_TEST(TestParser_Pong);

    RUN_TEST(TestParser_Motd);

    RUN_TEST(TestParser_Privmsg);
    RUN_TEST(TestParser_PrivmsgNoRecipient);
    RUN_TEST(TestParser_PrivmsgNoTextToSend);

    RUN_TEST(TestParser_LUsers);

    /*
     * JOIN tests.
     */
    RUN_TEST(TestParser_Join);
    RUN_TEST(TestParser_JoinNeedMoreParams);

    /*
     * PART tests.
     */
    RUN_TEST(TestParser_Part);
    RUN_TEST(TestParser_PartNeedMoreParams);

    /*
     * LIST tests.
     */
    RUN_TEST(TestParser_List);
    RUN_TEST(TestParser_ListChannels);


    return UnityEnd();
}