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
#include <cstdint>
#include "pixel_map.h"
#include "image_mdk_common.h"

#ifdef __cplusplus
extern "C" {
#endif

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
int32_t OH_ImageConvert_YuvToRgb(OHOS::Media::PixelMap *srcPixelMap, OHOS::Media::PixelMap **destPixelMap,
                                 OHOS::Media::PixelFormat destPixelFormat);

/**
 * @brief Convert image format from RGB to YUV by PixelMap.
 *
 * @param srcPixelMap Source Image PixelMap
 * @param destPixelMap Destination Image PixelMap
 * @return Returns {@link IRNdkErrCode} IMAGE_RESULT_SUCCESS - if the operation is successful.
 *
 * @Syscap SystemCapability.Multimedia.Image
 */
int32_t OH_ImageConvert_RgbToYuv(OHOS::Media::PixelMap *srcPixelMap, OHOS::Media::PixelMap **destPixelMap,
                                 OHOS::Media::PixelFormat destPixelFormat);

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

#ifdef __cplusplus
};
#endif
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_FORMAT_CONVERT_MDK_H