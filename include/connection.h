//
// Created by mana on 8/18/19.
//

#ifndef CIRC_CONNECTION_H
#define CIRC_CONNECTION_H

#include <stdbool.h>
#include <stdint.h>

#include <glib.h>

#include "irc_constants.h"

#define CONN_RESULT_ERROR -1
#define CONN_RESULT_NORMAL 0
#define CONN_RESULT_CLOSE 1


/*
 * Represents a connection between the IRC server and a client.
 */
typedef struct conn_s {
    int         fd;
    bool        discardNext;
    uint32_t    eagain;
    ssize_t     totalRead;
    GQueue *    responses;
    char        message[IRC_MSG_SIZE];
    char        store  [IRC_MSG_SIZE];
} conn_s;

/**
 * Do a non-blocking read on the connection's file descriptor. After doing the read,
 * parse and handle the messages in the store buffer.
 * @param conn The connection to perform the read on.
 * @return CONN_RESULT_ERROR on error, CONN_RESULT_NORMAL on normal read,
 * CONN_RESULT_CLOSE on client closed connection.
 */
int handle_read(conn_s *conn);


/**
 * Do a non-blocking read on the connection's file descriptor. After doing the read,
 * parse and handle the messages in the store buffer.
 * @param conn The connection to perform the read on.
 * @return CONN_RESULT_ERROR on error, CONN_RESULT_NORMAL on normal read,
 * CONN_RESULT_CLOSE on client closed connection.
 */
int Conn_HandleRead(conn_s *);


/**
 * Do a series of non-blocking writes on the connection's file descriptor, trying to
 * send as many messages as possible from the connection's message buffer.
 *
 * @param conn The connection to perform the writes on.
 * @return
 */
int Conn_HandleWrite(conn_s *);


#endif //CIRC_CONNECTION_H
