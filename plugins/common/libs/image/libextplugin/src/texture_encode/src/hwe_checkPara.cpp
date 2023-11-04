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

#include "hwe_checkPara.h"
constexpr int32_t LOG_SHUT_OFF_MASK = 0x00000080;
namespace OHOS {
namespace ImagePlugin {
static void LogDefault(const int8_t *fileName, uint32_t line, uint32_t uiChannelID, LogLevel eLevel,
    const int8_t *pszFmt, va_list arg)
{
    const char *pszPrefix = NULL;
    switch (eLevel) {
        case LOG_ERROR:
            pszPrefix = "error";
            break;
        case LOG_WARN:
            pszPrefix = "warning";
            break;
        case LOG_INFO:
            pszPrefix = "info";
            break;
        case LOG_DEBUG:
            pszPrefix = "debug";
            break;
        default:
            pszPrefix = "unknown";
            break;
    }
    int32_t ret = fprintf(stderr, "[%s\t]: %d %s %d: ", pszPrefix, uiChannelID, fileName, line);
    if (ret < 0) {
        return;
    }
    ret = vfprintf(stderr, (const char *)pszFmt, arg);
    if (ret < 0) {
        return;
    }
}

void DFX_ParamLog(const int8_t *fileName, uint32_t line, int32_t level, const char *msg, ...)
{
    LogLevel eLevel = (LogLevel)((uint32_t)level & (~LOG_SHUT_OFF_MASK));
    const int8_t *pszTempMsg = (const int8_t *)msg;
    if (((uint32_t)level & LOG_SHUT_OFF_MASK) == LOG_SHUT_OFF_MASK) {
        return;
    }
    va_list arg;
    va_start(arg, msg);
    LogDefault(fileName, line, 0, eLevel, pszTempMsg, arg);
    va_end(arg);
    return;
}

#define CHECK_PARAM_VALID_CONDITION(expression, val, msg)                          \
    do {                                                                           \
        if ((expression)) {                                                        \
            DFX_ParamLog((const int8_t *)DFX_FILE, DFX_LINE, LOG_ERROR, msg, val); \
            goto fail;                                                             \
        }                                                                          \
    } while (0)

#define CHECK_PARAM_VALID(val, minValue, maxValue, msg)                                                \
    do {                                                                                               \
        if ((val) > (maxValue) || (val) < (minValue)) {                                                \
            DFX_ParamLog((const int8_t *)DFX_FILE, DFX_LINE, LOG_ERROR, msg, val, minValue, maxValue); \
            goto fail;                                                                                 \
        }                                                                                              \
    } while (0)

bool CheckValidParam(TextureEncodeOptions *param)
{
    return true;
}
} // namespace ImagePlugin
} // namespace OHOS