/*
 * Ac test
 */

#include <stdlib.h>
#include <stdio.h>

#include "basetype.h"
#include "debug.h"
#include "ac.h"

typedef enum pid_type
{
    pid_type_test,
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
    {(Byte *)"test", 4, pid_type_test},
    {(Byte *)"number", 6, pid_type_number},
    {(Byte *)"find", 4, pid_type_find},
    {(Byte *)"main", 4, pid_type_main},
    {(Byte *)"printf", 6, pid_type_printf},
};


int main(int argc, char *argv[])
{
    Uint ret;
    Uint Num;
    Uint uiLoop;
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

    ret = Ac_Compile(Handle); 
    if (ERROR_SUCCESS != ret)
    {
        printf("Ac_Compile failed \n");
    }

    return 0;
}
