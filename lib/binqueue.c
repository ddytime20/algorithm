#include <stdlib.h>
#include "binqueue.h"
#include "debug.h"

/*
 * binomail queue
 */


/*
 * Free tree node
 */
static inline void DestroyTree(BinNode *n)
{
    if (n){
        DestroyTree(n->LeftChild);
        DestroyTree(n->NextSibling);
        free(n);
    }
}


/*
 * Return the result of merging equal-sized n1 and n2
 */
static inline BinNode *CombineTree(BinNode *n1, BinNode *n2)
{
    if (n1->Element > n2->Element)
        return CombineTree(n2, n1);

    n2->NextSibling = n1->LeftChild;
    n1->LeftChild = n2;

    return n1;
}


/*
 * Create a new BinQueue
 */
BinQueue *Bqueue_Create(void)
{
    BinQueue *q;
    int i;

    q = (BinQueue *)malloc(sizeof(BinQueue));
    if (NULL == q){
        Trace("bqueue_create: out of memory");
        return NULL;
    }

    q->Size = 0;
    for (i = 0; i < MAX_TREE; i++){
        q->Trees[i] = NULL;
    }

    return q;
} 



/*
 * merge two binomial queue
 * q1 contains merge result
 * if merge success , q2 will free so q2 not in heap, only in stack
 */
BinQueue *Bqueue_Merge(BinQueue *q1, BinQueue *q2)
{
    BinNode *n1, *n2, *carry = NULL;
    int i, j;

    if (q1->Size + q2->Size > CAPACITY)
        Error("bqueue_merge:merge too big");

    q1->Size += q2->Size;
    for (i = 0, j = 1; j <= q1->Size; i++, j *= 2){
        n1 = q1->Trees[i];
        n2 = q2->Trees[i];
        switch(!!n1 + 2 * !!n2 + 4 * !!carry){
            case 0: /* no tree */
            case 1: /* only n1 */
                break;
            case 2: /* only n2 */
                q1->Trees[i] = n2;
                q2->Trees[i] = NULL;
                break;
            case 3: /* n1 and n2 */
                carry = CombineTree(n1, n2);
                q1->Trees[i] = NULL;
                q2->Trees[i] = NULL;
                break;
            case 4: /* only carry */
                q1->Trees[i] = carry;
                carry = NULL;
                break;
            case 5: /* n1 and carry */
                carry = CombineTree(n1, carry);
                q1->Trees[i] = NULL;
                break;
            case 6: /* n2 and carry */
                carry = CombineTree(n2, carry);
                q2->Trees[i] = NULL;
                break;
            case 7: /* all three */
                q1->Trees[i] = carry;
                carry = CombineTree(n1, n2);
                q2->Trees[i] = NULL;
                break;
        }
    }
    //q2 merge to q1, free q2
    free(q2);

    return q1;
}


/*
 * Insert a new ET x to BinQueue q
 */
BinQueue *Bqueue_Insert(ET x, BinQueue *q)
{
    BinQueue *qx;
    BinNode *node;

    node = (BinNode *)malloc(sizeof(BinNode)); 
    if (NULL == node){
        Trace("bqueue_insert: out of memory");
        return NULL;
    }

    node->Element = x;
    node->LeftChild = node->NextSibling = NULL;

    qx = Bqueue_Create();
    if (NULL == qx){
        Trace("bqueue_insert: bqueue create error");
        free(node);
        return NULL;
    }

    qx->Size = 1;
    qx->Trees[0] = node;

    return Bqueue_Merge(q, qx);
}


/*
 * Return q is empty
 */
int Bqueue_IsEmpty(BinQueue *q)
{
    return q->Size == 0;
}


/*
 * Return q is full
 */
int Bqueue_IsFull(BinQueue *q)
{
    return q->Size == CAPACITY;  
}


/*
 * Delete a min element from q
 * Return the min element
 */
ET Bqueue_Delete(BinQueue *q)
{
    ET MinItem;
    int i, MinTree;
    BinNode *DelNode, *OldRoot;
    BinQueue *Newq;

    if (Bqueue_IsEmpty(q)){
        Trace("bqueue_delete: q is empty");
        return -INFINITY;
    }

    MinItem = INFINITY;
    for (i = 0; i < MAX_TREE; i++){
        if (q->Trees[i] &&
                q->Trees[i]->Element < MinItem){
            MinItem = q->Trees[i]->Element;
            MinTree = i;
        }
    }

    Newq = Bqueue_Create();
    if (NULL == Newq){
        Trace("bqueue_delete: create new bqueue failed");
        return -INFINITY;
    }

    DelNode = q->Trees[MinTree];
    OldRoot = DelNode;
    DelNode = DelNode->LeftChild;
    free(OldRoot);    

    Newq->Size = (1 << MinTree) - 1;
    for (i = MinTree-1; i >= 0; i--){
        Newq->Trees[i] = DelNode;
        DelNode = DelNode->NextSibling;
        Newq->Trees[i]->NextSibling = NULL;
    }

    q->Trees[MinTree] = NULL;
    q->Size -= Newq->Size + 1; 

    Bqueue_Merge(q, Newq);

    return MinItem;
}



/*
 * Destroy the BinQueue q
 */
void Bqueue_Destroy(BinQueue *q)
{
    int i;

    for (i = 0; i < MAX_TREE; i++){
        DestroyTree(q->Trees[i]);        
    }

    free(q);

}



/*
 * Find the min element form q
 * Return the min element
 */
ET Bqueue_FindMin(BinQueue *q)
{
    ET MinItem;
    int i;
    
    if (Bqueue_IsEmpty(q)){
        Trace("bqueue_findmin: q is empty");
        return 0;
    }
    MinItem = CAPACITY;
    for (i = 0; i < MAX_TREE; i++){
        if (q->Trees[i] &&
                MinItem > q->Trees[i]->Element){
            MinItem = q->Trees[i]->Element;
        }
    }

    return MinItem;
}
