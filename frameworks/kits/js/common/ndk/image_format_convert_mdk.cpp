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

#include "image_format_convert_mdk.h"
#include "image_format_convert_mdk_kits.h"

using namespace OHOS::Media;

#ifdef __cplusplus
extern "C" {
#endif

struct ImageFormatConvertNative_ {
    std::shared_ptr<PixelMap> pixelMap = nullptr;
};

MIDK_EXPORT
ImageFormatCovnertNative* OH_ImageCovnertNative_InitNative()
{
    std::unique_ptr<ImageFormatCovnertNative> result = std::make_unique<ImageFormatCovnertNative>();
    return result.release();
}

MIDK_EXPORT
int32_t OH_ImageCovnertNative_ReleaseNative(ImageFormatCovnertNative* native)
{
    if (native != nullptr) {
        delete native;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageConvert_YuvToRgb(ImageFormatCovnertNative *srcPixelMap, ImageFormatCovnertNative *destPixelMap,
                                 int32_t destPixelFormat)
{
    if (srcPixelMap == nullptr || srcPixelMap->pixelMap == nullptr || destPixelMap == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageFormatConvertArgs args;
    args.srcPixelMap = srcPixelMap->pixelMap;
    args.srcFormatType = YUV_TYPE;
    args.destPixelFormat = PixelFormat(destPixelFormat);
    args.destFormatType = RGB_TYPE;
    
    int32_t ret = ImageConvertNativeCall(CTX_FUNC_IMAGE_CONVERT_EXEC, &args);
    if (ret == IMAGE_RESULT_SUCCESS) {
        destPixelMap->pixelMap = args.destPixelMap;
    }
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageConvert_RgbToYuv(ImageFormatCovnertNative *srcPixelMap, ImageFormatCovnertNative *destPixelMap,
                                 int32_t destPixelFormat)
{
    if (srcPixelMap == nullptr || srcPixelMap->pixelMap == nullptr || destPixelMap == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageFormatConvertArgs args;
    args.srcPixelMap = srcPixelMap->pixelMap;
    args.srcFormatType = RGB_TYPE;
    args.destPixelFormat = PixelFormat(destPixelFormat);
    args.destFormatType = YUV_TYPE;
    
    int32_t ret = ImageConvertNativeCall(CTX_FUNC_IMAGE_CONVERT_EXEC, &args);
    if (ret == IMAGE_RESULT_SUCCESS) {
        destPixelMap->pixelMap = args.destPixelMap;
    }
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageConvert_JsToC_PixelMap(napi_env env, napi_value pixelMapValue, ImageFormatCovnertNative *pixelMap)
{
    if (pixelMapValue == nullptr || pixelMap == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageFormatConvertArgs args;
    args.env = env;
    args.pixelMapValue = pixelMapValue;
    int32_t ret = ImageConvertNativeCall(CTX_FUNC_IMAGE_CONVERT_JS_TO_C_PIXEL_MAP, &args);
    if (ret == IMAGE_RESULT_SUCCESS) {
        pixelMap->pixelMap = args.srcPixelMap;
    }
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageConvert_CToJs_PixelMap(napi_env env, ImageFormatCovnertNative *pixelMap, napi_value *result)
{
    if (pixelMap == nullptr || pixelMap->pixelMap == nullptr ||result == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageFormatConvertArgs args;
    args.destPixelMap = pixelMap->pixelMap;
    args.env = env;
    args.result = result;
    return ImageConvertNativeCall(CTX_FUNC_IMAGE_CONVERT_C_TO_JS_PIXEL_MAP, &args);
}

#ifdef __cplusplus
}
#endif