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

#include "hwe_memoryManager.h"

namespace OHOS {
namespace ImagePlugin {
HWE_ReturnVal HWE_InitMemoryManager(HWE_HANDLE *memoryHandle, const int32_t *outHandle, HWE_MallocCallback mallocFunc,
    HWE_FreeCallback freeFunc)
{
    int32_t ret;
    MemoryManager *memoryManager = (MemoryManager *)HWE_Malloc(sizeof(MemoryManager));
    HWE_RETURN_IF_NULL(memoryManager, "memoryManager malloc failed!\n");
    *memoryHandle = memoryManager;

    ret = HWE_PthreadMutexInit(&memoryManager->mutex);
    if (ret) {
        HWE_Free(memoryManager);
        HWE_LOGE("Init mutex failed.\n");
        return HWE_RET_FAILED;
    }
    memoryManager->freeCount = 0;
    memoryManager->usedCount = 0;
    memoryManager->maxMemoryNumber = MAX_MEMORY_NUM;
    memoryManager->bytesMalloc = 0;
    if (outHandle && mallocFunc && freeFunc) {
        memoryManager->outHandle = outHandle;
        memoryManager->mallocCallback = mallocFunc;
        memoryManager->freeCallback = freeFunc;
    } else {
        memoryManager->outHandle = NULL;
        memoryManager->mallocCallback = NULL;
        memoryManager->freeCallback = NULL;
    }

    if (ret) {
        HWE_PthreadMutexDestroy(&memoryManager->mutex);
        HWE_Free(memoryManager);
        HWE_LOGE("Init memoryInfoArray failed.\n");
        return HWE_RET_FAILED;
    }

    return HWE_RET_OK;
}

HWE_ReturnVal HWE_FreeMemory(HWE_HANDLE memoryHandle)
{
    HWE_RETURN_IF_NULL(memoryHandle, "memoryHandle is NULL!\n");
    MemoryManager *memoryManager = (MemoryManager *)memoryHandle;
    uint64_t i;
    int32_t ret;
    for (i = 0; i < memoryManager->usedCount; i++) {
        if (memoryManager->memoryInfoArray[i].isUsed != TRUE) {
            continue;
        }

        if (memoryManager->memoryInfoArray[i].idx != i) {
            HWE_LOGE("Memory lack, idx is %d.\n", i);
            continue;
        }

        if (memoryManager->memoryInfoArray[i].memAddr == NULL) {
            HWE_LOGE("Memory addr is null.\n");
            continue;
        }

        if (memoryManager->outHandle && memoryManager->freeCallback) {
            memoryManager->freeCallback(memoryManager->outHandle, memoryManager->memoryInfoArray[i].memAddr);
        } else {
            HWE_Free(memoryManager->memoryInfoArray[i].memAddr);
        }

        memoryManager->freeCount++;
    }
    if (memoryManager->freeCount != memoryManager->usedCount) {
        HWE_LOGE("Memory leak[free:%d, used:%d]!\n", memoryManager->freeCount, memoryManager->usedCount);
        return HWE_RET_FAILED;
    }
    ret = HWE_PthreadMutexDestroy(&memoryManager->mutex);
    HWE_RETURN_IF_CHECK(ret, 0, "Destroy mutex failed.\n");
    HWE_Free(memoryManager);
    return HWE_RET_OK;
}

void *HWE_MemAlloc(HWE_HANDLE memoryHandle, size_t size)
{
    if (size == 0) {
        return NULL;
    }
    MemoryManager *memoryManager = (MemoryManager *)memoryHandle;
    HWE_PthreadMutexLock(&memoryManager->mutex);
    if (memoryManager->usedCount >= memoryManager->maxMemoryNumber) {
        HWE_LOGE("Don't enough memory[max:%d, used:%d]!\n", memoryManager->maxMemoryNumber, memoryManager->usedCount);
        HWE_PthreadMutexUnLock(&memoryManager->mutex);
        return NULL;
    }
    void *p = NULL;

    if (memoryManager->outHandle && memoryManager->mallocCallback) {
        p = memoryManager->mallocCallback(memoryManager->outHandle, size);
    } else {
        p = HWE_Malloc(size);
    }
    memoryManager->memoryInfoArray[memoryManager->usedCount].memAddr = p;
    memoryManager->memoryInfoArray[memoryManager->usedCount].memSize = size;
    memoryManager->memoryInfoArray[memoryManager->usedCount].isUsed = TRUE;
    memoryManager->memoryInfoArray[memoryManager->usedCount].idx = memoryManager->usedCount;
    memoryManager->bytesMalloc += size;
    memoryManager->usedCount++;
    HWE_PthreadMutexUnLock(&memoryManager->mutex);
    return p;
}
} // namespace ImagePlugin
} // namespace OHOS