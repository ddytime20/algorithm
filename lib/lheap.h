#ifndef _L_HEAP_H_

#define _L_HEAP_H_

typedef int et;

struct lheap_t {
    int npl;
    et  element;
    struct lheap_t left;
    struct lheap_t right;
};

typedef struct lheap_t lnode_t;

#define Lheap_Insert(x, h) (h = Lheap_Insert1((x), h))
#define Lheap_Delete(h) (h = Lheap_Delete1(h))

lheap_t * Lheap_Create(void);
lheap_t *Lheap_Insert1(et x, lheap_t *h);
lheap_t *Lheap_Delete1(lheap_t *h);
lheap_t *Lheap_Merge(lheap_t *h1, lheap_t *h2);
void Lheap_Destroy(lheap_t *h);
int Lheap_IsEmpty(lheap_t *h);
et Lheap_FindMin(lheap_t *h);


#endif
