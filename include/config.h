//
// Created by mana on 8/8/19.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <string.h>

#define CIRC_HOSTNAME_DEFAULT "localhost"
#define CIRC_PORT_NUM_DEFAULT 50002
#define HOSTNAME_MAX 255

typedef void (*parse_err_msg_handler)(char *);

/**
 * Structure to hold configuration details for the server.
 */
struct config_s {
    uint16_t port;
    char host[HOSTNAME_MAX + 1];
    char *oper_passwd;
};

/**
 * Populates the given config_s structure with the defaults.
 * The default hostname is given by CIRC_HOSTNAME_DEFAULT;
 * the default port number is given by CIRC_PORT_NUM_DEFAULT.
 *
 * @param conf
 */
void set_default_configuration(struct config_s *conf);

/**
 * Parses the command-line options as described by argc and argv into the
 * config_s structure given by conf. On error, writes an error message int
 * the buffer pointed to by msg, with a max length of msg_len bytes.
 *
 * @param conf
 * @param argc
 * @param argv
 * @param msg_len
 * @param msg
 * @return 0 on success, -1 on error
 */
int parse_configuration(struct config_s *conf, int argc, char *argv[], size_t msg_len, char *msg);


#endif
