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

#include "pixelmap2.h"

#include "common_utils.h"
#include "image_type.h"
#include "pixelmap2_impl.h"

using namespace OHOS::Media;
#ifdef __cplusplus
extern "C" {
#endif

struct OH_Pixelmap_InitializationOptions {
    uint32_t width;
    uint32_t height;
    PIXEL_FORMAT pixelFormat = PIXEL_FORMAT::UNKNOWN;
    uint32_t editable = false;
    PIXELMAP_ALPHA_TYPE alphaType = PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN;
};

struct OH_Pixelmap_ImageInfo {
    uint32_t width;
    uint32_t height;
    uint32_t rowStride;
    int32_t pixelFormat = PIXEL_FORMAT::UNKNOWN;
    PIXELMAP_ALPHA_TYPE alphaType = PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN;
};

static PIXEL_FORMAT ParsePixelForamt(int32_t val)
{
    if (val <= static_cast<int32_t>(PIXEL_FORMAT::NV12)) {
        return PIXEL_FORMAT(val);
    }

    return PIXEL_FORMAT::UNKNOWN;
}

static PIXELMAP_ALPHA_TYPE ParseAlphaType(int32_t val)
{
    if (val <= static_cast<int32_t>(PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNPEMUL)) {
        return PIXELMAP_ALPHA_TYPE(val);
    }

    return PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_CreateInitializationOptions(OH_Pixelmap_InitializationOptions **ops)
{
    *ops = new OH_Pixelmap_InitializationOptions();
    if (ops == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetWidth(OH_Pixelmap_InitializationOptions *ops,
    uint32_t *width)
{
    if ( ops == nullptr || width == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *width = ops->width;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetWidth(OH_Pixelmap_InitializationOptions *ops,
    uint32_t width)
{
    if ( ops == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ops->width = width;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetHeight(OH_Pixelmap_InitializationOptions *ops,
    uint32_t *height)
{
    if ( ops == nullptr || height == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *height = ops->height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetHeight(OH_Pixelmap_InitializationOptions *ops,
    uint32_t height)
{
    if ( ops == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ops->height = height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetPixelFormat(OH_Pixelmap_InitializationOptions *ops,
    int32_t *pixelFormat)
{
    if ( ops == nullptr || pixelFormat == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *pixelFormat = static_cast<int32_t>(ops->pixelFormat);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetPixelFormat(OH_Pixelmap_InitializationOptions *ops,
    int32_t pixelFormat)
{
    if ( ops == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ops->pixelFormat = ParsePixelForamt(pixelFormat);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetAlphaType(OH_Pixelmap_InitializationOptions *ops,
    int32_t *alphaType)
{
    if ( ops == nullptr || alphaType == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *alphaType = static_cast<int32_t>(ops->alphaType);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetAlphaType(OH_Pixelmap_InitializationOptions *ops,
    int32_t alphaType)
{
    if ( ops == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ops->alphaType = ParseAlphaType(alphaType);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ReleaseInitializationOptions(OH_Pixelmap_InitializationOptions *ops)
{
    if ( ops == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    delete ops;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_CreateImageInfo(OH_Pixelmap_ImageInfo **info)
{
    *info = new OH_Pixelmap_ImageInfo();
    if (*info == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ImageInfoGetWidth(OH_Pixelmap_ImageInfo *infos, uint32_t *width)
{
    if ( infos == nullptr || width == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *width = infos->width;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ImageInfoGetHeight(OH_Pixelmap_ImageInfo *infos, uint32_t *height)
{
    if ( infos == nullptr || height == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *height = infos->height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ImageInfoGetPixelRowStride(OH_Pixelmap_ImageInfo *infos, uint32_t *rowStride)
{
    if ( infos == nullptr || rowStride == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *rowStride = infos->rowStride;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ImageInfoGetPixelFormat(OH_Pixelmap_ImageInfo *infos, int32_t *pixelFormat)
{
    if ( infos == nullptr || pixelFormat == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *pixelFormat = infos->pixelFormat;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ImageInfoGetAlphaType(OH_Pixelmap_ImageInfo *infos, int32_t *alphaType)
{
    if ( infos == nullptr || alphaType == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *alphaType = static_cast<int32_t>(infos->alphaType);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ReleaseImageInfo(OH_Pixelmap_ImageInfo *infos)
{
    if ( infos == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    delete infos;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_CreatePixelMap(uint8_t *data, size_t colorLength,
    OH_Pixelmap_InitializationOptions *opts, OH_Pixelmap **pixelmap)
{
    if (data == nullptr || opts == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    InitializationOptions info;
    info.editable = true;
    info.alphaType = static_cast<AlphaType>(opts->alphaType);
    info.pixelFormat = static_cast<PixelFormat>(opts->pixelFormat);
    info.size.height = opts->height;
    info.size.width = opts->width;

    auto pixelMap2 = new OH_Pixelmap(reinterpret_cast<uint32_t*>(data), static_cast<uint32_t>(colorLength), info);
    if (pixelMap2 == nullptr || pixelMap2->GetInnerPixelMap() == nullptr) {
        if (pixelMap2) {
            delete pixelMap2;
        }
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    *pixelmap = pixelMap2;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_ReadPixels(OH_Pixelmap *pixelmap, uint8_t *destination, size_t *bufferSize)
{
    if (pixelmap == nullptr || destination == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return static_cast<Image_ErrorCode>(pixelmap->GetInnerPixelMap()->ReadPixels(*bufferSize, destination));
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_WritePixels(OH_Pixelmap *pixelmap, uint8_t *source, size_t *bufferSize)
{
    if (pixelmap == nullptr || source == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return static_cast<Image_ErrorCode>(pixelmap->GetInnerPixelMap()->WritePixels(source, *bufferSize));
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_GetImageInfo(OH_Pixelmap *pixelmap, OH_Pixelmap_ImageInfo *imageInfo)
{
    if (pixelmap == nullptr || imageInfo == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageInfo srcInfo;
    pixelmap->GetInnerPixelMap()->GetImageInfo(srcInfo);
    imageInfo->width = srcInfo.size.width;
    imageInfo->height = srcInfo.size.height;
    imageInfo->rowStride = pixelmap->GetInnerPixelMap()->GetRowStride();
    imageInfo->pixelFormat = static_cast<int32_t>(srcInfo.pixelFormat);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_Opacity(OH_Pixelmap *pixelmap, float rate)
{
    if (pixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelmap->GetInnerPixelMap()->SetAlpha(rate);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_Scale(OH_Pixelmap *pixelmap, float x, float y)
{
    if (pixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelmap->GetInnerPixelMap()->scale(x, y);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_Translate(OH_Pixelmap *pixelmap, float x, float y)
{
    if (pixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelmap->GetInnerPixelMap()->translate(x, y);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_Rotate(OH_Pixelmap *pixelmap, float angle)
{
    if (pixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelmap->GetInnerPixelMap()->rotate(angle);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_Flip(OH_Pixelmap *pixelmap, bool horizontal, bool vertical)
{
    if (pixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelmap->GetInnerPixelMap()->flip(horizontal, vertical);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_Crop(OH_Pixelmap *pixelmap, Image_Region *region)
{
    if (pixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    Rect rect;
    rect.left = region->x;
    rect.top = region->y;
    rect.width = region->width;
    rect.height = region->height;
    pixelmap->GetInnerPixelMap()->crop(rect);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Pixelmap2_Release(OH_Pixelmap *pixelmap)
{
    if (pixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if(pixelmap->IsLockPixelMap())
    {
        return IMAGE_RESULT_MEDIA_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    pixelmap->~OH_Pixelmap();
    return IMAGE_RESULT_SUCCESS;
}

#ifdef __cplusplus
};
#endif