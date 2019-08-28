#include "unity.h"

#include "codes.h"
#include "handlers.h"
#include "log.h"

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

/*
 * Structure to hold nickname-client ID pairs.
 */
typedef struct nick_id_s {
    int id;
    char nick[IRC_NICK_SIZE];
} nick_id_s;

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

static void run_handle_message()
{
    handle_message1(&client, &server, &message, response, &len);
}

void setUp()
{
    nicknames = g_hash_table_new((GHashFunc) g_str_hash, (GEqualFunc) g_str_equal);
    clients   = g_hash_table_new(g_direct_hash, g_direct_equal);
    memset(response, 0, IRC_MSG_SIZE);
    len = 0;
}

void tearDown()
{
    g_hash_table_destroy(nicknames);
    g_hash_table_destroy(clients);
}

static void test_nick_handler()
{
    with_client(.clientId = 1);
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .message.nick = {.nick = "nick"});

    run_handle_message();

    expect_nicknames_contains(.id = 1, .nick = "nick");
    expect_response("");
    expect_response_len(0);

}

static void test_nick_handler_new_nick()
{
    with_client(.clientId = 1, .nickname = "nickOld", .receivedNick = true);
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .message.nick = {.nick = "nickNew"});

    run_handle_message();

    expect_nicknames_contains(.id = 1, .nick = "nickNew");
    expect_nicknames_contains_not("nickOld");
    expect_response("");
    expect_response_len(0);
}

static void test_nick_handler_nick_taken()
{
    with_client(.clientId = 2);
    with_nicks({.id = 1, .nick = "nick"});
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .message.nick = {.nick = "nick"});

    run_handle_message();

    expect_nicknames_contains(.id = 1, .nick = "nick");
    expect_response("433 * nick :Nickname is already in use\r\n");
}

static void test_nick_handler_no_nick_given()
{
    with_client(.clientId = 1);
    with_server(.nicks = nicknames);
    with_message(.type = NICK, .parse_err = ERR_NONICKNAMEGIVEN);

    run_handle_message();

    expect_response("431 * :No nickname given\r\n");
}

static void test_nick_handler_successful_registration()
{
    with_client(.clientId = 1, .username = "john", .fullname = "John Doe", .receivedUser = true,
            .hostname = "localhost");
    with_server(.nicks = nicknames, .clients = clients);
    with_message(.type = NICK, .message.nick = {.nick = "nick"});

    run_handle_message();

    expect_nicknames_contains(.id = 1, .nick = "nick");
    TEST_ASSERT_TRUE(client.registered);
    expect_response("001 nick :Welcome to the Internet Relay Network nick!john@localhost\r\n");

}

static void test_unknown_handler()
{
    with_client(.clientId = 1);
    with_message(.type = UNKNOWN, .command = "UNKNOWN");

    run_handle_message();

    expect_response("421 * UNKNOWN :Unknown command\r\n");

}

int main()
{
    UnityBegin("test_handlers.c");

    RUN_TEST(test_nick_handler);
    RUN_TEST(test_nick_handler_no_nick_given);
    RUN_TEST(test_nick_handler_new_nick);
    RUN_TEST(test_nick_handler_nick_taken);
    RUN_TEST(test_nick_handler_successful_registration);

    RUN_TEST(test_unknown_handler);

    return UnityEnd();
}