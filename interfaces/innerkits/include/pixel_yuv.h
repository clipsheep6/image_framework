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

struct UVPos {
    size_t upos = 0;
    size_t vpos = 0;
};

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
    NATIVEEXPORT void SetTranslateDataDefault(uint8_t *srcPixels, int32_t width, int32_t height);
    NATIVEEXPORT void translate(float xAxis, float yAxis) override;
    NATIVEEXPORT uint32_t ReadPixel(const Position &pos, uint32_t &dst) override;
    NATIVEEXPORT bool WritePixels(const uint32_t &color) override;
    NATIVEEXPORT uint32_t WritePixel(const Position &pos, const uint32_t &color) override;
    ColorYUV420 GetYUV420Color(uint32_t x, uint32_t y);
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
    static bool YUV420WritePixels(Size &size, uint8_t *dstPixels, const PixelFormat &format, const uint32_t &color);
    static bool YUVReadPixel(const uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
                             uint32_t &dst);
    static bool YUVWritePixel(uint8_t *srcPixels, Size &size, const PixelFormat &format, const Position &pos,
                              const uint32_t &color);
    static bool YUVTransLate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, const PixelFormat &format,
                             float xAxis, float yAxis);
    static bool WritePixelsConvert(const void *srcPixels, const ImageInfo &srcInfo, void *dstPixels,
                                   const Position &dstPos, const ImageInfo &dstInfo);
    static bool ReadPixelsConvert(const void *srcPixels, const Position &srcPos, const ImageInfo &srcInfo,
                           void *dstPixels, const ImageInfo &dstInfo);
    bool CheckPixelsInput(const uint8_t *dst, const uint64_t &bufferSize, const uint32_t &offset, const Rect &region);
    static bool YUVCrop(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, const Rect &rect,
                        const PixelFormat &srcFormat);
    static bool CropYU12(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Rect rect);
    static void SwapUV(uint8_t *dstPixels, int32_t width, int32_t heigth);
    static bool CropNV21(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Rect rect);
    bool YUVRotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int32_t degrees,
                   const PixelFormat &format);
    static bool I420AndYV12Transfers(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height);
    static uint8_t GetYUV420Y(uint32_t x, uint32_t y, int32_t width, const uint8_t *in);
    static uint8_t GetYUV420U(uint32_t x, uint32_t y, int32_t width, int32_t height,
                              PixelFormat format, const uint8_t *in);
    static uint8_t GetYUV420V(uint32_t x, uint32_t y, int32_t width, int32_t height,
                              PixelFormat format, const uint8_t *in);
    void SetRowDataSizeForImageInfo(ImageInfo info);
    static void YUV420PTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, float xAxis, float yAxis,
                                 bool isI420);
    static void YUV420SPTranslate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, float xAxis, float yAxis);
    static void YUV420PReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst, bool isI420);
    static void YUV420SPReadPixel(const uint8_t *srcPixels, Size &size, const Position &pos, uint32_t &dst,
                                  bool isNV12);
    static void YUV420PWritePixel(uint8_t *srcPixels, Size &size, const Position &pos, const uint32_t &color,
                                  bool isI420);
    static void YUV420SPWritePixel(uint8_t *srcPixels, Size &size, const Position &pos, const uint32_t &color,
                                   bool isNV12);
    static void YUV420PWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isI420);
    static void YUV420SPWritePixels(Size &size, uint8_t *srcPixels, const uint32_t &color, bool isNV12);
    static void WriteDataI420Convert(const uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Position dstPos,
                                     const Size &dstSize);
    static void WriteDataNV12Convert(uint8_t *srcPixels, const Size &size, uint8_t *dstPixels, Position dstPos,
                                     const Size &dstSize);
    static uint32_t GetImageSize(int32_t width, int32_t height);
    static bool IsYuvFormat(PixelFormat format);
    void AssignYUVDataOnType(PixelFormat format, int32_t width, int32_t height);
    YUVDataInfo yuvDataInfo_;
#ifdef IMAGE_COLORSPACE_FLAG
    bool CheckColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace);
    int32_t ColorSpaceBGRAToYuv(uint8_t *bgraData, SkTransYuvInfo &dst, ImageInfo &imageInfo, PixelFormat &format,
        const OHOS::ColorManager::ColorSpace &grColorSpace);
    NATIVEEXPORT uint32_t ApplyColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace) override;
