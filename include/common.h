//
// Created by mana on 8/5/19.
//

#ifndef CIRC_COMMON_H
#define CIRC_COMMON_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USAGE_STRING "circ [-h hostname] [-p port]"

#define usageErr(...) {             \
    fprintf(stderr, "usage: ");     \
    fprintf(stderr, __VA_ARGS__);   \
    fprintf(stderr, "\n");          \
    exit(EXIT_FAILURE);             \
}

#define exitErr(...) {                          \
    fprintf(stderr, __VA_ARGS__);               \
    fprintf(stderr, ": %s\n", strerror(errno)); \
    exit(EXIT_FAILURE);                         \
}

#define fatal(...) {                \
    fprintf(stderr, __VA_ARGS__);   \
    fprintf(stderr, "\n");          \
    exit(EXIT_FAILURE);             \
}

#endif //CIRC_COMMON_H
