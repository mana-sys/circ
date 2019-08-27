#ifndef PARSER_H
#define PARSER_H

#include "irc.h"

int parse_message(char *buf, struct irc_message *message);

#endif
