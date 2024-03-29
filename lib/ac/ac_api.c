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
#include "ac_compile.h"
#include "ac.h"

#define AC_MSG_CONTINUE   0UL
#define AC_MSG_FULL       1UL

static Uint g_uiFullStateMax;

static void _Ac_InitBatchIterate(ac_batch_iterate_s *pBatch)
{
    memset(pBatch, 0, sizeof(ac_batch_iterate_s));
    pBatch->Batch[0] = AC_MSG_TREE;
}

static Uint _Ac_GetTrieInfo(ac_trie_s *pTrie, ac_batch_iterate_s *pBatch,
        Byte *pBuff, Uint BuffLen)
{
    ac_msg_head_s *head;
    ac_msg_tree_s *tree;

    if (BuffLen < (sizeof(ac_msg_head_s) + sizeof(ac_msg_tree_s)))
    {
        return 0;
    }
    
    head = (ac_msg_head_s *)pBuff;
    head->MsgType = AC_MSG_TREE;

    tree = (ac_msg_tree_s *)(head + 1);
    tree->PidNum = pTrie->PidNum;
    tree->StateNum = pTrie->StateNum;
    tree->AcceptNum = pTrie->AcceptNum;

    pBatch->Batch[0] = AC_MSG_STATE;

    return (Uint)(sizeof(ac_msg_head_s) + sizeof(ac_msg_tree_s));
}

static Uint _Ac_FillStateInfo(ac_trie_s *pTrie, ac_tmp_state_s *pState,
    Byte *pBuff, Uint BuffLen, Uint *pPathIndex, Uint *pDataLen)
{
    Uint ret = AC_MSG_CONTINUE;
    Uint Loop;
    Uint NewState;
    Uint NewChildState;
    Uint Length = 0;
    ac_tmp_path_s *Path;
    ac_msg_state_s *MsgState;

    MsgState = (ac_msg_state_s *)pBuff;
    NewState = pTrie->pNewState[pState->StateID];
    NewState = AC_GET_ID(NewState);  // delete falg

    for (Loop = *pPathIndex; Loop < pState->PathNum; Loop++)
    {
        if (Length + sizeof(ac_msg_state_s) > BuffLen)
        {
            ret = AC_MSG_FULL;
            break;
        }

        Path = &pState->PathList[Loop];
        NewChildState = pTrie->pNewState[Path->ChildID];
        MsgState->StateID = NewState;
        MsgState->SubStateID = NewChildState;
        MsgState->Ascii = Path->Ascii;

        MsgState++;
        Length += sizeof(ac_msg_state_s);
    }

    *pPathIndex = Loop;
    *pDataLen = Length;
    
    return ret;
}


static Uint _Ac_GetTrieState(ac_trie_s *pTrie, ac_batch_iterate_s *pBatch,
        Byte *pBuff, Uint BuffLen)
{
    Uint ret;
    Uint StateID;
    Uint PathIndex;
    Uint DataLen;
    Uint TotalLen;
    ac_msg_head_s *Head;
    ac_tmp_state_s *State;    

    if (BuffLen < (sizeof(ac_msg_head_s) + sizeof(ac_msg_state_s)))
    {
        return 0;
    }

    Head = (ac_msg_head_s *)pBuff;
    Head->MsgType = AC_MSG_STATE;

    StateID = pBatch->Batch[1];
    PathIndex = pBatch->Batch[2];
    State = Ac_MemPool_GetNode(pTrie->pTmpPool, StateID);
    TotalLen = sizeof(ac_msg_head_s);

    do
    {
        ret = _Ac_FillStateInfo(pTrie, State, pBuff+TotalLen, BuffLen-TotalLen, &PathIndex, &DataLen);
        TotalLen += DataLen;

        if (AC_MSG_FULL == ret)
        {
            pBatch->Batch[1] = State->StateID;
            pBatch->Batch[2] = PathIndex;
            break;
        }

        State = DLIST_NEXT_ENTRY(&pTrie->StateList, State, Node);
        PathIndex = 0;
    }while(NULL != State);

    if (NULL == State)
    {
        memset(pBatch, 0, sizeof(ac_batch_iterate_s));
        pBatch->Batch[0] = AC_MSG_PID;
    }
    
    return TotalLen;
}

