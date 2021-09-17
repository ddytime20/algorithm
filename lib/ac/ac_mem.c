/*
 *
 * AC MEMORY
 *
 */
#include <stdlib.h>
#include <string.h>
#include "basetype.h"
#include "ac_mem.h"
#include "debug.h"

/*
 * Enlarge ac memory pool
 */
static Uint _EnlargeMemPool(ac_mem_pool_s *pstMemPool)
{
    Ulong ulPoolLen;
    void *pPool;
    Byte **pPoolList;

    ulPoolLen = AC_POOL_MAX * pstMemPool->NodeSize;
    pPool = AC_MALLOC(ulPoolLen);
    if (!pPool)
    {
        return ERROR_MEM;
    }
    memset(pPool, 0, ulPoolLen);

    pPoolList = AC_REALLOC(pstMemPool->PoolList, (sizeof(Byte *) * (pstMemPool->PoolNum + 1)));
    if (NULL == pPoolList)
    {
        AC_PRINTF("ac_mem, realloc pool list failed \n");
        AC_FREE(pPool);
        return ERROR_MEM;
    }
    pstMemPool->PoolList = pPoolList;
    pstMemPool->PoolList[pstMemPool->PoolNum] = pPool;
    pstMemPool->PoolNum++;
    pstMemPool->NextIndex = 0;

    return ERROR_SUCCESS;
}

/*
 * AC get one node form memory pool
 */
void *Ac_MemPool_Malloc_Node(ac_mem_pool_s *pstMemPool, Uint *pNid)
{
    Uint uiPoolId;
    Uint uiOffset;
    Byte *pPool;

    if (AC_POOL_MAX == pstMemPool->PoolNum)
    {
        if (ERROR_SUCCESS != _EnlargeMemPool(pstMemPool))
        {
            AC_PRINTF("ac_mem, malloc node enlarge pool failed \n");
            return NULL;
        }
    }

    //printf("pool list %x, pool num %d, next index %d \n",pstMemPool->PoolList[0], pstMemPool->PoolNum, pstMemPool->NextIndex);
    uiPoolId = pstMemPool->PoolNum - 1;
    uiOffset = pstMemPool->NodeSize * pstMemPool->NextIndex;
    *pNid = (uiPoolId * AC_POOL_MAX) + pstMemPool->NextIndex;
    pstMemPool->NextIndex++;
    pPool = pstMemPool->PoolList[uiPoolId];

    //printf("ac_mem malloc node pool addr %x, offset %d\n", pPool, uiOffset);
    return (void *)(pPool + uiOffset);
}

/*
 * Get node by node id
 */
void *Ac_MemPool_GetNode(ac_mem_pool_s *pstMemPool, Uint uiNid)
{
    Uint uiPoolId;
    Uint uiIndex;
    Byte *pPool;

    uiPoolId = uiNid >> AC_POOL_SITE_MAX; 
    uiIndex = uiNid & (AC_POOL_MAX - 1);

    if (uiPoolId >= pstMemPool->PoolNum)
    {
        return NULL;
    }

    // check top of pool
    if (uiPoolId == (pstMemPool->PoolNum - 1))
    {
        if (uiIndex >= pstMemPool->NextIndex)
        {
            return NULL;
        }
    }

    pPool = pstMemPool->PoolList[uiPoolId];
    
    return (void *)(pPool + (uiIndex * pstMemPool->NodeSize));
}
/*
 * AC memory pool get all node number
 */
Uint Ac_MemPool_GetNodeNum(ac_mem_pool_s *pstMemPool)
{
    Uint Nume;

    Nume = AC_POOL_MAX * (pstMemPool->PoolNum - 1) + pstMemPool->NextIndex;

    return Nume;
}


/*
 * AC memory pool init
 */
Uint Ac_MemPool_Init(ac_mem_pool_s *pstMemPool, Uint Size)
{
    memset(pstMemPool, 0, sizeof(ac_mem_pool_s));
    pstMemPool->NodeSize = Size;

    return _EnlargeMemPool(pstMemPool);
}

/*
 * AC memory pool fini
 */
void Ac_MemPool_Fini(ac_mem_pool_s *pstMemPool)
{
    Uint uiLoop;
    void *pPool;

    if (pstMemPool->PoolList)
    {
        for (uiLoop = 0; uiLoop < pstMemPool->PoolNum; uiLoop++)
        {
            pPool = pstMemPool->PoolList[uiLoop];
            if (pPool)
            {
                AC_FREE(pPool);
            }
        }
        AC_FREE(pstMemPool->PoolList);
    }

    memset(pstMemPool, 0, sizeof(ac_mem_pool_s));

    return;
}
