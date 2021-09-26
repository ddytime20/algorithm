/*
 *
 * AC DATA
 *
 */

#ifndef __AC_DATA_H__
#define __AC_DATA_H__

EXTERN_C_BEGIN

#define AC_INVALID_FAIL_ID             0xFFFFFFFFUL
#define AC_GETROOTSTATE(pstMemPool)    Ac_MemPool_GetNode(pstMemPool, 0)

#define AC_STATE_65535                 65535UL
#define AC_STATE_32767                 32767UL
#define AC_STATE_127                   127UL

#define AC_BUFF_MAX                    8192UL

#define AC_ASCII_NUM                   256UL

#define AC_GET_ID8(StateID)           ((StateID) & 0x7F)   
#define AC_SET_FLAG8(StateID)         ((StateID) | 0x80)
#define AC_GET_FLAG8(StateID)         ((StateID) & 0x80)

#define AC_GET_ID16(StateID)          ((StateID) & 0x7FFF)
#define AC_SET_FLAG16(StateID)        ((StateID) | 0x8000)
#define AC_GET_FLAG16(StateID)        ((StateID) & 0x8000)

#define AC_GET_ID(StateID)            ((StateID) & 0x7FFFFFFF)
#define AC_SET_FLAG(StateID)          ((StateID) | 0x80000000)
#define AC_GET_FLAG(StateID)          ((StateID) & 0x80000000)

typedef Uint   AC_STATE_ROW32[AC_ASCII_NUM];
typedef Uint16 AC_STATE_ROW16[AC_ASCII_NUM];
typedef Byte   AC_STATE_ROW8[AC_ASCII_NUM]; 


/*
 * Pattern id info
 */
typedef struct ac_pid
{
    Uint16 PattLen;
    Uint PidID;
}ac_pid_s;

/*
 * AC path node
 */
typedef struct ac_tmp_path
{
    Uint  ChildID;
    Byte  Ascii;
}ac_tmp_path_s;

/*
 * AC state node
 */
typedef struct ac_tmp_state
{
    dlist_node_s Node;
    Uint StateID;
    Uint FailStateID;
    Uint PidNum;
    Byte PathNum;
    ac_tmp_path_s *PathList;
    ac_pid_s *PidList;
}ac_tmp_state_s;

typedef struct ac_state_info
{
    void *pPidList;
    Uint FailStateID;
    Uint PidNum;
}ac_state_info_s;

/*
 * AC trie
 */
typedef struct ac_trie
{
    Uint PidNum;
    Uint StateNum;
    Uint NextIndex;
    Byte bCompile;

    Uint FullStateNum;
    void *pFullStateTable;
    ac_state_info_s *pStateInfo;
    ac_pid_s *pPid;

    Uint *pNewState;
    dlist_head_s StateList;
    ac_mem_pool_s *pTmpPool;
}ac_trie_s;

typedef enum ac_msg_type
{
    AC_MSG_TREE = 0,
    AC_MSG_STATE,
    AC_MSG_PID,
    AC_MSG_FAILSTATE,
    AC_MSG_END
}ac_msg_type_e;

typedef struct ac_msg_tree
{
    Uint PidNum;
    Uint StateNum;
}ac_msg_tree_s;


/*
 * ac msg path
 */
typedef struct ac_msg_state
{
    Uint StateID;
    Uint SubStateID;
    Byte Ascii;
    Byte Recv[3];
}ac_msg_state_s;

typedef struct ac_msg_pid
{
    Uint StateID;
    Uint PidID;
    Uint PattLen;
}ac_msg_pid_s;

typedef struct ac_msg_failstate
{
    Uint StateID;
    Uint FailStateID;
}ac_msg_failstate_s;

/*
 * AC msg
 */
typedef struct ac_msg_head
{
    Byte MsgType;
    Byte Recv[3];
}ac_msg_head_s;

/*
 * AC batch
 */
typedef struct ac_batch_iterate
{
    Uint Batch[4];
}ac_batch_iterate_s;


extern ac_trie_s *Ac_CreateEmptyTrie(void);
extern Uint Ac_AddOnePatternToTrie(ac_trie_s *pTrie, Byte *pattern, ac_pid_s *pid);
extern Uint Ac_AddPid(ac_trie_s *pTrie, ac_pid_s *Pid, ac_tmp_state_s *State);
extern void Ac_FreeTmpState(ac_trie_s *pTrie);
extern void Ac_FreeResource(ac_trie_s *pTrie);

EXTERN_C_END

#endif

