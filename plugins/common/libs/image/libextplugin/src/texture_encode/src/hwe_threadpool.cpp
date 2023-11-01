#include "hwe_threadpool.h"
#include <stdio.h>


static void *HWE_ThreadPoolWorkerFunc(void *pool)
{
    if (!pool) {
        return NULL;
    }

    HWE_ThreadPool *threadPool = (HWE_ThreadPool *)pool;
    if (threadPool->initFunc) {
        threadPool->initFunc(threadPool->initArg);
    }

    HWE_ThreadPoolJobQueue *jobQ = &threadPool->jobQueue;
    HWE_JobNode *jobNode = NULL;

    HWE_PthreadMutexLock(&jobQ->lock);
    while (1) {
        while ((!jobQ->actualCount) &&
            (!threadPool->shutdown)) {
            HWE_PthreadCondWait(&jobQ->notEmpty, &jobQ->lock);
        }

        if (threadPool->shutdown) {
            HWE_PthreadMutexUnLock(&jobQ->lock);
            break;
        }

        HWE_LIST_POP_HEAD(jobQ->head, jobQ->tail, jobNode);
        if (jobNode != NULL) {
            jobQ->actualCount--;
        } else {
            jobQ->actualCount = 0;
            HWE_PthreadCondBroadcast(&jobQ->empty);
            continue;
        }

        if (!jobQ->actualCount) {
            HWE_PthreadCondBroadcast(&jobQ->empty);
        }

        threadPool->busyThreadCount++;
        HWE_PthreadMutexUnLock(&jobQ->lock);

        (void)jobNode->func(jobNode->funcArg);
        HWE_Free(jobNode);
        jobNode = NULL;

        HWE_PthreadMutexLock(&jobQ->lock);
        threadPool->busyThreadCount--;

        if (!threadPool->busyThreadCount) {
            HWE_PthreadCondBroadcast(&jobQ->notBusy);
        }
    }

    return NULL;
}

static void HWE_DestroyThreads(HWE_ThreadPool *pool)
{
    if (!pool) {
        return;
    }

    HWE_ThreadPoolJobQueue *jobQ = &pool->jobQueue;

    HWE_PthreadMutexLock(&jobQ->lock);
    if (pool->shutdown || jobQ->shutdown) {
        HWE_PthreadMutexUnLock(&jobQ->lock);
        return;
    }

    jobQ->shutdown = 1;
    while (jobQ->actualCount) {
        HWE_PthreadCondWait(&jobQ->empty, &jobQ->lock);
    }

    pool->shutdown = 1;
    HWE_PthreadMutexUnLock(&jobQ->lock);
    HWE_PthreadCondBroadcast(&jobQ->notEmpty);

    HW_S32 i;
    for (i = 0; i < pool->threadCount; i++) {
        HWE_PthreadJoin(pool->pthreads[i]);
    }

    return;
}


HWE_ThreadPool *HWE_CreateThreadPool(HW_S32 threadCount, HWE_ThreadPoolParam *threadParam)
{
    if (threadCount <= 0) {
        return NULL;
    }

    HWE_ThreadPool *pool = (HWE_ThreadPool *)HWE_Malloc(sizeof(HWE_ThreadPool));
    if (!pool) {
        return NULL;
    }

    HWE_ThreadPoolJobQueue *jobQ = &pool->jobQueue;
    jobQ->actualCount = jobQ->shutdown = 0;
    jobQ->head = jobQ->tail = NULL;

    pool->threadCount = pool->busyThreadCount = pool->shutdown = 0;
    pool->pthreads = (HWE_Pthread *)HWE_Malloc(sizeof(HWE_Pthread) * threadCount);
    if (!pool->pthreads) {
        HWE_Free(pool);
        return NULL;
    }

    pool->initArg = NULL;
    pool->initFunc = NULL;
    if (threadParam) {
        pool->initArg = threadParam->initArg;
        pool->initFunc = threadParam->initFunc;
    }

    HWE_PthreadMutexInit(&jobQ->lock);
    HWE_PthreadCondInit(&jobQ->notEmpty);
    HWE_PthreadCondInit(&jobQ->empty);
    HWE_PthreadCondInit(&jobQ->notBusy);

    for (HW_S32 i = 0; i < threadCount; i++) {
        HW_S32 ret = HWE_PthreadCreate(&(pool->pthreads[i]), HWE_ThreadPoolWorkerFunc, (void *)pool);
        if (ret) {
            HWE_DestroyThreadPool(pool);
            return NULL;
        } else {
            pool->threadCount++;
        }
#if !HWE_IANDROID_ARM
        if (threadParam) {
            HWE_SetThreadAffinityMask(&(pool->pthreads[i]), threadParam->cpuNum, threadParam->cpuIdxArray);
            HWE_SetThreadPriority(&(pool->pthreads[i]), threadParam->schedPriority);
        }
#endif
    }

    return pool;
}


void HWE_ThreadPoolPushJob(HWE_ThreadPool *pool, HWE_ThreadFunc func, void *funcArg)
{
    if ((!pool) || (!func)) {
        return;
    }

    HWE_ThreadPoolJobQueue *jobQ = &pool->jobQueue;

    HWE_PthreadMutexLock(&jobQ->lock);
    if (pool->shutdown || jobQ->shutdown) {
        HWE_PthreadMutexUnLock(&jobQ->lock);
        return;
    }

    HWE_JobNode *jobNode = (HWE_JobNode *)HWE_Malloc(sizeof(HWE_JobNode));
    if (!jobNode) {
        HWE_PthreadMutexUnLock(&jobQ->lock);
        return;
    }

    jobNode->func = func;
    jobNode->funcArg = funcArg;
    jobNode->next = NULL;
    HWE_LIST_PUSH_TAIL(jobQ->head, jobQ->tail, jobNode);
    HWE_PthreadCondBroadcast(&jobQ->notEmpty);
    jobQ->actualCount++;
    HWE_PthreadMutexUnLock(&jobQ->lock);
    return;
}


void HWE_ThreadPoolWaitAllJobDone(HWE_ThreadPool *pool)
{
    if (!pool) {
        return;
    }

    HWE_ThreadPoolJobQueue *jobQ = &pool->jobQueue;

    HWE_PthreadMutexLock(&jobQ->lock);
    while (jobQ->actualCount) {
        HWE_PthreadCondWait(&jobQ->empty, &jobQ->lock);
    }

    while (pool->busyThreadCount) {
        HWE_PthreadCondWait(&jobQ->notBusy, &jobQ->lock);
    }
    
    HWE_PthreadMutexUnLock(&jobQ->lock);
    return;
}


void HWE_DestroyThreadPool(HWE_ThreadPool *pool)
{
    if (!pool) {
        return;
    }

    HWE_ThreadPoolJobQueue *jobQ = &pool->jobQueue;

    HWE_DestroyThreads(pool);
    HWE_PthreadCondDestroy(&jobQ->notEmpty);
    HWE_PthreadCondDestroy(&jobQ->empty);
    HWE_PthreadCondDestroy(&jobQ->notBusy);
    HWE_PthreadMutexDestroy(&jobQ->lock);
    HWE_LIST_FREE_ALL(jobQ->head, jobQ->tail, HWE_JobNode);
    HWE_Free(pool->pthreads);
    HWE_Free(pool);
    return;
}