static Uint _Ac_FillFailStateInfo(ac_trie_s *pTrie, ac_tmp_state_s *pState,
        Byte *pBuff, Uint BuffLen, Uint *pDataLen)
{
    Uint NewStateID;
    ac_msg_failstate_s *MsgFailState;

    if (BuffLen < sizeof(ac_msg_failstate_s))
    {
        return AC_MSG_FULL;
    }
    if (AC_INVALID_FAIL_ID == pState->FailStateID)
    {
        return AC_MSG_CONTINUE;
    }
    NewStateID = pTrie->pNewState[pState->StateID];
    NewStateID = AC_GET_ID(NewStateID);
    MsgFailState = (ac_msg_failstate_s *)pBuff;
    MsgFailState->StateID = NewStateID;
    NewStateID = pTrie->pNewState[pState->FailStateID];
    NewStateID = AC_GET_ID(NewStateID); // fail id do not need flag
    MsgFailState->FailStateID = NewStateID;
    
    *pDataLen = sizeof(ac_msg_failstate_s);

    return AC_MSG_CONTINUE;
}

static Uint _Ac_FillPidInfo(ac_trie_s *pTrie, ac_tmp_state_s *pState,
        Byte *pBuff, Uint BuffLen, Uint *pIndex, Uint *pDataLen)
{
    Uint ret = AC_MSG_CONTINUE;
    Uint Loop;
    Uint Length = 0;
    Uint NewStateID;
    ac_msg_pid_s *MsgPid;
    ac_pid_s *Pid;

    MsgPid = (ac_msg_pid_s *)pBuff;
    NewStateID = pTrie->pNewState[pState->StateID];
    NewStateID = AC_GET_ID(NewStateID); // delete flag

    for (Loop = *pIndex; Loop < pState->PidNum; Loop++)
    {
        if (Length + sizeof(ac_msg_pid_s) > BuffLen)
        {
            ret = AC_MSG_FULL;
            break;
        }
        
        Pid = &pState->PidList[Loop];
        MsgPid->StateID = NewStateID;
        MsgPid->PidID = Pid->PidID;
        MsgPid->PattLen = Pid->PattLen;

        MsgPid++;
        Length += sizeof(ac_msg_pid_s);
    }
    
    *pIndex = Loop;
    *pDataLen = Length;

    return ret;
}


static Uint _Ac_GetTriePid(ac_trie_s *pTrie, ac_batch_iterate_s *pBatch, 
        Byte *pBuff, Uint BuffLen)
{
    Uint ret;
    Uint TotalLen;
    Uint DataLen;
    Uint StateID;
    Uint Index;
    ac_msg_head_s *Head;
    ac_tmp_state_s *State;

    if (BuffLen < (sizeof(ac_msg_head_s) + sizeof(ac_msg_pid_s)))
    {
        return 0;
    }
    
    Head = (ac_msg_head_s *)pBuff;
    Head->MsgType = AC_MSG_PID;

    StateID = pBatch->Batch[1];
    Index = pBatch->Batch[2];
    State = Ac_MemPool_GetNode(pTrie->pTmpPool, StateID);
    TotalLen = sizeof(ac_msg_head_s);

    do
    {
        ret = _Ac_FillPidInfo(pTrie, State, pBuff+TotalLen, BuffLen-TotalLen, 
                &Index, &DataLen);
        TotalLen += DataLen;
        if (AC_MSG_FULL == ret)
        {
            pBatch->Batch[1] = State->StateID;
            pBatch->Batch[2] = Index;
            break;
        }

        State = DLIST_NEXT_ENTRY(&pTrie->StateList, State, Node);
        Index = 0;
    }while(NULL != State);

    if (NULL == State)
    {
        memset(pBatch, 0, sizeof(ac_batch_iterate_s));
        pBatch->Batch[0] = AC_MSG_FAILSTATE;
    }

    return TotalLen;
}

