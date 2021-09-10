/*
 *
 * Ac compile
 *
 */

#include <stdlib.h>
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
    ac_mem_pool_s *MemPool = pTrie->TmpPool;
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

    FailID = pState->FailedID;
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
            ChildState->FailedID = ChildFailState->StateID;
        }
        else
        {
            // if not find failed node, set failed jump node to root;
            ChildState->FailedID = 0; // root
        }
    }
    
    return;
}


static void _MakeFailureState(ac_trie_s *pTrie)
{
    ac_tmp_state_s *State;
    ac_mem_pool_s *MemPool = pTrie->TmpPool;

    /*
     * set root state, like kvm next[0] = -1;
     */
    State = AC_GETROOTSTATE(MemPool); 
    State->FailedID = AC_INVALID_FAIL_ID;

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

    for (i = 0; i < pFailState->PidNum; i++)
    {
        Pid = &pFailState->PidList[i];
        ret = Ac_AddPid(pTrie, Pid, pState);
        if (ERROR_SUCCESS != ret)
        {
            printf("ac_compile, add one pid faile \n");
            break;
        }
    }

    return ret;
}


static Uint _InheritPid(ac_trie_s *pTrie)
{
    Uint ret;
    ac_tmp_state_s *State;
    ac_tmp_state_s *FailState;
    
    DLIST_FOREACH_ENTRY(&pTrie->StateList, State, Node)
    {
        FailState = _GetFailState(pTrie->TmpPool, State);
        ret = _InheritFailPid(pTrie, State, FailState);
        if (ERROR_SUCCESS != ret)
        {
            printf("ac_compile, Inherit fail pid fail\n");
            break;
        }
    }

    return ret;
}

/*
 * Create fail jump table and inherit pid
 */
Uint Ac_PreCompile(ac_trie_s *pTrie)
{
    Uint ret;
    Uint NodeNum;

    NodeNum = Ac_MemPool_GetNodeNum(pTrie->TmpPool);

    if (NodeNum != pTrie->StateNum)
    {
        printf("ac_compile state number error \n");
        return ERROR_FAILED;
    }
    printf("ac_compile state number %d\n", NodeNum);
    _MakeLayer(pTrie);

    _MakeFailureState(pTrie);
    
    ret = _InheritPid(pTrie);

    return ret;
}

