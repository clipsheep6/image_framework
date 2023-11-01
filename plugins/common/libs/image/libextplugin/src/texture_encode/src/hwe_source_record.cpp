#include <stdio.h>
#include "hwe_source_record.h"
#include "hwe_osdep.h"


static HWE_PthreadMutex g_mutex;

static HW_S32 g_mallocMemCount = 0;
static HW_S32 g_freeMemCount = 0;

static HW_S32 g_initMutexCount = 0;
static HW_S32 g_destoryMutexCount = 0;

static HW_S32 g_initCondCount = 0;
static HW_S32 g_destoryCondCount = 0;

static HW_S32 g_initThreadCount = 0;
static HW_S32 g_destoryThreadCount = 0;

void RecordMallocMemCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_mallocMemCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void RecordFreeMemCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_freeMemCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void RecordInitMutexCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_initMutexCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void RecordDestoryMutexCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_destoryMutexCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void RecordInitCondCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_initCondCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void RecordDestoryCondCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_destoryCondCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void RecordInitThreadCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_initThreadCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void RecordDestoryThreadCount()
{
    HWE_PthreadMutexLock(&g_mutex);
    g_destoryThreadCount++;
    HWE_PthreadMutexUnLock(&g_mutex);
}

void InitResourceInfo()
{
    HWE_PthreadMutexInit(&g_mutex);
}

void DestroyResourceInfo()
{
    HWE_PthreadMutexDestroy(&g_mutex);
    printf("Memroy Info: malloc %d free %d\n", g_mallocMemCount, g_freeMemCount);
    printf("Mutex Info: init %d destroy %d\n", g_initMutexCount, g_destoryMutexCount);
    printf("Cond Info: init %d destroy %d\n", g_initCondCount, g_destoryCondCount);
    printf("Thread Info: init %d destroy %d\n", g_initThreadCount, g_destoryThreadCount);
}