static Uint _Ac_GetTrieFailState(ac_trie_s *pTrie, ac_batch_iterate_s *pBatch, 
        Byte *pBuff, Uint uiBuffLen)
{
    Uint ret;
    Uint TotalLen;
    Uint DataLen;
    Uint StateID;
    ac_msg_head_s *Head;
    ac_tmp_state_s *TmpState;

    if (uiBuffLen < (sizeof(ac_msg_head_s) + sizeof(ac_msg_failstate_s)))
    {
        return 0;
    }
    Head = (ac_msg_head_s *)pBuff;
    Head->MsgType = AC_MSG_FAILSTATE;

    StateID = pBatch->Batch[1];
    TmpState = Ac_MemPool_GetNode(pTrie->pTmpPool, StateID);
    TotalLen = sizeof(ac_msg_head_s);
    
    do
    {
        ret = _Ac_FillFailStateInfo(pTrie, TmpState, pBuff+TotalLen,
               uiBuffLen-TotalLen, &DataLen); 
        TotalLen += DataLen;
        if (AC_MSG_FULL == ret)
        {
            pBatch->Batch[1] = TmpState->StateID;
            break;
        }

        TmpState = DLIST_NEXT_ENTRY(&pTrie->StateList, TmpState, Node);

    }while(NULL != TmpState);

    if (NULL == TmpState)
    {
        memset(pBatch, 0, sizeof(ac_batch_iterate_s));
        pBatch->Batch[0] = AC_MSG_END;
    }

    return TotalLen;
}

static Uint _Ac_TrieEnd(ac_trie_s *pTrie)
{
    Ac_FreeTmpState(pTrie);

    if (NULL != pTrie->pNewState)
    {
        AC_FREE(pTrie->pNewState);
        pTrie->pNewState = NULL;
    }

    return ERROR_SUCCESS;
}


static Uint _Ac_MallocTrieResource(ac_trie_s *pTrie, Uint uiPidNum, Uint uiStateNum)
{
    Uint Len;
    void *pData;
    ac_state_info_s *StateInfo;
    ac_pid_s *PidInfo;

    DBGASSERT(uiPidNum);
    DBGASSERT(uiStateNum);

    if (AC_STATE_65535 < uiStateNum)
    {
        return ERROR_FAILED; 
    }
    else if (AC_STATE_32767 < uiStateNum)
    {
        return ERROR_FAILED;
    }
    else if (AC_STATE_127 < uiStateNum)
    {
        Len = sizeof(AC_STATE_ROW16) * uiStateNum;
    }
    else
    {
        Len = sizeof(AC_STATE_ROW8) * uiStateNum;
    }

    pData = AC_MALLOC(Len);
    if (NULL == pData)
    {
        AC_PRINTF("ac_api, out of memory\n");
        return ERROR_MEMORY;
    }
    memset(pData, 0, Len);
    pTrie->pFullStateTable = pData;

    StateInfo = AC_MALLOC(sizeof(ac_state_info_s) * uiStateNum);
    if (NULL == StateInfo)
    {
        AC_PRINTF("ac_api, out of memory \n");
        return ERROR_MEMORY;
    }

    memset(StateInfo, 0, sizeof(ac_state_info_s) *uiStateNum);
    pTrie->pStateInfo = StateInfo;

    PidInfo = AC_MALLOC(sizeof(ac_pid_s) * uiPidNum);
    if (NULL == PidInfo)
    {
        AC_PRINTF("ac_api, out of memory \n");
        return ERROR_MEMORY;
    }
    pTrie->pPid = PidInfo;
    pTrie->StateNum = uiStateNum;
    pTrie->FullStateNum = uiStateNum;
    pTrie->PidNum = uiPidNum;

    return ERROR_SUCCESS;
}


static Uint _Ac_SetTrieInfo(ac_trie_s *pTrie, Byte *pBuff, Uint BuffLen)
{
    ac_msg_tree_s *tree;
    Uint PidNum;
    Uint StateNum;

    tree = (ac_msg_tree_s *)pBuff;
    pTrie->AcceptNum = tree->AcceptNum;
    PidNum = tree->PidNum;
    StateNum = tree->StateNum;
    
    return _Ac_MallocTrieResource(pTrie, PidNum, StateNum);
}

