/*
 * File name:                   socket.c
 * Date of creation:            10 September 2019
 * Date of last modification:   10 September 2019
 * Author:                      mana-
 * Description:                 Simple helper library for easier creation and usage of sockets.
 */

#include "socket.h"

int Socket_Listen(uint16_t port)
{
    int fd, optval;
    struct sockaddr_in addr;

    /*
     * Create socket.
     */
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    /*
     * Set SO_REUSEADDR.
     */
    optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        return -1;

    /*
     * Fill out socket address info, including family, port, and IP to bind on.
     */
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, WILDCARD_IP, &addr.sin_addr) == -1)
        return -1;

    /*
     * Bind socket.
     */
    if (bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1)
        return -1;

    /*
     * Listen for connections on socket.
     */
    if (listen(fd, BACKLOG) == -1)
        return -1;

    return fd;
}