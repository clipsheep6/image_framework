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
 * @Syscap SystemCapability.Multimedia.Image.Core
 * @since 10
 * @version 2.0
 */

/**
 * @file pixelmap2.h
 *
 * @brief Declares functions that access the image rectangle, size, format, and component data.
 * Need link <b>libimagendk.z.so</b>
 *
 * @since 10
 * @version 2.0
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PIXELMAP2_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PIXELMAP2_H
#include "image_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define a PixelMap struct type, used for pixelmap pointer controls.
 *
 * @since 10
 * @version 2.0
 */
typedef struct OH_Pixelmap OH_Pixelmap;

/**
 * @brief Define a pixelmap alpha type.
 *
 * @since 10
 * @version 2.0
 */
typedef enum {
    /*
    * Unknown format
    */
    PIXELMAP_ALPHA_TYPE_UNKNOWN = 0,
     /*
    * Opaque format
    */
    PIXELMAP_ALPHA_TYPE_OPAQUE = 1,
     /*
    * Premultiplied format
    */
    PIXELMAP_ALPHA_TYPE_PREMUL = 2,
     /*
    * Unpremultiplied format
    */
    PIXELMAP_ALPHA_TYPE_UNPEMUL = 3,
}PIXELMAP_ALPHA_TYPE;

typedef enum {
    UNKNOWN = 0,
    RGB_565 = 2,
    RGBA_8888 = 3,
    BGRA_8888 = 4,
    RGB_888 = 5,
    ALPHA_8 = 6,
    RGBA_F16 = 7,
    NV21 = 8,
    NV12 = 9,
} PIXEL_FORMAT;

/**
 * @brief Defines the options used for creating a pixel map.
 *
 * @since 10
 * @version 2.0
 */
typedef struct OH_Pixelmap_InitializationOptions OH_Pixelmap_InitializationOptions;

/**
 * @brief Create a point for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_CreateInitializationOptions
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_CreateInitializationOptions(OH_Pixelmap_InitializationOptions **ops);

/**
 * @brief Get width number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param width the number of image width.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsGetWidth
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetWidth(OH_Pixelmap_InitializationOptions *ops,
    uint32_t *width);

/**
 * @brief Set width number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param width the number of image width.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsSetWidth
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetWidth(OH_Pixelmap_InitializationOptions *ops, uint32_t width);

/**
 * @brief Get height number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param height the number of image height.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsGetHeight
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetHeight(OH_Pixelmap_InitializationOptions *ops,
    uint32_t *height);

/**
 * @brief Set height number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param height the number of image height.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsSetHeight
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetHeight(OH_Pixelmap_InitializationOptions *ops,
    uint32_t height);

/**
 * @brief Get pixelFormat number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param pixelFormat the number of image pixelFormat.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsGetPixelFormat
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetPixelFormat(OH_Pixelmap_InitializationOptions *ops,
    int32_t *pixelFormat);

/**
 * @brief Set pixelFormat number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param pixelFormat the number of image pixelFormat.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsSetPixelFormat
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetPixelFormat(OH_Pixelmap_InitializationOptions *ops,
    int32_t pixelFormat);

/**
 * @brief Get alphaType number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param alphaType the number of image alphaType.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsGetAlphaType
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsGetAlphaType(OH_Pixelmap_InitializationOptions *ops,
    int32_t *alphaType);

/**
 * @brief Set alphaType number for InitializationOtions struct.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @param alphaType the number of image alphaType.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_InitializationOptionsSetAlphaType
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_InitializationOptionsSetAlphaType(OH_Pixelmap_InitializationOptions *ops,
    int32_t alphaType);

/**
 * @brief delete InitializationOtions pointer.
 *
 * @param ops The InitializationOtions pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_ReleaseInitializationOptions
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ReleaseInitializationOptions(OH_Pixelmap_InitializationOptions *ops);

/**
 * @brief Defines the pixel map information.
 *
 * @since 10
 * @version 2.0
 */
typedef struct OH_Pixelmap_ImageInfo OH_Pixelmap_ImageInfo;

