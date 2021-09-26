/*
 * AC api
 */

#ifndef __AC_H__
#define __AC_H__

EXTERN_C_BEGIN

#define INVALID_HANDLE      0
#define AC_HIT_NUM          32
#define AC_REPEAT_HIT_COUNT 8

typedef struct ac_hit_pid
{
    Uint PidID;
    Uint HitCount;
    Byte *pHitStart[AC_REPEAT_HIT_COUNT];
    Byte *pHitEnd[AC_REPEAT_HIT_COUNT];
}ac_hit_pid_s;

typedef struct ac_full_result
{
    Uint PidNum;
    Uint RecordCount;
    ac_hit_pid_s HitPid[AC_HIT_NUM]; 
}ac_full_result_s;

extern ACHANDLE Ac_CreateHandle(void);

extern Uint Ac_AddPattern(ACHANDLE AcHandle, Byte *Pattern, \
        Uint16 PattLen, Uint Pid);

extern Uint Ac_Compile(ACHANDLE Handle);

extern void Ac_FullSearch(ACHANDLE Handle, Byte *pStart, \
        Uint uiLen, ac_full_result_s *pResutl);

extern void Ac_DestroyTrie(ACHANDLE Handle);

EXTERN_C_END

#endif
