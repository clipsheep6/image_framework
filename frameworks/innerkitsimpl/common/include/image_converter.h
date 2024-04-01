/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_OPEN_SOURCE_LIBYUV_IMAGE_CONVERTER_H
#define OHOS_OPEN_SOURCE_LIBYUV_IMAGE_CONVERTER_H

#include <cstdint>

namespace OHOS {
namespace OpenSourceLibyuv {
#ifdef __cplusplus
extern "C" {
#endif

typedef enum FilterMode {
    K_FILTER_NONE = 0,
    K_FILTER_LINEAR = 1,
    K_FILTER_BILINEAR = 2,
    K_FILTER_BOX = 3
} FilterModeEnum;

typedef enum RotationMode {
    K_ROTATE_0 = 0,
    K_ROTATE_90 = 90,
    K_ROTATE_180 = 180,
    K_ROTATE_270 = 270,
} RotationModeEnum;

struct ImageConverter {
    int32_t (*I420ToRGBA)(const uint8_t* srcY, int srcStrideY, const uint8_t* srcU, int srcStrideU,
        const uint8_t* srcV, int srcStrideV, uint8_t* dstRgba, int dstStrideRgba,
        int width, int height);
    int32_t (*ARGBToNV12)(const uint8_t* srcArgb, int srcStrideArgb, uint8_t* dstY, int dstStrideY,
        uint8_t* dstUV, int dst_stride_uv, int width, int height);
    int32_t (*I420Scale)(const uint8_t* srcY, int srcStrideY, const uint8_t* srcU, int srcStrideU,
                        const uint8_t* srcV, int srcStrideV, int srcWidth, int srcHeight,
                        uint8_t* dstY, int dstStrideY, uint8_t* dstU, int dstStrideU,
                        uint8_t* dstV, int dstStrideV, int dstWidth, int dstHeight,
                        enum FilterMode filtering);
    int32_t (*NV12ToI420)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcUV, int srcStrideUV,
                            uint8_t *dstY, int dstStrideY, uint8_t *dstU, int dstStrideU,
                            uint8_t *dstV, int dstStrideV, int width, int height);
    int32_t (*I420ToNV21)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcU, int srcStrideU,
                            const uint8_t *srcV, int srcStrideV, uint8_t *dstY, int dstStrideY,
                            uint8_t *dst_vu, int dst_stride_vu, int width, int height);
    void (*ScalePlane)(const uint8_t *src, int src_stride, int srcWidth, int srcHeight, uint8_t *dst,
                        int dst_stride, int dstWidth, int dstHeight, enum FilterMode filtering);
    void (*SplitUVPlane)(const uint8_t *srcUV, int srcStrideUV, uint8_t *dstU, int dstStrideU,
                            uint8_t *dstV, int dstStrideV, int width, int height);
    void (*MergeUVPlane)(const uint8_t *srcU, int srcStrideU, const uint8_t *srcV, int srcStrideV,
                            uint8_t *dstUV, int dst_stride_uv, int width, int height);
    int32_t (*I420ToNV12)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcU, int srcStrideU,
                            const uint8_t *srcV, int srcStrideV, uint8_t *dstY, int dstStrideY, uint8_t *dstUV,
                            int dst_stride_uv, int width, int height);
    int32_t (*I420Mirror)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcU, int srcStrideU,
                            const uint8_t *srcV, int srcStrideV, uint8_t *dstY, int dstStrideY, uint8_t *dstU,
                            int dstStrideU, uint8_t *dstV, int dstStrideV, int width, int height);
    int32_t (*NV12ToI420Rotate)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcUV, int srcStrideUV,
                                uint8_t *dstY, int dstStrideY, uint8_t *dstU, int dstStrideU, uint8_t *dstV,
                                int dstStrideV, int width, int height, enum RotationMode mode);
    int32_t (*ARGBToI420)(const uint8_t *srcArgb, int srcStrideArgb, uint8_t *dstY, int dstStrideY,
                            uint8_t *dstU, int dstStrideU, uint8_t *dstV,
                            int dstStrideV, int width, int height);
    int32_t (*NV12ToARGB)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcUV, int srcStrideUV,
                            uint8_t *dstArgb,
                            int dstStrideArgb, int width, int height);
    int32_t (*NV21ToARGB)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcVU, int srcStrideVU,
                            uint8_t *dstArgb, int dstStrideArgb,
                            int width, int height);
    int32_t (*ARGBToBGRA)(const uint8_t *srcArgb, int srcStrideArgb, uint8_t *dst_bgra,
                            int dst_stride_bgra, int width, int height);
    int32_t (*I420Copy)(const uint8_t *srcY, int srcStrideY, const uint8_t *srcU, int srcStrideU,
                        const uint8_t *srcV, int srcStrideV, uint8_t *dstY, int dstStrideY, uint8_t *dstU,
                        int dstStrideU, uint8_t *dstV, int dstStrideV, int width, int height);
};

struct ImageConverter GetImageConverter(void);

#ifdef __cplusplus
}
#endif
} // namespace OpenSourceLibyuv
} // namespace OHOS
#endif // OHOS_OPEN_SOURCE_LIBYUV_IMAGE_CONVERTER_H