static Uint _Ac_SetStateInfo(ac_trie_s *pTrie, Byte *pBuff, Uint BuffLen)
{
    Uint ChildID;
    //Uint StateID;
    ac_msg_state_s *State;
    Uint16 *SubState16;
    Uint16 ChildID16;
    Byte *SubState8;
    Byte ChildID8;
    AC_STATE_ROW16 *StateTable16;
    AC_STATE_ROW8 *StateTable8;

    State = (ac_msg_state_s *)pBuff;    

    for (;State < (ac_msg_state_s *)(pBuff + BuffLen);)
    {
        //StateID = State->StateID;
        ChildID = State->SubStateID;

        if (pTrie->StateNum > AC_STATE_65535)
        {
            break;
        }
        else if (pTrie->StateNum > AC_STATE_32767)
        {
            break;
        }
        else if (pTrie->StateNum > AC_STATE_127)
        {
            StateTable16 = (AC_STATE_ROW16 *)pTrie->pFullStateTable;
            ChildID16 = (Uint16)AC_GET_ID(ChildID);
            if (AC_GET_FLAG(ChildID))
            {
                ChildID16 = AC_SET_FLAG16(ChildID16);
            }

            SubState16 = StateTable16[State->StateID];
            SubState16[State->Ascii] = ChildID16;
        }
        else
        {
            StateTable8 = (AC_STATE_ROW8 *)pTrie->pFullStateTable;
            ChildID8 = (Byte)AC_GET_ID(ChildID);
            if (AC_GET_FLAG(ChildID))
            {
                ChildID8 = AC_SET_FLAG8(ChildID8);
            }

            SubState8 = StateTable8[State->StateID];
            SubState8[State->Ascii] = ChildID8;
        }
        State++;
    }

    return ERROR_SUCCESS;
}


static void _Ac_InheritFailState(ac_trie_s *pTrie, Uint StateID, Uint ChildID)
{
    AC_STATE_ROW8 *FullTable8;
    AC_STATE_ROW16 *FullTable16;
    //AC_STATE_ROW *FullTable;
    //Uint *StateTable;
    //Uint *ChilidTable;
    Uint16 *StateTable16;
    Uint16 *ChildTable16;
    Byte *StateTable8;
    Byte *ChildTable8;
    Uint Ascii;

    if (NULL == pTrie->pFullStateTable)
    {
        return;
    }

    for (Ascii = 0; Ascii < 256; Ascii++)
    {
        if (pTrie->StateNum > AC_STATE_65535)
        {
            continue;
        }
        else if (pTrie->StateNum > AC_STATE_32767)
        {
            continue;
        }
        else if (pTrie->StateNum > AC_STATE_127)
        {
            FullTable16 = (AC_STATE_ROW16 *)pTrie->pFullStateTable;
            StateTable16 = FullTable16[StateID];
            ChildTable16 = FullTable16[ChildID];
            if ((0 == StateTable16[Ascii]) && (0 != ChildTable16[Ascii]))
            {
                StateTable16[Ascii] = ChildTable16[Ascii];
            }
        }
        else
        {
            FullTable8 = (AC_STATE_ROW8 *)pTrie->pFullStateTable;
            StateTable8 = FullTable8[StateID];
            ChildTable8 = FullTable8[ChildID];
            if ((0 == StateTable8[Ascii]) && (0 != ChildTable8[Ascii]))
            {
                StateTable8[Ascii] = ChildTable8[Ascii];
            }
        }
    }

}

static Uint _Ac_SetFailStateInfo(ac_trie_s *pTrie, Byte *pBuff, Uint uiBuffLen)
{
    ac_msg_failstate_s *MsgFail;
    ac_state_info_s *StateInfo;

    DBGASSERT(0 == uiBuffLen%sizeof(ac_msg_failstate_s));
    MsgFail = (ac_msg_failstate_s *)pBuff;

    for (;MsgFail < (ac_msg_failstate_s *)(pBuff + uiBuffLen);)
    {
        StateInfo = &pTrie->pStateInfo[MsgFail->StateID];
        StateInfo->FailStateID = MsgFail->FailStateID;
        _Ac_InheritFailState(pTrie, MsgFail->StateID, MsgFail->FailStateID);
        MsgFail++;
    }

    return ERROR_SUCCESS;
}


