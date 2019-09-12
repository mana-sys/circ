//
// Created by mana on 8/12/19.
//

#ifndef CIRC_CHANNEL_H
#define CIRC_CHANNEL_H

#include <glib.h>


#define CHANNEL_NAME_MAX 50

struct client_s;

typedef struct channel_s {
    char            name[CHANNEL_NAME_MAX + 1];     /* The name of the channel */
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
 * Verifies the validity of the given channel name.
 * @param name The name to test.
 * @return 0 if the name is valid, -1 if not.
 */
int channel_verify_name(const char *name);


#endif //CIRC_CHANNEL_H
