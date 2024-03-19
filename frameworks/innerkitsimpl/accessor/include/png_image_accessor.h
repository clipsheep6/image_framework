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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_IMAGE_ACCESSOR_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_IMAGE_ACCESSOR_H

#include "abstract_image_accessor.h"

namespace OHOS {
namespace Media {
static unsigned char pngSignature[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,
};

class PngImageAccessor : public AbstractImageAccessor {
public:
    PngImageAccessor(std::shared_ptr<ImageStream> &stream);
    ~PngImageAccessor();

    virtual uint32_t ReadMetadata() override;
    virtual uint32_t WriteMetadata() override;
    virtual bool ReadExifBlob(DataBuf &blob) const override;
    virtual uint32_t WriteExifBlob(DataBuf &blob) override;

private:
    bool IsPngType() const;
    size_t ReadChunk(DataBuf &buffer) const;
    bool FindTiffFromText(const DataBuf &data, const std::string chunkType, DataBuf &tiffData) const;
    bool ProcessExifData(DataBuf &blob, std::string chunkType, uint32_t chunkLength) const;
};
} // namespace Media
} // namespace OHOS

#endif