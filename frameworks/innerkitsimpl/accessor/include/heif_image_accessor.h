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

#ifndef HEIF_IMAGE_ACCESSOR_H_
#define HEIF_IMAGE_ACCESSOR_H_

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
class HeifImageAccessor : public AbstractImageAccessor {
public:
    HeifImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~HeifImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual uint32_t WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual uint32_t WriteExifBlob(DataBuf &blob) override;
};
} // namespace Media
} // namespace OHOS

#endif
