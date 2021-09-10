/*
 *
 * AC basetype
 *
 */


#ifndef __BASETYPE_H__
#define __BASETYPE_H__

#ifndef EXTERN_C_BEGIN
#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((unsigned long)(&(((TYPE *)0)->MEMBER)))
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define True  1
#define False 0

/*
 * Define error code
 */
#define ERROR_SUCCESS                    0x0u
#define ERROR_FAILED                     0x1u
#define ERROR_MEM                        0x2u

#ifdef DEBUG_VERSION 
extern void Assert(char *filename, unsigned int line);
#define DBGASSERT(expression) \
    do{ \
        if (expression) \
            NULL; \
        else \
            Assert(__FILE__, __LINE__); \
    }while(0)
#else
#define DBGASSERT(expression)
#endif

typedef unsigned long  ACHANDLE;
typedef unsigned long  Ulong;
typedef unsigned int   Uint;
typedef unsigned short Uint16;
typedef short          Int16;
typedef unsigned char  Byte;
typedef unsigned char  Bool;


#endif