static Uint _Ac_SetPidInfo(ac_trie_s *pTrie, Byte *pBuff, Uint BuffLen)
{
    Uint StateID;
    ac_msg_pid_s *MsgPid;
    ac_pid_s *Pid;
    ac_state_info_s *StateInfo;

    DBGASSERT(0 == BuffLen%sizeof(ac_msg_pid_s));
    MsgPid = (ac_msg_pid_s *)pBuff;

    for (; MsgPid < (ac_msg_pid_s *)(pBuff + BuffLen);)
    {
        StateID = MsgPid->StateID;

        Pid = &pTrie->pPid[pTrie->NextIndex];
        Pid->PidID = MsgPid->PidID;
        Pid->PattLen = MsgPid->PattLen;

        StateInfo = &pTrie->pStateInfo[StateID];
        if (NULL == StateInfo->pPidList)
        {
            StateInfo->pPidList = (void *)Pid;
            StateInfo->PidNum = 1;
        }
        else
        {
            StateInfo->PidNum++;
        }
        pTrie->NextIndex++;
        MsgPid++;
    }

    return ERROR_SUCCESS;
}


static Uint _Ac_GetNextBatchInfo(ac_trie_s *pTrie, ac_batch_iterate_s *pBatch, 
        Byte *pBuff, Uint BuffLen)
{
    Uint ret = 0;

    switch (pBatch->Batch[0])
    {
        case AC_MSG_TREE:
            {
                ret = _Ac_GetTrieInfo(pTrie, pBatch, pBuff, BuffLen);
                //AC_PRINTF("ac_api.c, get AC_MSG_TREE ret %d\n", ret);
                break;
            }
        case AC_MSG_STATE:
            {
                ret = _Ac_GetTrieState(pTrie, pBatch, pBuff, BuffLen);
                break;
            }
        case AC_MSG_PID:
            {
                ret = _Ac_GetTriePid(pTrie, pBatch, pBuff, BuffLen);
                break;
            }
        case AC_MSG_FAILSTATE:
            {
                ret = _Ac_GetTrieFailState(pTrie, pBatch, pBuff, BuffLen);
                break;
            }
        case AC_MSG_END:
            {
                ret = _Ac_TrieEnd(pTrie);
                break;
            }
        default:
            {
                DBGASSERT(False);
                break;
            }
    }

    return ret;
}

static Uint _Ac_BatchProc(ac_trie_s *pTrie, Byte *pBuff, Uint BuffLen)
{
    Uint ret = ERROR_FAILED;
    ac_msg_head_s *Head;
    Byte *Data;
    Uint DataLen;

    Head = (ac_msg_head_s *)pBuff;
    Data = (Byte *)(Head + 1);
    DataLen = BuffLen - sizeof(ac_msg_head_s);

    switch (Head->MsgType)
    {
        case AC_MSG_TREE:
            {
                ret = _Ac_SetTrieInfo(pTrie, Data, DataLen);
                break;
            }
        case AC_MSG_STATE:
            {
                ret = _Ac_SetStateInfo(pTrie, Data, DataLen);
                break;
            }
        case AC_MSG_PID:
            {
                ret = _Ac_SetPidInfo(pTrie, Data, DataLen);
                break;
            }
        case AC_MSG_FAILSTATE:
            {
                ret = _Ac_SetFailStateInfo(pTrie, Data, DataLen);
                break;
            }
        default:
            {
                DBGASSERT(False);
                break;
            }
    }

    return ret;
}


