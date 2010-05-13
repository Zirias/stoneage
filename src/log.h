#ifndef STONEAGE_LOG_H
#define STONEAGE_LOG_H

#include <stdarg.h>

#define ERRMSG_OOM "Out of memory!"

extern int log_msg(const char* fmt, ...);
extern int log_err(const char* fmt, ...);
extern int vlog_msg(const char* fmt, va_list ap);
extern int vlog_err(const char* fmt, va_list ap);

#endif
