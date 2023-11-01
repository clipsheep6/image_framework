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

#ifndef HWE_THREADPOOL_H
#define HWE_THREADPOOL_H

#include "hwe_type.h"
#include "hwe_osdep.h"
#include "hwe_list.h"

// caller job function pointer type waiting thread execution
typedef HWE_GenericFunc HWE_ThreadFunc;

typedef struct HWE_ThreadPoolJobQueueType {
    HWE_JobNode *head;        // list head of job queue
    HWE_JobNode *tail;        // list tail of job queue
    HW_S32 actualCount;       // actual job number
    HWE_PthreadMutex lock;    // mutex
    HWE_PthreadCond notEmpty; // job queue notEmpty condition
    HWE_PthreadCond empty;    // job queue empty condition
    HWE_PthreadCond notBusy;  // job queue notBusy condition
    HW_S32 shutdown;          // job queue shutdown control, job queue don't accept any job when job queue is shutdown
} HWE_ThreadPoolJobQueue;

typedef struct HWE_ThreadPoolType {
    HWE_ThreadPoolJobQueue jobQueue; // job queue pointer
    HW_S32 threadCount;              // thread number
    HW_S32 busyThreadCount;          // busy thread number
    HWE_Pthread *pthreads;           // thread id
    HW_S32 shutdown; // thread pool shutdown control, all threads will exit after complete all job in job queue when
                     // thread pool is shutdown 
    void (*initFunc)(void *); // init function
    void *initArg;            // init arg
} HWE_ThreadPool;

typedef struct HWE_ThreadPoolParamType {
    HW_U32 cpuNum;
    HW_U32 *cpuIdxArray;
    HW_S32 schedPriority;
    void (*initFunc)(void *);
    void *initArg;
} HWE_ThreadPoolParam;

#ifdef __cplusplus
extern "C" {
#endif

HWE_ThreadPool *HWE_CreateThreadPool(HW_S32 threadCount, HWE_ThreadPoolParam *threadParam);

void HWE_ThreadPoolPushJob(HWE_ThreadPool *pool, HWE_ThreadFunc func, void *funcArg);

void HWE_ThreadPoolWaitAllJobDone(HWE_ThreadPool *pool);

void HWE_DestroyThreadPool(HWE_ThreadPool *pool);

#ifdef __cplusplus
}
#endif

#endif
