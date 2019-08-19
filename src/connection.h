//
// Created by mana on 8/18/19.
//

#ifndef CIRC_CONNECTION_H
#define CIRC_CONNECTION_H

#include "client.h"
#include "irc_constants.h"

typedef struct conn_s {
    bool     discardNext;           /* Whether to discard the next message. */
    ssize_t  totalRead;             /* Total bytes in the store buffer. */
    struct client_s client;
    char     message[IRC_MSG_SIZE]; /* Buffer to store a full individual message. */
    char     response[IRC_MSG_SIZE];
    char     store[IRC_MSG_SIZE];   /* Buffer to store bytes read from the client socket. */
} conn_s;

/**
 * Do a non-blocking read on the connection's file descriptor. After doing the read,
 * parse and handle the messages in the store buffer.
 * @param conn
 * @return 0 on success, -1 on error
 */
int handle_read(conn_s *conn);


#endif //CIRC_CONNECTION_H
