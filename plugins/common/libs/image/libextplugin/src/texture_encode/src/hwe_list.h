#ifndef HWE_LIST_H
#define HWE_LIST_H

typedef void *(*HWE_GenericFunc)(void *funcArg);

typedef struct HWE_NodeType
{
    void *element;
    struct HWE_NodeType *next;
} HWE_Node;

typedef struct HWE_JobNodeType
{
    HWE_GenericFunc func;
    void *funcArg;
    struct HWE_JobNodeType *next;
} HWE_JobNode;

#define HWE_LIST_FREE_ALL(head, tail, nodeType) \
    do {                                        \   
        while (head) {                          \
            nodeType *tmpNode = (head);         \ 
            if ((tmpNode)->next) {              \
                (head) = tmpNode->next;         \ 
            } else {                            \
                (head) = NULL;                  \
            }                                   \ 
            HWE_Free(tmpNode);                  \
        }                                       \  
        (tail) = NULL;                          \
    } while (0)

#define HWE_LIST_PUSH_TAIL(head, tail, node) \
    do {                                     \ 
        if (!(tail)) {                       \
            (head) = (node);                 \ 
            (tail) = (node);                 \
        } else {                             \
            (tail)->next = (node);           \
            (tail) = (node);                 \
        }                                    \
    } while (0)

#define HWE_LIST_POP_HEAD(head, tail, node) \
    do {                                    \
        if (!(head)) {                      \
            (node) = NULL;                  \ 
        } else {                            \
            (node) = (head);                \ 
            if ((node)->next) {             \
                (head) = (node)->next;      \  
                (node)->next = NULL;        \
            } else {                        \
                (head) = NULL;              \
                (tail) = NULL;              \  
                (node)->next = NULL;        \ 
            }                               \
        }                                   \  
    } while (0)