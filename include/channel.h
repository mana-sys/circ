//
// Created by mana on 8/12/19.
//

#ifndef CIRC_CHANNEL_H
#define CIRC_CHANNEL_H

#include <glib.h>
#include <stdbool.h>

#include "response.h"

#define CHANNEL_NAME_MAX 50
#define CHANNEL_TOPIC_MAX 255

struct client_s;

typedef struct channel_s {
    char            name[CHANNEL_NAME_MAX + 1];     /* The name of the channel */
    char            topic[CHANNEL_TOPIC_MAX + 1];   /* The topic of the channel */
    GHashTable *    members;                        /* Members of the channel (hash from client ID to client pointer */
    GHashTable *    member_modes;                   /* Modes of the channel members (hash from client ID to mode */
} channel_s;


/**
 * Creates a new channel, with the specified client added to the channel
 * as its operator.
 *
 * @param name The name of the channel to be created.
 * @param operator The client who will become the operator of this channel.
 * @return A pointer to the new channel structure.
 */
channel_s *channel_new(const char *name, struct client_s *operator);


/**
 *
 * @param channel
 * @return
 */
channel_s *channel_free(channel_s *channel);


/**
 * Joins the specified client to the channel, if permitted.
 *
 * @param channel The channel to join to.
 * @param client The client to be joined.
 * @return 0 on success, -1 on error.
 */
int channel_join(channel_s *channel, struct client_s *client);


/**
 * Sends the message to the specified channel. The message will be forwarded
 * to all members of the channel, except for the client identified by source.
 * If source is less than 0, then the message will be forwarded to all members of the
 * channel, including the client identified by source.
 *
 * @param channel The channel to send the message to.
 * @param response The message to send.
 */
int channel_send_message(channel_s *channel, response_s *message, int source);


/**
 *
 */
int channel_sendall_join(channel_s *channel, struct client_s *source);


/**
 * Verifies the validity of the given channel name.
 * @param name The name to test.
 * @return 0 if the name is valid, -1 if not.
 */
int channel_verify_name(const char *name);


/**
 *
 * @param channel
 * @param source
 * @param contents
 * @return
 */
int channel_sendall_privmsg(channel_s *channel, struct client_s *source, const char *contents);


int channel_part(channel_s *channel, struct client_s *client);


/**
 * Checks if the given client is a member of the channel.
 *
 * @param channel The channel to check membership in.
 * @param client The client to check membership for.
 * @return true if the client is a member, false if not.
 */
bool channel_is_member(channel_s *channel, struct client_s *client);


/**
 * Removes the specified client from the channel.
 *
 * @param channel The channel the operation will be performed on.
 * @param client The client to remove.
 */
void channel_remove_member(channel_s *channel, struct client_s *client);


size_t channel_size(channel_s *channel);



#endif //CIRC_CHANNEL_H
