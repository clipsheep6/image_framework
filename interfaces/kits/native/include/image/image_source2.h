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
 * @Syscap SystemCapability.Multimedia.Image.ImageSource
 * @since 10
 * @version 2.0
 */

/**
 * @file image_source2.h
 *
 * @brief Declares functions that access the image rectangle, size, format, and component data.
 * Need link <b>libimagendk.z.so</b>
 *
 * @since 10
 * @version 2.0
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_SOURCE2_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_SOURCE2_H
#include "image_common.h"

#include "pixelmap2.h"
#include "raw_file.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines an image source object for the image interface.
 *
 * @since 10
 * @version 2.0
 */
typedef struct OH_ImageSource OH_ImageSource;

/**
 * @brief Defines image source infomation
 * {@link OH_ImageSource2_CreateImageInfo} .
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 10
 * @version 4.0
 */
typedef struct OH_ImageSource_ImageInfo OH_ImageSource_ImageInfo;

/**
 * @brief Create a point for OH_ImageSource_ImageInfo struct.
 *
 * @param info The OH_ImageSource_ImageInfo pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_CreateImageInfo
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_CreateImageInfo(OH_ImageSource_ImageInfo **info);

/**
 * @brief Get width number for OH_ImageSource_ImageInfo struct.
 *
 * @param info The DecodingOptions pointer will be operated.
 * @param width the number of image width.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_ImageInfoGetWidth
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_ImageInfoGetWidth(OH_ImageSource_ImageInfo *info, uint32_t *width);

/**
 * @brief Get height number for OH_ImageSource_ImageInfo struct.
 *
 * @param info The DecodingOptions pointer will be operated.
 * @param height the number of image height.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_ImageInfoGetHeight
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_ImageInfoGetHeight(OH_ImageSource_ImageInfo *info, uint32_t *height);

/**
 * @brief delete OH_ImageSource_ImageInfo pointer.
 *
 * @param info The OH_ImageSource_ImageInfo pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_ReleaseImageInfo
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_ReleaseImageInfo(OH_ImageSource_ImageInfo *info);

/**
 * @brief Defines the options for decoding the image source.
 * It is used in {@link OH_ImageSource2_Create} and {@link OH_ImageSource2_CreatePixelMapList}.
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 10
 * @version 4.0
 */
typedef struct OH_ImageSource_DecodingOptions OH_ImageSource_DecodingOptions;

