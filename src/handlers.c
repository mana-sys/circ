#include <string.h>

#include "codes.h"
#include "handlers.h"
#include "log.h"
#include "irc.h"

static int handle_nick_message(struct context_client *client, struct irc_message *message, char *buf)
{
    circlog(L_DEBUG, "Handling nick message...");

    // Check for errors encountered while parsing the NICK message.
    if (message->parse_err == ERR_NONICKNAMEGIVEN) {
        circlog(L_DEBUG, "Sending ERR_NONICKNAMEGIVEN.");
    }

    strcpy(client->nickname, message->message.nick.nick);
    client->receivedNick = true;

    if (client->receivedUser) {
        circlog(L_DEBUG, "Registration for %s@%s complete.", client->username, "TODO");
        client->registered = true;
    }

    return 0;
}

static int handle_user_message(struct context_client *client, struct irc_message *message, char *buf)
{
    circlog(L_DEBUG, "Handling user message...");

    // Check for errors encountered while parsing the USER message.
    if (message->parse_err == ERR_NEEDMOREPARAMS) {
        circlog(L_DEBUG, "Sending ERR_NEEDMOREPARAMS");
        return 1;
    }

    if (client->registered) {
        circlog(L_DEBUG, "Attempted re-registration by %s@%s.", client->username, "TODO");
        return 1;
    } else {
        strcpy(client->username, message->message.user.username);
        strcpy(client->username, message->message.user.fullname);
        client->receivedUser = true;
        if (client->receivedNick) {
            circlog(L_DEBUG, "Registration for %s@%s complete.", client->username, "TODO");
            client->registered = true;
        }
    }

    return 0;
}

int handle_message(struct context_client *client, struct context_server *server, struct irc_message *message, char *buf)
{
    switch (message->type) {
        case USER:
            return handle_user_message(client, message, buf);
        case NICK:
            return handle_nick_message(client, message, buf);
        default:
            circlog(L_DEBUG, "Unsupported message type: %s.", message->command);
    }
    return 0;
}
