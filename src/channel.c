#include <ctype.h>

#include "channel.h"
#include "client.h"

#define INVALID_CHARSET "\x07\r\n,"

channel_s *channel_new(const char *name, client_s *operator)
{
    channel_s *channel;

    channel = calloc(1, sizeof(channel_s));
    strncpy(channel->name, name, CHANNEL_NAME_MAX);

    /*
     * Initialize channel data structures.
     */
    channel->members      = g_hash_table_new(g_direct_hash, g_direct_equal);
    channel->member_modes = g_hash_table_new(g_direct_hash, g_direct_equal);

    /*
     * First member is the channel operator.
     */
    g_hash_table_insert(channel->members, GINT_TO_POINTER(operator->clientId), operator);


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
    g_hash_table_insert(channel->members, GINT_TO_POINTER(client->clientId), client);

    return 0;
}