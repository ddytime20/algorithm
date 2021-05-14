#ifndef _STACK_H_
#define _STACK_H_

// stack 数组实现

#ifndef STACK_CAPACITY_MAX
#define STACK_CAPACITY_MAX 200
#endif

#ifndef STACK_ELEMENT_TYPE
#define STACK_ELEMENT_TYPE unsigned long 
#endif

struct StackRecord
{
    int Capacity;
    int TopOfStack;
    STACK_ELEMENT_TYPE *pArrary;
};

#define STACK_EMPTY -1

#define STACK_CAPACITY(name) \
    STACK_ELEMENT_TYPE stack_capacity_##name[STACK_CAPACITY_MAX];

#define STACK_INIT(name) \
{   \
    .Capacity = STACK_CAPACITY_MAX, \
    .TopOfStack = STACK_EMPTY, \
    .pArrary = stack_capacity_##name, \
}

#define STACK_NAME(name) \
    STACK_CAPACITY(name) \
    struct StackRecord name = STACK_INIT(name)

static inline int StackIsFull(struct StackRecord *Stack)
{
    return (Stack->TopOfStack + 1 == Stack->Capacity);
}

static inline int StackIsEmpty(struct StackRecord *Stack)
{
    return Stack->TopOfStack == STACK_EMPTY;
}

static inline void StatckMakeEmpty(struct StackRecord *Stack)
{
    if (Stack)
        Stack->TopOfStack = STACK_EMPTY;
}

static inline void StackFree(struct StackRecord *Stack)
{
    if (Stack){
        if(Stack->pArrary)
            free(Stack->pArrary);
        free(Stack);
    }
}

static inline struct StackRecord *CreateStack(int ElementNum)
{
    struct StackRecord *Stack;
    Stack = malloc(sizeof(struct StackRecord));
    if (NULL == Stack)
    {
        printf("no enough space\n");
        return Stack;
    }
    Stack->pArrary = malloc(ElementNum * sizeof(STACK_ELEMENT_TYPE));
    if (NULL == Stack->pArrary)
    {
        printf("no enough space\n");
        StackFree(Stack);
        return NULL;
    }
    Stack->TopOfStack = STACK_EMPTY;
    Stack->Capacity = ElementNum;
    
    return Stack;
}

static inline STACK_ELEMENT_TYPE StackTop(struct StackRecord *Stack)
{
    if(!StackIsEmpty(Stack))
        return Stack->pArrary[Stack->TopOfStack];
    return 0;
}

static inline STACK_ELEMENT_TYPE StackPop(struct StackRecord *Stack)
{
    if(!StackIsEmpty(Stack))
        return Stack->pArrary[Stack->TopOfStack--];
    return 0;
}

static inline void StackPush(struct StackRecord *Stack, STACK_ELEMENT_TYPE element)
{
    if(!StackIsFull(Stack))
        Stack->pArrary[++Stack->TopOfStack] = element;
    else
        printf("Stack is Full\n");
}

#endif
