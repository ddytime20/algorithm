/*
 * 
 *  Doubly linked list
 *
 */

#ifndef __DLIST_H__
#define __DLIST_H__

EXTERN_C_BEGIN

typedef struct doub_list_node
{
    struct doub_list_node *next;
    struct doub_list_node *prev;
}dlist_node_s;

typedef struct doub_list_node dlist_head_s;


#define DLIST_HEAD(head) dlist_head_t head = {&(head), &(head)}

#define DLIST_ENTRY(node, type, member) \
    container_of(node, type, member)

/*
 * Init doubly linked list head
 */
static inline void INIT_DLIST_HEAD(dlist_head_s *head)
{
    head->next = head;
    head->prev = head;
}

static inline void dlist_node_init(dlist_node_s *node)
{
    node->next = (dlist_node_s *)NULL;
    node->prev = (dlist_node_s *)NULL;
}

static inline dlist_node_s *dlist_next(dlist_node_s *node)
{
    return node->next;
}

static inline dlist_node_s *dlist_prev(dlist_node_s *node)
{
    return node->prev;
}

static inline Uint dlist_isend(dlist_head_s *head, dlist_node_s *node)
{
    if (NULL == node)
    {
        return True;
    }
    return (head == node->next);
}
/*
 * Add a list node to the beginning of the list
 */
static inline void dlist_add_head(dlist_node_s *node, dlist_head_s *head)
{
    node->next = head->next;
    node->prev = head;
    head->next = node;
    node->next->prev = node;
}

/*
 * Add a list node to the end of the list
 */
static inline void dlist_add_tail(dlist_node_s *node, dlist_head_s *head)
{
    node->next = head;
    node->prev = head->prev;
    head->prev = node;
    node->prev->next = node;
}

/*
 * Remove a node from the list
 */
static inline void dlist_del(dlist_node_s *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
}

static inline int dlist_isempty(dlist_head_s *head)
{
    return (head->next == head);
}


/*
 * Macro for walk the doubly linked list
 */

#define DLIST_NEXT_ENTRY(list, entry, member) \
    ((dlist_isend(list, &((entry)->member))) ? \
    NULL : \
    DLIST_ENTRY(dlist_next(&((entry)->member)),typeof(*(entry)), member))

/*
 * Itetate over the list
 * The nodes and the head of the list must must be kept unmodified while
 * iterating through it. Any modifications to the the list will cause undefined
 * behavior.
 */
#define DLIST_FOREACH(list, node) \
    for ((node) = (list)->next; (node) != (list); (node) = (node)->next)

/*
 * Iterate over list nodes and allow deletes
 * The current node (iterator) is allowed to be removed from the list. Any
 * other modifications to the the list will cause undefined behavior.
 */
#define DLIST_FOREACH_SAFE(list, node, nextnode) \
    for (((node) = (list)->next, (nextnode) = (node)->next); \
        (node) != (list); \
        ((node) = (nextnode), (nextnode) = (nextnode)->next))


#define DLIST_FOREACH_REVERSE(list, node) \
    for ((node) = (list)->prev; (node) != (list); (node) = (node)->prev)

#define DLIST_FOREACH_REVERSE_SAFE(list, node, prevnode) \
    for (((node) = (list)->prev, (prevnode) = (node)->prev); \
        (node) != (list); \
        ((node) = (prevnode), (prevnode) = (prevnode)->prev))

/*
 * Iterate over list entries 
 * The nodes and the head of the list must must be kept unmodified while
 * iterating through it. Any modifications to the the list will cause undefined
 * behavior
 */
#define DLIST_FOREACH_ENTRY(list, entry, member) \
    for ((entry) = DLIST_ENTRY((list)->next, typeof(*(entry)), member); \
        &(entry)->member != list; \
        (entry) = DLIST_ENTRY((entry)->member.next, typeof(*(entry)), member))

/*
 * Iterate over list entries and allow deletes
 * The current node (iterator) is allowed to be removed from the list. Any
 * other modifications to the the list will cause undefined behavior.
 */
#define DLIST_FOREACH_ENTRY_SAFE(list, entry, nextentry, member) \
    for (((entry) = DLIST_ENTRY((list)->next, typeof(*(entry)), member), \
        (nextentry) = DLIST_ENTRY((entry)->member.next, typeof(*(entry)), member)); \
        &(entry)->member != list; \
        ((entry) = (nextentry), (nextentry) = DLIST_ENTRY((nextentry)->member.next, typeof(*(entry)), member)))


#define DLIST_FOREACHE_ENTRY_REVERSE(list, entry, member) \
    for ((entry) = DLIST_ENTRY((list)->prev, typeof(*(entry)), member); \
        &(entry)->member != list; \
        (entry) = DLIST_ENTRY((entry)->member.prev, typeof(*(entry)), member))


#define DLIST_FOREACH_ENTRY_REVERSE_SAFE(list, entry, preventry, member) \
    for (((entry) = DLIST_ENTRY((list)->prev, typeof(*(entry)), member), \
        (preventry) = DLIST_ENTRY((entry)->member.prev, typeof(*(entry)), member); \
        &(entry)->member != list; \
        ((entry) = (preventry), (preventry) = DLIST_ENTRY((preventry)->member.prev, typeof(*entry)), member)))



EXTERN_C_END

#endif
