#include <stdlib.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "server.h"

#define ERR_MESSAGE_SIZE 255

int main(int argc, char *argv[])
{
    struct config_s conf;
    char msg[ERR_MESSAGE_SIZE];

    set_default_configuration(&conf);

    if (parse_configuration(&conf, argc, argv, ERR_MESSAGE_SIZE, msg) == -1) {
        fprintf(stderr, "%s\n", msg);
        usageErr(USAGE_STRING);
    }

    set_loglevel(L_TRACE); /* Default logging setting */

    start_server(&conf);
}