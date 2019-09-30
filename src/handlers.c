#include <ctype.h>
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

#define push_reply(responses, reply_func, ...) do {                 \
    response_s *response = calloc(1, sizeof(response_s));           \
    response->len = reply_func(__VA_ARGS__, response->response);    \
    g_queue_push_tail(responses, response);                         \
} while (0)


static handler1_t handlers1[20];
static handler2_t handlers2[20];

static int Handler_Unknown (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Nick    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_User    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Ping    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Pong    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Motd    (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_Privmsg (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_LUsers  (client_s *, server_s *, irc_message_s *, GQueue *);
static int Handler_WhoIs   (client_s *, server_s *, irc_message_s *, GQueue *);
static int handler_join    (client_s *, server_s *, irc_message_s *, GQueue *);
static int handler_part    (client_s *, server_s *, irc_message_s *, GQueue *);
static int handler_list    (client_s *, server_s *, irc_message_s *, GQueue *);
static int handler_topic   (client_s *, server_s *, irc_message_s *, GQueue *);
static int handler_names   (client_s *, server_s *, irc_message_s *, GQueue *);
static int handler_away    (client_s *, server_s *, irc_message_s *, GQueue *);


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
    handlers2[LUSERS]  = Handler_LUsers;
    handlers2[WHOIS]   = Handler_WhoIs;
    handlers2[JOIN]    = handler_join;
    handlers2[PART]    = handler_part;
    handlers2[LIST]    = handler_list;
    handlers2[TOPIC]   = handler_topic;
    handlers2[NAMES]   = handler_names;
    handlers2[AWAY]    = handler_away;
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
    int             targetId;
    char            first;
    channel_s *     channel;
    response_s *    response;
    client_s *      target;

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


    first = message->message.privmsg.msgtarget[0];
    if ((first == '#') || (first == '+') || (first == '!') || (first == '&')) {

        channel = g_hash_table_lookup(server->channels, message->message.privmsg.msgtarget);
        if (channel) {

            circlog(L_DEBUG, "Sending to channel %s", message->message.privmsg.msgtarget);

            channel_sendall_privmsg(channel, client, message->message.privmsg.contents);
        }

    } else {


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

        /*
         * If the target client is away, also send the automatic reply string to the source client.
         */
        if (target->away) {
            push_reply(responses, format_rpl_away, client, server, target->nickname, target->away_message);
        }
    }

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


static int Handler_LUsers(client_s * client, server_s *server, irc_message_s *message, GQueue *responses)
{

    push_reply(responses, Reply_RplLUserClient, client, server);

    if (server->nOperators) {
        push_reply(responses, Reply_RplLUserOp, client, server);
    }

    if (server->nUnknown) {
        push_reply(responses, Reply_RplLUserUnknown, client, server);
    }

    if (server->nChannels) {
        push_reply(responses, Reply_RplLUserChannels, client, server);
    }

    push_reply(responses, Reply_RplLUserMe, client, server);

    return 0;
}

static int Handler_WhoIs(client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    int          queriedId;
    const char * nickMask;
    client_s   * queried;

    if ((nickMask = message->message.whois.mask) == NULL) {
        return 0;
    }

    queriedId = GPOINTER_TO_INT(g_hash_table_lookup(server->nicks, nickMask));

    if (!queriedId) {
        push_reply(responses, Reply_ErrNoSuchNick, client, nickMask);
        return -1;
    }

    queried = g_hash_table_lookup(server->clients, GINT_TO_POINTER(queriedId));

    push_reply(responses, Reply_RplWhoIsUser, client, queried);
    push_reply(responses, Reply_RplWhoIsServer, client, queried);
    push_reply(responses, Reply_RplEndOfWhoIs, client, queried);

    return 0;
}


static int handler_join(client_s * client, server_s * server, irc_message_s * message, GQueue *responses)
{
    char *      channel_name;
    char *      save_ptr;
    channel_s * channel;

    circlog(L_DEBUG, "Handling JOIN message.");

    /*
     * Check for ERR_NEEDMOREPARAMS.
     */
    if (message->parse_err == ERR_NEEDMOREPARAMS) {
        push_reply(responses, Reply_ErrNeedMoreParams, client, "JOIN");
        return -1;
    }
    
    /*
     * The channels field may be a list of channels.
     */
    channel_name = strtok_r(message->message.join.channels, ",", &save_ptr);
    while (channel_name) {

        /*
         * TODO: verify validity of channel name
         */

        /*
         * Check for channel's existence. If channel exists, join the channel.
         * If the channel doesn't exist, have the server create the channel with the client
         * as the channel operator.
         */
        channel = server_get_channel(server, channel_name);
        if (channel) {

            channel_join(channel, client);
            circlog(L_DEBUG, "client %d (nick= %s) has joined channel =%s", client->clientId,
                    client->nickname, channel->name);

        } else {

            channel = server_create_channel(server, channel_name, client);
            circlog(L_INFO, "client =%d (nick =%s) has created a new channel =%s", client->clientId,
                    client->username, channel_name);
        }

        /*
         * Reply with a JOIN message, RPL_TOPIC, RPL_NAMREPLY, and RPL_ENDOFNAMES.
         */
        push_reply(responses, Format_MessageJoin, client, channel->name);

        /*
         * Only send RPL_TOPIC if the channel has a topic set.
         */
        if (strlen(channel->topic)) {
            push_reply(responses, Reply_RplTopic, client, server, channel);
        }

        push_reply(responses, Reply_RplNamReply, client, server, channel);
        push_reply(responses, Reply_RplEndOfNames, client, server, channel);

        /*
         * Get the next channel name.
         */
        channel_name = strtok_r(NULL, ",", &save_ptr);
    }

    return 0;
}

static int handler_part(client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    char *      channel_name, *c;
    channel_s * channel;

    Log_Debug("Handling PART message.");

    /*
     * Convert channel name to lowercase.
     * TODO: Support comma-delimited list of channels.
     */
    channel_name = message->message.part.channels;
    for (c = channel_name; *c; c++) {
        *c = (char) tolower((int) *c);
    }

    /*
     * Check for channel existence; if not found, send ERR_NOSUCHCHANNEL.
     */
    if ((channel = server_get_channel(server, channel_name)) == NULL) {
        push_reply(responses, Reply_ErrNoSuchChannel, client, server, channel_name);
        return -1;
    }

    /*
     * Check for channel membership; if not a member, send ERR_NOTONCHANNEL.
     */
    if (!channel_is_member(channel, client)) {
        push_reply(responses, Reply_ErrNotOnChannel, client, server, channel);
        return -1;
    }

    /*
     * Remove from the channel.
     */
    Log_Debug("Is leaving channel.");
    channel_sendall_part(channel, client, message->message.part.part_message);
    channel_remove_member(channel, client);
    if (channel_size(channel) == 0) {
        Log_Info("Removing channel %s.", channel->name);
        server_remove_channel(server, channel);
    }


    return 0;
}


static int handler_list(client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    GHashTableIter  iter;
    gpointer        key;
    channel_s *     channel;
    char *          channel_name;
    char *          saveptr;


    push_reply(responses, Reply_RplListStart, client, server);

    printf("%p", message->message.list.channels);

    if (message->message.list.channels == 0) {
        g_hash_table_iter_init(&iter, server->channels);

        while (g_hash_table_iter_next(&iter, &key, (gpointer *) &channel)) {
            push_reply(responses, Reply_RplList, client, server, channel);
        }
    } else {

        channel_name = strtok_r(message->message.list.channels, ",", &saveptr);

        while (channel_name != NULL) {

            if ((channel = server_get_channel(server, channel_name))) {
                push_reply(responses, Reply_RplList, client, server, channel);
            }

            channel_name = strtok_r(NULL, ",", &saveptr);
        }

    }

    push_reply(responses, Reply_RplListEnd, client, server);
    return 0;
}


static int handler_topic(client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    channel_s *channel;


    if (message->parse_err == ERR_NEEDMOREPARAMS) {
        push_reply(responses, Reply_ErrNeedMoreParams, client, "TOPIC");
        return -1;
    }

    channel = server_get_channel(server, message->message.topic.channel);

    /*
     * If no topic was specified, then we simply return the current topic.
     */
    if (message->message.topic.topic == NULL) {

        if (channel == NULL) {
            /*
             * TODO: This should be ERR_NOTONCHANNEL
             */
            push_reply(responses, Reply_ErrNoSuchChannel, client, server, message->message.topic.channel);
            return -1;
        }

        if (*channel->topic) {
            push_reply(responses, Reply_RplTopic, client, server, channel);
            return 0;
        }

        push_reply(responses, Reply_RplNoTopic, client, server, channel);
        return 0;
    }

    /*
     * If a topic was specified, then replace the channel's current topic.
     * No reply message is sent.
     */
    channel_set_topic(channel, message->message.topic.topic);
    return 0;
}


static int handler_names(client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{
    char *saveptr;
    char *tok;
    channel_s *channel;

    /*
     * If a list of channels was given, then send RPL_NAMREPLY for each channel in the list.
     */
    if (message->message.names.channels) {
        tok = strtok_r(message->message.names.channels, ",", &saveptr);
        while (tok) {

            fprintf(stderr, "NAMES for %s\n", tok);

            /*
             * Reply with RPL_NAMREPLY and RPL_ENDOFNAMES if the server contains the
             * specified channel.
             */
            if ((channel = server_get_channel(server, tok))) {
                push_reply(responses, Reply_RplNamReply, client, server, channel);
                push_reply(responses, Reply_RplEndOfNames, client, server, channel);
            }

            /*
             * Advance to the next channel in the list.
             */
            tok = strtok_r(NULL, ",", &saveptr);

        }
    }

    return 0;
}


static int handler_away(client_s *client, server_s *server, irc_message_s *message, GQueue *responses)
{

    /*
     * Check if the <text> parameter is specified. If it is, then set the client's status as AWAY and
     * set its away_message to the given message in the <text> parameter. If it is not, unmark the client
     * as AWAY.
     */
    if (message->message.away.text) {
        client_set_away(client, message->message.away.text);
        push_reply(responses, format_rpl_nowaway, client, server);
    } else {
        client_unset_away(client);
        push_reply(responses, format_rpl_unaway, client, server);
    }

    return 0;
}