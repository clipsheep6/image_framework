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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_ABSTRACT_IMAGE_ACCESSOR_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_ABSTRACT_IMAGE_ACCESSOR_H

#include "image_accessor_interface.h"

namespace OHOS {
namespace Media {
class AbstractImageAccessor : public ImageAccessorInterface {
public:
    AbstractImageAccessor(std::shared_ptr<ImageStream> &stream);
    virtual ~AbstractImageAccessor();

    virtual bool CreateExifMetadata() override;
    virtual std::shared_ptr<ExifMetadata> GetExifMetadata() override;
    virtual bool WriteToOutput(ImagePlugin::OutputDataStream &soutput) override;

protected:
    std::shared_ptr<ExifMetadata> exifMetadata_ = nullptr;
    std::shared_ptr<ImageStream> imageStream_ = nullptr;
};
} // namespace Media
} // namespace OHOS

#endif