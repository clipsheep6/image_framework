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

#include "exif_meta_data.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "image_log.h"
#include "media_errors.h"

namespace OHOS {
namespace Media {

ExifMetaData::ExifMetaData()
{
    Exiv2::XmpParser::initialize();
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif
}

ExifMetaData::~ExifMetaData()
{
    Exiv2::XmpParser::terminate();
}

uint32_t ExifMetaData::CreateExiv2Image(const std::string &path)
{
    if (path.empty()) {
        IMAGE_LOGE("[ExifMetaData]path can not be empty.");
        return ERR_OPEN_EXIV2_IMAGE_PATH;
    }
    std::unique_lock<std::mutex> guard(dataMutex_);
    image_ = Exiv2::ImageFactory::open(path.c_str());
    if (image_.get() == nullptr) {
        IMAGE_LOGE("[ExifMetaData]open image file with path failed, path:%{public}s.", path.c_str());
        return ERR_OPEN_EXIV2_IMAGE_PATH;
    }
    return SUCCESS;
}

uint32_t ExifMetaData::CreateExiv2Image(uint8_t *data, uint32_t size)
{
    if (data == nullptr || size == 0) {
        IMAGE_LOGE("[ExifMetaData]exiv2 image buf can not be empty.");
        return ERR_OPEN_EXIV2_IMAGE_BUF;
    }
    std::unique_lock<std::mutex> guard(dataMutex_);
    image_ = Exiv2::ImageFactory::open(data, size);
    if (image_.get() == nullptr) {
        IMAGE_LOGE("[ExifMetaData]open image file with buf failed, size:%{public}u.", size);
        return ERR_OPEN_EXIV2_IMAGE_BUF;
    }
    return SUCCESS;
}

void ExifMetaData::ReadMetadata()
{
    std::unique_lock<std::mutex> guard(dataMutex_);
    image_->readMetadata();
    exifData_ = image_->exifData();
}

uint32_t ExifMetaData::GetImagePropertyInt(const std::string &key, int32_t &value)
{
    std::unique_lock<std::mutex> guard(dataMutex_);
    Exiv2::ExifData::iterator itExif = exifData_.findKey(Exiv2::ExifKey(key));
    if (itExif == exifData_.end()) {
        IMAGE_LOGE("[ExifMetaData]get image property failed, key:%{public}s.", key.c_str());
        return ERR_GET_EXIV2_IMAGE_PROPERTY;
    }
    Exiv2::Exifdatum &datumExif = exifData_[key];
    value = static_cast<int32_t>(datumExif.value().toInt64());
    return SUCCESS;
}

uint32_t ExifMetaData::GetImagePropertyString(const std::string &key, std::string &value)
{
    std::unique_lock<std::mutex> guard(dataMutex_);
    Exiv2::ExifData::iterator itExif = exifData_.findKey(Exiv2::ExifKey(key));
    if (itExif == exifData_.end()) {
        IMAGE_LOGE("[ExifMetaData]get image property failed, key:%{public}s.", key.c_str());
        return ERR_GET_EXIV2_IMAGE_PROPERTY;
    }
    Exiv2::Exifdatum &datumExif = exifData_[key];
    value = datumExif.print();
    return SUCCESS;
}

uint32_t ExifMetaData::ModifyImageProperty(const std::string &key, const std::string &value)
{
    IMAGE_LOGD("[ExifMetaData]key:%{public}s value:%{public}s.", key.c_str(), value.c_str());
    std::unique_lock<std::mutex> guard(dataMutex_);
    Exiv2::ExifData::iterator itExif = exifData_.findKey(Exiv2::ExifKey(key));
    if (itExif != exifData_.end()) {
        IMAGE_LOGD("[ExifMetaData]use iterator setvalue.");
        itExif->setValue(value);
        return SUCCESS;
    }
    IMAGE_LOGD("[ExifMetaData]key not exist & add key value key:%{public}s value:%{public}s.", key.c_str(), value.c_str());
    exifData_[key] = value;
    return SUCCESS;
}

uint32_t ExifMetaData::GetExiv2ImageData(uint8_t **imageData, uint32_t &imageSize)
{
    std::unique_lock<std::mutex> guard(dataMutex_);
    Exiv2::BasicIo& imageIo = image_->io();
    *imageData = static_cast<uint8_t *>(imageIo.mmap(true));
    imageSize = static_cast<uint32_t>(imageIo.size());
    return SUCCESS;
}

void ExifMetaData::GetExifMetaData(Exiv2::ExifData &exifData)
{
    std::unique_lock<std::mutex> guard(dataMutex_);
    exifData = exifData_;
}

void ExifMetaData::SetExifMetaData(const Exiv2::ExifData &exifData)
{
    std::unique_lock<std::mutex> guard(dataMutex_);
    exifData_ = exifData;
}

void ExifMetaData::WriteMetadata()
{
    std::unique_lock<std::mutex> guard(dataMutex_);
    image_->setExifData(exifData_);
    image_->writeMetadata();
}

} // namespace Media
} // namespace OHOS