/**
 * @brief Create a point for InitializationOtions struct.
 *
 * @param opts The DecodingOptions pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_CreateDecodingOptions
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_CreateDecodingOptions(OH_ImageSource_DecodingOptions **opts);

/**
 * @brief Get pixelFormat number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param pixelFormat the number of image pixelFormat.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsGetPixelFormat
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetPixelFormat(OH_ImageSource_DecodingOptions *opts,
    int32_t *pixelFormat);

/**
 * @brief Set pixelFormat number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param pixelFormat the number of image pixelFormat.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsSetPixelFormat
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetPixelFormat(OH_ImageSource_DecodingOptions *opts,
    int32_t pixelFormat);

/**
 * @brief Get index number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param index the number of image index.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsGetFitDensity
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetIndex(OH_ImageSource_DecodingOptions *opts, uint32_t *index);

/**
 * @brief Set index number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param index the number of image index.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsSetFitDensity
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetIndex(OH_ImageSource_DecodingOptions *opts, uint32_t index);

/**
 * @brief Get rotate number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param rotate the number of image rotate.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsGetFitDensity
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetRotate(OH_ImageSource_DecodingOptions *opts, float *rotate);

/**
 * @brief Set rotate number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param rotate the number of image rotate.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsSetFitDensity
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetRotate(OH_ImageSource_DecodingOptions *opts, float rotate);

/**
 * @brief Get desiredSize number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param desiredSize the number of image desiredSize.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsGetDesiredSize
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetDesiredSize(OH_ImageSource_DecodingOptions *opts,
    Image_Size *desiredSize);

/**
 * @brief Set desiredSize number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param desiredSize the number of image desiredSize.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsSetDesiredSize
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetDesiredSize(OH_ImageSource_DecodingOptions *opts,
    Image_Size *desiredSize);

/**
 * @brief Set desiredRegion number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param desiredRegion the number of image desiredRegion.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsGetDesiredRegion
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetDesiredRegion(OH_ImageSource_DecodingOptions *opts,
    Image_Region *desiredRegion);

/**
 * @brief Set desiredRegion number for DecodingOptions struct.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @param desiredRegion the number of image desiredRegion.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_DecodingOptionsSetDesiredRegion
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetDesiredRegion(OH_ImageSource_DecodingOptions *opts,
    Image_Region *desiredRegion);

/**
 * @brief delete DecodingOptions pointer.
 *
 * @param ops The DecodingOptions pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_ImageSource2_ReleaseDecodingOptions
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_ImageSource2_ReleaseDecodingOptions(OH_ImageSource_DecodingOptions *opts);

/**
 * @brief Creates an void pointer
 * image source URI and {@link OH_ImageSource2_SourceOptions} structs.
 *
 * @param uri Indicates a pointer to the image source URI. Only a file URI or Base64 URI is accepted.
 * @param size Indicates the length of the image source URI.:q
 * 
 * @param ops Indicates a pointer to the options for creating the image source.
 * For details, see {@link OhosImageSourceOps}.
 * @param res Indicates a pointer to the <b>ImageSource</b> object created at the C++ native layer.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImageSource2_CreateFromUri}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImageSource2_CreateFromUri(char* uri, size_t uriSize, OH_ImageSource** res);

/**
 * @brief Creates an void pointer
 * image source file descriptor and {@link OH_ImageSource2_SourceOptions} structs.
 *
 * @param fd Indicates the image source file descriptor.
 * @param ops Indicates a pointer to the options for creating the image source.
 * For details, see {@link OH_ImageSource2_SourceOptions}.
 * @param res Indicates a void pointer to the <b>ImageSource</b> object created at the C++ native layer.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImageSource2_CreateFromFd}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImageSource2_CreateFromFd(int32_t fd, OH_ImageSource** res);

/**
 * @brief Creates an void pointer
 * image source data and {@link OH_ImageSource2_SourceOptions} structs.
 *
 * @param data Indicates a pointer to the image source data. Only a formatted packet data or Base64 data is accepted.
 * @param dataSize Indicates the size of the image source data.
 * @param ops Indicates a pointer to the options for creating the image source.
 * For details, see {@link OhosImageSourceOps}.
 * @param res Indicates a void pointer to the <b>ImageSource</b> object created at the C++ native layer.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OH_ImageSource2_CreateFromData}
 * @since 11
 * @version 4.1
 */
Image_ErrorCode OH_ImageSource2_CreateFromData(uint8_t* data, size_t dataSize, OH_ImageSource** res);

/**
 * @brief Creates an void pointer
 * raw file's file descriptor and {@link OH_ImageSource2_SourceOptions} structs.
 *
 * @param rawFile Indicates the raw file's file descriptor.
 * @param ops Indicates a pointer to the options for creating the image source.
 * For details, see {@link OhosImageSourceOps}.
 * @param res Indicates a void pointer to the <b>ImageSource</b> object created at the C++ native layer.
 * @return Returns {@link Image_ErrorCode}
 * @see {@link OhosImageSourceOps}
 * @since 11
 * @version 4.1
 */ 
Image_ErrorCode OH_ImageSource2_CreateFromRawFile(RawFileDescriptor rawFile, OH_ImageSource** res);

/**
 * @brief Decodes an void pointer
 * based on the specified {@link OH_ImageSource_DecodingOptions} struct.
 *
 * @param source Indicates a void pointer(from ImageSource pointer convert).
 * @param ops Indicates a pointer to the options for decoding the image source.
 * For details, see {@link OhosImageDecodingOps}.
 * @param resPixMap Indicates a void pointer to the <b>PixelMap</b> object obtained at the C++ native layer.
 * @return Returns {@link Image_ErrorCode}
 * @since 10
 * @version 4.0
 */ 
Image_ErrorCode OH_ImageSource2_CreatePixelMap(OH_ImageSource* source, OH_ImageSource_DecodingOptions* ops,
    OH_Pixelmap** resPixMap);

