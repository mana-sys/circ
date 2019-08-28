#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <irc.h>

#include "codes.h"
#include "handlers.h"
#include "log.h"
#include "irc.h"
#include "replies.h"

static handler1_t handlers1[20];

static bool Handler_Unknown (client_s *, server_s *, irc_message_s *, char *, size_t *);
//static bool Handler_NickOrUser (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_Nick    (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_User    (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_Ping    (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_Pong    (client_s *, server_s *, irc_message_s *, char *, size_t *);
static bool Handler_Motd    (client_s *, server_s *, irc_message_s *, char *, size_t *);

__attribute__((constructor))
static void register_handlers1()
{
    handlers1[UNKNOWN] = Handler_Unknown;
    handlers1[NICK]    = Handler_Nick;
    handlers1[USER]    = Handler_User;
    handlers1[PING]    = Handler_Ping;
    handlers1[PONG]    = Handler_Pong;
    handlers1[MOTD]    = Handler_Motd;
}


int handle_message1(client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{
    return handlers1[message->type](client, server, message, response, len);
}

static bool Handler_Unknown (client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{
    circlog(L_TRACE, "Handling UNKNOWN message.");
    *len = Reply_ErrUnknownCommand(client, message->command, response);
    return 0;
}

static bool Handler_Nick    (client_s *client, server_s *server, irc_message_s *message, char *response, size_t *len)
{

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

    strncpy(client->username, message->message.user.username, IRC_MSG_SIZE);
    strncpy(client->fullname, message->message.user.fullname, IRC_MSG_SIZE);
    client->receivedUser = true;
    Client_TryRegister(client, server, response, len);
    return 0;

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

static bool Handler_Motd    (client_s * client, server_s *server, irc_message_s *message, char *response, size_t *len)
{
    if (!server->motd) {
        *len = Reply_ErrNoMotd(client, response);
        return -1;
    }




    return 0;
}

