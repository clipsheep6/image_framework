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

#ifndef IMAGE_RECEIVER_UTILS_H
#define IMAGE_RECEIVER_UTILS_H

#include <stdarg.h>
#include <string>
#include "hilog/log.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "securec.h"

namespace OHOS {
namespace Media {
class ImageReceiverUtils {
public:
    static bool LogWhenNull(void *obj, const std::string &msg);
    static bool ThrowExceptionWhenNull(void *obj, JNIEnv *env, const std::string &msg);
    static void ThrowException(JNIEnv *env, const std::string &msg);
    static std::string GetFmtMsg(const char *fmt, ...);

private:
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
        LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageReceiverUtils"
    };
};
} // namespace Media
} // namespace OHOS

#endif // IMAGE_RECEIVER_UTILS_H