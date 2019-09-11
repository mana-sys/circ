//
// Created by manaxlalanes on 7/18/19.
//
#include <errno.h>
#include <string.h>

#ifndef LOG_H
#define LOG_H

#define L_TRACE 0
#define L_DEBUG 1
#define L_INFO 2
#define L_WARNING 3
#define L_ERROR 4
#define L_CRITICAL 5

typedef int loglevel_t;

loglevel_t parse_loglevel(char * levelstr);
void set_loglevel(loglevel_t level);
void circlog(loglevel_t level, char *fmt, ...);

#define logExitErr(...) do {                        \
    circlog(L_CRITICAL, __VA_ARGS__);               \
    circlog(L_CRITICAL, "%s", strerror(errno));     \
    exit(EXIT_FAILURE);                             \
} while(0)

#define Log_Trace(...) circlog(L_TRACE, __VA_ARGS__)
#define Log_Debug(...) circlog(L_DEBUG, __VA_ARGS__)
#define Log_Info(...) circlog(L_INFO, __VA_ARGS__)
#define Log_Warning(...) circlog(L_WARNING, __VA_ARGS__)
#define Log_Error(...) circlog(L_ERROR, __VA_ARGS__)
#define Log_Critical(...) circlog(L_CRITICAL, __VA_ARGS__)


#endif
