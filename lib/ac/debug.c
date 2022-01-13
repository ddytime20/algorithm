/*
 *
 * AC Debug
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define DEBUG_BUFF_LEN 0x400U

void Assert(char *filename, unsigned int line)
{
    fflush(stdout);
    fprintf(stderr, "\nAssert failed: %s, line %u \n",filename, line);
    fflush(stderr);
    abort();
}

void AC_PRINTF(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void Ac_Printf(const char *func, const char *line, const char *fmt, ...)
{
    char szBuff[DEBUG_BUFF_LEN];
    va_list pstArgs;

    va_start(pstArgs, fmt);
    vsnprintf(szBuff, DEBUG_BUFF_LEN, fmt, pstArgs);
    va_end(pstArgs);

    fprintf(stdout, "[%20s:%4s], %s", func, line, szBuff);
}