/**
 * @brief Decodes an void pointer
 * the <b>PixelMap</b> objects at the C++ native layer
 * based on the specified {@link OH_ImageSource_DecodingOptions} struct.
 *
 * @param source Indicates a void pointer(from ImageSource pointer convert).
 * @param ops Indicates a pointer to the options for decoding the image source.
 * For details, see {@link OhosImageDecodingOps}.
 * @param resVecPixMap Indicates a pointer array to the <b>PixelMap</b> objects obtained at the C++ native layer.
 * @return Returns {@link Image_ErrorCode}
 * @since 10
 * @version 4.0
 */
Image_ErrorCode OH_ImageSource2_CreatePixelMapList(OH_ImageSource* source, OH_ImageSource_DecodingOptions* ops,
        OH_Pixelmap** resVecPixMap[], size_t* outSize);

/**
 * @brief Obtains the delay time list from some <b>ImageSource</b> objects (such as GIF image sources).
 *
 * @param source Indicates a void pointer(from ImageSource pointer convert).
 * @param res Indicates a pointer to the delay time list obtained.
 * When the input <b>delayTimeList</b> is a null pointer and
 * <b>size</b> is <b>0</b>, the size of the delay time list is returned through <b>size</b> in <b>res</b>.
 * To obtain the complete delay time list, a space greater than <b>size</b> is required.
 * @return Returns {@link Image_ErrorCode}
 * @since 10
 * @version 4.0
 */
Image_ErrorCode OH_ImageSource2_GetDelayTime(OH_ImageSource* source, int32_t* delayTimeList, size_t* size);

/**
 * @brief Obtains image source information from an <b>ImageSource</b> object by index.
 *
 * @param source Indicates a void pointer(from ImageSource pointer convert).
 * @param index Indicates the index of the frame.
 * @param info Indicates a pointer to the image source information obtained.
 * For details, see {@link OhosImageSourceInfo}.
 * @return Returns {@link Image_ErrorCode}
 * @since 10
 * @version 4.0
 */
Image_ErrorCode OH_ImageSource2_GetImageInfo(OH_ImageSource* source, int32_t index,
    OH_ImageSource_ImageInfo* info);

/**
 * @brief Obtains the value of an image property from an <b>ImageSource</b> object.
 *
 * @param source Indicates a void pointer(from ImageSource pointer convert).
 * @param key Indicates a pointer to the property. For details, see {@link Image_String}.
 * @param value Indicates a pointer to the property value obtained.
 * If the input <b>value</b> is a null pointer and <b>size</b> is <b>0</b>, the size of the property value is returned
 * through <b>size</b> in <b>value</b>.
 * To obtain the complete property value, a space greater than <b>size</b> is required.
 * @return Returns {@link Image_ErrorCode}
 * @since 10
 * @version 4.0
 */
Image_ErrorCode OH_ImageSource2_GetImageProperty(OH_ImageSource* source, Image_String* key,
    Image_String* value);

/**
 * @brief Modifies the value of an image property of an <b>ImageSource</b> object.
 * @param source Indicates a void pointer(from ImageSource pointer convert).
 * @param key Indicates a pointer to the property. For details, see {@link Image_String}.
 * @param value Indicates a pointer to the new value of the property.
 * @return Returns {@link Image_ErrorCode}
 * @since 10
 * @version 4.0
 */
Image_ErrorCode OH_ImageSource2_ModifyImageProperty(OH_ImageSource* source, Image_String* key,
    Image_String* value);

/**
 * @brief Obtains the number of frames from an <b>ImageSource</b> object.
 *
 * @param source Indicates a pointer to the {@link ImageSourc} object at the C++ native layer.
 * @param res Indicates a pointer to the number of frames obtained.
 * @return Returns {@link Image_ErrorCode}
 * @since 10
 * @version 4.0
 */
Image_ErrorCode OH_ImageSource2_GetFrameCount(OH_ImageSource* source, uint32_t* frameCount);

/**
 * @brief Releases an <b>ImageSourc</b> object.
 *
 * @param source Indicates a void pointer(from ImageSource pointer convert).
 * @return Returns {@link Image_ErrorCode}
 * @see {@link ImageSource} {@link OH_ImageSourceCapi_CreateIncremental}
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 10
 * @version 4.0
 */
Image_ErrorCode OH_ImageSource2_Release(OH_ImageSource* source);

#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_IMAGE_SOURCE2_H