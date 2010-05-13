
#include <stdio.h>

#include "common.h"

int
vlog_msg(const char *fmt, va_list ap)
{
    return vfprintf(stdout, fmt, ap);
}

int
vlog_err(const char *fmt, va_list ap)
{
    return vfprintf(stderr, fmt, ap);
}

int
log_msg(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    return vlog_msg(fmt, ap);
}

int
log_err(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    return vlog_err(fmt, ap);
}

