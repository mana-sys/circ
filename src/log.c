#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "log.h"

static loglevel_t threshold = L_DEBUG;

loglevel_t parse_loglevel(char * levelstr)
{
    size_t levelstrlen;

    levelstrlen = strlen(levelstr);
    if (strncasecmp("TRACE", levelstr, levelstrlen) == 0)
        return L_TRACE;
    if (strncasecmp("DEBUG", levelstr, levelstrlen) == 0)
        return L_DEBUG;
    if (strncasecmp("INFO", levelstr, levelstrlen) == 0)
        return L_INFO;
    if (strncasecmp("WARNING", levelstr, levelstrlen) == 0)
        return L_WARNING;
    if (strncasecmp("ERROR", levelstr, levelstrlen) == 0)
        return L_ERROR;
    if (strncasecmp("CRITICAL", levelstr, levelstrlen) == 0)
        return L_CRITICAL;
    return -1;
}

void set_loglevel(loglevel_t level)
{
    threshold = level;
}

static void circlog_internal(loglevel_t level, char *fmt, va_list argptr)
{
    time_t t;
    char buf[80], *levelstr;

    if (level < threshold)
        return;

    t = time(NULL);
    strftime(buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&t));

    switch (level) {
        case L_CRITICAL:
            levelstr = "FATAL";
            break;
        case L_ERROR:
            levelstr = "ERROR";
            break;
        case L_WARNING:
            levelstr = "WARN";
            break;
        case L_INFO:
            levelstr = "INFO";
            break;
        case L_DEBUG:
            levelstr = "DEBUG";
            break;
        case L_TRACE:
            levelstr = "TRACE";
            break;
        default:
            levelstr = "UNKNOWN";
    }

    flockfile(stdout);
    printf("[%.5s]\t %s - ", levelstr, buf);
    vprintf(fmt, argptr);
    printf("\n");
    fflush(stdout);
    funlockfile(stdout);

}

void circlog(loglevel_t level, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    circlog_internal(level, fmt, ap);
    va_end(ap);
}