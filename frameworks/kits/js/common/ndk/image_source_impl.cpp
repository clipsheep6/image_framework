/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "image_source_impl.h"
#include <map>
#include "image_log.h"
#include "media_errors.h"
#include "securec.h"

using namespace OHOS;
using namespace OHOS::Media;

ImageSourceCapi::~ImageSourceCapi()
{
    if (innerImageSource_) {
        innerImageSource_ = nullptr;
    }
}

std::shared_ptr<ImageSource> ImageSourceCapi::GetInnerImageSource()
{
    return innerImageSource_;
}

ImageSourceCapi::ImageSourceCapi(std::shared_ptr<ImageSource> imageSource)
{
    innerImageSource_ = imageSource;
}

ImageSourceCapi::ImageSourceCapi(char* uri, size_t size, SourceOptions opts)
{
    std::string strUri = std::string(uri, size);
    std::string path = UrlToPath(strUri);
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    std::unique_ptr<ImageSource> nativeImageSource = ImageSource::CreateImageSource(path, opts, errorCode);
    if (nativeImageSource == nullptr) {
        IMAGE_LOGD("ImageSourceCapiCreateFromUri create failed:%{public}d", errorCode);
    }
    innerImageSource_ = std::move(nativeImageSource);
    if (innerImageSource_ == nullptr) {
        IMAGE_LOGE("ImageSourceCapiCreateFromUri native create failed");
    }
    IMAGE_LOGD("ImageSourceCapiCreateFromUri success");
}

ImageSourceCapi::ImageSourceCapi(int32_t fd, SourceOptions opts)
{
    IMAGE_LOGD("ImageSourceCapiCreateFromFd");
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    std::unique_ptr<ImageSource> nativeImageSource = ImageSource::CreateImageSource(fd, opts, errorCode);
    if (nativeImageSource == nullptr) {
        IMAGE_LOGD("ImageSourceCapiCreateFromFd create failed:%{public}d", errorCode);
    }
    innerImageSource_ = std::move(nativeImageSource);
    if (innerImageSource_ == nullptr) {
        IMAGE_LOGE("ImageSourceCapiCreateFromFd native create failed");
    }
    IMAGE_LOGD("ImageSourceCapiCreateFromFd success");
}

ImageSourceCapi::ImageSourceCapi(uint8_t* data, size_t dataSize, SourceOptions opts)
{
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    std::unique_ptr<ImageSource> nativeImageSource = ImageSource::CreateImageSource(
        data, dataSize, opts, errorCode);
    if (nativeImageSource == nullptr) {
        IMAGE_LOGD("ImageSourceCapiCreateFromData create failed:%{public}d", errorCode);
    }
    innerImageSource_ = std::move(nativeImageSource);
    if (innerImageSource_ == nullptr) {
        IMAGE_LOGE("ImageSourceCapiCreateFromData native create failed");
    }
    IMAGE_LOGD("ImageSourceCapiCreateFromData success");
}

ImageSourceCapi::ImageSourceCapi(RawFileDescriptor rawFile, SourceOptions opts)
{
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    int32_t rawFileLength = rawFile.start + rawFile.length;
    std::unique_ptr<ImageSource> nativeImageSource = ImageSource::CreateImageSource(
        rawFile.fd, rawFile.start, rawFileLength, opts, errorCode);
    if (nativeImageSource == nullptr) {
        IMAGE_LOGD("ImageSourceCapiCreateFromRawFile create failed:%{public}d", errorCode);
    }
    innerImageSource_ = std::move(nativeImageSource);
    if (innerImageSource_ == nullptr) {
        IMAGE_LOGE("ImageSourceCapiCreateFromRawFile native create failed");
    }
    IMAGE_LOGD("ImageSourceCapiCreateFromRawFile success");
}

ImageSourceCapi::ImageSourceCapi(ImageSourceImpl* source, IncrementalSourceOptions incOpts)
{
    incOpts.incrementalMode = IncrementalMode::INCREMENTAL_DATA;
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateIncrementalImageSource(incOpts, errorCode);
    if (imageSource == nullptr) {
        IMAGE_LOGE("ImageSourceCapiCreateIncremental native imagesource failed");
    }
    if (source->buffer != nullptr && source->bufferSize > 0) {
        IMAGE_LOGD("ImageSourceCapiCreateIncremental update dataArray");
        imageSource->UpdateData(source->buffer, source->bufferSize, false);
    }
    innerImageSource_ = std::move(imageSource);
    IMAGE_LOGD("ImageSourceCapiCreateIncremental success");
}

std::string ImageSourceCapi::UrlToPath(const std::string &path)
{
    const std::string filePrefix = "file://";
    if (path.size() > filePrefix.size() &&
        (path.compare(0, filePrefix.size(), filePrefix) == 0)) {
        return path.substr(filePrefix.size());
    }
    return path;
}
