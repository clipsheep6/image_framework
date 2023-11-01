#ifndef HWE_THREADPOOL_H
#define HWE_THREADPOOL_H

#include "hwe_type.h"
#include "hwe_osdep.h"
#include "hwe_list.h"

typedef HWE_GenericFunc HWE_ThreadFunc;

typedef struct HWE_ThreadPoolJobQueueType {
    HWE_JobNode *head;
    HWE_JobNode *tail;
    HW_S32 actualCount;
    HWE_PthreadMutex lock;
    HWE_PthreadCond notEmpty;
    HWE_PthreadCond empty;
    HWE_PthreadCond notBusy;
    HW_S32 shutdown;
} HWE_ThreadPoolJobQueue;

typedef struct HWE_ThreadPoolType {
    HWE_ThreadPoolJobQueue jobQueue;
    HW_S32 threadCount;
    HW_S32 busyThreadCount;
    HWE_Pthread *pthreads;
    HW_S32 shutdown;
    void (*initFunc)(void *);
    void *initArg;
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
