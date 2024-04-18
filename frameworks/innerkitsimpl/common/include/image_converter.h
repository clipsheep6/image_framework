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

#include <stdint.h>

namespace OHOS {
namespace OpenSourceLibyuv {
#ifdef __cplusplus
extern "C" {
#endif

typedef enum FilterMode {
    kFilterNone = 0,
    kFilterLinear = 1,
    kFilterBilinear = 2,
    kFilterBox = 3
} FilterModeEnum;

typedef enum RotationMode {
    kRotate0 = 0,
    kRotate90 = 90,
    kRotate180 = 180,
    kRotate270 = 270,
} RotationModeEnum;

struct ImageConverter {
	int32_t (*I420ToRGBA)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_u, int src_stride_u,
		const uint8_t* src_v, int src_stride_v, uint8_t* dst_rgba, int dst_stride_rgba,
		int width, int height);
	int32_t (*ARGBToNV12)(const uint8_t* src_argb, int src_stride_argb, uint8_t* dst_y, int dst_stride_y,
		uint8_t* dst_uv, int dst_stride_uv, int width, int height);
	int32_t (*I420Scale)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_u, int src_stride_u,
						const uint8_t* src_v, int src_stride_v, int src_width, int src_height,
						uint8_t* dst_y, int dst_stride_y, uint8_t* dst_u, int dst_stride_u,
						uint8_t* dst_v, int dst_stride_v, int dst_width, int dst_height,
						enum FilterMode filtering);
	int32_t (*NV12ToI420)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_uv, int src_stride_uv,
							uint8_t *dst_y, int dst_stride_y, uint8_t *dst_u, int dst_stride_u,
							uint8_t *dst_v, int dst_stride_v, int width, int height);
	int32_t (*I420ToNV21)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_u, int src_stride_u,
							const uint8_t *src_v, int src_stride_v, uint8_t *dst_y, int dst_stride_y,
							uint8_t *dst_vu, int dst_stride_vu, int width, int height);
	void (*ScalePlane)(const uint8_t *src, int src_stride, int src_width, int src_height, uint8_t *dst,
						int dst_stride, int dst_width, int dst_height, enum FilterMode filtering);
	void (*SplitUVPlane)(const uint8_t *src_uv, int src_stride_uv, uint8_t *dst_u, int dst_stride_u,
							uint8_t *dst_v, int dst_stride_v, int width, int height);
	void (*MergeUVPlane)(const uint8_t *src_u, int src_stride_u, const uint8_t *src_v, int src_stride_v,
							uint8_t *dst_uv, int dst_stride_uv, int width, int height);
	int32_t (*I420ToNV12)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_u, int src_stride_u,
							const uint8_t *src_v, int src_stride_v, uint8_t *dst_y, int dst_stride_y, uint8_t *dst_uv,
							int dst_stride_uv, int width, int height);
	int32_t (*I420Mirror)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_u, int src_stride_u,
							const uint8_t *src_v, int src_stride_v, uint8_t *dst_y, int dst_stride_y, uint8_t *dst_u,
							int dst_stride_u, uint8_t *dst_v, int dst_stride_v, int width, int height);
	int32_t (*NV12ToI420Rotate)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_uv, int src_stride_uv,
								uint8_t *dst_y, int dst_stride_y, uint8_t *dst_u, int dst_stride_u, uint8_t *dst_v,
								int dst_stride_v, int width, int height, enum RotationMode mode);
	int32_t (*ARGBToI420)(const uint8_t *src_argb, int src_stride_argb, uint8_t *dst_y, int dst_stride_y,
							uint8_t *dst_u, int dst_stride_u, uint8_t *dst_v,
							int dst_stride_v, int width, int height);
	int32_t (*NV12ToARGB)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_uv, int src_stride_uv,
							uint8_t *dst_argb,
							int dst_stride_argb, int width, int height);
	int32_t (*NV21ToARGB)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_vu, int src_stride_vu,
							uint8_t *dst_argb, int dst_stride_argb,
							int width, int height);
	int32_t (*ARGBToBGRA)(const uint8_t *src_argb, int src_stride_argb, uint8_t *dst_bgra,
							int dst_stride_bgra, int width, int height);
	int32_t (*I420Copy)(const uint8_t *src_y, int src_stride_y, const uint8_t *src_u, int src_stride_u,
						const uint8_t *src_v, int src_stride_v, uint8_t *dst_y, int dst_stride_y, uint8_t *dst_u,
						int dst_stride_u, uint8_t *dst_v, int dst_stride_v, int width, int height);
	int32_t (*NV12ToRGB565)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_uv, int src_stride_uv,
							uint8_t* dst_rgb565, int dst_stride_rgb565, int width, int height);		  	
	int32_t (*NV21ToNV12)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_vu, int src_stride_vu,
						  uint8_t* dst_y, int dst_stride_y, uint8_t* dst_uv, int dst_stride_uv, int width, int height);
	int32_t (*RGB565ToI420)(const uint8_t* src_rgb565, int src_stride_rgb565, uint8_t* dst_y, int dst_stride_y,
		 					uint8_t* dst_u, int dst_stride_u, uint8_t* dst_v, int dst_stride_v, int width, int height);
	int32_t (*BGRAToNV21)(const uint8_t* src_argb, int src_stride_argb, uint8_t* dst_y, int dst_stride_y,
						  uint8_t* dst_vu, int dst_stride_vu, int width, int height);
	int32_t (*I420ToRGBAMatrix)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_u, int src_stride_u,
								const uint8_t* src_v, int src_stride_v, uint8_t* dst_rgba, int dst_stride_rgba,
								const struct YuvConstants* yuvconstants, int width, int height);
	int32_t (*I420ToBGRA)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_u, int src_stride_u,
						  const uint8_t* src_v, int src_stride_v, uint8_t* dst_bgra, int dst_stride_bgra,
						  int width, int height);
	int32_t (*RGB24ToI420)(const uint8_t* src_rgb24, int src_stride_rgb24, uint8_t* dst_y, int dst_stride_y,
						   uint8_t* dst_u, int dst_stride_u, uint8_t* dst_v, int dst_stride_v, int width, int height);
	int32_t (*NV12ToRGB24Matrix)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_uv, int src_stride_uv,
								 uint8_t* dst_rgb24, int dst_stride_rgb24, const struct YuvConstants* yuvconstants,
								 int width, int height);
	int32_t (*NV21ToRGB24Matrix)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_vu, int src_stride_vu,
								 uint8_t* dst_rgb24, int dst_stride_rgb24, const struct YuvConstants* yuvconstants,
								 int width, int height);
	int32_t (*NV21ToI420)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_vu, int src_stride_vu,
						  uint8_t* dst_y, int dst_stride_y, uint8_t* dst_u, int dst_stride_u, uint8_t* dst_v,
						  int dst_stride_v, int width, int height);
	int32_t (*I420ToRGB565Matrix)(const uint8_t* src_y, int src_stride_y, const uint8_t* src_u, int src_stride_u,
								  const uint8_t* src_v, int src_stride_v, uint8_t* dst_rgb565, int dst_stride_rgb565,
								  const struct YuvConstants* yuvconstants, int width, int height);
};
struct ImageConverter GetImageConverter(void);

#ifdef __cplusplus
}
#endif
} // namespace OpenSourceLibyuv
} // namespace OHOS
#endif // OHOS_OPEN_SOURCE_LIBYUV_IMAGE_CONVERTER_H