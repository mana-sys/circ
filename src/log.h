//
// Created by manaxlalanes on 7/18/19.
//

#ifndef LOG_H
#define LOG_H

#define L_TRACE 0
#define L_DEBUG 1
#define L_INFO 2
#define L_WARNING 3
#define L_ERROR 4
#define L_CRITICAL 5

typedef int loglevel_t;

void set_loglevel(loglevel_t level);
void circlog(loglevel_t level, char *fmt, ...);

#endif
