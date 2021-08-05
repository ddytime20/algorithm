#include <stdlib.h>
#include "lheap.h"
#include "debug.h"


/*
 * leftist heap
 */
static inline swap_child(lheap_t *h)
{
    lnode_t *tmp;

    tmp = h->left;
    h->left = h->right;
    h->right = tmp;
}

/*
 * h1 smaller than h2, merge h1->right and h2
 */
static inline lheap_t *lheap_merge(lheap_t *h1, lheap_t *h2)
{
    // signal node, right node is null
    if (NULL == h1->left){
        h1->left = h2;
    } else {
        h1->right = Lheap_Merge(h1->right, h2);
        if (h1->left->npl < h1->right->npl)
            swap_child(h1);
        h1->npl = h1->right->npl + 1;
    }

    return h1;
}

/*
 * create leftist heap
 */
lheap_t *Lheap_Create(void)
{
    return NULL;
}


/*
 * get leftist heap is empty or not
 */
int Lheap_IsEmpty(lheap_t *h)
{
    return h == NULL;
}


/*
 * leftist heap merge
 */
lheap_t *Lheap_Merge(lheap_t *h1, lheap_t *h2)
{
    if (NULL == h1)
        return h2;

    if (NULL == h2)
        return h1;

    if (h1->element < h2->element){
        return lheap_merge(h1, h2);
    } else {
        return lheap_merge(h2, h1);
    }
}

lheap_t *Lheap_Insert1(et x, lheap_t *h)
{
    lnode_t *node;

    node = (lnode_t *)malloc(sizeof(lnode_t));
    if (NULL == node){
        Error("lheap_insert1: out of memory");
        return NULL;
    }

    node->npl = 0;
    node->element = x;
    node->left = node->right = NULL;

    return Lheap_Merge(node, h);
}

/*
 * delete min element from h
 */
lheap_t *Lheap_Delete1(lheap_t *h)
{
    lnode_t *left, *right;

    if (Lheap_IsEmpty(h)){
        Trace("lheap_delete:heap is empty");
        return h;
    }

    left = h->left;
    right = h->right;
    free(h);

    return Lheap_Merge(left, right);
}


/*
 * get the smaller element from h
 */
et Lheap_FindMin(lheap_t *h)
{
    return h->element;
}

void Lheap_Destroy(lheap_t *h)
{
    if (NULL == h)
        return;
    Lheap_Destroy(h->right);
    Lheap_Destroy(h->left);
    free(h);
}
