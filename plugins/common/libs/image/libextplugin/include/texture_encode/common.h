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

#ifndef COMMON_H
#define COMMON_H
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "securectype.h"
#include "securec.h"
#define MAX_CHAR_LEN (256)
#define HWE_HANDLE int32_t *
#ifndef DFX_FILE
#define DFX_FILE (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
#endif

#define DFX_LINE __LINE__

#define GET_TIME 0
constexpr char g_version[MAX_CHAR_LEN] = "Texture Encoder(MIT Video Engineering Department) DEBUG v1.0";

#define CLAMP3(x, xmin, xmax) \
    (((int)(x)) > ((int)(xmax)) ? ((int)(xmax)) : (((int)(x)) < ((int)(xmin)) ? ((int)(xmin)) : ((int)(x))))
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))

namespace OHOS {
namespace ImagePlugin {
typedef union UNION_TYPE {
    int32_t i32;
    uint32_t u32;
    int16_t i16[2];
    uint16_t u16[2];
    int8_t i8[4];
    uint8_t u8[4];
} UNION_TYPE;

typedef enum {
    HWE_LOG_OFF = 0,
    HWE_LOG_ERROR = 1,
    HWE_LOG_WARNING = 2,
    HWE_LOG_INFO = 3,
    HWE_LOG_DEBUG = 4,
} HWE_LogLevel;

typedef enum HWE_ReturnVal {
    HWE_RET_OK = 0,
    HWE_RET_FAILED = 1,
} HWE_ReturnVal;

#define HWE_FILE __FILE__
#define HWE_LINE __LINE__
#define HWE_FUNC __FUNCTION__
void HWE_Log(const char *fileName, int line, HWE_LogLevel level.const char *msg, ...);

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
    do {                                  \
        if ((ret) != (exp)) {             \
            HWE_LOGE(msg);                \
            goto FAILED;                  \
        }                                 \
    } while (0)

#define HWE_RETURN_IF_NULL(pointer, msg) \
    do {                                 \
        if ((pointer) == NULL) {         \
            HWE_LOGE(msg);               \
            return HWE_RET_FAILED;       \
        }                                \
    } while (0)

#define HWE_GOTO_FAILED_IF_NULL(pointer, msg) \
    do {                                  \
        if ((pointer) == NULL) {          \
            HWE_LOGE(msg);                \
            goto FAILED;                  \
        }                                 \
    } while (0)
} // namespace ImagePlugin
} // namespace OHOS
#endif