/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "image_packer2_impl.h"
#include "image_source2_impl.h"
#include "pixelmap2_impl.h"

using namespace OHOS;
using namespace Media;
#ifdef __cplusplus
extern "C" {
#endif


OH_ImagePacker::OH_ImagePacker()
{
    imagePacker_ = std::make_shared<OHOS::Media::ImagePacker>();
}

OH_ImagePacker::OH_ImagePacker(std::shared_ptr<OHOS::Media::ImagePacker> imagePacker)
{
    imagePacker_ = imagePacker;
}

OH_ImagePacker::~OH_ImagePacker()
{
    if (imagePacker_) {
        imagePacker_ = nullptr;
    }
}

int32_t OH_ImagePacker::PackingFromImageSource(OHOS::Media::PackOption* option, OH_ImageSource* imageSource,
    uint8_t* outData, int64_t* size)
{
    if (option == nullptr || imageSource == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    OHOS::Media::ImageSource* imageSourcePtr = imageSource->GetInnerImageSource().get();
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    const int64_t DEFAULT_BUFFER_SIZE = 25 * 1024 * 1024;
    int64_t bufferSize = (*size <= 0)?DEFAULT_BUFFER_SIZE:(*size);
    ret = imagePacker->StartPacking(outData, bufferSize, *option);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*imageSourcePtr);
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

int32_t OH_ImagePacker::PackingFromPixelMap(OHOS::Media::PackOption* option, OH_Pixelmap* pixelMap, 
    uint8_t* outData, int64_t* size)
{
    if (option == nullptr || pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    OHOS::Media::PixelMap* pixelMapPtr = pixelMap->GetInnerPixelMap().get();
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    const int64_t DEFAULT_BUFFER_SIZE = 25 * 1024 * 1024;
    int64_t bufferSize = (*size <= 0)?DEFAULT_BUFFER_SIZE:(*size);
    ret = imagePacker->StartPacking(outData, bufferSize, *option);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*pixelMapPtr);
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

int32_t OH_ImagePacker::PackToFileFromImageSource(OHOS::Media::PackOption* option, OH_ImageSource* imageSource,
    const int fd)
{
    if (option == nullptr || imageSource == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    OHOS::Media::ImageSource* imageSourcePtr = imageSource->GetInnerImageSource().get();
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    ret = imagePacker->StartPacking(fd, *option);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*imageSourcePtr);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    return imagePacker->FinalizePacking(packedSize);
}

int32_t OH_ImagePacker::PackToFileFromPixelMap(OHOS::Media::PackOption* option, OH_Pixelmap* pixelMap,
    int32_t fd)
{
    if (option == nullptr || pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    OHOS::Media::ImagePacker* imagePacker = imagePacker_.get();
    OHOS::Media::PixelMap* pixelMapPtr = pixelMap->GetInnerPixelMap().get();
    int64_t packedSize = 0;
    uint32_t ret = IMAGE_RESULT_SUCCESS;
    ret = imagePacker->StartPacking(fd, *option);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    ret = imagePacker->AddImage(*pixelMapPtr);
    if (ret != IMAGE_RESULT_SUCCESS) {
        return ret;
    }
    return imagePacker->FinalizePacking(packedSize);
}

#ifdef __cplusplus
};
#endif