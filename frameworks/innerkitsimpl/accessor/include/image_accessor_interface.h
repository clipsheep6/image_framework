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

#ifndef IMAGE_ACCESSOR_INTERFACE_H
#define IMAGE_ACCESSOR_INTERFACE_H

#define MOCK_NATIVE_INCLUDE_HILOG_LOG_C_H 1

#include <memory>
#include "exif_metadata.h"
#include "image_stream.h"
#include "data_buf.h"
#include "output_data_stream.h"

namespace OHOS {
namespace Media {
class ImageAccessor {
public:
    virtual uint32_t ReadMetadata() = 0;
    virtual uint32_t WriteMetadata() = 0;
    virtual bool CreateExifMetadata() = 0;
    virtual bool WriteToOutput(ImagePlugin::OutputDataStream& output) = 0;
    virtual bool ReadExifBlob(DataBuf &blob) const = 0;
    virtual uint32_t WriteExifBlob(DataBuf &blob) = 0;
    virtual std::shared_ptr<ExifMetadata> GetExifMetadata() = 0;
};
} // namespace Media
} // namespace OHOS

#endif
