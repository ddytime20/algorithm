/*
 * 
 * AC MEM
 * 
 */

#ifndef __AC_MEM_H__
#define __AC_MEM_H__

EXTERN_C_BEGIN

typedef struct ac_mem_pool
{
    Byte **PoolList;     // Pool list
    Uint PoolNum;
    Uint NodeSize;
    Uint16 NextIndex;
}ac_mem_pool_s;

#define AC_POOL_SITE_MAX 0xaU
#define AC_POOL_MAX (0x1U << AC_POOL_SITE_MAX)

#define AC_MALLOC(size)  malloc(size)
#define AC_REALLOC(pdata, size) realloc(pdata, size)
#define AC_FREE(pdata)   free(pdata)


extern void *Ac_MemPoll_Malloc_Node(ac_mem_pool_s *pstMemPool, Uint *pNid);
extern void *Ac_MemPool_GetNode(ac_mem_pool_s *pstMemPool, Uint Nid);
extern Uint Ac_MemPool_GetNodeNum(ac_mem_pool_s *pstMemPool);
extern Uint Ac_MemPool_Init(ac_mem_pool_s *pstMemPool, Uint size);
extern void Ac_MemPool_Fini(ac_mem_pool_s *pstMemPool);


EXTERN_C_END

#endif
