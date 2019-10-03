#include <getopt.h>
#include <stdio.h>

#include "common.h"
#include "config.h"

#define MESSAGE_FMT_INVALID_PORT        "error: Argument %s for option -%c is not a valid port number."
#define MESSAGE_FMT_NAN                 "error: Argument %s for option -%c is not a number."
#define MESSAGE_FMT_MISSING_ARG         "error: Missing argument for option -%c."
#define MESSAGE_FMT_UNEXPECTED_CASE     "error: Unexpected case in switch."
#define MESSAGE_FMT_UNRECOGNIZED_OPT    "error: Unrecognized option -%c."

void set_default_configuration(struct config_s *conf)
{
    conf->port = CIRC_PORT_NUM_DEFAULT;

    strncpy(conf->host, CIRC_HOSTNAME_DEFAULT, HOSTNAME_MAX);
    conf->host[HOSTNAME_MAX] = 0;

    conf->oper_passwd = "oper.txt";
}

int parse_configuration(struct config_s *conf, int argc, char *argv[], size_t msg_len, char *msg)
{
    long l;
    int opt;
    char *s;

    while ((opt = getopt(argc, argv, ":h:p:o:")) != -1) {
        switch(opt) {
            case 'h':
                strncpy(conf->host, optarg, HOSTNAME_MAX);
                break;
            case 'p':
                l = strtol(optarg, &s, 10);

                /*
                 * Check if optarg is a number.
                 */
                if (s == optarg) {
                    if (msg)
                        snprintf(msg, msg_len, MESSAGE_FMT_NAN, optarg, opt);
                    return -1;
                }

                /*
                 * Check that a valid port number (0 - 65535) was passed.
                 */
                if (l > UINT16_MAX) {
                    if (msg)
                        snprintf(msg, msg_len, MESSAGE_FMT_INVALID_PORT, optarg, opt);
                }
                break;
//            case 'l':
//                loglevelstr = optarg;
//                break;
            case ':':
                if (msg)
                    snprintf(msg, msg_len, MESSAGE_FMT_MISSING_ARG, optopt);
                return -1;
            case '?':
                if (msg)
                    snprintf(msg, msg_len, MESSAGE_FMT_UNRECOGNIZED_OPT, optopt);
                return -1;
            default:
                if (msg)
                    snprintf(msg, msg_len, MESSAGE_FMT_UNEXPECTED_CASE);
                return 1;
        }
    }

    return 0;
}
