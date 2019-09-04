#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <irc.h>
#include <connection.h>

#include "codes.h"
#include "handlers.h"
#include "log.h"
#include "irc.h"
#include "replies.h"

#define MOTD_FRAGMENT_MAX 80

static handler1_t handlers1[20];
static handler2_t handlers2[20];

static int Handler_Unknown (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Nick    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_User    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Ping    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Pong    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Motd    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Privmsg    (client_s *, server_s *, irc_message_s *, GQueue *);


__attribute__((constructor))
static void Handler_Register()
{
    handlers2[UNKNOWN] = Handler_Unknown;
    handlers2[NICK]    = Handler_Nick;
    handlers2[USER]    = Handler_User;
    handlers2[PING]    = Handler_Ping;
    handlers2[PONG]    = Handler_Pong;
    handlers2[MOTD]    = Handler_Motd;
    handlers2[PRIVMSG] = Handler_Privmsg;
}

int Handler_HandleMessage (client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    return handlers2[message->type](client, server, message, responses);
}

int handle_message1(client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{
    return handlers1[message->type](client, server, message, response, len);
}

static int Handler_Unknown (client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    response_s *response;

    circlog(L_TRACE, "Handling UNKNOWN message.");

    response = malloc(sizeof(response_s));
    response->len = Reply_ErrUnknownCommand(client, message->command, response->response);

    g_queue_push_tail(responses, response);

    return 0;
}

static int Handler_Nick (client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    response_s *response;

    Log_Debug("Handing NICK message.");

    /*
     * Make sure nick was given and validate.
     */
    if (message->parse_err == ERR_NONICKNAMEGIVEN) {

        circlog(L_DEBUG, "No nick given; responding with ERR_NONICKNAMEGIVEN.");

        response = calloc(1, sizeof(response_s));
        response->len = Reply_ErrNoNicknameGiven(client, response->response);
        g_queue_push_tail(responses, response);
        return -1;
    }

    Log_Debug("Client %s is trying to set their nickname to %s.", client->nickname,
            message->message.nick.nick);

    /*
     * Try to register the nickname.
     */
    if (Client_TryChangeNick(client, server, message->message.nick.nick, &response) == -1) {

        g_queue_push_tail(responses, response);
        return -1;
    }

    Log_Debug("Changed nickname successfully.");

    Client_TryRegister(client, server, &response);

    /*
     * If we have a message, add it to the message queue.
     */
    if (response) {
        g_queue_push_tail(responses, response);
    }

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
static int Handler_User(client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{

    response_s *response;

    circlog(L_TRACE, "Handling USER message.");

    if (message->parse_err == ERR_NEEDMOREPARAMS) {

        circlog(L_DEBUG, "Not enough parameters; sending ERR_NEEDMOREPARAMS");

        response = calloc(1, sizeof(response_s));
        response->len = Reply_ErrNeedMoreParams(client, "USER", response->response);

        g_queue_push_tail(responses, response);

        return -1;
    }

    if (client->registered) {

        circlog(L_DEBUG, "Already registered; sending ERR_ALREADYREGISTERED");

        response = calloc(1, sizeof(response_s));
        response->len = Reply_ErrAlreadyRegistered(client, response->response);

        g_queue_push_tail(responses, response);

        return -1;
    }

    strncpy(client->username, message->message.user.username, IRC_MSG_SIZE);
    strncpy(client->fullname, message->message.user.fullname, IRC_MSG_SIZE);
    client->receivedUser = true;

    Client_TryRegister(client, server, &response);
    if (response)
        g_queue_push_tail(responses, response);

    return 0;

}


static int Handler_Privmsg    (client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    int targetId;
    response_s *response;
    client_s *target;

    circlog(L_DEBUG, "Handling PRIVMSG");

    if (message->parse_err == ERR_NORECIPIENT) {
        circlog(L_DEBUG, "No recipient specified; sending ERR_NORECIPIENT");

        response = calloc(1, sizeof(response_s));
        response->len = Reply_ErrNoRecipient(client, "PRIVMSG", response->response);

        g_queue_push_tail(responses, response);

        return -1;
    } else if (message->parse_err == ERR_NOTEXTTOSEND) {
        circlog(L_DEBUG, "No text to send found; sending ERR_NOTEXTTOSEND");

        response = calloc(1, sizeof(response_s));
        response->len = Reply_ErrNoTextToSend(client, response->response);

        g_queue_push_tail(responses, response);

        return -1;
    }

    targetId = GPOINTER_TO_INT(g_hash_table_lookup(server->nicks, message->message.privmsg.msgtarget));
    if (targetId == 0) {
        circlog(L_DEBUG, "Nick not found; sending ERR_NOSUCHNICK");

        response = calloc(1, sizeof(response_s));
        response->len = Reply_ErrNoSuchNick(client, message->message.privmsg.msgtarget, response->response);

        g_queue_push_tail(responses, response);

        return -1;
    }

    target = g_hash_table_lookup(server->clients, GINT_TO_POINTER(targetId));

    response = calloc(1, sizeof(response_s));
    response->len = Message_Privmsg(client, message->message.privmsg.msgtarget, message->message.privmsg.contents,
            response->response);

    Client_Send(target, response);

    return 0;
}

static int Handler_Ping(client_s * client, server_s * server, irc_message_s * message, GQueue *responses)
{
    response_s *response;

    response = calloc(1, sizeof(response_s));
    response->len = Reply_Pong(server->hostname, response->response);
    g_queue_push_tail(responses, response);

    return 0;
}

static int Handler_Pong(client_s * client, server_s * server, irc_message_s * message, GQueue *responses)
{
    /* Silently ignore. */
    return 0;
}

static int Handler_Motd(client_s * client, server_s *server, irc_message_s *message, GQueue *responses)
{
    size_t len, numParts, j;
    response_s *response;
    const char *motd;
    char part[MOTD_FRAGMENT_MAX + 1];

    if (!server->motd) {

        circlog(L_DEBUG, "No motd.txt found, sending ERR_NOMOTD");

        response = calloc(1, sizeof(response_s));
        response->len = Reply_ErrNoMotd(client, response->response);
        g_queue_push_tail(responses, response);

        return -1;
    }

    motd = server->motd;
    len = strlen(motd);

    numParts = (len / MOTD_FRAGMENT_MAX) + ((len % MOTD_FRAGMENT_MAX) ? 1 : 0);

    response = calloc(1, sizeof(response_s));
    response->len = Reply_RplMotdStart(client, server->hostname, response->response);
    g_queue_push_tail(responses, response);

    for (j = 0; j < numParts; j++) {
        strncpy(part, motd, MOTD_FRAGMENT_MAX);

        if (j < numParts - 1)
            part[MOTD_FRAGMENT_MAX] = 0;
        else
            part[len % MOTD_FRAGMENT_MAX] = 0;

        response = calloc(1, sizeof(response_s));
        response->len = Reply_RplMotd(client, part, response->response);
        g_queue_push_tail(responses, response);

        motd += MOTD_FRAGMENT_MAX;
    }

    response = calloc(1, sizeof(response_s));
    response->len = Reply_RplEndOfMotd(client, response->response);
    g_queue_push_tail(responses, response);



    return 0;
}

