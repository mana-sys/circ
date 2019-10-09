#include <ctype.h>
#include <log.h>
#include <stdio.h>

#include "channel.h"
#include "client.h"
#include "replies.h"

#define INVALID_CHARSET "\x07\r\n,"

channel_s *channel_new(const char *name, client_s *operator)
{
    channel_s *channel;
    channel_member_mode_u mode;

    channel = calloc(1, sizeof(channel_s));
    strncpy(channel->name, name, CHANNEL_NAME_MAX);

    /*
     * Initialize channel data structures.
     */
    channel->members      = g_hash_table_new(g_direct_hash, g_direct_equal);
    channel->member_modes = g_hash_table_new(g_direct_hash, g_direct_equal);

    /*
     * First member is the channel operator, so we set it member mode to include the operator mode.
     */
    g_hash_table_insert(channel->members, GINT_TO_POINTER(operator->clientId), operator);

    mode.packed = 0;
    mode.chanop = 1;

    g_hash_table_insert(channel->member_modes, GINT_TO_POINTER(operator->clientId), GINT_TO_POINTER(mode.packed));

    return channel;
}


int channel_verify_name(const char *name)
{
    int colon_occurences;
    size_t len;

    /*
     * Channel name up to fifty (50) characters.
     */
    if ((len = strlen(name)) > CHANNEL_NAME_MAX)
        return -1;

    /*
     * First character is '&', '#', '+', or '!'
     */
    switch (name[0]) {
        case '&':
        case '#':
        case '+':
        case '!':
            break;
        default:
            return -1;
    }

    /*
     * Does not contain ^G, CR, LF, or ','.
     */
    if (strcspn(name, INVALID_CHARSET) != len)
        return -1;

    /*
     * TODO: At most one colon as channel mask delimiter.
     */

    return 0;
}


int channel_join(channel_s *channel, struct client_s *client)
{
    channel_member_mode_u mode;

    if (g_hash_table_lookup(channel->members, GINT_TO_POINTER(client->clientId))) {
        circlog(L_DEBUG, "Client ID =%d is already part of channel %s", client->clientId, channel->name);
    } else {

        /*
         * Insert the new client, also allocating new space for its member modes.
         */
        mode.packed = 0;

        g_hash_table_insert(channel->members, GINT_TO_POINTER(client->clientId), client);
        g_hash_table_insert(channel->member_modes, GINT_TO_POINTER(client->clientId), GINT_TO_POINTER(mode.packed));

        /*
         * Notify all other channel members.
         * TODO: This probably shouldn't be in this method to keep separation of concerns. Maybe move this out?
         */
        channel_sendall_join(channel, client);
    }

    return 0;
}


int channel_send_message(channel_s *channel, response_s *message, int source)
{
    GHashTableIter  iter;
    gpointer        key;
    client_s *      client;
    response_s *    copy;

    g_hash_table_iter_init(&iter, channel->members);

    while (g_hash_table_iter_next (&iter, &key, (gpointer) &client)) {

        if (GPOINTER_TO_INT(key) != source) {
            circlog(L_DEBUG, "Forwarding '%s' to client =%d", message->response, client->clientId);

            copy = calloc(1, sizeof(response_s));
            memcpy(copy, message, sizeof(response_s));

            Client_Send(client, copy);
        }
    }

    return 0;
}


int channel_sendall_join(channel_s *channel, client_s *source)
{
    response_s response;

    memset(&response, 0, sizeof(response_s));
    response.len = Format_MessageJoin(source, channel->name, response.response);

    channel_send_message(channel, &response, source->clientId);

    return 0;
}


int channel_sendall_part(channel_s *channel, struct client_s *source, const char *part_message)
{
    response_s response;

    memset(&response, 0, sizeof(response_s));
    response.len = Format_MessagePart(source, channel->name, part_message, response.response);

    channel_send_message(channel, &response, source->clientId);

    return 0;
}


int channel_sendall_privmsg(channel_s *channel, client_s *source, const char *contents)
{
    response_s response;

    memset(&response, 0, sizeof(response_s));
    response.len = Message_Privmsg(source, channel->name, contents, response.response);

    channel_send_message(channel, &response, source->clientId);

    return 0;
}


bool channel_is_member(channel_s *channel, struct client_s *client)
{
    if (g_hash_table_lookup(channel->members, GINT_TO_POINTER(client->clientId)) != NULL)
        return true;

    return false;
}


void channel_remove_member(channel_s *channel, struct client_s *client)
{
    g_hash_table_remove(channel->members, GINT_TO_POINTER(client->clientId));
    g_hash_table_remove(channel->member_modes, GINT_TO_POINTER(client->clientId));
}


size_t channel_size(channel_s *channel)
{
    return (size_t) g_hash_table_size(channel->members);
}


void channel_set_topic(channel_s *channel, const char *topic)
{
    strncpy(channel->topic, topic, CHANNEL_TOPIC_MAX);
    channel->topic[CHANNEL_TOPIC_MAX] = 0;
}