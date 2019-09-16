//
// Created by mana on 9/15/19.
//



#ifndef CIRC_RESPONSE_H
#define CIRC_RESPONSE_H

#include <stdlib.h>

#include "irc_constants.h"

/*
 * Structure to represent a response message, consisting of the response
 * body and its length.
 */
typedef struct response_s {
    char    response[IRC_MSG_SIZE + 1];
    size_t  len;
} response_s;


#endif //CIRC_RESPONSE_H
