#ifndef HWE_OSDEP_H
#define HWE_OSDEP_H

#include "hwe_type.h"
#include <stdlib.h>
#include <limits.h>
#if defined(_WIN32)
#include <process.h>
#include <windows.h>
#include <time.h>
#else
#include <sched.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/prctl.h>
#endif
#include "hwe_source_record.h"

#define HWE_FILE __FILE__
#define HWE_LINE __LINE__

#if HWE_ARM_AARCH64
typedef enum HWE_Kernel {
    KERNEL_TYPE_C = 0,
    KERNEL_TYPE_NEON,
    KERNEL_TYPE_TOTAL
} HWE_KernelType;
#elif HWE_ARM_AARCH32
typedef enum HWE_Kernel {
    KERNEL_TYPE_C = 0,
    KERNEL_TYPE_NEON,
    KERNEL_TYPE_TOTAL
} HWE_KernelType;
#elif HWE_X86_64
typedef enum HWE_Kernel {
    KERNEL_TYPE_C = 0,
    KERNEL_TYPE_ASM,
    KERNEL_TYPE_TOTAL,
    KERNEL_TYPE_SSE2,
    KERNEL_TYPE_SSE4,
    KERNEL_TYPE_AVX,
    KERNEL_TYPE_AVX2
} HWE_KernelType;
#elif HWE_X86_32
typedef enum HWE_Kernel {
    KERNEL_TYPE_C = 0,
    KERNEL_TYPE_INTRINSICS,
    KERNEL_TYPE_TOTAL
} HWE_KernelType;
#else
typedef enum HWE_Kernel {
    KERNEL_TYPE_C = 0,
    KERNEL_TYPE_ASM,
    KERNEL_TYPE_TOTAL
} HWE_KernelType;
#endif

HWE_KernelType HWE_DetectSimdCapibility(void);

#ifdef __ICL
#define inline __inline
#endif

#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define HWE_INLINE __attribute__((always_inline)) inline
#else
#ifdef __ICL
#define HWE_INLINE __forceinline
#else
#define HWE_INLINE __inline
#endif
#endif


#if defined(_WIN32)
#define HWE_SPIN_COUNT 0
#define HWE_PTHREAD_MUTEX_INITIALIZER \
    {                                 \
        0                             \
    }

typedef CRITICAL_SECTION HWE_PthreadMutex;
typedef struct HWE_PthreadCond {
    void *ptr;
} HWE_PthreadCond;
typedef struct HWE_Pthread {
    void *handle;
    void *(*func)(void *arg);
    void *arg;
    void *ret;
    HW_B8 isInit;
} HWE_Pthread;
typedef struct HWE_Win32ThreadControl {
    HWE_PthreadMutex staticMutex;
    void(WINAPI *condBroadcast)(HWE_PthreadCond *cond);
    void(WINAPI *condInit)(HWE_PthreadCond *cond);
    void(WINAPI *condSignal)(HWE_PthreadCond *cond);
    BOOL(WINAPI *condWait)(HWE_PthreadCond *cond, HWE_PthreadMutex *mutex, DWORD milliseconds);
} HWE_Win32ThreadControl;
typedef struct HWE_Win32Cond {
    HWE_PthreadMutex mtxBroadcast;
    HWE_PthreadMutex mtxWaiterCount;
    volatile HW_S32 waiterCount;
    HANDLE semaphore;
    HANDLE waitersDone;
    volatile HW_S32 isBroadcast;
} HWE_Win32Cond;

HW_S32 HWE_PthreadMutexInit(HWE_PthreadMutex *mutex);
HW_S32 HWE_PthreadMutexLock(HWE_PthreadMutex *mutex);
HW_S32 HWE_PthreadMutexUnLock(HWE_PthreadMutex *mutex);
HW_S32 HWE_PthreadMutexDestroy(HWE_PthreadMutex *mutex);
HW_S32 HWE_PthreadCondInit(HWE_PthreadCond *cond);
HW_S32 HWE_PthreadCondDestroy(HWE_PthreadCond *cond);
HW_S32 HWE_PthreadCondWait(HWE_PthreadCond *cond, HWE_PthreadMutex *mutex);
HW_S32 HWE_PthreadCondSignal(HWE_PthreadCond *cond);
HW_S32 HWE_PthreadCondBroadcast(HWE_PthreadCond *cond);
HW_S32 HWE_PthreadCreate(HWE_Pthread *thread, void *(*func)(void *), void *arg);
HW_S32 HWE_PthreadJoin(HWE_Pthread thread);
#else
typedef struct HWE_PthreadMutexType {
    HW_B8 isInit;
    pthread_mutex_t mutex;
} HWE_PthreadMutex;

