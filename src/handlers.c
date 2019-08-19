#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "codes.h"
#include "handlers.h"
#include "log.h"
#include "irc.h"
#include "replies.h"

static handler_t handlers[20];

static bool handle_unknown_message (ctx_client_s *, ctx_server_s *, irc_message_s *, char *);
static bool handle_nick_message    (ctx_client_s *, ctx_server_s *, irc_message_s *, char *);
static bool handle_user_message    (ctx_client_s *, ctx_server_s *, irc_message_s *, char *);

__attribute__((constructor))
static void register_handlers()
{
    handlers[UNKNOWN] = handle_unknown_message;
    handlers[NICK]    = handle_nick_message;
    handlers[USER]    = handle_user_message;
}

int handle_message(struct context_client *client, struct context_server *server, struct irc_message *message, char *buf)
{
    return handlers[message->type](client, server, message, buf);
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


