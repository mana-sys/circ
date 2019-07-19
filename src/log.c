#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "log.h"

static loglevel_t threshold = L_DEBUG;

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
            levelstr = "CRITICAL";
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
    printf("[%s] %6s - ", levelstr, buf);
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