typedef struct HWE_PthreadCondType {
    HW_B8 isInit;
    pthread_cond_t cond;
} HWE_PthreadCond;

typedef struct HWE_PthreadType {
    HW_B8 isInit;
    pthread_t thread;
} HWE_Pthread;

static HWE_INLINE HW_S32 HWE_PthreadMutexInit(HWE_PthreadMutex *mutex)
{
    HW_S32 ret = pthread_mutex_init(&mutex->mutex, NULL);
    mutex->isInit = (ret == 0) ? TRUE : FALSE;
    if (mutex->isInit) {
        RecordInitMutexCount();
    }
    return ret;
}

static HWE_INLINE HW_S32 HWE_PthreadMutexLock(HWE_PthreadMutex *mutex)
{
    return mutex->isInit ? pthread_mutex_lock(&mutex->mutex) : -1;
}

static HWE_INLINE HW_S32 HWE_PthreadMutexUnLock(HWE_PthreadMutex *mutex)
{
    return mutex->isInit ? pthread_mutex_unlock(&mutex->mutex) : -1;
}

static HWE_INLINE HW_S32 HWE_PthreadMutexDestroy(HWE_PthreadMutex *mutex)
{
    if (mutex->isInit) {
        mutex->isInit = 0;
        RecordDestoryMutexCount();
        return pthread_mutex_destroy(&mutex->mutex);
    } else {
        return -1;
    }
}

static HWE_INLINE HW_S32 HWE_PthreadCondInit(HWE_PthreadCond *cond)
{
    HW_S32 ret = pthread_cond_init(&cond->cond, NULL);
    cond->isInit = (ret == 0) ? TRUE : FALSE;
    if (cond->isInit) {
        RecordInitCondCount();
    }
    return ret;
}

static HWE_INLINE HW_S32 HWE_PthreadCondDestroy(HWE_PthreadCond *cond)
{
    if (cond->isInit) {
        cond->isInit = 0;
        RecordDestoryCondCount();
        return pthread_cond_destroy(&cond->cond);
    } else {
        return -1;
    }
}

static HWE_INLINE HW_S32 HWE_PthreadCondWait(HWE_PthreadCond *cond, HWE_PthreadMutex *mutex)
{
    return (cond->isInit && mutex->isInit) ? pthread_cond_wait(&cond->cond, &mutex->mutex) : -1;
}

static HWE_INLINE HW_S32 HWE_PthreadCondSignal(HWE_PthreadCond *cond)
{
    return cond->isInit ? pthread_cond_signal(&cond->cond) : -1;
}

static HWE_INLINE HW_S32 HWE_PthreadCondBroadcast(HWE_PthreadCond *cond)
{
    return cond->isInit ? pthread_cond_broadcast(&cond->cond) : -1;
}

static HWE_INLINE HW_S32 HWE_PthreadCreate(HWE_Pthread *thread, void *(*func)(void *), void *arg)
{
    HW_S32 ret = pthread_create(&thread->thread, NULL, func, arg);
    thread->isInit = (ret == 0) ? TRUE : FALSE;
    if (thread->isInit) {
        RecordInitThreadCount();
    }
    return ret;
}

static HWE_INLINE HW_S32 HWE_PthreadJoin(HWE_Pthread thread)
{
    if (thread.isInit) {
        thread.isInit = 0;
        RecordDestoryThreadCount();
        return pthread_join(thread.thread, NULL);
    } else {
        return -1;
    }
}

#endif

HW_S32 HWE_SetThreadAffinityMask(const HWE_Pthread *thread, HW_U32 cpuNum, const HW_U32 *cpuIdxArray);
HW_S32 HWE_SetThreadPriority(const HWE_Pthread *thread, HW_S32 schedPriority);

static HWE_INLINE void *HWE_Malloc(size_t size)
{
    if (size == 0 || size >= INT_MAX) {
        return NULL;
    }

    void *p = malloc(size);

    if (p != NULL) {
        RecordMallocMemCount();
        return p;
    } else {
        return NULL;
    }
}
static HWE_INLINE void HWE_Free(void *p)
{
    if (p != NULL) {
        RecordFreeMemCount();
        free(p);
        p = NULL;
    }
}

#ifdef _WIN32
static HWE_INLINE HW_U64 HWE_GetCurrentTime(void)
{
    clock_t time;
    time = clock();
    return (HW_U64)time;
}
#else
#define TIME_FACTOR 1000000
static HWE_INLINE HW_U64 HWE_GetCurrentTime(void)
{
    HW_U64 time;
    struct timeval t;
    gettimeofday(&t, NULL);

    time = (HW_U64)t.tv_sec * TIME_FACTOR + (HW_U64)t.tv_usec;
    return time;
}
#endif
#endif