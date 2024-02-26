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

#ifndef INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_H
#define INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_H

#include "exiv2/exiv2.hpp"

#include <mutex>

namespace OHOS {
namespace Media {
class ExifMetaData
{
public:
    ExifMetaData();
    ~ExifMetaData();
    uint32_t CreateExiv2Image(const std::string &path);
    uint32_t CreateExiv2Image(uint8_t *data, uint32_t size);
    uint32_t CreateExiv2Image(const int fd);
    uint32_t GetImagePropertyInt(const std::string &key, int32_t &value);
    uint32_t GetImagePropertyString(const std::string &key, std::string &value);
    uint32_t ModifyImageProperty(const std::string &key, const std::string &value);
    uint32_t GetExiv2ImageBuf(uint8_t **imageBuf, uint32_t &imageSize);
    void GetExifMetaData(Exiv2::ExifData &exifData);
    uint32_t SetExifMetaData(const Exiv2::ExifData &exifData, uint8_t **imageBuf, uint32_t &imageSize);
    void WriteMetadata();
private:
    uint8_t *innerData_ = nullptr;
    std::mutex dataMutex_;
    Exiv2::ExifData exifData_;
    Exiv2::Image::UniquePtr image_;
};
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_EXIF_META_DATA_H
