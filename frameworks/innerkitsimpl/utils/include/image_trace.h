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

#ifndef FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_IMAGE_TRACE_H_
#define FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_IMAGE_TRACE_H_

#include <string>

namespace OHOS {
namespace Media {
class ImageTrace {
public:
    explicit ImageTrace(const std::string &title);
    explicit ImageTrace(const char *fmt, ...);
    ~ImageTrace();

private:
    std::string title_;
};
} // namespace Media
} // namespace OHOS
#endif // FRAMEWORKS_INNERKITSIMPL_UTILS_INCLUDE_IMAGE_TRACE_H_
