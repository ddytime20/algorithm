/*
 *
 * AC api.c
 *
 */

#include <stdlib.h>
#include <string.h>
#include "basetype.h"
#include "dlist.h"
#include "debug.h"
#include "ac_mem.h"
#include "ac_data.h"



Uint Ac_AddPattern(ACHANDLE AcHandle, Byte *Pattern,
        Uint16 PattLen, Uint Pid)
{
    ac_pid_s stPid;
    ac_trie_s *Trie = (ac_trie_s *)AcHandle; 
    memset(&stPid, 0, sizeof(stPid));

    stPid.PattLen = PattLen;
    stPid.Pid = Pid;

    return Ac_AddOnePatternToTrie(Trie, Pattern, &stPid);
}

/*
 * Ac create empty trie
 */
ACHANDLE Ac_CreateHandle(void)
{
    ac_trie_s *pstTrie;

    pstTrie = Ac_CreateEmptyTrie();
    if (!pstTrie)
    {
        return 0;
    }

    return (ACHANDLE)pstTrie;
}

/*
 *
 */
Uint Ac_Compile(ACHANDLE Handle)
{
    ac_trie_s *pTrie;

    pTrie = (ac_trie_s *)Handle;
    
    if (ERROR_SUCCESS != Ac_PreCompile(pTrie))
    {
        printf("ac_api, ac prepare compile failed \n");
        return ERROR_FAILED;
    }

    return ERROR_SUCCESS;
}
