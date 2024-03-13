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

#include "tiff_parser.h"
#include "image_log.h"

#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "TiffParser"

namespace OHOS {
namespace Media {

void TiffParser::Decode(const unsigned char *dataPtr, const uint32_t &size, ExifData **exifData)
{
    if (dataPtr == nullptr) {
        return;
    }
    *exifData = exif_data_new();
    exif_data_unset_option(*exifData, EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS);
    exif_data_load_data_general(*exifData, dataPtr, size);
    exif_data_unref(*exifData);
}

void TiffParser::Encode(unsigned char **dataPtr, uint32_t &size, ExifData *exifData)
{
    if (exifData == nullptr) {
        return;
    }
    exif_data_save_data_general(exifData, dataPtr, &size);
    IMAGE_LOGE("[Encode] Encode dataPtr size is [%{public}u].", size);

}

void TiffParser::DecodeJpegExif(const unsigned char *dataPtr, const uint32_t &size, ExifData **exifData)
{
    IMAGE_LOGE("[DecodeJpegExif].");
    if (dataPtr == nullptr) {
        return;
    }
    *exifData = exif_data_new();
    exif_data_unset_option(*exifData, EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS);
    exif_data_load_data(*exifData, dataPtr, size);
    exif_data_unref(*exifData);
}

void TiffParser::EncodeJpegExif(unsigned char **dataPtr, uint32_t &size, ExifData *exifData)
{
    IMAGE_LOGE("[EncodeJpegExif].");
    if (exifData == nullptr) {
        return;
    }
    exif_data_save_data(exifData, dataPtr, &size);
}

} // namespace Media
} // namespace OHOS
