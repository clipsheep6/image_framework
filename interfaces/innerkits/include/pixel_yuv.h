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

#ifndef INTERFACES_INNERKITS_INCLUDE_PIXEL_YUV_H
#define INTERFACES_INNERKITS_INCLUDE_PIXEL_YUV_H

#include "image_type.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "memory_manager.h"
#include "pixel_map.h"

#ifdef LIBYUV_ENABLE
#include "libyuv.h"
#endif

namespace OHOS {
namespace Media {

struct PixelSize {
    int32_t srcW = 0;
    int32_t srcH = 0;
    int32_t dstW = 0;
    int32_t dstH = 0;
};

struct SkTransYuvInfo {
    SkRect r;
    SkImageInfo info;
    SkBitmap bitmap;
};

class PixelYuv : public PixelMap {
public:
    PixelYuv() {}
    virtual ~PixelYuv();
    NATIVEEXPORT const uint8_t *GetPixel8(int32_t x, int32_t y) override;
    NATIVEEXPORT const uint16_t *GetPixel16(int32_t x, int32_t y) override;
    NATIVEEXPORT const uint32_t *GetPixel32(int32_t x, int32_t y) override;
    NATIVEEXPORT bool GetARGB32Color(int32_t x, int32_t y, uint32_t &color) override;
    NATIVEEXPORT uint8_t GetARGB32ColorA(uint32_t color) override;
    NATIVEEXPORT uint8_t GetARGB32ColorR(uint32_t color) override;
    NATIVEEXPORT uint8_t GetARGB32ColorG(uint32_t color) override;
    NATIVEEXPORT uint8_t GetARGB32ColorB(uint32_t color) override;
    NATIVEEXPORT uint32_t SetAlpha(const float percent) override;
    NATIVEEXPORT uint32_t getPixelBytesNumber();
    NATIVEEXPORT int32_t GetByteCount() override;
    NATIVEEXPORT void rotate(float degrees) override;
    NATIVEEXPORT uint32_t crop(const Rect &rect) override;
    NATIVEEXPORT void scale(float xAxis, float yAxis) override;
    NATIVEEXPORT void scale(float xAxis, float yAxis, const AntiAliasingOption &option) override;
    NATIVEEXPORT bool resize(float xAxis, float yAxis) override;
    NATIVEEXPORT void flip(bool xAxis, bool yAxis) override;
    NATIVEEXPORT uint32_t WritePixels(const uint8_t *source, const uint64_t &bufferSize, const uint32_t &offset,
                                      const uint32_t &stride, const Rect &region) override;
    NATIVEEXPORT uint32_t ReadPixels(const uint64_t &bufferSize, const uint32_t &offset, const uint32_t &stride,
                                     const Rect &region, uint8_t *dst) override;
    NATIVEEXPORT void translate(float xAxis, float yAxis) override;
    NATIVEEXPORT uint32_t ReadPixel(const Position &pos, uint32_t &dst) override;
    NATIVEEXPORT bool WritePixels(const uint32_t &color) override;
    NATIVEEXPORT uint32_t WritePixel(const Position &pos, const uint32_t &color) override;
    ColorYuv420 GetYuv420Color(uint32_t x, uint32_t y);
    NATIVEEXPORT void SetPixelsAddr(void *addr, void *context, uint32_t size, AllocatorType type,
                                    CustomFreePixelMap func) override;
    NATIVEEXPORT virtual void SetImageYUVInfo(YUVDataInfo &yuvInfo) override
    {
        yuvDataInfo_ = yuvInfo;
    }
    NATIVEEXPORT virtual void GetImageYUVInfo(YUVDataInfo &yuvInfo) const override
    {
        yuvInfo = yuvDataInfo_;
    }
    NATIVEEXPORT int32_t ScaleYuvToBGRA(ImageInfo &imageInfo, PixelFormat &format);
    NATIVEEXPORT int32_t ScaleBGRAToYuv(ImageInfo &imageInfo, PixelFormat &format);
private:
    bool CheckPixelsInput(const uint8_t *dst, const uint64_t &bufferSize, const uint32_t &offset, const Rect &region);
    void SetRowDataSizeForImageInfo(ImageInfo info);
    static uint32_t GetImageSize(int32_t width, int32_t height);
    static bool IsYuvFormat(PixelFormat format);
    void AssignYuvDataOnType(PixelFormat format, int32_t width, int32_t height);
    YUVDataInfo yuvDataInfo_;
#ifdef IMAGE_COLORSPACE_FLAG
    bool CheckColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace);
    int32_t ColorSpaceBGRAToYuv(uint8_t *bgraData, SkTransYuvInfo &dst, ImageInfo &imageInfo, PixelFormat &format,
        const OHOS::ColorManager::ColorSpace &grColorSpace);
    NATIVEEXPORT uint32_t ApplyColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace) override;
#endif

#ifdef LIBYUV_ENABLE
    static void ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option);
    void ScaleYuv420(float xAxis, float yAxis, const AntiAliasingOption &option = AntiAliasingOption::NONE);
    static bool NV21Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
        uint8_t *dstPixels, libyuv::RotationMode &rotateNum);
    static bool NV12Rotate(const uint8_t *src, uint8_t *dstData, PixelSize &size,
        uint8_t *dstPixels, libyuv::RotationMode &rotateNum);
    static bool I420Rotate(const uint8_t *src, PixelSize &size, uint8_t *dstPixels, libyuv::RotationMode &rotateNum);
    static bool FlipXaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format);
    static void FlipYaxis(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat format);
#else
    bool FlipXaxis(const uint8_t *src, uint8_t *dst, int32_t srcW, int32_t srcH, PixelFormat format);
    bool FlipYaxis(const uint8_t *src, uint8_t *dst, int32_t srcW, int32_t srcH, PixelFormat format);
#endif
};
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_PIXEL_YUV_H