#ifndef READ_MESSAGE_H
#define READ_MESSAGE_H

#include <stdlib.h>

typedef ssize_t (*read_func_type)(int fd, void *buf, size_t count);
ssize_t read_message(int fd, char *msg, char *buf, ssize_t *storeTotalRead, read_func_type read);

#endif
