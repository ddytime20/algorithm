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

void _FreeTmpStateNode(ac_tmp_state_s *pTmpState)
{
    if (NULL != pTmpState->PathList)
    {
        AC_FREE(pTmpState->PathList);
        pTmpState->PathList = NULL;
        pTmpState->PathNum = 0;
    }

    if (NULL != pTmpState->PidList)
    {
        AC_FREE(pTmpState->PidList);
        pTmpState->PidList = NULL;
        pTmpState->PidNum = 0;
    }
}

/*
 * Free temporary state node
 */
void _FreeTmpState(ac_trie_s *pstTrie)
{
    if (NULL == pstTrie->pTmpPool)
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
    pstTrie->pTmpPool = TmpPool;

    if (ERROR_SUCCESS == Ac_MemPool_Init(TmpPool, sizeof(ac_tmp_state_s)))
    {
        // get trie root node
        State = Ac_MemPool_Malloc_Node(TmpPool, &uiNodeId);
        if (NULL == State)
        {
            AC_PRINTF("ac_data ac alloc root node failed\n");
            _FreeTmpState(pstTrie);
            return ERROR_FAILED;
        }
        // initialize root state
        State->StateID = 0;
        pstTrie->StateNum++;
    } else {
        AC_PRINTF("ac_data ac memory pool initializa failed \n");
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
    ac_mem_pool_s *MemPool = pTrie->pTmpPool;

    NextState = Ac_GetChildState(MemPool, FatherState, Ascii);
    if (NULL == NextState)
    {
        // not find child state , add new state to father state
        NextState = Ac_MemPool_Malloc_Node(MemPool, &NodeID);
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
            if (0 == memcmp(pPid, &pState->PidList[uiLoop], sizeof(ac_pid_s)))
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
        pTrie->PidNum++;
    }

    return ERROR_SUCCESS;
}

Uint Ac_AddOnePatternToTrie(ac_trie_s *pTrie, Byte *Pattern, ac_pid_s *pPid)
{
    Byte Ascii;
    Uint uiLoop;
    ac_tmp_state_s *State;

    //AC_PRINTF("ac_data ac add one pattern %s\n", Pattern);
    // first time, initialize temporary state
    if (NULL == pTrie->pTmpPool)
    {
        if (ERROR_SUCCESS != _AllocTmpState(pTrie))
        {
            AC_PRINTF("ac_data ac alloc temporary state failed\n");
            return ERROR_FAILED;
        }
    }

    State = AC_GETROOTSTATE(pTrie->pTmpPool);
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

void Ac_FreeTmpState(ac_trie_s *pTrie)
{
    Uint Loop;
    Uint Max;
    ac_tmp_state_s *TmpState;

    if (NULL == pTrie->pTmpPool)
    {
        return;
    }

    Max = Ac_MemPool_GetNodeNum(pTrie->pTmpPool);
    for (Loop = 0; Loop < Max; Loop++)
    {
        TmpState = Ac_MemPool_GetNode(pTrie->pTmpPool, Loop);
        _FreeTmpStateNode(TmpState);
    }

    Ac_MemPool_Fini(pTrie->pTmpPool);
    AC_FREE(pTrie->pTmpPool);
    pTrie->pTmpPool = NULL;
    
    return;
}

void Ac_FreeResource(ac_trie_s *pTrie)
{
    if (pTrie)
    {
        if (pTrie->pFullStateTable)
        {
            AC_FREE(pTrie->pFullStateTable);
            pTrie->pFullStateTable = NULL;
        }

        if (pTrie->pStateInfo)
        {
            AC_FREE(pTrie->pStateInfo);
            pTrie->pStateInfo = NULL;
        }

        if (pTrie->pPid)
        {
            AC_FREE(pTrie->pPid);
            pTrie->pPid = NULL;
        }

        if (pTrie->pNewState)
        {
            AC_FREE(pTrie->pNewState);
            pTrie->pNewState = NULL;
        }

        Ac_FreeTmpState(pTrie);
    }
}
