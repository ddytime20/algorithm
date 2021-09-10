/*
 *
 * AC DATA
 *
 */

#ifndef __AC_DATA_H__
#define __AC_DATA_H__

EXTERN_C_BEGIN

#define AC_INVALID_FAIL_ID  0xFFFFFFFFUL
#define AC_GETROOTSTATE(pstMemPool)    Ac_MemPool_GetNode(pstMemPool, 0)
/*
 * Pattern id info
 */
typedef struct ac_pid
{
    Uint16 PattLen;
    Uint Pid;
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
    Uint FailedID;
    Uint PidNum;
    Byte PathNum;
    ac_tmp_path_s *PathList;
    ac_pid_s *PidList;
}ac_tmp_state_s;

/*
 * AC trie
 */
typedef struct ac_trie
{
    Uint PidSum;
    Uint StateNum;
    Byte bCompile;
    dlist_head_s StateList;
    ac_mem_pool_s *TmpPool;
}ac_trie_s;


extern ac_trie_s *Ac_CreateEmptyTrie(void);
extern Uint Ac_AddOnePatternToTrie(ac_trie_s *pTrie, Byte *pattern, ac_pid_s *pid);
extern Uint Ac_AddPid(ac_trie_s *pTrie, ac_pid_s *Pid, ac_tmp_state_s *State);

EXTERN_C_END

#endif