Uint Ac_AddPattern(ACHANDLE AcHandle, Byte *Pattern,
        Uint16 PattLen, Uint Pid)
{
    ac_pid_s stPid;
    ac_trie_s *Trie = (ac_trie_s *)AcHandle; 
    memset(&stPid, 0, sizeof(stPid));

    stPid.PattLen = PattLen;
    stPid.PidID = Pid;

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
    Uint ret;
    ac_batch_iterate_s Batch;
    ac_trie_s *pTrie;
    Byte *pBuff;

    pTrie = (ac_trie_s *)Handle;
    if (ERROR_SUCCESS != Ac_PreCompile(pTrie))
    {
        AC_PRINTF("ac_api, ac prepare compile failed \n");
        return ERROR_FAILED;
    }
    
    AC_PRINTF("ac_api, ac perpare compile success \n");
    pBuff = AC_MALLOC(AC_BUFF_MAX);
    if (NULL == pBuff)
    {
        return ERROR_MEMORY;
    }

    _Ac_InitBatchIterate(&Batch);
    for ( ret = _Ac_GetNextBatchInfo(pTrie, &Batch, pBuff, AC_BUFF_MAX); ret > 0;
            ret = _Ac_GetNextBatchInfo(pTrie, &Batch, pBuff, AC_BUFF_MAX))
    {
        if (ERROR_SUCCESS != _Ac_BatchProc(pTrie, pBuff, ret))
        {
            AC_PRINTF("ac compile failed \n");
            break;
        }
    }

    AC_FREE(pBuff);

    return ERROR_SUCCESS;
}

static void _Ac_RecordOnePid(ac_pid_s *pPid, Byte *pStart,
        Byte *pCursor, ac_full_result_s *pResult)
{
    Uint PidNum;
    Uint PidID;
    Uint Index;
    ac_hit_pid_s *HitPid;

    PidNum = pResult->PidNum;
    PidID = pPid->PidID;
    if (0 == PidNum)
    {
        HitPid = &pResult->HitPid[0];
        HitPid->PidID = pPid->PidID;
        HitPid->HitCount = 1;
        HitPid->pHitStart[0] = pStart;
        HitPid->pHitEnd[0] = pCursor;
        pResult->PidNum = 1;
        pResult->RecordCount = 1;
    }
    else
    {
        for (Index = 0; Index < PidNum; Index++)
        {
            HitPid = &pResult->HitPid[Index];
            if (PidID == HitPid->PidID)
            {
                if (AC_REPEAT_HIT_COUNT > HitPid->HitCount)
                {
                    HitPid->pHitStart[HitPid->HitCount] = pStart;
                    HitPid->pHitEnd[HitPid->HitCount] = pCursor;
                    HitPid->HitCount++;
                    pResult->RecordCount++;
                }

                break;
            }
        }

        if (Index == PidNum && PidNum != AC_HIT_NUM)
        {
            HitPid = &pResult->HitPid[PidNum];
            HitPid->PidID = PidID;
            HitPid->HitCount = 1;
            HitPid->pHitStart[0] = pStart;
            HitPid->pHitEnd[0] = pCursor;
            pResult->PidNum++;
            pResult->RecordCount++;
        }
    }

    return;
}


static void _Ac_RecordHit(ac_state_info_s *pStateInfo, Byte *pStart,
        Byte *pCursor, ac_full_result_s *pResult)
{
    Uint16 PattLen;
    Byte *HitStart;
    ac_pid_s *Pid;
    ac_pid_s *PidEnd;

    if (NULL == pStateInfo->pPidList)
    {
        return;
    }

    Pid = (ac_pid_s *)pStateInfo->pPidList;
    PidEnd = Pid + pStateInfo->PidNum;
    PattLen = (Uint16)(pCursor - pStart);
    for (;Pid < PidEnd;Pid++)
    {
        if (PattLen < Pid->PattLen)
        {
            HitStart = pStart;
        }
        else
        {
            HitStart = pCursor - Pid->PattLen;
            HitStart++;
        }

        _Ac_RecordOnePid(Pid, HitStart, pCursor, pResult);
    }

    return;
}
static void _Ac_FullSearch8(ac_trie_s *pTrie, Byte *pStart, 
        Uint uiLen, ac_full_result_s *pResult)
{
    //Uint FullStateMax;
    Byte *Cursor;
    Byte *End;
    Byte StateID;
    Byte BaseID;
    Byte Ascii;
    AC_STATE_ROW8 *StateRow;

    StateRow = (AC_STATE_ROW8 *)pTrie->pFullStateTable;
    if (NULL == StateRow)
    {
        return;
    }

    //FullStateMax = pTrie->FullStateNum;
    Cursor = pStart;
    End = pStart + uiLen;
    StateID = 0;

    do
    {
        Ascii = *Cursor;
        StateID = StateRow[StateID][Ascii];
        if (AC_GET_FLAG8(StateID))
        {
            BaseID = AC_GET_ID8(StateID);
            _Ac_RecordHit(&pTrie->pStateInfo[BaseID],pStart,Cursor,pResult);
            StateID = BaseID;
        }

        Cursor++;
    }while(Cursor != End);

    return;
}

