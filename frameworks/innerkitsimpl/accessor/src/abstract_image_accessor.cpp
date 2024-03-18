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

#include "abstract_image_accessor.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "AbstractImageAccessor"

namespace OHOS {
namespace Media {

AbstractImageAccessor::AbstractImageAccessor(std::shared_ptr<ImageStream> &stream) : imageStream_(stream) {}

AbstractImageAccessor::~AbstractImageAccessor() {}

bool AbstractImageAccessor::CreateExifMetadata()
{
    exifMetadata_ = std::make_shared<ExifMetadata>();
    return exifMetadata_->CreateExifdata();
}

bool AbstractImageAccessor::WriteToOutput(ImagePlugin::OutputDataStream &output)
{
    if (imageStream_ == nullptr) {
        return false;
    }

    return output.Write(imageStream_->GetAddr(), imageStream_->GetSize());
}

std::shared_ptr<ExifMetadata> AbstractImageAccessor::GetExifMetadata()
{
    return exifMetadata_;
}

} // namespace Media
} // namespace OHOS
