#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "codes.h"
#include "handlers.h"
#include "log.h"
#include "irc.h"
#include "replies.h"

static handler_t  handlers[20];
static handler1_t handlers1[20];

static bool handle_unknown_message (ctx_client_s *, ctx_server_s *, irc_message_s *, char *);
static bool handle_nick_message    (ctx_client_s *, ctx_server_s *, irc_message_s *, char *);
static bool handle_user_message    (ctx_client_s *, ctx_server_s *, irc_message_s *, char *);

static bool Handler_Unknown (client_s *, server_s *, irc_message_s *, char *, size_t *);
//static bool Handler_NickOrUser (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_Nick    (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_User    (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_Ping    (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_Pong    (client_s *, server_s *, irc_message_s *, char *, size_t *);

__attribute__((constructor))
static void register_handlers()
{
    handlers[UNKNOWN] = handle_unknown_message;
    handlers[NICK]    = handle_nick_message;
    handlers[USER]    = handle_user_message;
}

__attribute__((constructor))
static void register_handlers1()
{
    handlers1[UNKNOWN] = Handler_Unknown;
    handlers1[NICK]    = Handler_Nick;
    handlers1[USER]    = Handler_User;
    handlers1[PING]    = Handler_Ping;
    handlers1[PONG]    = Handler_Pong;
}


int handle_message(struct context_client *client, struct context_server *server, struct irc_message *message, char *buf)
{
    return handlers[message->type](client, server, message, buf);
}

