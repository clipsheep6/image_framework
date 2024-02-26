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

#include "..\include\image_packer_impl.h"

ImagePackerCapi::ImagePackerCapi()
{
    //MEDIA_DEBUG_LOG("Camera_Manager Constructor is called");
    imagePacker_ = new ImagePacker();
}

ImagePackerCapi::ImagePackerCapi(ImagePacekr* imagePacker)
{
    imagePacker_ = imagePacker;
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
    if (imagePackerCapi == nullptr || option == nullptr || imageSourceCapi == nullptr || outData != nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImagePacker* imagePacker = imagePacker_;
    share_ptr<OHOS::Media::ImageSource> imageSource = imageSourceCapi->GetImageSource();
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    PackOption outOption;
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

int32_t ImagePackerCapi::PackingFromPixelMap(ImagePackerCapi* imagePackerCapi, ImagePacker_Opts* option,
    PixelMapCapi* pixelMapCapi, uint8_t** outData, int64_t* size)
{
    if (imagePackerCapi == nullptr || option == nullptr || pixelMapCapi == nullptr || outData != nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImagePacker* imagePacker = imagePacker_;
    PixelMap* pixelMap = pixelMapCapi->get...;
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    PackOption outOption;
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

int32_t ImagePackerCapi::PackToFileFromImageSource(ImagePackerCapi* imagePackerCapi, ImagePacker_Opts* option,
    ImageSourceCapi* imageSourceCapi, const int fd)
{
    if (imagePackerCapi == nullptr || option == nullptr || imageSourceCapi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImagePacker* imagePacker = imagePacker_;
    ImageSource* imageSource = imageSourceCapi->GetImageSource();
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    PackOption outOption;
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

int32_t ImagePackerCapi::PackToFileFromPixelMap(ImagePackerCapi* imagePackerCapi, ImagePacker_Opts* option,
    PixelMapCapi* pixelMapCapi, int fd)
{
    if (imagePackerCapi == nullptr || option == nullptr || pixelMapCapi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImagePacker* imagePacker = imagePacker_;
    PixelMap* pixelMap = pixelMapCapi->get...;
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    PackOption outOption;
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