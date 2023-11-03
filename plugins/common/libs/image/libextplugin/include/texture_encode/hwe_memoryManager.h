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

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include "common.h"
#include "hwe_osdep.h"

namespace OHOS {
namespace ImagePlugin {
#define HWE_LOGE(msg, ...)                                              \
    do {                                                                \
        HWE_Log(HWE_FILE, HWE_LINE, HWE_LOG_ERROR, msg, ##__VA_ARGS__); \
    } while (0)

#define HWE_LOGW(msg, ...)                                                \
    do {                                                                  \
        HWE_Log(HWE_FILE, HWE_LINE, HWE_LOG_WARNING, msg, ##__VA_ARGS__); \
    } while (0)

#define HWE_LOGI(msg, ...)                                             \
    do {                                                               \
        HWE_Log(HWE_FILE, HWE_LINE, HWE_LOG_INFO, msg, ##__VA_ARGS__); \
    } while (0)

#define HWE_LOGD(msg, ...)                                              \
    do {                                                                \
        HWE_Log(HWE_FILE, HWE_LINE, HWE_LOG_DEBUG, msg, ##__VA_ARGS__); \
    } while (0)

#define HWE_RETURN_IF_CHECK(ret, exp, msg) \
    do {                                   \
        if ((ret) != (exp)) {              \
            HWE_LOGE(msg);                 \
            return HWE_RET_FAILED;         \
        }                                  \
    } while (0)

#define HWE_GOTO_FAILED_IF_CHECK(ret, exp, msg) \
    do {                                        \
        if ((ret) != (exp)) {                   \
            HWE_LOGE(msg);                      \
            goto FAILED;                        \
        }                                       \
    } while (0)

#define HWE_RETURN_IF_NULL(pointer, msg) \
    do {                                 \
        if ((pointer) == NULL) {         \
            HWE_LOGE(msg);               \
            return HWE_RET_FAILED;       \
        }                                \
    } while (0)

#define HWE_GOTO_FAILED_IF_NULL(pointer, msg) \
    do {                                      \
        if ((pointer) == NULL) {              \
            HWE_LOGE(msg);                    \
            goto FAILED;                      \
        }                                     \
    } while (0)

int32_t *HWE_MemAlloc(HWE_HANDLE memoryHandle, size_t size);

#define HWE_RETURN_IF_MALLOCZERO(ret, handle, pointer, size, type, name) \
    do {                                                                 \
        if ((handle) != NULL) {                                          \
            (pointer) = (type)HWE_MemAlloc(handle, size);                \
        } else {                                                         \
            (pointer) = (type)HWE_Malloc(size);                          \
        }                                                                \
        if ((pointer) == NULL) {                                         \
            HWE_LOGE("malloc %s failed!\n", (name));                     \
            return (ret);                                                \
        }                                                                \
        if (memset_s((pointer), (size), 0, (size)) != 0) {               \
            if ((handle) == NULL) {                                      \
                HWE_Free((pointer));                                     \
            }                                                            \
            HWE_LOGE("memset %s failed!\n", (name));                     \
            return (ret);                                                \
        }                                                                \
    } while (0)

#define HWE_GOTO_FAILED_IF_MALLOCZERO(handle, pointer, size, type, name) \
    do {                                                                 \
        if ((handle) != NULL) {                                          \
            (pointer) = (type)HWE_MemAlloc(handle, size);                \
        } else {                                                         \
            (pointer) = (type)HWE_Malloc(size);                          \
        }                                                                \
        if ((pointer) == NULL) {                                         \
            HWE_LOGE("malloc %s failed!\n", (name));                     \
            goto FAILED;                                                 \
        }                                                                \
        if (memset_s((pointer), (size), 0, (size)) != 0) {               \
            if ((handle) == NULL) {                                      \
                HWE_Free((pointer));                                     \
                (pointer) = NULL;                                        \
            }                                                            \
            HWE_LOGE("memset %s failed!\n", (name));                     \
            goto FAILED;                                                 \
        }                                                                \
    } while (0)

#define MAX_MEMORY_NUM 0xfffff

typedef uint32_t (*HWE_FreeCallback)(const int32_t *outHandle, void *addr);
typedef uint32_t *(*HWE_MallocCallback)(const int32_t *outHandle, size_t len);

typedef struct MemoryInfo {
    uint64_t idx;
    signed char isUsed;
    size_t memSize;
    void *memAddr;
} MemoryInfo;
typedef struct MemoryManager {
    uint64_t maxMemoryNumber;
    uint64_t usedCount;
    uint64_t freeCount;
    HWE_PthreadMutex mutex;
    MemoryInfo memoryInfoArray[MAX_MEMORY_NUM];
    const int32_t *outHandle;
    HWE_MallocCallback mallocCallback;
    HWE_FreeCallback freeCallback;
    size_t bytesMalloc;
} MemoryManager;

HWE_ReturnVal HWE_InitMemoryManager(HWE_HANDLE *memoryHandle, const int32_t *outHandle, HWE_MallocCallback mallocFunc,
    HWE_FreeCallback freeFunc);
HWE_ReturnVal HWE_FreeMemory(HWE_HANDLE memoryHandle);
} // namespace ImagePlugin
} // namespace OHOS
#endif // MEMORYMANAGER_H