#endif

#ifdef LIBYUV_ENABLE
    static bool BGRAToYUV420(const uint8_t *src, uint8_t *dst, int src_w, int src_h, PixelFormat pixelFormat);
    static bool YUV420ToBGRA(const uint8_t *sample, uint8_t *dst_argb, int width, int height, PixelFormat pixelFormat);
    static bool YUV420ToARGB(const uint8_t *sample, uint8_t *dst_argb, int width, int height, PixelFormat pixelFormat);
    static void ConvertYuvMode(libyuv::FilterMode &filterMode, const AntiAliasingOption &option);
    void ScaleYUV420(float xAxis, float yAxis, const AntiAliasingOption &option = AntiAliasingOption::NONE);
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
    static bool NV21ToI420(const uint8_t *src, const Size &size, uint8_t *dst);
    static bool NV12ToI420(const uint8_t *src, const Size &size, uint8_t *dst);
    static bool I420ToNV12(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height);
    static bool I420ToNV21(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height);
    static bool YV12Rotate(const uint8_t *srcPixels, Size &size, uint8_t *dstPixels, int degrees);
    static void MergeUVPlane(const uint8_t *srcU, int srcStrideU, const uint8_t *srcV, int32_t srcStrideV,
                             uint8_t *dstUV, int32_t dstStrideUV, int32_t width, int32_t height);
    static void MergeUV(const uint8_t *srcU, const uint8_t *srcV, uint8_t *dstUV, int32_t width);
    static void RotatePlane270(const uint8_t *src, int32_t srcStride, uint8_t *dst,
                               int32_t dstStride, const Size &half);
    static void RotatePlane180(const uint8_t *src, int32_t srcStride, uint8_t *dst,
                               int32_t dstStride, const Size &half);
    static void RotatePlane90(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride, const Size &half);
    static void TransposePlane(const uint8_t *src, int32_t srcStride, uint8_t *dst,
                               int32_t dstStride, const Size &half);
    static void TransposeW8(const uint8_t *src, int32_t srcStride, uint8_t *dst, int32_t dstStride, int32_t width);
    static void MirrorRow(const uint8_t *src, uint8_t *dst, int32_t width);
    static bool BGRAToYUV420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height, PixelFormat pixelFormat);
    static bool YUV420ToBGRA(const uint8_t *in, uint8_t *out, int32_t width, int32_t height, PixelFormat pixelFormat);
    static bool YUV420ToARGB(const uint8_t *in, uint8_t *out, int32_t width, int32_t height, PixelFormat pixelFormat);
    static int32_t I420Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
                            const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY, uint8_t *dstU,
                            int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV, int32_t width, int32_t height);
    static bool BGRAToI420(const uint8_t *src, uint8_t *dst, int32_t width, int32_t height);
    static void TranslateRGBToYUV(const uint8_t *src, uint8_t *dst, Size &size,  size_t &i, UVPos &uvPos);
    static void MirrorPlane(const uint8_t *srcY, int32_t srcStrideY, uint8_t *dstY, int32_t dstStrideY, int32_t width,
                            int32_t height);
    static void MirrorUVRow(const uint8_t *srcUV, uint8_t *dstUV, int32_t width);
    static void MirrorUVPlane(const uint8_t *srcUV, int32_t srcStrideUV, uint8_t *dstUV, int32_t dstStrideUV,
                              int32_t width, int32_t height);
    static int32_t I420Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcU, int32_t srcStrideU,
                              const uint8_t *srcV, int32_t srcStrideV, uint8_t *dstY, int32_t dstStrideY, uint8_t *dstU,
                              int32_t dstStrideU, uint8_t *dstV, int32_t dstStrideV, int32_t width, int32_t height);
    static int32_t NV12Mirror(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
                              uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int dstStrideUV, int32_t width,
                              int32_t height);
    static int32_t NV12Copy(const uint8_t *srcY, int32_t srcStrideY, const uint8_t *srcUV, int32_t srcStrideUV,
                            uint8_t *dstY, int32_t dstStrideY, uint8_t *dstUV, int32_t dstStrideUV,
                            int32_t width, int32_t height);
    static void CopyPlane(const uint8_t *srcY, int32_t srcStrideY, uint8_t *dstY, int32_t dstStrideY, int32_t width,
                          int32_t height);
#endif
};
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_PIXEL_YUV_H