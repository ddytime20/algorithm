#ifndef _BIN_QUEUE_H_

#define _BIN_QUEUE_H_

#define MAX_TREE (14) /* store 2^14 - 1 */
#define CAPACITY (16383) /* 2^14 - 1 */ 
#define INFINITY (20000L) /* MAX */
typedef int ET;

typedef struct BinNode
{
    ET Element;
    struct BinNode *LeftChild;
    struct BinNode *NextSibling;
}BinNode;

typedef struct BinQueue
{
    int Size;
    struct BinNode *Trees[MAX_TREE];
}BinQueue;

BinQueue *Bqueue_Create(void);
BinQueue *Bqueue_Merge(BinQueue *q1, BinQueue *q2);
BinQueue *Bqueue_Insert(ET x, BinQueue *q);
ET Bqueue_Delete(BinQueue *q);
ET Bqueue_FindMin(BinQueue *q);
void Bqueue_Destroy(BinQueue *q);
int Bqueue_IsFull(BinQueue *q);
int Bqueue_IsEmpty(BinQueue *q);


#endif
