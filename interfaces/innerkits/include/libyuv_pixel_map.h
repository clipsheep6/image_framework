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

#ifndef INTERFACES_INNERKITS_INCLUDE_LIBYUV_PIXEL_MAP_H
#define INTERFACES_INNERKITS_INCLUDE_LIBYUV_PIXEL_MAP_H

#include "image_type.h"
#include "memory_manager.h"
#include "pixel_yuv.h"

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"

#include "libyuv.h"

namespace OHOS {
namespace Media {

class LibYuvPixelMap : public PixelYuv {
public:
    LibYuvPixelMap() {}
    virtual ~LibYuvPixelMap();
    NATIVEEXPORT void rotate(float degrees) override;
    NATIVEEXPORT void scale(float xAxis, float yAxis) override;
    NATIVEEXPORT void scale(float xAxis, float yAxis, const AntiAliasingOption &option) override;
    NATIVEEXPORT void flip(bool xAxis, bool yAxis) override;
#ifdef IMAGE_COLORSPACE_FLAG
    NATIVEEXPORT uint32_t ApplyColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace) override;
#endif
    void PrintYuvDataInfo();
    static void ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option);
    void ScaleYuv420(float xAxis, float yAxis, const AntiAliasingOption &option = AntiAliasingOption::NONE);
    static bool NV21Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
        uint8_t *dstPixels, libyuv::RotationMode &rotateNum);
    static bool NV12Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
        uint8_t *dstPixels, libyuv::RotationMode &rotateNum);
    static bool I420Rotate(const uint8_t *src, PixelSize &size, uint8_t *dstPixels, libyuv::RotationMode &rotateNum);
    static bool FlipXaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format);
    static void FlipYaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format);
};
} // namespace Media
} // namespace OHOS
#endif // INTERFACES_INNERKITS_INCLUDE_LIBYUV_PIXEL_MAP_H