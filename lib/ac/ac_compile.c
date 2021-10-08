/*
 *
 * Ac compile
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
 *
 */
static void _WalkForEachPath(ac_mem_pool_s *pMemPool, dlist_head_s *pStateList,
        ac_tmp_state_s *pState)
{
    Uint uiLoop;
    ac_tmp_state_s *ChildState;
    ac_tmp_path_s *Path;

    for (uiLoop = 0; uiLoop < pState->PathNum; uiLoop++)
    {
        Path = &pState->PathList[uiLoop];
        ChildState = Ac_MemPool_GetNode(pMemPool, Path->ChildID);
        DBGASSERT(ChildState);
        dlist_add_tail(&ChildState->Node, pStateList);
    }

    return;
}


/*
 *  Hang state chain by layer
 */
static void _MakeLayer(ac_trie_s *pTrie)
{
    ac_tmp_state_s *State;
    ac_mem_pool_s *MemPool = pTrie->pTmpPool;
    dlist_head_s *StateList = &pTrie->StateList;

    State = AC_GETROOTSTATE(MemPool);
    // add root node
    dlist_add_head(&State->Node, StateList);    
    
    while (State)
    {
    
        _WalkForEachPath(MemPool, StateList, State);
        State = DLIST_NEXT_ENTRY(StateList, State, Node);
    }
}

/*
 * Get failed state
 */
static ac_tmp_state_s *_GetFailState(ac_mem_pool_s *pMemPool, ac_tmp_state_s *pState)
{
    Uint FailID;
    ac_tmp_state_s *FailState;

    FailID = pState->FailStateID;
    if (AC_INVALID_FAIL_ID == FailID)
    {
        return NULL;
    }

    FailState = (ac_tmp_state_s *)Ac_MemPool_GetNode(pMemPool, FailID);
    DBGASSERT(FailState);

    return FailState;
}


/*
 * Get child state
 */
static ac_tmp_state_s *_GetChildState(ac_mem_pool_s *pMemPool, ac_tmp_state_s *pFatherState, 
        Byte Ascii)
{
    Uint uiLoop;
    ac_tmp_path_s *Path;
    ac_tmp_state_s *ChildState = NULL;

    for (uiLoop = 0; uiLoop < pFatherState->PathNum; uiLoop++)
    {
        Path = &pFatherState->PathList[uiLoop];
        if (Ascii == Path->Ascii)
        {
            ChildState = Ac_MemPool_GetNode(pMemPool, Path->ChildID);
            break;
        }
    }

    return ChildState;
}

static ac_tmp_state_s *_GetFailedStateID(ac_mem_pool_s *pMemPool, ac_tmp_state_s *pFatherFailState,
        Byte Ascii)
{
    ac_tmp_state_s *FatherFailState;
    ac_tmp_state_s *ChildState = NULL;

    /*
     * Traverse failure state, find the state has the Ascii
     */
    for (FatherFailState = pFatherFailState; NULL != FatherFailState; 
            FatherFailState = _GetFailState(pMemPool, FatherFailState))
    {
        ChildState = _GetChildState(pMemPool, FatherFailState, Ascii);
        if (ChildState)
        {
            break;
        }
    }

    return ChildState;
}

/*
 * Set the failed jump of all child nodes according to the failed jump of the 
 * paren node
 */
static void _SetFailState(ac_mem_pool_s *pMemPool, ac_tmp_state_s *pFatherState)
{
    Uint uiLoop;
    ac_tmp_path_s *Path;
    ac_tmp_state_s *ChildState;
    ac_tmp_state_s *ChildFailState;
    ac_tmp_state_s *FatherFailState;

    /*
     * Get child state and corresponding Ascii, if the father failed state has the same
     * Ascii, then set the child failed state, otherwise set the child failed state root
     */
    FatherFailState = _GetFailState(pMemPool, pFatherState);
    for (uiLoop = 0; uiLoop < pFatherState->PathNum; uiLoop++)
    {
        Path = &pFatherState->PathList[uiLoop];
        ChildState = Ac_MemPool_GetNode(pMemPool, Path->ChildID);
        ChildFailState = _GetFailedStateID(pMemPool, FatherFailState, Path->Ascii);
        if (ChildFailState)
        {
            // if get failed node, set failed jump node;
            ChildState->FailStateID = ChildFailState->StateID;
        }
        else
        {
            // if not find failed node, set failed jump node to root;
            ChildState->FailStateID = 0; // root
        }
    }
    
    return;
}


