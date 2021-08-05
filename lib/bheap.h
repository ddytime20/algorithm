#ifndef _B_HEAP_H_

#define _B_HEAP_H_

#define MinData (-32767)
typedef int et;

typedef struct heap{
    et *array;
    int size;
    int max;
}heap_t;

heap_t *Heap_Create(int max);
void Heap_Destroy(heap_t *h);
int Heap_IsFull(heap_t *h);
int Heap_IsEmpty(heap_t *h);
void Heap_Empty(heap_t *h);
void Heap_Insert(heap_t *h, et x);
et Heap_Delete(heap_t *h);
et Heap_FindMin(heap_t *h);


#endif

