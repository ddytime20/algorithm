/*
 *
 * AC debug
 *
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

EXTERN_C_BEGIN

#ifndef _STDARG_H
#include <stdarg.h>
#endif

#define PRINTF_ATTR(fmt, args) __attribute__((format(printf, fmt, args)))

#define DEBUG_STRING(X)     #X
#define DEBUG_TOSTR(X)      DEBUG_STRING(X)

#define DEBUG_LOCATION_FUNC __func__
#define DEBUG_LOCATION_LINE DEBUG_TOSTR(__LINE__)

void Ac_Printf(const char *, const char *, const char *, ...) PRINTF_ATTR(3,4);

#define AC_PRINTF(format, ...)    \
   do{  \
      Ac_Printf(DEBUG_LOCATION_FUNC, DEBUG_LOCATION_LINE, (format), ## __VA_ARGS__);  \
   }while(0) 


EXTERN_C_END

#endif
