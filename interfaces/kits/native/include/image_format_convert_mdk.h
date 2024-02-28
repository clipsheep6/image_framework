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
 * @brief Provides native APIs to convert image format.
 *
 * @Syscap SystemCapability.Multimedia.Image
 */

/**
 * @file image_format_convert_mdk.h
 *
 * @brief Declares APIs to convert image fromat.
 *
 * @Syscap SystemCapability.Multimedia.Image
 */

#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_FORMAT_CONVERT_MDK_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_FORMAT_CONVERT_MDK_H

#include "napi/native_api.h"
#include "image_mdk_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines a native image format convert object for the image format convert APIs.
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 12
 */
struct ImageFormatConvertNative_;
/**
 * @brief Defines a native image format convert object for the image format convert APIs.
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 12
 */
typedef struct ImageFormatConvertNative_ ImageFormatCovnertNative;
ImageFormatCovnertNative* OH_ImageCovnertNative_InitNative();
int32_t OH_ImageCovnertNative_ReleaseNative(ImageFormatCovnertNative* native);
/**
 * @brief Creates an <b>ImageFormatConvert</b> object at the JavaScript native layer.
 *
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 */
int32_t OH_ImageConvert_Create();

/**
 * @brief Convert image format from RGB to YUV by PixelMap.
 *
 * @param srcPixelMap Source Image PixelMap
 * @param destPixelMap Destination Image PixelMap
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 *
 * @Syscap SystemCapability.Multimedia.Image
 */
int32_t OH_ImageConvert_YuvToRgb(ImageFormatCovnertNative *srcPixelMap, ImageFormatCovnertNative *destPixelMap,
                                 int32_t destPixelFormat);

/**
 * @brief Convert image format from RGB to YUV by PixelMap.
 *
 * @param srcPixelMap Source Image PixelMap
 * @param destPixelMap Destination Image PixelMap
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 *
 * @Syscap SystemCapability.Multimedia.Image
 */
int32_t OH_ImageConvert_RgbToYuv(ImageFormatCovnertNative *srcPixelMap, ImageFormatCovnertNative *destPixelMap,
                                 int32_t destPixelFormat);

/**
 * @brief Releases an <b>ImageFormatConvertNative</b> object.
 *
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 10
 * @version 4.0
 */
int32_t OH_ImageConvert_Release();

/**
 * @brief Convert Js objects to C pointers.
 **@param env Indicates the pointer to the JavaScript Native Interface (JNI) environment
 * @param pixelMapValue Indicates a JavaScript native API <b>PixelMapNapi</b> object
 * @param pixelMap Indicates the result after conversion
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 12
 * @version 4.0
 */
int32_t OH_ImageConvert_JsToC_PixelMap(napi_env env, napi_value pixelMapValue, ImageFormatCovnertNative *pixelMap);

/**
 * @brief Convert Js objects to C pointers.
 **@param env Indicates the pointer to the JavaScript Native Interface (JNI) environment
 * @param pixelMap Indicates a C native API <b>PixelMap</b> object pointer
 * @param result Indicates the result after conversion
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 *
 * @Syscap SystemCapability.Multimedia.Image
 * @since 12
 * @version 4.0
 */
int32_t OH_ImageConvert_CToJs_PixelMap(napi_env env, ImageFormatCovnertNative *pixelMap, napi_value *result);

#ifdef __cplusplus
};
#endif
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_FORMAT_CONVERT_MDK_H