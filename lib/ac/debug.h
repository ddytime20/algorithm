/*
 *
 * AC debug
 *
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define PRINTF_ATTR(fmt, args) __attribute__((format(printf, fmt, args)))

extern void AC_PRINTF(char *format, ...) PRINTF_ATTR(1, 2);


#endif
