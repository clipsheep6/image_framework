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

/**
 * @addtogroup image
 * @{
 *
 * @brief Provides APIs for access to the image interface.
 *
 * @Syscap SystemCapability.Multimedia.Image.Core
 * @since 12
 */

/**
 * @file image.h
 *
 * @brief Declares functions that access the image rectangle, size, format, and component data.
 * Need link <b>libimage_native.z.so</b>
 *
 * @since 12
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_H

#include "image_common.h"
#include "native_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines an <b>OH_Image</b> object.
 *
 * @since 12
 */
struct OH_Image;

/**
 * @brief Defines the data type name of a native image.
 *
 * @since 12
 */
typedef struct OH_Image OH_Image;

/**
 * @brief Obtains {@link Image_Size} of an {@link OH_Image} object.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @param size Indicates the pointer to the {@link Image_Size} object obtained.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_MEDIA_DEAD_OBJECT - if media object dead.
 * @since 12
 */
Image_ErrorCode OH_Image2_GetImageSize(OH_Image* image, Image_Size* size);

/**
 * @brief Obtains the image format of an {@link OH_Image} object.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @param format Indicates the pointer to the image format obtained.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_MEDIA_DEAD_OBJECT - if media object dead.
 * @since 12
 */
Image_ErrorCode OH_Image2_GetFormat(OH_Image* image, uint32_t* format);

/**
 * @brief Get type arry from an {@link OH_Image} object.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @param types Indicates the pointer to an {@link OH_Image} component arry obtained.
 * @param typeSize Indicates the pointer to the {@link OH_Image} component arry size obtained.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * @since 12
 */
Image_ErrorCode OH_Image2_GetComponentTypes(OH_Image* image, uint32_t** types, size_t* typeSize);

/**
 * @brief Get byte buffer from an {@link OH_Image} object by the component type.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @param componentType Indicates the type of component.
 * @param nativeBuffer Indicates the pointer to the component buffer obtained.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * @since 12
 */
Image_ErrorCode OH_Image2_GetByteBuffer(OH_Image* image, uint32_t componentType, OH_NativeBuffer** nativeBuffer);

/**
 * @brief Get size of buffer from an {@link OH_Image} object by the component type.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @param componentType Indicates the type of component.
 * @param size Indicates the pointer to the size of buffer obtained.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * @since 12
 */
Image_ErrorCode OH_Image2_GetBufferSize(OH_Image* image, uint32_t componentType, size_t* size);

/**
 * @brief Get row stride from an {@link OH_Image} object by the component type.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @param componentType Indicates the type of component.
 * @param rowStride Indicates the pointer to the row stride obtained.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * @since 12
 */
Image_ErrorCode OH_Image2_GetRowStride(OH_Image* image, uint32_t componentType, int32_t* rowStride);

/**
 * @brief Get pixel stride from an {@link OH_Image} object by the component type.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @param componentType Indicates the type of component.
 * @param pixelStride Indicates the pointer to the pixel stride obtained.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * @since 12
 */
Image_ErrorCode OH_Image2_GetPixelStride(OH_Image* image, uint32_t componentType, int32_t* pixelStride);

/**
 * @brief Releases an {@link OH_Image} object.
 * It is used to release the object {@link OH_Image}.
 *
 * @param image Indicates the pointer to an {@link OH_Image} object.
 * @return Returns {@link Image_ErrorCode} IMAGE_ERRORCODE_SUCCESS - if the operation is successful.
 * returns {@link Image_ErrorCode} IMAGE_ERRORCODE_INVALID_PARAMETER - if invalid parameter.
 * @since 12
 */
Image_ErrorCode OH_Image2_Release(OH_Image* image);

#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_H
