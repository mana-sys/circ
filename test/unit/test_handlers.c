#include <connection.h>
#include "unity.h"

#include "codes.h"
#include "handlers.h"
#include "log.h"

#define STOP_MARKER 513
#define MOTD_LONG "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam " \
    "imperdiet elementum leo, nec cursus diam."

#define MOTD_LONG_FIRST_PART "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam imperdiet elemen"
#define MOTD_LONG_SECOND_PART "tum leo, nec cursus diam."

/*
 * Test data to be passed to handlers.
 */
static client_s      client;
static server_s      server;
static irc_message_s message;
static GHashTable *  nicknames;
static GHashTable *  clients;
char                 response[IRC_MSG_SIZE];
size_t               len;
GQueue *             responses;

/*
 * Structure to hold nickname-client ID pairs.
 */
typedef struct nick_id_s {
    int id;
    char nick[IRC_NICK_SIZE];
} nick_id_s;

typedef struct clients_entry_s {
    int clientId;
    client_s *client;
} clients_entry_s;


void setUp()
{
    nicknames = g_hash_table_new((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal);
    clients   = g_hash_table_new(g_direct_hash, g_direct_equal);
    responses = g_queue_new();
    memset(response, 0, IRC_MSG_SIZE);
    len = 0;
}

void tearDown()
{
    g_hash_table_destroy(nicknames);
    g_hash_table_destroy(clients);
    g_queue_free(responses);
}


static void expect_responses_internal(response_s * expected)
{
    response_s *queued;

    while (expected->len != STOP_MARKER) {
        queued = g_queue_pop_head(responses);

        TEST_ASSERT_NOT_NULL_MESSAGE(queued, "Queue has less messages than expected")
        TEST_ASSERT_EQUAL_STRING(expected->response, queued->response);

        free(queued);
        expected++;
    }

    TEST_ASSERT_TRUE_MESSAGE(g_queue_is_empty(responses), "Received more messages than expected");

}

#define expect_responses(...) {                                                 \
    response_s *expected = (response_s[]) {__VA_ARGS__, {.len = STOP_MARKER}};  \
    expect_responses_internal(expected);                                        \
}

#define expect_responses_none() TEST_ASSERT_TRUE_MESSAGE(g_queue_is_empty(responses), "Queue is not empty")


/*
 * Adds the nicknames in nick_list to the nicknames hash.
 */
static void with_nicks_internal(nick_id_s * nick_list)
{
    for (; nick_list->id; nick_list++) {
        g_hash_table_insert(nicknames, nick_list->nick, GINT_TO_POINTER(nick_list->id));
    }
}

/*
 * Configures the message test data.
 */
static void with_message_internal(irc_message_s *message_)
{
    memcpy(&message, message_, sizeof(irc_message_s));
}

/*
 * Configures the client test data.
 */
static void with_client_internal(client_s *client_) {
    memcpy(&client, client_, sizeof(client_s));
}

/*
 * Configures the server test data.
 */
static void with_server_internal(server_s *server_) {
    memcpy(&server, server_, sizeof(server_s));
}

static void expect_nicknames_contains_internal(nick_id_s pair)
{
    int result;

    result = GPOINTER_TO_INT(g_hash_table_lookup(nicknames, pair.nick));
    TEST_ASSERT_EQUAL_INT(pair.id, result);

}

static void expect_clients_contains_internal(clients_entry_s pair)
{
    client_s *result;

    result = g_hash_table_lookup(server.clients, GINT_TO_POINTER(pair.clientId));
    TEST_ASSERT_EQUAL_PTR(pair.client, result);
}

#define expect_clients_contains(...) expect_clients_contains_internal((clients_entry_s){__VA_ARGS__})

static void expect_nicknames_contains_not(char *nick)
{
    TEST_ASSERT_NULL(g_hash_table_lookup(nicknames, nick));
}

/*
 * Setup macros to facilitate setting up test data.
 */
#define with_nicks(...) with_nicks_internal((nick_id_s []){__VA_ARGS__, {.id = 0}})
#define with_message(...) with_message_internal(&(irc_message_s) {__VA_ARGS__})
#define with_client(...) with_client_internal(&(client_s) {__VA_ARGS__})
#define with_server(...) with_server_internal(&(server_s) {__VA_ARGS__})

/*
 * Expectation macros for easier expectation checking.
 */

#define expect_nicknames_contains(...) expect_nicknames_contains_internal((nick_id_s) {__VA_ARGS__})
#define expect_response(expected) TEST_ASSERT_EQUAL_STRING(expected, response)
#define expect_response_len(expected) TEST_ASSERT_EQUAL_UINT(expected, len)

static void TestHandlers_Run()
{
    Handler_HandleMessage(&client, &server, &message, responses);
}


static void TestHandlers_Unknown()
{
    with_client(.clientId = 1);
    with_message(.type = UNKNOWN, .command = "UNKNOWN");

    TestHandlers_Run();

    expect_responses({.response = "421 * UNKNOWN :Unknown command\r\n", .len = 1});
}

