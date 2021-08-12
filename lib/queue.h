#ifndef _QUEUE_H_

#define _QUEUE_H_

#define QUEUE_MINSIZE 5
#define QUEUE_INVALID (~0UL)
typedef unsigned long ElementType;

typedef struct Queue
{
    int Capacity;
    int Front;
    int Rear;
    int Size;
    ElementType Array[1];
}Queue;

Queue *Queue_Create(int max);
void Queue_Destroy(Queue *Q);
int Queue_IsFull(Queue *Q);
int Queue_IsEmpty(Queue *Q);
void Queue_Enqueue(ElementType x, Queue *Q);
ElementType Queue_Dequeue(Queue *Q);
ElementType Queue_Front(Queue *Q);


#endif
