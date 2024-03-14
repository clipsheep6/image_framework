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

MIDK_EXPORT
int32_t OH_ImageConvert_YuvToRgb(OH_PixelMap2 *srcPixelMap, OH_PixelMap2 **destPixelMap, int32_t destPixelFormat)
{
    if (srcPixelMap == nullptr || destPixelMap == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageFormatConvertArgs args;
    args.srcPixelMap = srcPixelMap->GetInnerPixelMap();

    args.srcFormatType = YUV_TYPE;
    args.destPixelFormat = PixelFormat(destPixelFormat);
    args.destFormatType = RGB_TYPE;
    
    int32_t ret = ImageConvertNativeCall(CTX_FUNC_IMAGE_CONVERT_EXEC, &args);
    if (ret == IMAGE_RESULT_SUCCESS) {
        if (*destPixelMap != nullptr) {
            delete *destPixelMap;
        }
        *destPixelMap = new OH_PixelMap2(args.destPixelMap); 
    }
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageConvert_RgbToYuv(OH_PixelMap2 *srcPixelMap, OH_PixelMap2 **destPixelMap, int32_t destPixelFormat)
{
    if (srcPixelMap == nullptr || destPixelMap == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageFormatConvertArgs args;
    args.srcPixelMap = srcPixelMap->GetInnerPixelMap();
    args.srcFormatType = RGB_TYPE;
    args.destPixelFormat = PixelFormat(destPixelFormat);
    args.destFormatType = YUV_TYPE;
    
    int32_t ret = ImageConvertNativeCall(CTX_FUNC_IMAGE_CONVERT_EXEC, &args);
    if (ret == IMAGE_RESULT_SUCCESS) {
        if (*destPixelMap != nullptr) {
            delete *destPixelMap;
        }
        *destPixelMap = new OH_PixelMap2(args.destPixelMap);
    }
    return ret;
}

#ifdef __cplusplus
}
#endif