static void TestHandlers_Nick()
{
    with_client(.clientId = 1, .server = &server);
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .message.nick = {.nick = "nick"});

    TestHandlers_Run();

    expect_nicknames_contains(.id = 1, .nick = "nick");
    expect_responses_none();
}


static void TestHandlers_NickNewNick()
{
    with_client(.clientId = 1, .nickname = "nickOld", .receivedNick = true, .server = &server);
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .message.nick = {.nick = "nickNew"});

    TestHandlers_Run();

    expect_nicknames_contains(.id = 1, .nick = "nickNew");
    expect_nicknames_contains_not("nickOld");
    expect_responses_none();
}


static void TestHandlers_Nick_NicknameInUse()
{
    with_client(.clientId = 2, .server = &server);
    with_nicks({.id = 1, .nick = "nick"});
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .message.nick = {.nick = "nick"});

    TestHandlers_Run();

    expect_nicknames_contains(.id = 1, .nick = "nick");
    expect_responses({.response = "433 * nick :Nickname is already in use\r\n", .len = 1})
}


static void TestHandlers_Nick_NoNicknameGiven()
{
    with_client(.clientId = 1, .server = &server);
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .parse_err = ERR_NONICKNAMEGIVEN);

    TestHandlers_Run();

    expect_responses({.response = "431 * :No nickname given\r\n", .len = 1})
}


static void TestHandlers_Nick_SuccessfulRegistration()
{
    with_client(.clientId = 1, .username = "john", .fullname = "John Doe", .receivedUser = true,
            .hostname = "localhost", .server = &server);
    with_server(.nicks = nicknames, .clients = clients);
    with_message(.type = NICK, .message.nick = {.nick = "nick"});

    TestHandlers_Run();

    expect_nicknames_contains(.id = 1, .nick = "nick");
    TEST_ASSERT_TRUE(client.registered);
    expect_responses({.response = "001 nick :Welcome to the Internet Relay Network nick!john@localhost\r\n", .len = 1})
}

static void TestHandlers_User()
{
    with_client(.clientId = 1, .server = &server);
    with_server(.nicks = nicknames, .clients = clients);
    with_message(.type = USER, .message.user.fullname = "fullname", .message.user.username = "username");

    TestHandlers_Run();

    TEST_ASSERT_EQUAL_STRING("fullname", client.fullname);
    TEST_ASSERT_EQUAL_STRING("username", client.username);
    expect_responses_none();
}

static void TestHandlers_UserNeedMoreParams()
{
    with_client(.clientId = 1, .server = &server);
    with_message(.type = USER, .parse_err = ERR_NEEDMOREPARAMS);

    TestHandlers_Run();

    expect_responses({.response = "461 * USER :Not enough parameters\r\n", .len = 1})
}

static void TestHandlers_UserAlreadyRegistered()
{
    with_client(.clientId = 1, .registered = true, .receivedNick = true, .receivedUser = true,
            .nickname = "nick", .username = "username", .fullname = "fullname", .server = &server);
    with_message(.type = USER, .message.user.username = "username", .message.user.fullname = "fullname");

    TestHandlers_Run();

    expect_responses({.response = "462 nick :You may not reregister\r\n", .len = 1})
}

static void TestHandlers_UserSuccessfulRegistration()
{
    with_client(.clientId = 1, .receivedNick = true, .nickname = "nick", .hostname = "localhost",
                .server = &server);
    with_server(.clients = clients);
    with_message(.type = USER, .message.user.username = "username", .message.user.fullname = "fullname");

    TestHandlers_Run();

    expect_clients_contains(.client = &client, .clientId = 1);
    expect_responses({.response = "001 nick :Welcome to the Internet Relay Network nick!username@localhost\r\n", .len = 1})
}

static void TestHandlers_PrivmsgNoRecipient()
{
    with_client(.clientId = 1, .receivedNick = true, .nickname = "nick", .hostname = "localhost",
                .server = &server);
    with_server(.clients = clients);
    with_message(.type = PRIVMSG, .parse_err = ERR_NORECIPIENT);

    TestHandlers_Run();

    expect_responses({.response = "411 nick :No recipient given (PRIVMSG)\r\n", .len = 1});
}

static void TestHandlers_PrivmsgNoTextToSend()
{
    with_client(.clientId = 1, .receivedNick = true, .nickname = "nick", .hostname = "localhost",
                .server = &server);
    with_server(.clients = clients);
    with_message(.type = PRIVMSG, .parse_err = ERR_NOTEXTTOSEND);

    TestHandlers_Run();

    expect_responses({.response = "412 nick :No text to send\r\n", .len = 1});
}

static void TestHandlers_Ping()
{
    with_message(.type = PING);
    with_server(.hostname = "localhost");

    TestHandlers_Run();

    expect_responses({.response = "PONG localhost\r\n", .len = 1})
}

static void TestHandlers_Pong()
{
    with_message(.type = PONG);

    TestHandlers_Run();

    expect_responses_none();
}