static void _Ac_FullSearch16ex(ac_trie_s *pTrie, Byte *pStart,
        Uint uiLen, ac_full_result_s *pResult)
{
    //Uint FullStateMax;
    Uint16 StateID;
    Uint16 BaseID;
    Uint16 AcceptNum;
    Byte *Cursor;
    Byte *End;
    Byte Ascii;
    AC_STATE_ROW16 *StateRow;

    StateRow = (AC_STATE_ROW16 *)pTrie->pFullStateTable;
    if (NULL == StateRow)
    {
        return;
    }

    AcceptNum = pTrie->AcceptNum;
    //FullStateMax = pTrie->FullStateNum;
    Cursor = pStart;
    End = pStart + uiLen;
    StateID = 0;

    do
    {
        Ascii = *Cursor;
        StateID = StateRow[StateID][Ascii];
        if (AcceptNum <= StateID)
        {
            BaseID = AC_GET_ID16(StateID);
            _Ac_RecordHit(&pTrie->pStateInfo[BaseID],pStart, Cursor, pResult);
            StateID = BaseID;
        }

        Cursor++;
    }while(Cursor != End);

    return;

}

static void _Ac_FullSearch16(ac_trie_s *pTrie, Byte *pStart,
        Uint uiLen, ac_full_result_s *pResult)
{
    //Uint FullStateMax;
    Uint16 StateID;
    Uint16 BaseID;
    Byte *Cursor;
    Byte *End;
    Byte Ascii;
    AC_STATE_ROW16 *StateRow;

    StateRow = (AC_STATE_ROW16 *)pTrie->pFullStateTable;
    if (NULL == StateRow)
    {
        return;
    }
    //FullStateMax = pTrie->FullStateNum;
    Cursor = pStart;
    End = pStart + uiLen;
    StateID = 0;
    do
    {
        Ascii = *Cursor;
        StateID = StateRow[StateID][Ascii];
        if (AC_GET_FLAG16(StateID))
        {
            BaseID = AC_GET_ID16(StateID);
            _Ac_RecordHit(&pTrie->pStateInfo[BaseID],pStart, Cursor, pResult);
            StateID = BaseID;
        }

        Cursor++;
    }while(Cursor != End);

    return;
}

void Ac_FullSearch(ACHANDLE Handle, Byte *pStart, Uint uiLen, 
        ac_full_result_s *pResult)
{
    ac_trie_s *Trie;
    
    Trie = (ac_trie_s *)Handle;

    if (0 == uiLen)
    {
        return;
    }

    if (Trie->StateNum > AC_STATE_65535)
    {
        AC_PRINTF("trie state number %d\n", Trie->StateNum);
        // _Ac_FullSearch32();
        return;
    }
    else if (Trie->StateNum > AC_STATE_32767)
    {
        AC_PRINTF("trie state number %d\n", Trie->StateNum);
        _Ac_FullSearch16ex(Trie, pStart, uiLen, pResult);
    }
    else if (Trie->StateNum > AC_STATE_127)
    {
        _Ac_FullSearch16(Trie, pStart, uiLen, pResult);
    }
    else
    {
        _Ac_FullSearch8(Trie, pStart, uiLen, pResult);
    }

    return;
}

void Ac_DestroyTrie(ACHANDLE Handle)
{
    ac_trie_s *pTrie;

    pTrie = (ac_trie_s *)Handle;

    Ac_FreeResource(pTrie);

    return;
}
