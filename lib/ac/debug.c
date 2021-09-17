/*
 *
 * AC Debug
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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

