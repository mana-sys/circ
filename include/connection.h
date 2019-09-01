//
// Created by mana on 8/18/19.
//

#ifndef CIRC_CONNECTION_H
#define CIRC_CONNECTION_H

#include <glib.h>

#include "client.h"
#include "irc_constants.h"

#define CONN_RESULT_ERROR -1
#define CONN_RESULT_NORMAL 0
#define CONN_RESULT_CLOSE 1


/*
 * Represents a connection between the IRC server and a client.
 */
typedef struct conn_s {
    bool     discardNext;               /* Whether to discard the next message. */
    size_t   responseLen;               /* Length of the response. */
    ssize_t  totalRead;                 /* Total bytes in the store buffer. */
    client_s client;                    /* Holds actual information about the client. */
    server_s server;                    /* Holds information about the server. */
    char     message[IRC_MSG_SIZE];     /* Buffer to store a full individual message. */
    char     response[IRC_MSG_SIZE];    /* Buffer to hold a response message. */
    char     store[IRC_MSG_SIZE];       /* Buffer to store bytes read from the client socket. */
    GQueue * responses;                 /* Queue to hold responses to be sent. */
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


#endif //CIRC_CONNECTION_H
