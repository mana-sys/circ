/*
 * File name:                   socket.h
 * Date of creation:            10 September 2019
 * Date of last modification:   10 September 2019
 * Author:                      mana-
 * Description:                 Simple helper library for easier creation and usage of sockets.
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>

#define BACKLOG     5
#define WILDCARD_IP "0.0.0.0"

/**
 * Creates a socket in the AF_INET family, binds it to the wildcard IP address on the specified port,
 * and listens on it.
 * @param port The port to bind on.
 * @return The file descriptor of the socket on success, -1 on error.
 */
int Socket_Listen(uint16_t port);

#endif