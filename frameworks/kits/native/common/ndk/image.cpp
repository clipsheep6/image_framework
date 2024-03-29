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

#include <inttypes.h>
#include "common_utils.h"
#include "image_log.h"
#include "image.h"
#include "image_kits.h"

#ifdef __cplusplus
extern "C" {
#endif

MIDK_EXPORT
Image_ErrorCode OH_Image2_GetImageSize(OH_Image* image, Image_Size* size)
{
    if (nullptr == image || nullptr == image->imgNative || nullptr == size) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p, size=0x%{public}p", image, size);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    return (Image_ErrorCode)image->imgNative->GetSize(size->width, size->height);
}

MIDK_EXPORT
Image_ErrorCode OH_Image2_GetFormat(OH_Image* image, uint32_t* format)
{
    if (nullptr == image || nullptr == image->imgNative || nullptr == format) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p, format=0x%{public}p", image, format);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    int32_t fmt = 0;
    Image_ErrorCode err = (Image_ErrorCode)image->imgNative->GetFormat(fmt);
    *format = fmt;
    return err;
}

MIDK_EXPORT
Image_ErrorCode OH_Image2_GetComponentTypes(OH_Image* image, uint32_t** types, size_t* typeSize)
{
    if (nullptr == image || nullptr == image->imgNative || nullptr == typeSize) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p, typeSize=0x%{public}p", image, typeSize);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    auto& components = image->imgNative->GetComponents();
    *typeSize = components.size();
    if (nullptr == types) {
        IMAGE_LOGE("OH_Image get data failed.");
        return IMAGE_ERRORCODE_SUCCESS;
    }

    uint32_t* p = *types;
    for (auto itor = components.begin(); itor != components.end(); ++itor) {
        *p = itor->first;
        p++;
    }

    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Image2_GetByteBuffer(OH_Image* image, uint32_t componentType, OH_NativeBuffer** nativeBuffer)
{
    if (nullptr == image || nullptr == image->imgNative || nullptr == nativeBuffer) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p, nativeBuffer=0x%{public}p", image, nativeBuffer);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    auto component = image->imgNative->GetComponent(componentType);
    if (nullptr == component) {
        IMAGE_LOGE("OH_Image get data failed.");
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    auto buffer = image->imgNative->GetBuffer();
    if (buffer != nullptr) {
        *nativeBuffer = buffer->SurfaceBufferToNativeBuffer();
    }
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Image2_GetBufferSize(OH_Image* image, uint32_t componentType, size_t* size)
{
    if (nullptr == image || nullptr == image->imgNative || nullptr == size) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p, size=0x%{public}p", image, size);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    auto component = image->imgNative->GetComponent(componentType);
    if (nullptr == component) {
        IMAGE_LOGE("OH_Image get data failed.");
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    *size = component->size;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Image2_GetRowStride(OH_Image* image, uint32_t componentType, int32_t* rowStride)
{
    if (nullptr == image || nullptr == image->imgNative || nullptr == rowStride) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p, rowStride=0x%{public}p", image, rowStride);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    auto component = image->imgNative->GetComponent(componentType);
    if (nullptr == component) {
        IMAGE_LOGE("OH_Image get data failed.");
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    *rowStride = component->rowStride;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Image2_GetPixelStride(OH_Image* image, uint32_t componentType, int32_t* pixelStride)
{
    if (nullptr == image || nullptr == image->imgNative || nullptr == pixelStride) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p, pixelStride=0x%{public}p", image, pixelStride);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    auto component = image->imgNative->GetComponent(componentType);
    if (nullptr == component) {
        IMAGE_LOGE("OH_Image get data failed.");
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    *pixelStride = component->pixelStride;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_Image2_Release(OH_Image* image)
{
    if (nullptr == image) {
        IMAGE_LOGE("Invalid parameter: image=0x%{public}p", image);
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr != image->imgNative) {
        delete image->imgNative;
    }
    IMAGE_LOGI("OH_Image 0x%{public}p has been deleted.", image);
    delete image;
    return IMAGE_ERRORCODE_SUCCESS;
}

#ifdef __cplusplus
};
#endif
