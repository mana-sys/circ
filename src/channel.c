//
// Created by mana on 8/12/19.
//
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "channel.h"

struct circ_channel_s *circ_channel_new(const char *name)
{
    struct circ_channel_s *chan;
    struct sockaddr_un addr;

    chan = calloc(1, sizeof(struct circ_channel_s));
    strncpy(chan->name, name, CHANNEL_NAME_MAX);

    chan->fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (chan->fd == -1)
        goto err;

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(&addr.sun_path[1], chan->name, sizeof(addr.sun_path) - 2);

    if (bind(chan->fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1)
        goto err;

    return chan;

    err:
    free(chan);
    return NULL;

}
