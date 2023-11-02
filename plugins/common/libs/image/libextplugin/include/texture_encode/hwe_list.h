/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HWE_LIST_H
#define HWE_LIST_H

// generic node function pointer type for queue
typedef void *(*HWE_GenericFunc)(void *funcArg);

// generic node struct type
typedef struct HWE_NodeType {
    void *element;             //next data pointer
    struct HWE_NodeType *next; //next node pointer
} HWE_Node;

// job node struct type
typedef struct HWE_JobNodeType {
    HWE_GenericFunc func;         //job node function
    void *funcArg;                // the argument of job node function
    struct HWE_JobNodeType *next; // next job node pointer
} HWE_JobNode;

/*
 * free all nodes of list
 * nodeType: node type name, e.g. HWE_Node, HWE_JobNode
 */
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

/*
 * push node to tail of list
 * macro don't allocate node memory, caller is responsible for node memory allocation
 */
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

/*
 * push node from tail of list
 * macro don't free node memory, caller is responsible for node memory free
 */
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
#endif