#include <stdlib.h>
#include "queue.h"
#include "debug.h"


/*
 * queue.c
 */

/*
 * Queue is full
 */
int Queue_IsFull(Queue *Q)
{
    return Q->Size == Q->Capacity;
}


/*
 * Queue is empty
 */
int Queue_IsEmpyt(Queue *Q)
{
    return Q->Size == 0;
}


/*
 * Queue dequeue
 */
ElementType Queue_Dequeue(Queue *Q)
{
    ElementType t;

    if (Queue_IsEmpty(Q))
        return 0;

    Q->Size--;
    t = Q->Array[Q->Front];
    if (++Q->Front == Q->Capacity)
        Q->Front = 0;

    return t;
}


/*
 * Queue front
 */
ElementType Queue_Front(Queue *Q)
{
    if (Queue_IsEmpty(Q))
        return 0;

    return Q->Array[Q->Front];
}

/*
 * Queue dequeue
 */
void Queue_Enquee(ElementType x, Queue *Q)
{
    if (Queue_IsFull(Q))
        return;

    Q->Size++;
    if (++Q->Rear == Q->Capacity)
        Q->Rear = 0;
    Q->Array[Q->Rear] = x;
}


/*
 * Create queue
 */
Queue *Create_Queue( int Max)
{
    Queue *Q;

    if (QUEUE_MINSIZE > Max)
    {
        Trace("create_queue:size too small");
        return NULL;
    }

    Q = (Queue *)malloc(sizeof(ElementType)*(MAX -1) + sizeof(Queue));
    if (NULL == Q)
    {
        Trace("create_queue: out of memory");
        return NULL;
    }

    Q->Capacity = Max;
    Q->Size = 0;
    Q->Front = 1;
    Q->Rear = 0;

    return Q;
}


/*
 * Queue destroy
 */
void Queue_Destroy(Queue *Q)
{
    if (NULL != Q)
    {
        free(Q);
    }
}
