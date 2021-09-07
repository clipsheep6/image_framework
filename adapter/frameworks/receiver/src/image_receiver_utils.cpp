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

#include "image_receiver_utils.h"

namespace OHOS {
namespace Media {
static constexpr uint32_t ERR_MSG_BUFFER_SIZE = 256;

using namespace OHOS::HiviewDFX;

bool ImageReceiverUtils::LogWhenNull(void *obj, const std::string &msg)
{
    if (obj != nullptr) {
        return false;
    }
    if (!msg.empty()) {
        HiLog::Error(LABEL, "%{public}s", msg.c_str());
    }
    return true;
}

bool ImageReceiverUtils::ThrowExceptionWhenNull(void *obj, JNIEnv *env, const std::string &msg)
{
    if (obj != nullptr) {
        return false;
    }
    ThrowException(env, msg);
    return true;
}

void ImageReceiverUtils::ThrowException(JNIEnv *env, const std::string &msg)
{
    if (!msg.empty()) {
        HiLog::Error(LABEL, "%{public}s", msg.c_str());
        JkitThrowIllegalStateException(env, msg.c_str());
    }
}

std::string ImageReceiverUtils::GetFmtMsg(const char *fmt, ...)
{
    char buffer[ERR_MSG_BUFFER_SIZE] = { 0 };
    va_list args;
    va_start(args, fmt);
    int32_t ret = vsnprintf_s(buffer, ERR_MSG_BUFFER_SIZE, (ERR_MSG_BUFFER_SIZE - 1), fmt, args);
    if (ret <= 0) {
        HiLog::Error(LABEL, "fail to build format error message:%{public}d", ret);
    }
    va_end(args);
    std::string result = buffer;
    return result;
}
} // namespace Media
} // namespace OHOS