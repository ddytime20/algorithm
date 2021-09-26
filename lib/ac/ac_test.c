/*
 * Ac test
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "basetype.h"
#include "debug.h"
#include "ac.h"

typedef enum pid_type
{
    pid_type_test = 1,
    pid_type_number,
    pid_type_find,
    pid_type_main,
    pid_type_printf,
}pid_type_e;

typedef struct pattern
{
    Byte *Pattern;
    Uint16 PattLen;
    Uint Pid;
}pattern_s;

ACHANDLE Handle;
pattern_s Patterns[] = {
    {(Byte *)"main", 4, pid_type_main},
    {(Byte *)"test", 4, pid_type_test},
    {(Byte *)"printf", 6, pid_type_printf},
};

Byte *String = (Byte *)"maintestestjprintfainabcdfghjkmainj";

static void ac_test_result_print(Byte *pStart, Byte *pEnd)
{
    printf("hit pid ");

    while(pStart <= pEnd)
    {
        printf("%c",*pStart);
        pStart++;
    } 
    printf("\n");
}

int main(int argc, char *argv[])
{
    Uint ret;
    Uint Num;
    Uint uiLoop;
    ac_full_result_s Result;
    Handle = Ac_CreateHandle();
    if (INVALID_HANDLE == Handle)
    {
        AC_PRINTF("invalid ac hadndle \n");        
        return 0;
    }
    Num = sizeof(Patterns)/sizeof(pattern_s);
    for (uiLoop = 0; uiLoop < Num; uiLoop++)
    {
        ret = Ac_AddPattern(Handle, Patterns[uiLoop].Pattern,
                Patterns[uiLoop].PattLen, Patterns[uiLoop].Pid);
        if (ERROR_SUCCESS != ret)
        {
            break;
        }
    }

    if (ERROR_SUCCESS != ret)
    {
        printf("ac_test add pattern failed \n");
    }

    ret = Ac_Compile(Handle); 
    if (ERROR_SUCCESS != ret)
    {
        printf("Ac_Compile failed \n");
        return 0;
    }
    else
    {
        printf("ac compile success\n");
    }
    
    memset(&Result, 0, sizeof(ac_full_result_s));
    Ac_FullSearch(Handle, String, strlen(String), &Result);
    if (Result.PidNum)
    {
        int pidnum  = Result.PidNum;
        int index;
        int loop;
        ac_hit_pid_s *hitpid;
        printf("result hit pid number %d \n", pidnum);
        printf("*************************************\n");
        for (index = 0; index < pidnum; index++)
        {
            hitpid = &Result.HitPid[index];
            printf("hit pid ID %d \n",hitpid->PidID);
            printf("hit count %d \n", hitpid->HitCount);
            for(loop = 0; loop < hitpid->HitCount; loop++)
            {
                ac_test_result_print(hitpid->pHitStart[loop], hitpid->pHitEnd[loop]);
            }
        }
    }

    return 0;
}
