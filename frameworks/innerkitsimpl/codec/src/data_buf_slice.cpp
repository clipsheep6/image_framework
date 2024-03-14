/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "image_log.h"
#include "data_buf_slice.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "DataBufferSlice"

namespace OHOS {
namespace Media {
SliceBase::SliceBase(size_t begin, size_t end) : begin_(begin), end_(end)
{
    if (begin >= end) {
        IMAGE_LOGE("Begin must be smaller than end");
    }
}

void SliceBase::rangeCheck(size_t index) const
{
    if (index >= size()) {
        IMAGE_LOGE("Index outside of the slice");
    }
}
} // namespace Media
} // namespace OHOS