static void TestHandlers_Motd()
{
    with_server(.motd = "MOTD is here", .hostname = "hostname");
    with_client(.nickname = "nick", .receivedNick = TRUE, .server = &server);
    with_message(.type = MOTD);

    TestHandlers_Run();

    expect_responses({.response = "375 nick :- hostname Message of the day - \r\n", .len = 1},
                     {.response = "372 nick :- MOTD is here\r\n", .len = 1},
                     {.response = "376 nick :End of MOTD command\r\n", .len = 1});
}


static void TestHandlers_Motd_LongMessage()
{
    with_server(.motd = MOTD_LONG, .hostname = "hostname");
    with_client(.nickname = "nick", .receivedNick = TRUE, .server = &server);
    with_message(.type = MOTD);

    TestHandlers_Run();


    expect_responses({.response = "375 nick :- hostname Message of the day - \r\n", .len = 1},
                     {.response = "372 nick :- " MOTD_LONG_FIRST_PART "\r\n", .len = 1},
                     {.response = "372 nick :- " MOTD_LONG_SECOND_PART "\r\n", .len = 1},
                     {.response = "376 nick :End of MOTD command\r\n", .len = 1});
}


static void TestHandlers_Motd_NoMotd()
{
    with_server(.motd = NULL);
    with_client(.nickname = "nick", .receivedNick = TRUE, .server = &server);
    with_message(.type = MOTD);

    TestHandlers_Run();

    expect_responses({.response = "422 nick :MOTD File is missing\r\n", .len = 1});
}

static void TestHandlers_LUsers()
{
    with_server(.nUsers = 1, .nServices = 1, .nServers = 1);
    with_client(.nickname = "nick", .registered = true, .server = &server);
    with_message(.type = LUSERS);

    TestHandlers_Run();

    expect_responses({.response = "251 nick :There are 1 users and 1 services on 1 servers\r\n", .len = 1},
                     {.response = "255 nick :I have 1 clients and 1 servers\r\n"})
}

static void TestHandlers_LUsers_Full()
{
    with_server(.nUsers = 1, .nOperators = 1, .nUnknown = 1, .nChannels = 1, .nServers = 1, .nServices = 1);
    with_client(.nickname = "nick", .registered = true, .server = &server);
    with_message(.type = LUSERS);

    TestHandlers_Run();

    expect_responses({.response = "251 nick :There are 1 users and 1 services on 1 servers\r\n", .len = 1},
                     {.response = "252 nick 1 :operator(s) online\r\n", .len = 1},
                     {.response = "253 nick 1 :unknown connection(s)\r\n", .len = 1},
                     {.response = "254 nick 1 :channels formed\r\n", .len = 1},
                     {.response = "255 nick :I have 2 clients and 1 servers\r\n", .len = 1});
}


static void TestHandlers_AwayMessage()
{
    with_client(.nickname = "nick");
    with_message(.type = AWAY, .message.away.text = "Back in 5 min");

    TestHandlers_Run();

    TEST_ASSERT_EQUAL_INT(TRUE, client.away);
    TEST_ASSERT_EQUAL_STRING("Back in 5 min", client.away_message);
}

static void TestHandlers_AwayNoMessage()
{
    with_client(.nickname = "nick");
    with_message(.type = AWAY, .message.away.text = NULL);

    TestHandlers_Run();

    TEST_ASSERT_EQUAL_INT(FALSE, client.away);
}

int main()
{
    UnityBegin("test_handlers.c");

    RUN_TEST(TestHandlers_Unknown);

    /*
     * NICK tests.
     */
    RUN_TEST(TestHandlers_Nick);
    RUN_TEST(TestHandlers_NickNewNick);
    RUN_TEST(TestHandlers_Nick_NicknameInUse);
    RUN_TEST(TestHandlers_Nick_NoNicknameGiven);
    RUN_TEST(TestHandlers_Nick_SuccessfulRegistration);

    /*
     * USER tests.
     */
    RUN_TEST(TestHandlers_User);
    RUN_TEST(TestHandlers_UserNeedMoreParams);
    RUN_TEST(TestHandlers_UserAlreadyRegistered);
    RUN_TEST(TestHandlers_UserSuccessfulRegistration);

    RUN_TEST(TestHandlers_Ping);

    RUN_TEST(TestHandlers_Pong);

    /*
     * MOTD tests.
     */
    RUN_TEST(TestHandlers_Motd);
    RUN_TEST(TestHandlers_Motd_LongMessage);
    RUN_TEST(TestHandlers_Motd_NoMotd);

    /*
     * PRIVMSG tests.
     */
    RUN_TEST(TestHandlers_PrivmsgNoRecipient);
    RUN_TEST(TestHandlers_PrivmsgNoTextToSend);

    /*
     * LUSERS tests.
     */
    RUN_TEST(TestHandlers_LUsers);
    RUN_TEST(TestHandlers_LUsers_Full);

    /*
     * AWAY tests.
     */
    RUN_TEST(TestHandlers_AwayMessage);
    RUN_TEST(TestHandlers_AwayNoMessage);


    return UnityEnd();
}