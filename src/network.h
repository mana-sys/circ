//
// Created by mana on 8/9/19.
//

#ifndef CIRC_NETWORK_H
#define CIRC_NETWORK_H

#include <arpa/inet.h>
#include <sys/socket.h>

struct socket_s {
    int fd;
    struct sockaddr_in addr;
};

#endif //CIRC_NETWORK_H