static void _MakeFailureState(ac_trie_s *pTrie)
{
    ac_tmp_state_s *State;
    ac_mem_pool_s *MemPool = pTrie->pTmpPool;

    /*
     * set root state, like kvm next[0] = -1;
     */
    State = AC_GETROOTSTATE(MemPool); 
    State->FailStateID = AC_INVALID_FAIL_ID;

    /*
     * walk every node set fail state;
     */
    DLIST_FOREACH_ENTRY(&pTrie->StateList, State, Node)
    {
        _SetFailState(MemPool, State);
    }
}


static Uint _InheritFailPid(ac_trie_s *pTrie, ac_tmp_state_s *pState, ac_tmp_state_s *pFailState)
{
    Uint i;
    Uint ret;
    ac_pid_s *Pid;
    if (NULL == pFailState || 0 == pFailState->PidNum)
    {
        return ERROR_SUCCESS; 
    }

    AC_PRINTF("ac inherit failed state \n");
    for (i = 0; i < pFailState->PidNum; i++)
    {
        Pid = &pFailState->PidList[i];
        ret = Ac_AddPid(pTrie, Pid, pState);
        if (ERROR_SUCCESS != ret)
        {
            AC_PRINTF("ac_compile, add one pid faile \n");
            break;
        }
    }

    return ret;
}


static Uint _InheritPid(ac_trie_s *pTrie)
{
    Uint ret = ERROR_SUCCESS;
    ac_tmp_state_s *State;
    ac_tmp_state_s *FailState;
    
    DLIST_FOREACH_ENTRY(&pTrie->StateList, State, Node)
    {
        FailState = _GetFailState(pTrie->pTmpPool, State);
        ret = _InheritFailPid(pTrie, State, FailState);
        if (ERROR_SUCCESS != ret)
        {
            AC_PRINTF("ac_compile, Inherit fail pid fail\n");
            break;
        }
    }

    return ret;
}

static Uint _ReAssignStateID(ac_trie_s *pTrie)
{
    ac_tmp_state_s *State;
    Uint NewStateID;
    Uint NewStateBase;
    Uint *pNewState;
    Uint16 *pAccept;
    Uint16 AcceptCount = 0;
    Uint16 Loop;

    pNewState = AC_MALLOC(sizeof(Uint) * pTrie->StateNum);
    if (NULL == pNewState)
    {
        return ERROR_MEMORY;
    }
    memset(pNewState, 0, (sizeof(Uint) * pTrie->StateNum));
    pTrie->pNewState = pNewState;
    NewStateBase = 0;

    if ((pTrie->StateNum > AC_STATE_32767) && (pTrie->StateNum <= AC_STATE_65535))
    {
        pAccept = AC_MALLOC(pTrie->StateNum * sizeof(Uint16));
        if (NULL == pAccept)
        {
            AC_FREE(pNewState);
            pTrie->pNewState = NULL;
            return ERROR_MEMORY;
        }
        memset(pAccept, 0, sizeof(Uint16)*pTrie->StateNum);
        DLIST_FOREACH_ENTRY(&pTrie->StateList, State, Node)
        {
            if (State->PidNum != 0)
            {
                pAccept[AcceptCount] = State->StateID;
                AcceptCount++;
            }
            else
            {
                NewStateID = NewStateBase;
                NewStateBase++;
                pNewState[State->StateID] = NewStateID;
            }
        }

        pTrie->AcceptNum = NewStateBase;
        for (Loop = 0; Loop < AcceptCount; Loop++)
        {
            NewStateID = (Uint)pAccept[Loop];
            pNewState[NewStateID] = NewStateBase;
            NewStateBase++;
        }

        if (NULL != pAccept)
        {
            AC_FREE(pAccept);
        }
    }
    else
    {
        DLIST_FOREACH_ENTRY(&pTrie->StateList, State, Node)
        {
            NewStateID = NewStateBase;
            NewStateBase++;
            // set pid flag
            if (State->PidNum)
            {
                NewStateID = AC_SET_FLAG(NewStateID);
            }
            pNewState[State->StateID] = NewStateID;
        }
    }
    
    return ERROR_SUCCESS;
}

/*
 * Create fail jump table and inherit pid
 */
Uint Ac_PreCompile(ac_trie_s *pTrie)
{
    Uint ret = ERROR_FAILED;
    Uint NodeNum;

    NodeNum = Ac_MemPool_GetNodeNum(pTrie->pTmpPool);

    if (NodeNum != pTrie->StateNum)
    {
        AC_PRINTF("ac_compile state number error \n");
        return ret;
    }
    AC_PRINTF("ac_compile state number %d\n", NodeNum);
    _MakeLayer(pTrie);

    _MakeFailureState(pTrie);
    
    if (ERROR_SUCCESS == _InheritPid(pTrie))
    {
        ret = _ReAssignStateID(pTrie);
    }
    else
    {
        AC_PRINTF("ac_compile, inherit pid fail \n");
    }

    return ret;
}

