//
// Created by mana on 8/12/19.
//

#ifndef CIRC_CHANNEL_H
#define CIRC_CHANNEL_H

#include <glib.h>

#define CHANNEL_NAME_MAX 50

struct circ_channel_sub_s {
    int fd, clientId;
};

struct circ_channel_s {
    char            name[CHANNEL_NAME_MAX + 1];    /* The name of the channel */
    GHashTable *    members;                       /* Members of the channel */
};

struct circ_channel_s *circ_channel_new(const char *name);
void circ_channel_free(struct circ_channel_s *chan);
void circ_channel_add_subscriber(struct circ_channel_s *chan, int subscriberId);

int circ_channel_send_message(struct circ_channel_s *channel);

#endif //CIRC_CHANNEL_H
