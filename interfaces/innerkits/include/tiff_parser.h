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

#ifndef INTERFACES_INNERKITS_INCLUDE_FILE_TIFF_PARSER_H
#define INTERFACES_INNERKITS_INCLUDE_FILE_TIFF_PARSER_H

#include <libexif/exif-data.h>
#include <algorithm>
#include <map>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdint.h>
#include <vector>

namespace OHOS {
namespace Media {

class TiffParser {
public:
    // 将exif buffer解码为ExifData数据结构 *dataPtr --> *exifData
    static void Decode(const unsigned char *dataPtr, const uint32_t &size, ExifData **exifData);
    // 将ExifData数据编码为exif buffer *exifData --> *dataPtr
    static void Encode(unsigned char **dataPtr, uint32_t &size, ExifData *exifData);

    // 将exif buffer解码为ExifData数据结构 (针对JPEG图片格式) *dataPtr --> *exifData
    static void DecodeJpegExif(const unsigned char *dataPtr, const uint32_t &size, ExifData **exifData);
    // 将ExifData数据编码为exif buffer (针对JPEG图片格式) *exifData --> *dataPtr
    static void EncodeJpegExif(unsigned char **dataPtr, uint32_t &size, ExifData *exifData);
};


} // namespace MultimediaPlugin
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_FILE_TIFF_PARSER_H
