#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include "common.h"
#include "hwe_osdep.h"
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
    }while (0)
#define HWE_LOGD(msg, ...)                                              \
    do {                                                                \
        HWE_Log(HWE_FILE, HWE_LINE, HWE_LOG_DEBUG, msg, ##__VA_ARGS__); \
    }while (0)

#define HWE_RETURN_IF_CHECK(ret, exp, msg) \
    do {                                   \
        if ((ret) != (exp)) {              \
            HWE_LOGE(msg);                 \
            return HWE_RET_FAILED;         \
        }                                  \
    }while (0)

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

void *HWE_MemAlloc(HWE_HANDLE memoryHandle, size_t size);

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

    typedef void (*HWE_FreeCallback)(const void *outHandle, void *addr);
    typedef void *(*HWE_MallocCallback)(const void *outHandle, size_t len);

    typedef struct MemoryInfo {
        HW_U64 idx;
        HW_B8 isUsed;
        size_t memSize;
        void *memAddr;
    } MemoryInfo;
    typedef struct MemoryManager {
        HW_U64 maxMemoryNumber;
        HW_U64 usedCount;
        HW_U64 freeCount;
        HWE_PthreadMutex mutex;
        MemoryInfo memoryInfoArray[MAX_MEMORY_NUM];
        const void *outHandle;
        HWE_MallocCallback mallocCallback;
        HWE_FreeCallback freeCallback;
        size_t bytesMalloc;
    }MemoryManager;

    HWE_ReturnVal HWE_InitMemoryManager(HWE_HANDLE *memoryHandle, const void *outHandle, HWE_MallocCallback mallocFunc,
        HWE_FreeCallback freeFunc);
    HWE_ReturnVal HWE_FreeMemory(HWE_HANDLE memoryHandle);
    #endif
