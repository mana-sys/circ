//
// Created by manaxlalanes on 7/19/19.
//

#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>
#include "hashtable.h"
#include "irc_constants.h"

struct context_client {
    bool registered, receivedNick, receivedUser;
    int fd;
    char nickname[IRC_NICK_SIZE + 1];
    char username[IRC_MSG_SIZE];
    char fullname[IRC_MSG_SIZE];
};

struct context_server {
    struct hashtable_table *nick_table;
};



#endif //CIRC_CONTEXT_H
