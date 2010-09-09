
#include <stdio.h>

#include "common.h"

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

static int windows_console;

static void createWindowsConsole()
{
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;

    if (windows_console) return;
    windows_console = 1;

    AllocConsole();
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 512;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, 0, _IONBF, 0 );
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, 0, _IONBF, 0 );
}

#endif

int
vlog_msg(const char *fmt, va_list ap)
{
#ifdef WIN32
    createWindowsConsole();
#endif
    return vfprintf(stdout, fmt, ap);
}

int
vlog_err(const char *fmt, va_list ap)
{
#ifdef WIN32
    createWindowsConsole();
#endif
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

