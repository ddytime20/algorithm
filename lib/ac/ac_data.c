/*
 *
 * Ac data
 *
 */

#include <stdlib.h>
#include <string.h>
#include "basetype.h"
#include "dlist.h"
#include "debug.h"
#include "ac_mem.h"
#include "ac_data.h"

/*
 * Free temporary state node
 */
void _FreeTmpState(ac_trie_s *pstTrie)
{
    if (NULL == pstTrie->TmpPool)
    {
        return;
    }

}

Uint _AllocTmpState(ac_trie_s *pstTrie)
{
    Uint uiNodeId;
    ac_mem_pool_s *TmpPool;
    ac_tmp_state_s *State
    
    DBGASSERT(NULL == pstTrie->TmpPool);
    TmpPool = AC_MALLOC(sizeof(ac_mem_pool_s));
    if (NULL == TmpPool)
    {
        return ERROR_MEM;
    }
    pstTrie->TmpPool = TmpPool;

    if (ERROR_SUCCESS == Ac_MemPool_Init(TmpPool, sizeof(ac_tmp_state_s)))
    {
        // get trie root node
        State = Ac_MemPoll_Malloc_Node(TmpPool, &uiNodeId);
        if (NULL == State)
        {
            printf("ac_data ac alloc root node failed\n");
            _FreeTmpState(pstTrie);
            return ERROR_FAILED;
        }
        // initialize root state
        State->StateID = 0;
        pstTrie->StateNum++;
    } else {
        printf("ac_data ac memory pool initializa failed \n");
    }

    DBGASSERT(0 == uiNodeId);
    return ERROR_SUCCESS;
}

Uint _AddSubState(ac_tmp_state_s *FatherState, Byte Ascii, Uint SubID)
{
    ac_tmp_path_s *TmpPath;        
    ac_tmp_path_s *PathList;

    PathList = AC_REALLOC(FatherState->PathList, (sizeof(ac_tmp_path_s) * (FatherState->PathNum + 1)));
    if (NULL == PathList)
    {
        return ERROR_FAILED;
    }

    FatherState->PathList= PathList;
    TmpPath = &PathList[FatherState->PathNum];
    TmpPath->ChildID = SubID;
    TmpPath->Ascii = Ascii;
    FatherState->PathNum++;

    return ERROR_SUCCESS;
}

ac_tmp_state_s *Ac_GetChildState(ac_mem_pool_s *pstMemPool, 
        ac_tmp_state_s *FatherState, Byte Ascii)
{
    Uint uiLoop;
    ac_tmp_state_s *ChildState = NULL;
    ac_tmp_path_s *TmpPath;

    // find child state form father state
    for (uiLoop = 0; uiLoop < FatherState->PathNum; uiLoop++)
    {
        TmpPath = &FatherState->PathList[uiLoop];        
        if (Ascii == TmpPath->Ascii)
        {
            ChildState = Ac_MemPool_GetNode(pstMemPool, TmpPath->ChildID);
            DBGASSERT(ChildState);
            break;
        }
    }

    return ChildState;
}
ac_tmp_state_s *_AddNewState(ac_trie_s *pTrie, ac_tmp_state_s *FatherState, Byte Ascii)
{
    Uint NodeID;
    ac_tmp_state_s *NextState;
    ac_mem_pool_s *MemPool = pTrie->TmpPool;

    NextState = Ac_GetChildState(MemPool, FatherState, Ascii);
    if (NULL == NextState)
    {
        // not find child state , add new state to father state
        NextState = Ac_MemPoll_Malloc_Node(MemPool, &NodeID);
        if (NULL == NextState)
        {
            return NULL;
        }
        memset(NextState, 0, sizeof(ac_tmp_state_s));
        NextState->StateID = NodeID;

        if (ERROR_SUCCESS != _AddSubState(FatherState, Ascii, NodeID))
        {
            return NULL;
        }
        pTrie->StateNum++;
    }

    return NextState;
}



ac_trie_s *Ac_CreateEmptyTrie(void)
{
    ac_trie_s *pTrie;

    pTrie = AC_MALLOC(sizeof(ac_trie_s));
    if (pTrie)
    {
        memset(pTrie, 0, sizeof(ac_trie_s));
        INIT_DLIST_HEAD(&pTrie->StateList);
        return pTrie;
    } else {
        return NULL;
    }
}

Uint Ac_AddPid(ac_trie_s *pTrie, ac_pid_s *pPid, ac_tmp_state_s *pState)
{
    Uint uiLoop;
    Bool bFound = False;
    ac_pid_s *PidList;

    // find repeat pid
    if (pState->PidList)
    {
        for (uiLoop = 0; uiLoop < pState->PidNum; uiLoop++)
        {
            if (0 == memcpy(pPid, &pState->PidList[uiLoop], sizeof(ac_pid_s)))
            {
                bFound = True;
                break;
            }
        }
    }

    if (False == bFound)
    {
        PidList = AC_REALLOC(pState->PidList, (sizeof(ac_pid_s) * (pState->PidNum + 1)));
        if (NULL == PidList)
        {
            return ERROR_MEM;
        }
        pState->PidList = PidList;
        memcpy(pState->PidList + pState->PidNum, pPid, sizeof(ac_pid_s));
        pState->PidNum++;
        pTrie->PidSum++;
    }

    return ERROR_SUCCESS;
}

Uint Ac_AddOnePatternToTrie(ac_trie_s *pTrie, Byte *Pattern, ac_pid_s *pPid)
{
    Byte Ascii;
    Uint uiLoop;
    ac_tmp_state_s *State;

    printf("ac_data ac add one pattern %s\n", Pattern);
    // first time, initialize temporary state
    if (NULL == pTrie->TmpPool)
    {
        if (ERROR_SUCCESS != _AllocTmpState(pTrie))
        {
            printf("ac_data ac alloc temporary state failed\n");
            return ERROR_FAILED;
        }
    }

    State = AC_GETROOTSTATE(pTrie->TmpPool);
    for (uiLoop = 0; uiLoop < pPid->PattLen; uiLoop++)
    {
        Ascii = Pattern[uiLoop];
        State = _AddNewState(pTrie, State, Ascii);
        if (NULL == State)
        {
            break;
        }
    }

    if (NULL == State)
    {
        return ERROR_FAILED;
    }

    if (ERROR_SUCCESS != Ac_AddPid(pTrie, pPid, State))
    {
        return ERROR_FAILED;
    }

    return ERROR_SUCCESS;
}
