/*
 * AC api
 */

#ifndef __AC_H__
#define __AC_H__

EXTERN_C_BEGIN

#define INVALID_HANDLE 0

extern ACHANDLE Ac_CreateHandle(void);

extern Uint Ac_AddPattern(ACHANDLE AcHandle, Byte *Pattern, \
        Uint16 PattLen, Uint Pid);

extern Uint Ac_Compile(ACHANDLE Handle);


EXTERN_C_END

#endif
