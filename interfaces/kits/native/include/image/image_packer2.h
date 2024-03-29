/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
 * @Syscap SystemCapability.Multimedia.Image.ImagePacker
 * @since 10
 * @version 2.0
 */

/**
 * @file image_packer2.h
 *
 * @brief Declares functions that access the image rectangle, size, format, and component data.
 * Need link <b>libimagendk.z.so</b>
 *
 * @since 10
 * @version 2.0
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_PACKER2_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_PACKER2_H
#include "image_common.h"
#include "image_source2.h"
#include "pixelmap2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OH_ImagePacker OH_ImagePacker;

/**
 * @brief Defines the image packing options.
 *
 * @since 11
 * @version 4.1
 */
typedef struct OH_ImagePacker_PackingOptions OH_ImagePacker_PackingOptions;

/**
 * @brief Create a point for PackingOptions struct.
 *
 * @param opts The PackingOptions pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImagePacker2_CreatePackingOptions
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImagePacker2_CreatePackingOptions(OH_ImagePacker_PackingOptions **opts);

/**
 * @brief Get format number for DecodingOptions struct.
 *
 * @param opts The DecodingOptions pointer will be operated.
 * @param format the number of image format.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImagePacker2_PackingOptionsGetMimeType
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImagePacker2_PackingOptionsGetMimeType(OH_ImagePacker_PackingOptions *opts, Image_MimeType *format);

/**
 * @brief Set format number for DecodingOptions struct.
 *
 * @param opts The DecodingOptions pointer will be operated.
 * @param format the number of image format.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImagePacker2_PackingOptionsSetMimeType
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImagePacker2_PackingOptionsSetMimeType(OH_ImagePacker_PackingOptions *opts, Image_MimeType format);

/**
 * @brief Get quality number for DecodingOptions struct.
 *
 * @param opts The DecodingOptions pointer will be operated.
 * @param quality the number of image quality.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImagePacker2_PackingOptionsGetQuality
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImagePacker2_PackingOptionsGetQuality(OH_ImagePacker_PackingOptions *opts, uint32_t *quality);

/**
 * @brief Set quality number for DecodingOptions struct.
 *
 * @param opts The DecodingOptions pointer will be operated.
 * @param quality the number of image quality.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImagePacker2_PackingOptionsSetQuality
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImagePacker2_PackingOptionsSetQuality(OH_ImagePacker_PackingOptions *opts, uint32_t quality);

/**
 * @brief delete DecodingOptions pointer.
 *
 * @param opts The DecodingOptions pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImagePacker2_ReleasePackingOptions
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImagePacker2_ReleasePackingOptions(OH_ImagePacker_PackingOptions *opts);

/**
 * @brief Create a point for OH_ImagePacker struct.
 *
 * @param opts The OH_ImagePacker pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImagePacker2_Create
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImagePacker2_Create(OH_ImagePacker **imagePacker);

/**
 * @brief Encoding an <b>ImageSource</b> into the data with required format.
 *
 * @param imagePacker The imagePacker to use for packing.
 * @param option Indicates the encoding {@link ImagePacker_Opts_} .
 * @param imageSource The imageSource to be packed.
 * @param outData The output data buffer to store the packed image.
 * @param size A pointer to the size of the output data buffer.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImagePacker2_PackToDataFromImageSource}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImagePacker2_PackToDataFromImageSource(OH_ImagePacker* imagePacker, OH_ImagePacker_PackingOptions* option,
    OH_ImageSource* imageSource, uint8_t* outData, size_t* size);

/**
 * @brief Encoding a <b>PixelMap</b> into the data with required format.
 *
 * @param imagePacker The imagePacker to use for packing.
 * @param option Indicates the encoding {@link ImagePacker_Opts_} .
 * @param pixelMap The pixelMap to be packed.
 * @param outData The output data buffer to store the packed image.
 * @param size A pointer to the size of the output data buffer.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImagePacker2_PackToDataFromPixelMap}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImagePacker2_PackToDataFromPixelMap(OH_ImagePacker* imagePacker, OH_ImagePacker_PackingOptions* option,
    OH_Pixelmap* pixelMap, uint8_t* outData, size_t* size);

/**
 * @brief Encoding an <b>ImageSource</b> into the a file with fd with required format.
 *
 * @param imagePacker The image packer to use for packing.
 * @param option Indicates the encoding {@link ImagePacker_Opts_} .
 * @param imageSource The imageSource to be packed.
 * @param fd Indicates the a writable file descriptor.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImagePacker2_PackToFileFromImageSource}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImagePacker2_PackToFileFromImageSource(OH_ImagePacker* imagePacker, OH_ImagePacker_PackingOptions* option,
    OH_ImageSource* imageSource, int32_t fd);

/**
 * @brief Encoding a <b>PixelMap</b> into the a file with fd with required format
 *
 * @param imagePacker The image packer to use for packing.
 * @param option Indicates the encoding {@link ImagePacker_Opts_} .
 * @param pixelMap The pixelMap to be packed.
 * @param fd Indicates the a writable file descriptor.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImagePacker2_PackToFileFromPixelMap}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImagePacker2_PackToFileFromPixelMap(OH_ImagePacker* imagePacker, OH_ImagePacker_PackingOptions* option,
    OH_Pixelmap* pixelMap, int32_t fd);

/**
 * @brief Releases an imagePacker object.
 *
 * @param imagePacker A pointer to the image packer object to be released.  
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImagePacker2_Release}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImagePacker2_Release(OH_ImagePacker *imagePacker);

#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_PACKER2_H