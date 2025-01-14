/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "__config"
#include "iosfwd"
#include "hilog/log.h"
#include "hilog/log_cpp.h"
#include "ostream"

int HiLogPrint(LogType type, LogLevel level, unsigned int domain, const char *tag, const char *fmt, ...)
{
    return 0;
}

namespace OHOS {
namespace HiviewDFX {
using namespace std;
int HiLog::Debug(const HiLogLabel &label, const char *fmt, ...)
{
    return 0;
}

int HiLog::Info(const HiLogLabel &label, const char *fmt, ...)
{
    return 0;
}

int HiLog::Warn(const HiLogLabel &label, const char *fmt, ...)
{
    return 0;
}

int HiLog::Error(const HiLogLabel &label, const char *fmt, ...)
{
    return 0;
}

int HiLog::Fatal(const HiLogLabel &label, const char *fmt, ...)
{
    return 0;
}
} // namespace HiviewDFX
} // namespace OHOS