int handle_message1(client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{
    return handlers1[message->type](client, server, message, response, len);
}


static bool handle_nick_message(struct context_client *client,
        struct context_server *server, struct irc_message *message, char *buf)
{
    size_t numWritten;
    void *old;

    circlog(L_DEBUG, "Handling nick message...");

    // Check for errors encountered while parsing the NICK message.
    if (message->parse_err == ERR_NONICKNAMEGIVEN) {
        circlog(L_DEBUG, "Sending ERR_NONICKNAMEGIVEN.");
        numWritten = replyf_err_nonicknamegiven(client, NULL, buf);
        write(client->fd, buf, numWritten);
        return 1;
    }

    // Check for ERR_NICKNAMEINUSE
    pthread_mutex_lock(&server->nick_table_mutex);
    if (hashtable_search(server->nick_table, message->message.nick.nick) != NULL) {
        circlog(L_DEBUG, "Sending ERR_NICKNAMEINUSE.");
        numWritten = replyf_err_nicknameinuse(client, NULL, buf, message->message.nick.nick);
        write(client->fd, buf, numWritten);
        return 1;
    }

    if (client->receivedNick) {
        circlog(L_DEBUG, "Entry for old nickname found, removing that entry and adding the new one.");
        old = hashtable_search(server->nick_table, client->nickname);
        hashtable_remove(server->nick_table, client->nickname);
        strcpy(client->nickname, message->message.nick.nick);

        hashtable_insert(server->nick_table, client->nickname, old);
    } else {
        circlog(L_DEBUG, "Setting nickname and adding nickname to nick table.");
        strcpy(client->nickname, message->message.nick.nick);
        hashtable_insert(server->nick_table, client->nickname, (void *) 1);
        client->receivedNick = true;
    }
    pthread_mutex_unlock(&server->nick_table_mutex);

    if (!client->registered && client->receivedUser) {
        circlog(L_DEBUG, "Registration for %s@%s complete.", client->username, client->hostname);
        numWritten = replyf_rpl_welcome(client, buf);
        write(client->fd, buf, numWritten);
        client->registered = true;
    }

    return 0;
}

static bool handle_user_message(struct context_client *client,
        struct context_server *server, struct irc_message *message, char *buf)
{
    ssize_t numWritten;

    circlog(L_DEBUG, "Handling user message...");

    // Check for errors encountered while parsing the USER message.
    if (message->parse_err == ERR_NEEDMOREPARAMS) {
        circlog(L_DEBUG, "Sending ERR_NEEDMOREPARAMS");
        numWritten = replyf_err_needmoreparams(client, "USER", buf);
        write(client->fd, buf, numWritten);
        return 1;
    }

    if (client->registered) {
        circlog(L_DEBUG, "Attempted re-registration by %s@%s.", client->username, "TODO");
        numWritten = replyf_err_alreadyregistered(client, buf);
        write(client->fd, buf, numWritten);
        return 1;
    } else {
        strcpy(client->username, message->message.user.username);
        strcpy(client->fullname, message->message.user.fullname);
        client->receivedUser = true;
        if (client->receivedNick) {
            circlog(L_DEBUG, "Registration for %s@%s complete.", client->username, "TODO");
            numWritten = replyf_rpl_welcome(client, buf);
            write(client->fd, buf, numWritten);
            client->registered = true;
        }
    }

    return 0;
}

static bool handle_unknown_message(struct context_client *client, struct context_server *server, struct irc_message *message, char *buf)
{
    ssize_t numWritten;

    circlog(L_DEBUG, "Handling unknown command %s.", message->command);
    numWritten = replyf_err_unknowncommand(client, message->command, buf);
    write(client->fd, buf, numWritten);
    return 0;
}

static bool Handler_Unknown (client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{
    circlog(L_TRACE, "Handling UNKNOWN message.");
    *len = Reply_ErrUnknownCommand(client, message->command, response);
    return 0;
}

static bool Handler_Nick    (client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{
//    size_t numWritten;
//    void *old;
//
//

    Log_Debug("Handing NICK message.");

    /*
     * Make sure nick was given and validate.
     */
    if (message->parse_err == ERR_NONICKNAMEGIVEN) {
        circlog(L_DEBUG, "No nick given; responding with ERR_NONICKNAMEGIVEN.");
        *len = Reply_ErrNoNicknameGiven(client, response);
        return 1;
    }

    Log_Debug("Client %s is trying to set their nickname to %s.", client->nickname,
            message->message.nick.nick);

    /*
     * Try to register the nickname.
     */
    if (Client_TryChangeNick(client, server, message->message.nick.nick, response, len) == -1)
        return -1;

    Log_Debug("Changed nickname successfully.");

    Client_TryRegister(client, server, response, len);
    return 0;
}

/**
 * asdfasdfsa
 * @param client
 * @param server
 * @param message
 * @param response
 * @param len
 * @return
 */
static bool Handler_User    (client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{

    circlog(L_TRACE, "Handling USER message.");

    if (message->parse_err == ERR_NEEDMOREPARAMS) {
        circlog(L_DEBUG, "Not enough parameters; sending ERR_NEEDMOREPARAMS");
        *len = Reply_ErrNeedMoreParams(client, "USER", response);
        return -1;
    }

    if (client->registered) {
        circlog(L_DEBUG, "Already registered; sending ERR_ALREADYREGISTERED");
        *len = Reply_ErrAlreadyRegistered(client, response);
        return -1;
    }

    client->receivedUser = true;
    Client_TryRegister(client, server, response, len);
    return 0;


//    ssize_t numWritten;
//
//    circlog(L_DEBUG, "Handling user message...");
//
//    // Check for errors encountered while parsing the USER message.
//    if (message->parse_err == ERR_NEEDMOREPARAMS) {
//        circlog(L_DEBUG, "Sending ERR_NEEDMOREPARAMS");
//        numWritten = replyf_err_needmoreparams(client, "USER", buf);
//        write(client->fd, buf, numWritten);
//        return 1;
//    }
//
//    if (client->registered) {
//        circlog(L_DEBUG, "Attempted re-registration by %s@%s.", client->username, "TODO");
//        numWritten = replyf_err_alreadyregistered(client, buf);
//        write(client->fd, buf, numWritten);
//        return 1;
//    } else {
//        strcpy(client->username, message->message.user.username);
//        strcpy(client->fullname, message->message.user.fullname);
//        client->receivedUser = true;
//        if (client->receivedNick) {
//            circlog(L_DEBUG, "Registration for %s@%s complete.", client->username, "TODO");
//            numWritten = replyf_rpl_welcome(client, buf);
//            write(client->fd, buf, numWritten);
//            client->registered = true;
//        }
//    }


}

static bool Handler_Ping    (client_s * client, server_s * server, irc_message_s * message, char * response, size_t * len)
{
    *len = Reply_Pong(server->hostname, response);
    return 0;
}

static bool Handler_Pong    (client_s * client, server_s * server, irc_message_s * message, char * response, size_t * len)
{
    /* Silently ignore. */
    return 0;
}