/**
 * @brief Create imageinfo struct point.
 *
 * @param info The imageinfo pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_CreateImageInfo
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_CreateImageInfo(OH_Pixelmap_ImageInfo **info);

/**
 * @brief Get width number for imageinfo struct.
 *
 * @param info The imageinfo pointer will be operated.
 * @param width The number of imageinfo width.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_CreateImageInfo
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ImageInfoGetWidth(OH_Pixelmap_ImageInfo *infos, uint32_t *width);

/**
 * @brief Get height number for imageinfo struct.
 *
 * @param info The imageinfo pointer will be operated.
 * @param height The number of imageinfo height.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_ImageInfoGetHeight
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ImageInfoGetHeight(OH_Pixelmap_ImageInfo *infos, uint32_t *height);

/**
 * @brief Get rowStride number for imageinfo struct.
 *
 * @param info The imageinfo pointer will be operated.
 * @param rowStride The number of imageinfo rowStride.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_ImageInfoGetPixelRowStride
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ImageInfoGetPixelRowStride(OH_Pixelmap_ImageInfo *infos, uint32_t *rowStride);

/**
 * @brief Get pixelFormat number for imageinfo struct.
 *
 * @param info The imageinfo pointer will be operated.
 * @param pixelFormat The number of imageinfo pixelFormat.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_ImageInfoGetPixelFormat
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ImageInfoGetPixelFormat(OH_Pixelmap_ImageInfo *infos, int32_t *pixelFormat);

/**
 * @brief Get density number for imageinfo struct.
 *
 * @param info The imageinfo pointer will be operated.
 * @param density The number of imageinfo density.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_ImageInfoGetDensity
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ImageInfoGetAlphaType(OH_Pixelmap_ImageInfo *infos, int32_t *alphaType);

/**
 * @brief Delete imageinfo struct point.
 *
 * @param info The imageinfo pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_ReleaseImageInfo
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ReleaseImageInfo(OH_Pixelmap_ImageInfo *infos);

/**
 * @brief Creates a <b>PixelMap</b> object.
 *
 * @param data Color buffer in BGRA_8888 format.
 * @param colorLength Color buffer size in BGRA_8888 format.
 * @param opts IPixel properties, including the alpha type, size, scale mode, pixel format, and editable.
 * @param pixelMap PixelMap pointer for created.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_CreatePixelMap
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_CreatePixelMap(uint8_t *data, size_t colorLength,
    OH_Pixelmap_InitializationOptions *opts, OH_Pixelmap **pixelMap);

/**
 * @brief Reads data of this pixel map and writes the data to an Buffer. If this pixel map is created in
 *     the BGRA_8888 format, the data read is the same as the original data.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param buffer Buffer size to which the image pixel map data will be written.
 * @param bufferSize Buffer to which the image pixel map data will be written.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_ReadBuffer
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_ReadPixels(OH_Pixelmap *pixelMap, uint8_t *destination, size_t *bufferSize);

/**
 * @brief Reads image data in an ArrayBuffer and writes the data to a PixelMap object.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param source Buffer from which the image data will be read.
 * @param bufferSize Buffer size from which the image data will be read.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_PixelMapCapi_WriteBufferToPixels
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_WritePixels(OH_Pixelmap *pixelMap, uint8_t *source, size_t *bufferSize);

/**
 * @brief Obtains pixel map information of this image.
 *
* @param pixelMap The PixelMap pointer will be operated.
 * @param imageInfo Indicates the pointer to the image information.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_GetImageInfo
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_GetImageInfo(OH_Pixelmap *pixelMap, OH_Pixelmap_ImageInfo *imageInfo);

/**
 * @brief Sets an opacity rate for this image pixel map.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param density Opacity rate to set. The value ranges from 0 to 1.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_Opacity
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_Opacity(OH_Pixelmap *pixelMap, float rate);

/**
 * @brief Scales this image based on the input width and height.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param x Scaling ratio of the width.
 * @param y Scaling ratio of the height.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_Scale
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_Scale(OH_Pixelmap *pixelMap, float x, float y);

/**
 * @brief Translates this image based on the input coordinates.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param x X coordinate to translate.
 * @param y Y coordinate to translate.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_Translate
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_Translate(OH_Pixelmap *pixelMap, float x, float y);

/**
 * @brief Rotates this image based on the input angle.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param angle Angle to rotate.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_Rotate
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_Rotate(OH_Pixelmap *pixelMap, float angle);

/**
 * @brief Flips this image horizontally or vertically, or both.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param horizontal Whether to flip the image horizontally.
 * @param vertical Whether to flip the image vertically.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_Flip
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_Flip(OH_Pixelmap *pixelMap, bool horizontal, bool vertical);

/**
 * @brief Crops this image based on the input size.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @param region Area size, read according to area.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_Crop
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_Crop(OH_Pixelmap *pixelMap, Image_Region *region);

/**
 * @brief Releases an <b>OH_Pixelmap</b> object.
 *
 * @param pixelMap The PixelMap pointer will be operated.
 * @return Returns {@link Image_ErrorCode}
 * @see OH_Pixelmap2_Release
 * @since 10
 * @version 1.0
 */
Image_ErrorCode OH_Pixelmap2_Release(OH_Pixelmap *pixelMap);


#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PIXELMAP2_H
