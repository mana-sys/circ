#ifndef READ_MESSAGE_H
#define READ_MESSAGE_H

#include <stdbool.h>
#include <stdlib.h>

char *strncrlf(char *str, size_t limit);
ssize_t read_message(int fd, char *msg, char *buf, ssize_t *storeTotalRead, bool *storeDiscardNext);

#endif
