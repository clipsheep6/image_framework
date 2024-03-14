/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "webp_image_accessor.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "WebpImageAccessor"

namespace OHOS {
namespace Media {

WebpImageAccessor::WebpImageAccessor(std::shared_ptr<ImageStream> &stream)
    : AbstractImageAccessor(stream)
{

}

WebpImageAccessor::~WebpImageAccessor()
{

}

uint32_t WebpImageAccessor::ReadMetadata()
{
    return 0;
}

uint32_t WebpImageAccessor::WriteMetadata()
{
    return 0;
}

bool WebpImageAccessor::ReadExifBlob(DataBuf &blob) const
{
    return false;
}

uint32_t WebpImageAccessor::WriteExifBlob(DataBuf &blob)
{
    return 0;
}

} // namespace Media
} // namespace OHOS
