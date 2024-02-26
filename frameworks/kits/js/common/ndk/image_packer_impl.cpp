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

#include "image_packer_impl.h"

ImagePackerCapi::ImagePackerCapi()
{
    imagePacker_ = std::make_shared<OHOS::Media::ImagePacker>();
}

ImagePackerCapi::ImagePackerCapi(OHOS::Media::ImagePacker* imagePacker)
{
    imagePacker_ = std::shared_ptr<OHOS::Media::ImagePacker>(imagePacker);
}

ImagePackerCapi::~ImagePackerCapi()
{
    //MEDIA_DEBUG_LOG("~Camera_Manager is called");
    if (imagePacker_) {
        imagePacker_ = nullptr;
    }
}

int32_t ImagePackerCapi::PackingFromImageSource(ImagePacker_Opts* option, ImageSourceCapi* imageSourceCapi,
    uint8_t** outData, int64_t* size)
{
    if (option == nullptr || imageSourceCapi == nullptr || outData != nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    //OHOS::Media::ImageSource* imageSource = imageSourceCapi->GetImageSource().get();
    OHOS::Media::ImageSource* imageSource = nullptr;
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    OHOS::Media::PackOption outOption;
    outOption.format = option->format;
    outOption.quality = option->quality;
    const int64_t DEFAULT_BUFFER_SIZE = 25 * 1024 * 1024;
    int64_t bufferSize = (*size <= 0)?DEFAULT_BUFFER_SIZE:(*size);
    *outData = new uint8_t[bufferSize];
    ret = imagePacker->StartPacking(*outData, bufferSize, outOption);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*imageSource);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->FinalizePacking(packedSize);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    if (packedSize > 0 && (packedSize < bufferSize)) {
        *size = packedSize;
        return IMAGE_RESULT_SUCCESS;
    }
    return IMAGE_RESULT_BAD_PARAMETER;
}

int32_t ImagePackerCapi::PackingFromPixelMap(ImagePacker_Opts* option, PixelMapCapi* pixelMapCapi, 
    uint8_t** outData, int64_t* size)
{
    if (option == nullptr || pixelMapCapi == nullptr || outData != nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    //OHOS::Media::PixelMap* pixelMap = pixelMapCapi->GetPixelMapInstances().get();
    OHOS::Media::PixelMap* pixelMap = nullptr;
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    OHOS::Media::PackOption outOption;
    outOption.format = option->format;
    outOption.quality = option->quality;
    const int64_t DEFAULT_BUFFER_SIZE = 25 * 1024 * 1024;
    int64_t bufferSize = (*size <= 0)?DEFAULT_BUFFER_SIZE:(*size);
    *outData = new uint8_t[bufferSize];
    ret = imagePacker->StartPacking(*outData, bufferSize, outOption);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*pixelMap);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->FinalizePacking(packedSize);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    if (packedSize > 0 && (packedSize < bufferSize)) {
        *size = packedSize;
        return IMAGE_RESULT_SUCCESS;
    }
    return IMAGE_RESULT_BAD_PARAMETER;
}

int32_t ImagePackerCapi::PackToFileFromImageSource(ImagePacker_Opts* option, ImageSourceCapi* imageSourceCapi, const int fd)
{
    if (option == nullptr || imageSourceCapi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    //OHOS::Media::ImageSource* imageSource = imageSourceCapi->GetImageSource().get();
    OHOS::Media::ImageSource* imageSource = nullptr;
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    OHOS::Media::PackOption outOption;
    outOption.format = option->format;
    outOption.quality = option->quality;
    ret = imagePacker->StartPacking(fd, outOption);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*imageSource);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    return imagePacker->FinalizePacking(packedSize);
}

int32_t ImagePackerCapi::PackToFileFromPixelMap(ImagePacker_Opts* option, PixelMapCapi* pixelMapCapi, const int fd)
{
    if (option == nullptr || pixelMapCapi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    //OHOS::Media::PixelMap* pixelMap = pixelMapCapi->GetPixelMapInstances().get();
    OHOS::Media::PixelMap* pixelMap = nullptr;
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    OHOS::Media::PackOption outOption;
    outOption.format = option->format;
    outOption.quality = option->quality;
    ret = imagePacker->StartPacking(fd, outOption);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*pixelMap);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    return imagePacker->FinalizePacking(packedSize);
}