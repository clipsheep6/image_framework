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

#include "common.h"
#define LOG_SHUT_OFF_MASK 0x00000080
HWE_LogLevel g_userSetLogLevel = HWE_LOG_ERROR;
void HWE_Log(const char *fileName, int line, HWE_LogLevel level, const char *msg, ...)
{
    if (level > g_userSetLogLevel) {
        return;
    }
    const char *type = NULL;
    switch (level) {
        case HWE_LOG_ERROR:
            type = "error";
            break;
        case HWE_LOG_WARNING:
            type = "warning";
            break;
        case HWE_LOG_INFO:
            type = "info";
            break;
        case HWE_LOG_DEBUG:
            type = "debug";
            break;
        default:
            type = "unknown";
            break;
    }
    va_list arg;
    va_start(arg, msg);
    fprintf(stderr, "HWE [%s]: %s %d: ", type, fileName, line);
    vfprintf(stderr, (const char *)msg, arg);
    va_end(arg);
}