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

#include "image_packer2.h"

#include "common_utils.h"
#include "image_packer.h"
#include "image_packer2_impl.h"
#include "image_source2_impl.h"
#include "pixelmap2_impl.h"

using namespace OHOS;
using namespace Media;
#ifdef __cplusplus
extern "C" {
#endif

struct OH_ImagePacker_PackingOptions {
    Image_MimeType mimeType;
    int quality;
};

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_CreatePackingOptions(OH_ImagePacker_PackingOptions **opts)
{
    *opts = new OH_ImagePacker_PackingOptions();
    if (*opts == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackingOptionsGetMimeType(OH_ImagePacker_PackingOptions *opts,
    Image_MimeType *format)
{
    if (opts == nullptr || format == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    format->format = opts->mimeType.format;
    format->size = opts->mimeType.size;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackingOptionsSetMimeType(OH_ImagePacker_PackingOptions *opts,
    Image_MimeType format)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->mimeType.format = format.format;
    opts->mimeType.size = format.size;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackingOptionsGetQuality(OH_ImagePacker_PackingOptions *opts, uint32_t *quality)
{
    if (opts == nullptr || quality == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *quality = opts->quality;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackingOptionsSetQuality(OH_ImagePacker_PackingOptions *opts, uint32_t quality)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->quality = quality;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_ReleasePackingOptions(OH_ImagePacker_PackingOptions *opts)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    delete opts;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_Create(OH_ImagePacker **imagePacker)
{
    *imagePacker = new OH_ImagePacker();
    if (*imagePacker == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackToDataFromImageSource(OH_ImagePacker* imagePacker,
    OH_ImagePacker_PackingOptions* option, OH_ImageSource* imageSource, uint8_t* outData, size_t* size)
{
    if (imagePacker == nullptr || option == nullptr || imageSource == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    PackOption packOption;
    std::string format(option->mimeType.format, option->mimeType.size);
    packOption.format = format;
    packOption.quality = option->quality;
    return static_cast<Image_ErrorCode>(imagePacker->PackingFromImageSource(&packOption, imageSource,
        outData, reinterpret_cast<int64_t*>(size)));
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackToDataFromPixelMap(OH_ImagePacker* imagePacker,
    OH_ImagePacker_PackingOptions* option, OH_Pixelmap* pixelMap, uint8_t* outData, size_t* size)
{
    if (imagePacker == nullptr || option == nullptr || pixelMap == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    PackOption packOption;
    std::string format(option->mimeType.format, option->mimeType.size);
    packOption.format = format;
    packOption.quality = option->quality;
    return static_cast<Image_ErrorCode>(imagePacker->PackingFromPixelMap(&packOption, pixelMap, outData,
        reinterpret_cast<int64_t*>(size)));
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackToFileFromImageSource(OH_ImagePacker* imagePacker,
    OH_ImagePacker_PackingOptions* option, OH_ImageSource* imageSource, int32_t fd)
{
    if (imagePacker == nullptr || option == nullptr || imageSource == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    PackOption packOption;
    std::string format(option->mimeType.format, option->mimeType.size);
    packOption.format = format;
    packOption.quality = option->quality;
    return static_cast<Image_ErrorCode>(imagePacker->PackToFileFromImageSource(&packOption, imageSource, fd));
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_PackToFileFromPixelMap(OH_ImagePacker* imagePacker,
    OH_ImagePacker_PackingOptions* option, OH_Pixelmap* pixelMap, int32_t fd)
{
    if (imagePacker == nullptr || option == nullptr || pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    PackOption packOption;
    std::string format(option->mimeType.format, option->mimeType.size);
    packOption.format = format;
    packOption.quality = option->quality;
    return static_cast<Image_ErrorCode>(imagePacker->PackToFileFromPixelMap(&packOption, pixelMap, fd));
}

MIDK_EXPORT
Image_ErrorCode OH_ImagePacker2_Release(OH_ImagePacker *imagePacker)
{
    if (imagePacker == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    delete imagePacker;
    return IMAGE_RESULT_SUCCESS;
}

#ifdef __cplusplus
};
#endif