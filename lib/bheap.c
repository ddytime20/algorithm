#include <stdlib.h>

#define MinSize (10)

/*
 * init heap
 */
static inline void heap_init(heap_t *h, int max)
{
    h->array = (tp *)malloc(sizeof(tp) * (max + 1));
    if (NULL == h->array)
        Error("heap_init: out of memeory");

    h->max = max;
    h->size = 0;
    h->array[0] = MinData;
}

/*
 * find samller tp in heap
 */
tp Heap_FindMin(heap_t *h)
{
    if (Heap_IsEmpty(h)){
        Trace("heap find min: heap is empty");
        return MinData;
    }

    return h->array[1];
}

/*
 * delete smaller tp from heap
 */
tp Heap_Delete(heap_t *h)
{
    int i, child;
    tp last, first;

    if (Heap_IsEmpty(h)) {
        Trace("heap_delete:heap is empty");
        return MinData;
    }

    first = h->array[1];
    last = h->array[h->size--];

    for (i = 1; i*2 < h->array[h->size]; i = child){
        //get child id; 
        child = i * 2;
        //not exist right child and right child small left
        if (child != h->size && h->array[child + 1] < h->array[child])
            child++;

        if (last > h->array[child])
            h->array[i] = h->array[child]; 
        else
            break;
    } 

    h->array[i] = last;
    return first;
}

/*
 * insert x to heap x;
 */
void Heap_Insert(heap_t *h, tp x)
{
    int i;

    if (Heap_IsFull(h)){
        Trace("heap_insert:heap is full");
        return;
    }

    //init array[0] is the smaller in tp
    for ( i = ++h->size; h->array[i/2] > x; i /=2)
        h->array[i] = h->array[i/2];
    h->array[i] = x;
}

/*
 * empty heap h;
 */
void Heap_Empty(heap_t *h)
{
    h->size = 0;
}

/*
 * get heap is empty or not;
 */
int Heap_IsEmpty(heap_t *h)
{
    return h->size == 0;
}

/*
 * get heap is full or not ;
 */
int Heap_IsFull(heap_t *h)
{
    return h->size == h->max;
}

/*
 * destror heap;
 */
void Heap_Destroy(heap_t *h)
{
    if (NULL != h->array)
        free(h->array);
    free(h);
}


/*
 * create new heap;
 */
heap_t *Heap_Create(int max)
{
    heap_t *h;

    if (max < MinSize){
        Trace("heap_create: max too small");
        return NULL;
    }

    h = (heap_t *)malloc(sizeof(heap_t));
    if (NULL == h)
        Error("heap_t: out of memory");
    heap_init(h, max);

    return h;
}
