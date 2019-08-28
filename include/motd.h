#ifndef CIRC_MOTD_H
#define CIRC_MOTD_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Reads the message of the day from the file at the specified path
 * into a dynamically allocated string and returns it.
 * @param path
 * @return
 */
char * Motd_Get(const char *path, size_t *len);

#endif //CIRC_MOTD_H
