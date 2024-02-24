/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "image_format_convert_utils.h"

#include <cmath>
#include <iostream>
#include <cstring>
#include <string.h>
#include "securec.h"
#ifdef LIBYUV
#include "libyuv/convert.h"
#endif
#include "image_log.h"
#include "log_tags.h"

namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
    constexpr uint32_t NUM_3 = 3;
    constexpr uint32_t NUM_4 = 4;
    constexpr uint32_t NUM_5 = 5;
    constexpr uint32_t NUM_8 = 8;
    constexpr uint32_t NUM_11 = 11;
    constexpr uint32_t NUM_16 = 16;
    constexpr uint32_t NUM_18 = 18;
    constexpr uint32_t NUM_25 = 25;
    constexpr uint32_t NUM_38 = 38;
    constexpr uint32_t NUM_66 = 66;
    constexpr uint32_t NUM_74 = 74;
    constexpr uint32_t NUM_94 = 94;
    constexpr uint32_t NUM_100 = 100;
    constexpr uint32_t NUM_112 = 112;
	constexpr uint32_t NUM_128 = 128;
	constexpr uint32_t NUM_129 = 129;
    constexpr uint32_t NUM_208 = 208;
    constexpr uint32_t NUM_255 = 255;
    constexpr uint32_t NUM_298 = 298;
    constexpr uint32_t NUM_409 = 409;
    constexpr uint32_t NUM_516 = 516;
}

#undef LOG_TAG
#define LOG_TAG "ImageFormatConvert"
namespace OHOS {
namespace Media {


#ifdef LIBYUV
using namespace libyuv;
std::map<ColorSpace, const struct YuvConstants*> colorSpaceMap = {
    {ColorSpace::UNKNOWN, nullptr},
    {ColorSpace::DISPLAY_P3, &kYuvJPEGConstants},
    {ColorSpace::SRGB, &kYuvJPEGConstants},
    {ColorSpace::LINEAR_SRGB, &kYuvJPEGConstants},
    {ColorSpace::EXTENDED_SRGB, &kYuvJPEGConstants},
    {ColorSpace::LINEAR_EXTENDED_SRGB, &kYuvJPEGConstants},
    {ColorSpace::GENERIC_XYZ, &kYuvJPEGConstants},
    {ColorSpace::GENERIC_LAB, &kYuvJPEGConstants},
    {ColorSpace::ACES, &kYuvJPEGConstants},
    {ColorSpace::ACES_CG, &kYuvJPEGConstants},
    {ColorSpace::ADOBE_RGB_1998, &kYuvJPEGConstants},
    {ColorSpace::DCI_P3, &kYuvJPEGConstants},
    {ColorSpace::ITU_709, &kYuvF709Constants},
    {ColorSpace::ITU_2020, &kYuvV2020Constants},
    {ColorSpace::ROMM_RGB, &kYuvJPEGConstants},
    {ColorSpace::NTSC_1953, &kYuvJPEGConstants},
    {ColorSpace::SMPTE_C, &kYuvJPEGConstants},
};

const struct YuvConstants* mapColorSPaceToYuvConstants(ColorSpace colorSpace)
{
    auto it = colorSpaceMap.find(colorSpace);
    if(it != colorSpaceMap.end()) {
        return it->second;
    }
    return nullptr;
}
#endif

static void RGB565ToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    int dstYStride = imageSize.width;
    int dstUVStride = imageSize.width;
    for (int h = 0; h < imageSize.height; ++h) {
        const uint16_t* srcRow = reinterpret_cast<const uint16_t*>(srcBuffer + h * imageSize.width * NUM_2);
        uint8_t* dstYRow = (*destBuffer) + h * dstYStride;
        uint8_t* dstUVRow = (*destBuffer) + imageSize.width * imageSize.height +
            (h / NUM_2) * ((dstUVStride + NUM_1) / NUM_2) * NUM_2;
        for (int w = 0; w < imageSize.width; ++w) {
            int dstYIndex = w;
            int dstUVIndex = w / NUM_2;
            // Extract RGB components from 16-bit pixel value
            uint16_t pixel = srcRow[w];
            uint8_t r = static_cast<uint8_t>((pixel & 0xF800) >> NUM_8);
            uint8_t g = static_cast<uint8_t>((pixel & 0x07E0) >> NUM_3);
            uint8_t b = static_cast<uint8_t>((pixel & 0x001F) << NUM_3);
            // Convert RGB to YUV
            uint8_t y = static_cast<uint8_t>((NUM_66 * r + NUM_129 * g + NUM_25 * b + NUM_128) >> NUM_8) + NUM_16;
            uint8_t u = static_cast<uint8_t>((-NUM_38 * r - NUM_74 * g + NUM_112 * b + NUM_128) >> NUM_8) + NUM_128;
            uint8_t v = static_cast<uint8_t>((NUM_112 * r - NUM_94 * g - NUM_18 * b + NUM_128) >> NUM_8) + NUM_128;
            // Store Y and UV values
            dstYRow[dstYIndex] = y;
            if (h % NUM_2 == 0 && w % NUM_2 == 0) {
                dstUVRow[dstUVIndex * NUM_2] = u;
                dstUVRow[dstUVIndex * NUM_2 + NUM_1] = v;
            }
        }
    }
}

static void RGB565ToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    int dstYStride = imageSize.width;
    int dstUVStride = imageSize.width;
     for (int h = 0; h < imageSize.height; ++h) {
        const uint16_t* srcRow = reinterpret_cast<const uint16_t*>(srcBuffer + h * imageSize.width * NUM_2);
        uint8_t* dstYRow = (*destBuffer) + h * dstYStride;
        uint8_t* dstUVRow = (*destBuffer) + imageSize.width * imageSize.height + (h / NUM_2) *
            ((dstUVStride + NUM_1) / NUM_2) * NUM_2;
        for (int w = 0; w < imageSize.width; ++w) {
            int dstYIndex = w;
            int dstUVIndex = w / NUM_2;
            // Extract RGB components from 16-bit pixel value
            uint16_t pixel = srcRow[w];
            uint8_t r = static_cast<uint8_t>((pixel & 0xF800) >> NUM_8);
            uint8_t g = static_cast<uint8_t>((pixel & 0x07E0) >> NUM_3);
            uint8_t b = static_cast<uint8_t>((pixel & 0x001F) << NUM_3);
            // Convert RGB to YUV
            uint8_t y = static_cast<uint8_t>((NUM_66 * r + NUM_129 * g + NUM_25 * b + NUM_128) >> NUM_8) + NUM_16;
            uint8_t u = static_cast<uint8_t>((-NUM_38 * r - NUM_74 * g + NUM_112 * b + NUM_128) >> NUM_8) + NUM_128;
            uint8_t v = static_cast<uint8_t>((NUM_112 * r - NUM_94 * g - NUM_18 * b + NUM_128) >> NUM_8) + NUM_128;
            // Store Y and UV values
            dstYRow[dstYIndex] = y;
            if (h % NUM_2 == 0 && w % NUM_2 == 0) {
                dstUVRow[dstUVIndex * NUM_2] = v;
                dstUVRow[dstUVIndex * NUM_2 + NUM_1] = u;
            }
        }
    }
}

static void BGRAToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const int32_t destPlaneSizeY = imageSize.width * imageSize.height;
    const float kBGRToYCoeff[] = {0.114f, 0.587f, 0.299f};
    const float kBGRToUCoeff[] = {-0.169f, -0.331f, 0.5f};
    const float kBGRToVCoeff[] = {0.5f, -0.419f, -0.081f};
    for (int y = 0; y < imageSize.height; ++y) {
        const uint8_t* srcRowPtr = srcBuffer + y * imageSize.width * NUM_4;
        uint8_t* destRowPtr = *destBuffer + y * imageSize.width;
        for (int x = NUM_0; x < imageSize.width; ++x) {
            uint8_t b = srcRowPtr[x * NUM_4];
            uint8_t g = srcRowPtr[x * NUM_4 + NUM_1];
            uint8_t r = srcRowPtr[x * NUM_4 + NUM_2];
            float destPlaneValueY = 
                kBGRToYCoeff[NUM_0] * r + kBGRToYCoeff[NUM_1] * g + kBGRToYCoeff[NUM_2] * b;
            float destPlaneValueU = 
                kBGRToUCoeff[NUM_0] * r + kBGRToUCoeff[NUM_1] * g + kBGRToUCoeff[NUM_2] * b + NUM_128;
            float destPlaneValueV = 
                kBGRToVCoeff[NUM_0] * r + kBGRToVCoeff[NUM_1] * g + kBGRToVCoeff[NUM_2] * b + NUM_128;
            destRowPtr[x] = static_cast<uint8_t>(destPlaneValueY);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * 
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2) + (x / NUM_2) * NUM_2 + NUM_1]
                = static_cast<uint8_t>(destPlaneValueU);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * 
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2) + (x / NUM_2) * NUM_2]
                = static_cast<uint8_t>(destPlaneValueV);
        }
    }
}

static void NV12ToYV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const int32_t srcPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t srcPlaneSizeUV = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    for (int32_t i = NUM_0; i < srcPlaneSizeY; i++) {
        (*destBuffer)[i] = srcBuffer[i];
    }
    for (int32_t i = NUM_0; i < srcPlaneSizeUV; i++) {
        (*destBuffer)[srcPlaneSizeY + i] = srcBuffer[srcPlaneSizeY + i * NUM_2 + NUM_1];
        (*destBuffer)[srcPlaneSizeY + ((srcPlaneSizeUV + NUM_1) / NUM_2 * NUM_2) + i] =
            srcBuffer[srcPlaneSizeY + i * NUM_2];
    }
}

static void NV21ToYV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const int32_t srcPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t srcPlaneSizeUV = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    for (int32_t i = NUM_0; i < srcPlaneSizeY; i++) {
        (*destBuffer)[i] = srcBuffer[i];
    }
    for (int32_t i = NUM_0; i < srcPlaneSizeUV; i++) {
        (*destBuffer)[srcPlaneSizeY + i] = srcBuffer[srcPlaneSizeY + i * NUM_2];
        (*destBuffer)[srcPlaneSizeY + ((srcPlaneSizeUV + NUM_1) / NUM_2 * NUM_2) + i]
            = srcBuffer[srcPlaneSizeY + i * NUM_2 + NUM_1];
    }
}

static void NV12ToRGBAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t *destBuffer)
{
    const int32_t yStride = imageSize.width;
    const uint8_t *yPlane = srcBuffer;
    const uint8_t *uvPlane = srcBuffer + yStride * imageSize.height;

    for (int i = NUM_0; i < imageSize.height; i++) {
        int yRowIndex = i * imageSize.width;
        int uvRowIndex = (imageSize.width % NUM_2 == NUM_0) ? (i / NUM_2) * imageSize.width :
            (i / NUM_2) * (imageSize.width + NUM_1) / NUM_2 * NUM_2;
        for (int j = NUM_0; j < imageSize.width; j++) {
            int yIndex = yRowIndex + j;
            int uvIndex = uvRowIndex + (j / NUM_2) * NUM_2;
            int y = yPlane[yIndex];
            int u = uvPlane[uvIndex];
            int v = uvPlane[uvIndex + NUM_1];
            uint32_t r = static_cast<int>(y + 1.402 * (v - NUM_128) + 0.5);
            uint32_t g = static_cast<int>(y -0.344136 * (u - NUM_128) -0.714136 * (v - NUM_128) +0.5);
            uint32_t b = static_cast<int>(y + 1.772 * (u - NUM_128) +0.5);
            r = r < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : r);;
            g = g < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : g);
            b = b < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : b);
            int rgbaIndex = (i * imageSize.width + j) * NUM_4;
            destBuffer[rgbaIndex] = static_cast<uint8_t>(r);
            destBuffer[rgbaIndex + NUM_1] = static_cast<uint8_t>(g);
            destBuffer[rgbaIndex + NUM_2] = static_cast<uint8_t>(b);
            destBuffer[rgbaIndex + NUM_3] = NUM_255;
        }
    }
}

static void YU12ToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const int32_t destSizeY = imageSize.width * imageSize.height;
    const int32_t destSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    for (int32_t i = NUM_0; i < destSizeY; i++) {
        (*destBuffer)[i] = srcBuffer[i];
    }
    const uint8_t *srcPlaneU = srcBuffer + destSizeY;
    const uint8_t *srcPlaneV = srcPlaneU + destSizeVU;
    uint8_t *destPlaneVU = *destBuffer + destSizeY;
    for (int32_t i = NUM_0; i < destSizeVU; i++) {
        destPlaneVU[i * NUM_2] = srcPlaneV[i];
        destPlaneVU[i * NUM_2 + NUM_1] = srcPlaneU[i];
    }
}

static void NV21ToYU12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const int32_t srcPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t srcPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    for (int32_t i = NUM_0; i < srcPlaneSizeY; i++) {
        (*destBuffer)[i] = srcBuffer[i];
    }
    const uint8_t *srcVU = srcBuffer + srcPlaneSizeY;
    uint8_t *destPlaneSizeU = *destBuffer + srcPlaneSizeY;
    uint8_t *destPlaneSizeV = *destBuffer + srcPlaneSizeY + srcPlaneSizeVU;
    for (int32_t i = NUM_0; i < srcPlaneSizeVU; i++) {
        destPlaneSizeU[i] = srcVU[i * NUM_2 + NUM_1];
        destPlaneSizeV[i] = srcVU[i * NUM_2];
    }
}

static void NV12ToBGRAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t *destBuffer)
{
    const uint8_t *yPlane = srcBuffer;
    const uint8_t *uvPlane = srcBuffer + imageSize.width * imageSize.height;
    uint8_t *bgraBuffer = destBuffer;
    for (int y = NUM_0; y < imageSize.height; y++) {
        for (int x = NUM_0; x < imageSize.width; x++) {
             int uvIndex = (imageSize.width % NUM_2 == NUM_0) ? (y / NUM_2) * imageSize.width + (x / NUM_2) * NUM_2 :
                (y / NUM_2) * (imageSize.width + NUM_1) / NUM_2 * NUM_2 + (x / NUM_2) * NUM_2;
            int yIndex = y * imageSize.width + x;
            int Y = yPlane[yIndex];
            int U = uvPlane[uvIndex];
            int V = uvPlane[uvIndex + NUM_1];
            uint8_t R = (NUM_298 * (Y - NUM_16) + NUM_409 * (V - NUM_128) + NUM_128) >> NUM_8;
            uint8_t G = (NUM_298 * (Y - NUM_16) - NUM_100 * (U - NUM_128) - NUM_208 * (V - NUM_128) + NUM_128) >> NUM_8;
            uint8_t B = (NUM_298 * (Y - NUM_16) + NUM_516 * (U - NUM_128) + NUM_128) >> NUM_8;
            R = (R < NUM_0) ? NUM_0 : ((R > NUM_255) ? NUM_255 : R);
            G = (G < NUM_0) ? NUM_0 : ((G > NUM_255) ? NUM_255 : G);
            B = (B < NUM_0) ? NUM_0 : ((B > NUM_255) ? NUM_255 : B);
            bgraBuffer[NUM_4 * (y * imageSize.width + x)] = B;
            bgraBuffer[NUM_4 * (y * imageSize.width + x) + NUM_1] = G;
            bgraBuffer[NUM_4 * (y * imageSize.width + x) + NUM_2] = R;
            bgraBuffer[NUM_4 * (y * imageSize.width + x) + NUM_3] = NUM_255;
        }
    }
}

static void RGBAToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    const float kBGRToYCoeff[] = {0.114f, 0.587f, 0.299f};
    const float kBGRToUCoeff[] = {-0.169f, -0.331f, 0.5f};
    const float kBGRToVCoeff[] = {0.5f, -0.419f, -0.081f};
    for (int y = NUM_0; y < imageSize.height; ++y) {
        const uint8_t* srcRowPtr = srcBuffer + y * imageSize.width * NUM_4;
        uint8_t* destRowPtr = *destBuffer + y * imageSize.width;
        for (int x = NUM_0; x < imageSize.width; ++x) {
            uint8_t b = srcRowPtr[x * NUM_4];
            uint8_t g = srcRowPtr[x * NUM_4 + NUM_1];
            uint8_t r = srcRowPtr[x * NUM_4 + NUM_2];
            float destPlaneValueY = kBGRToYCoeff[NUM_0] * r + kBGRToYCoeff[NUM_1] * g + kBGRToYCoeff[NUM_2] * b;
            float destPlaneValueU = kBGRToUCoeff[NUM_0] * r + kBGRToUCoeff[NUM_1] * g + kBGRToUCoeff[NUM_2] * b +
                128.0f;
            float destPlaneValueV = kBGRToVCoeff[NUM_0] * r + kBGRToVCoeff[NUM_1] * g + kBGRToVCoeff[NUM_2] * b +
                128.0f;
            destRowPtr[x] = static_cast<uint8_t>(destPlaneValueY);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * ((imageSize.width+NUM_1) / NUM_2 * NUM_2) +
                (x / NUM_2) * NUM_2 + NUM_1] = static_cast<uint8_t>(destPlaneValueU);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * ((imageSize.width+NUM_1) / NUM_2 * NUM_2) +
                (x / NUM_2) * NUM_2] = static_cast<uint8_t>(destPlaneValueV);
        }
    }
}

static void RGBAToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    const float kBGRToYCoeff[] = {0.114f, 0.587f, 0.299f};
    const float kBGRToUCoeff[] = {-0.169f, -0.331f, 0.5f};
    const float kBGRToVCoeff[] = {0.5f, -0.419f, -0.081f};
    for (int y = NUM_0; y < imageSize.height; ++y) {
        const uint8_t* srcRowPtr = srcBuffer + y * imageSize.width * NUM_4;
        uint8_t* destRowPtr = *destBuffer + y * imageSize.width;
        for (int x = NUM_0; x < imageSize.width; ++x) {
            uint8_t b = srcRowPtr[x * NUM_4];
            uint8_t g = srcRowPtr[x * NUM_4 + NUM_1];
            uint8_t r = srcRowPtr[x * NUM_4 + NUM_2];
            float destPlaneValueY = kBGRToYCoeff[NUM_0] * r + kBGRToYCoeff[NUM_1] * g + kBGRToYCoeff[NUM_2] * b;
            float destPlaneValueU = kBGRToUCoeff[NUM_0] * r + kBGRToUCoeff[NUM_1] * g + kBGRToUCoeff[NUM_2] * b +
                128.0f;
            float destPlaneValueV = kBGRToVCoeff[NUM_0] * r + kBGRToVCoeff[NUM_1] * g + kBGRToVCoeff[NUM_2] * b +
                128.0f;
            destRowPtr[x] = static_cast<uint8_t>(destPlaneValueY);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * ((imageSize.width+NUM_1) / NUM_2 * NUM_2) +
                (x / NUM_2) * NUM_2 + NUM_1] = static_cast<uint8_t>(destPlaneValueV);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * ((imageSize.width+NUM_1) / NUM_2 * NUM_2) +
                (x / NUM_2) * NUM_2] = static_cast<uint8_t>(destPlaneValueU);
        }
    }
}

static void RGBToNV21Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t *destBuffer)
{
    for (int h = NUM_0; h < imageSize.height; ++h) {
        for (int w = NUM_0; w < imageSize.width; ++w) {
            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
            const int srcIndex = (h * imageSize.width + w) * NUM_3;
            const int desIndexY = h * imageSize.width + w;
            const int desIndexVU = imageSize.width * imageSize.height + (h / NUM_2) * widthEven + w / NUM_2 * NUM_2;
            const uint8_t r = srcBuffer[srcIndex];
            const uint8_t g = srcBuffer[srcIndex + NUM_1];
            const uint8_t b = srcBuffer[srcIndex + NUM_2];
            const uint8_t y = ((NUM_66 * r + NUM_129 * g + NUM_25 * b + NUM_128) >> NUM_8) + NUM_16;
            const uint8_t u = ((-NUM_38 * r - NUM_74 * g + NUM_112 * b + NUM_128) >> NUM_8) + NUM_128;
            const uint8_t v = ((NUM_112 * r - NUM_94 * g - NUM_18 * b + NUM_128) >> NUM_8) + NUM_128;
            destBuffer[desIndexY] = y;
            destBuffer[desIndexVU] = v;
            destBuffer[desIndexVU + NUM_1] = u;
        }
    }
}

static void NV21ToRGBAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcUV = srcBuffer + imageSize.width * imageSize.height;
    for (int h = NUM_0; h < imageSize.height; h++) {
        for (int w = NUM_0; w < imageSize.width; w++) {
            int yIndex = h * imageSize.width + w;
            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width :
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2);

            uint8_t y = srcY[yIndex];
            uint8_t u = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2 + NUM_1];
            uint8_t v = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2];

            uint8_t r = static_cast<uint8_t>(y + 1.402 * (v - NUM_128));
            uint8_t g = static_cast<uint8_t>(y - 0.344136 * (u - NUM_128) - 0.714136 * (v - NUM_128));
            uint8_t b = static_cast<uint8_t>(y + 1.772 * (u - NUM_128));

            (*destBuffer)[NUM_4 * yIndex] = r < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : r);
            (*destBuffer)[NUM_4 * yIndex + NUM_1] = g < NUM_0 ? NUM_0 : (g > NUM_255 ? NUM_255 : g);
            (*destBuffer)[NUM_4 * yIndex + NUM_2] = b < NUM_0 ? NUM_0 : (b > NUM_255 ? NUM_255 : b);
            (*destBuffer)[NUM_4 * yIndex + NUM_3] = NUM_255;
        }
    }
}

static void NV21ToRGB565Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcUV = srcBuffer + imageSize.width * imageSize.height;
    for (int h = NUM_0; h < imageSize.height; h++) {
        for (int w = NUM_0; w < imageSize.width; w++) {
            int yIndex = h * imageSize.width + w;

            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width :
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2);
            uint8_t y = srcY[yIndex];
            uint8_t u = srcUV[(h / NUM_2) * widthEven + NUM_1 + (w / NUM_2) * NUM_2 + NUM_1];
            uint8_t v = srcUV[(h / NUM_2) * widthEven + NUM_1 + (w / NUM_2) * NUM_2];

            uint8_t r = static_cast<uint8_t>(y + 1.402 * (v - NUM_128));
            uint8_t g = static_cast<uint8_t>(y - 0.344136 * (u - NUM_128) - 0.714136 * (v - NUM_128));
            uint8_t b = static_cast<uint8_t>(y + 1.772 * (u - NUM_128));

            int32_t r1 = r < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : r);
            int32_t g1 = g < NUM_0 ? NUM_0 : (g > NUM_255 ? NUM_255 : g);
            int32_t b1 = b < NUM_0 ? NUM_0 : (b > NUM_255 ? NUM_255 : b);

            uint16_t rgb565 = ((r1 >> NUM_3) << NUM_0) | ((g1 >> NUM_2) << NUM_5) | ((b1 >> NUM_3) << NUM_11);

            (*destBuffer)[yIndex * NUM_2] = rgb565 & 0xFF;
            (*destBuffer)[yIndex * NUM_2 + NUM_1] = (rgb565 >> 8) & 0xFF;
        }
    }
}

static void NV21ToBGRAManual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcUV = srcBuffer + imageSize.width * imageSize.height;
    for (int h = NUM_0; h < imageSize.height; h++) {
        for (int w = NUM_0; w < imageSize.width; w++) {
            int yIndex = h * imageSize.width + w;

            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width :
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2);
            uint8_t y = srcY[yIndex];
            uint8_t u = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2 + NUM_1];
            uint8_t v = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2];

            uint8_t r = static_cast<uint8_t>(y + 1.402 * (v - NUM_128));
            uint8_t g = static_cast<uint8_t>(y - 0.344136 * (u - NUM_128) - 0.714136 * (v - NUM_128));
            uint8_t b = static_cast<uint8_t>(y + 1.772 * (u - NUM_128));

            (*destBuffer)[NUM_4 * yIndex] = b < NUM_0 ? NUM_0 : (b > NUM_255 ? NUM_255 : b);
            (*destBuffer)[NUM_4 * yIndex + NUM_1] = g < NUM_0 ? NUM_0 : (g > NUM_255 ? NUM_255 : g);
            (*destBuffer)[NUM_4 * yIndex + NUM_2] = r < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : r);
            (*destBuffer)[NUM_4 * yIndex + NUM_3] = NUM_255;
        }
    }
}

static void RGBToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    for (int h = NUM_0; h < imageSize.height; ++h) {
        for (int w = NUM_0; w < imageSize.width; ++w) {
            const int srcIndex = (h * imageSize.width + w) * NUM_3;
            const int desIndexY = h * (imageSize.width) + w;

            const uint8_t r = srcBuffer[srcIndex];
            const uint8_t g = srcBuffer[srcIndex + NUM_1];
            const uint8_t b = srcBuffer[srcIndex + NUM_2];

            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width :
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2);
            const int desIndexUV = imageSize.width * imageSize.height + (h / NUM_2) * widthEven + (w / NUM_2) * NUM_2;
            (*destBuffer)[desIndexY] = static_cast<uint8_t>(0.114f * r + 0.587f * g + 0.299f * b);
            (*destBuffer)[desIndexUV] = static_cast<uint8_t>(-0.169f * r - 0.331f * g + 0.5 * b + NUM_128);
            (*destBuffer)[desIndexUV + NUM_1] = static_cast<uint8_t>(0.5f * r - 0.419f * g - 0.081f * b + NUM_128);

        }
    }
}

static void BGRAToNV12Manual(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer)
{
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    const float kBGRToYCoeff[] = {0.114f, 0.587f, 0.299f};
    const float kBGRToUCoeff[] = {-0.169f, -0.331f, 0.5f};
    const float kBGRToVCoeff[] = {0.5f, -0.419f, -0.081f};
    for (int y = NUM_0; y < imageSize.height; ++y) {
        const uint8_t* srcRowPtr = srcBuffer + y * imageSize.width * NUM_4;
        uint8_t* destRowPtr = *destBuffer + y * imageSize.width;
        for (int x = NUM_0; x < imageSize.width; ++x) {
            uint8_t b = srcRowPtr[x * NUM_4];
            uint8_t g = srcRowPtr[x * NUM_4 + NUM_1];
            uint8_t r = srcRowPtr[x * NUM_4 + NUM_2];
            float destPlaneValueY = kBGRToYCoeff[NUM_0] * r + kBGRToYCoeff[NUM_1] * g + kBGRToYCoeff[NUM_2] * b;
            float destPlaneValueU = kBGRToUCoeff[NUM_0] * r + kBGRToUCoeff[NUM_1] * g + kBGRToUCoeff[NUM_2] * b +
                128.0f;
            float destPlaneValueV = kBGRToVCoeff[NUM_0] * r + kBGRToVCoeff[NUM_1] * g + kBGRToVCoeff[NUM_2] * b +
                128.0f;
            destRowPtr[x] = static_cast<uint8_t>(destPlaneValueY);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * ((imageSize.width+NUM_1) / NUM_2 * NUM_2) +
                (x / NUM_2) * NUM_2] = static_cast<uint8_t>(destPlaneValueU);
            (*destBuffer)[destPlaneSizeY + (y / NUM_2) * ((imageSize.width+NUM_1) / NUM_2 * NUM_2) +
                (x / NUM_2) * NUM_2 + NUM_1] = static_cast<uint8_t>(destPlaneValueV);
        }
    }
}

bool NV12ToRGB565(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcUV = srcBuffer + imageSize.width * imageSize.height ;
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_2 );
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    int dstStrideRGB565 = imageSize.width * NUM_2;
    int widthEven = (imageSize.width % NUM_2 == 0) ? (imageSize.width) : (imageSize.width + NUM_1) / NUM_2 * NUM_2;
    int srcStrideUV = widthEven;
    libyuv::NV12ToRGB565Matrix(srcY, imageSize.width, srcUV ,widthEven, *destBuffer, dstStrideRGB565,
	    yuvConstants, imageSize.width, imageSize.height);
#else
    for (int h = NUM_0; h < imageSize.height; h++) {
        for (int w = NUM_0; w < imageSize.width; w++) {
            int yIndex = h * imageSize.width + w;
            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width :
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2);
            uint8_t y = srcY[yIndex];
            uint8_t v = srcUV[(h / NUM_2) * widthEven + NUM_1 + (w / NUM_2) * NUM_2 + NUM_1];
            uint8_t u = srcUV[(h / NUM_2) * widthEven + NUM_1 + (w / NUM_2) * NUM_2];
            uint32_t r = static_cast<uint32_t>(y + 1.402 * (v - NUM_128));
            uint32_t g = static_cast<uint32_t>(y - 0.344136 * (u - NUM_128) - 0.714136 * (v - NUM_128));
            uint32_t b = static_cast<uint32_t>(y + 1.772 * (u - NUM_128));
            int32_t r1 = r < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : r);
            int32_t g1 = g < NUM_0 ? NUM_0 : (g > NUM_255 ? NUM_255 : g);
            int32_t b1 = b < NUM_0 ? NUM_0 : (b > NUM_255 ? NUM_255 : b);
            uint16_t rgb565 = ((r1 >> NUM_3) << NUM_0) | ((g1 >> NUM_2) << NUM_5) | ((b1 >> NUM_3) << NUM_11);
            (*destBuffer)[yIndex * NUM_2] = rgb565 & 0xFF;
            (*destBuffer)[yIndex * NUM_2 + NUM_1] = (rgb565 >> 8) & 0xFF;
        }
    }
#endif
    return true;
}

bool RGBAF16ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize + (((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 ) * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr)
    {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    for (int h = 0; h < imageSize.height; h++) {
        for (int w = 0; w < imageSize.width; w++) {
            uint16_t r16 = static_cast<uint16_t>(srcBuffer[NUM_4 * (h * imageSize.width + w)]);
            uint16_t g16 = static_cast<uint16_t>(srcBuffer[NUM_4 * (h * imageSize.width + w) + NUM_1]);
            uint16_t b16 = static_cast<uint16_t>(srcBuffer[NUM_4 * (h * imageSize.width + w) + NUM_2]);
            uint32_t r = static_cast<uint32_t>(r16 >> NUM_8);
            uint32_t g = static_cast<uint32_t>(g16 >> NUM_8);
            uint32_t b = static_cast<uint32_t>(b16 >> NUM_8);
            float y = 0.299f * r + 0.587f * g + 0.114f * b;
            int currentIndex = h * imageSize.width + w;
            (*destBuffer)[currentIndex] = static_cast<uint8_t> (y < NUM_0) ? NUM_0 : ((y > NUM_255) ? NUM_255 : y);
            if (h % NUM_2 == 0 && w % NUM_2 == 0) {
                int width_even = (imageSize.width % NUM_2 == 0) ? (imageSize.width) : (imageSize.width + NUM_1);
                int uIndex = frameSize + (h / NUM_2) * (width_even / NUM_2) + (w / NUM_2) * NUM_2;
                int vIndex = frameSize + (h / NUM_2) * (width_even / NUM_2) + (w / NUM_2) * NUM_2 + NUM_1;
                float u = -0.14713f * r - 0.28886f * g + 0.436f * b + 128;
                float v = 0.615f * r - 0.51499f * g - 0.10001f * b + 128;
                (*destBuffer)[uIndex] = static_cast<uint8_t>(u < NUM_0) ? NUM_0 : ((u > NUM_255) ? NUM_255 : u);
                (*destBuffer)[vIndex] = static_cast<uint8_t>(v < NUM_0) ? NUM_0 : ((v > NUM_255) ? NUM_255 : v);
            }
        }
    }
    return true;
}

bool YU12ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    int32_t width = imageSize.width;
    int32_t height = imageSize.height;
    int32_t frameSize = width * height;
    destBufferSize = frameSize + ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2) * NUM_2;
    *destBuffer = new uint8_t[destBufferSize];
#ifdef LIBYUV
    const int32_t src_stride_y = width;
    const int32_t src_stride_u = (width + NUM_1) / NUM_2;
    const int32_t src_stride_v = (width + NUM_1) / NUM_2;
    const int32_t dest_stride_y = width;
    const int32_t dest_stride_uv = (width + NUM_1) / NUM_2 * NUM_2;
    libyuv::I420ToNV12(srcBuffer, src_stride_y, srcBuffer + width*height,
        src_stride_u, srcBuffer + frameSize + ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2),
        src_stride_v, *destBuffer, width, *destBuffer + width*height, dest_stride_uv, width, height);
#else
    uint8_t *yDest = *destBuffer;
    uint8_t *uvDest = *destBuffer + frameSize;
    int32_t uvSize = ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2);
    memcpy_s(*destBuffer, frameSize, srcBuffer, frameSize);
    const uint8_t *uSrc = srcBuffer + frameSize;
    const uint8_t *vSrc = uSrc + uvSize;
    for (int i = 0; i < uvSize; i++) {
        uvDest[i * NUM_2] = uSrc[i];
        uvDest[i * NUM_2 + NUM_1] = vSrc[i];
    }
#endif
    return true;
}

bool NV21ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    int32_t width = imageSize.width;
    int32_t height = imageSize.height;
    destBufferSize = width * height + ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2) * NUM_2;
    *destBuffer = new uint8_t[destBufferSize];
#ifdef LIBYUV
    int32_t i420_buffer_size = destBufferSize;
    std::unique_ptr<uint8_t[]> i420_buffer(new uint8_t[i420_buffer_size]);
    libyuv::NV21ToI420(srcBuffer, width, srcBuffer + width * height, ((width + NUM_1) / NUM_2) * NUM_2, i420_buffer,
        width, i420_buffer + width * height, (width + NUM_1) / NUM_2, i420_buffer + width * height +
        ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2), (width + NUM_1) / NUM_2, width, height);
    libyuv::I420ToNV12(i420_buffer, width, i420_buffer + width * height, (width + NUM_1) / NUM_2,
        i420_buffer + width * height + ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2), (width + NUM_1) / NUM_2,
        *destBuffer, width, *destBuffer + width * height, ((width + NUM_1) / NUM_2) * NUM_2, width, height);
#else
    int frameSize = width * height;
    int32_t uvSize = ((width + NUM_1) / NUM_2) * ((height + NUM_1) / NUM_2);
    memcpy_s(*destBuffer, frameSize, srcBuffer, frameSize);
    const uint8_t* srcUV = srcBuffer + frameSize;
    uint8_t* destUV = *destBuffer + frameSize;
    for (int i = NUM_0; i < uvSize; i++) {
        destUV[i * NUM_2] = srcUV[i * NUM_2 + NUM_1];
        destUV[NUM_2 * i + NUM_1] = srcUV[i * NUM_1];
    }

#endif
    return true;
}

bool NV21ToRGBAF16(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize * sizeof(uint64_t);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    for (int h = 0; h < imageSize.height; h++) {
        for (int w = 0; w < imageSize.width; w++) {
            uint8_t y = srcBuffer[h * imageSize.width + w];
            uint8_t u = srcBuffer[frameSize + (h / NUM_2) * ((imageSize.width + NUM_1) / NUM_2) + (w / NUM_2) * NUM_2];
            uint8_t v = srcBuffer[frameSize + (h / NUM_2) * ((imageSize.width + NUM_1) / NUM_2) + 
                                 (w / NUM_2) * NUM_2 + NUM_1];
            float yy = static_cast<float>(y);
            float uu = static_cast<float>(u) - 128.0f;
            float vv = static_cast<float>(v) - 128.0f;
            float r = yy + 1.13983f * vv;
            float g = yy - 0.39465f * uu - 0.5806f * vv;
            float b = yy + 2.03211f * uu;
            uint16_t r16 = static_cast<uint16_t>(r * 65535.0f);
            uint16_t g16 = static_cast<uint16_t>(g * 65535.0f);
            uint16_t b16 = static_cast<uint16_t>(b * 65535.0f);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w)] = static_cast<uint8_t>(r16 & 0xFF);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w) + NUM_1] = static_cast<uint8_t>(g16 & 0xFF);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w) + NUM_2] = static_cast<uint8_t>(b16 & 0xFF);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w) + NUM_3] = NUM_255;
            
        }
    }
    return true;
}

bool NV12ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) *((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]>yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if(yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    libyuv::NV12ToI420(srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height,
        ((imageSize.width + NUM_1) / NUM_2) * NUM_2, yu12Buffer, imageSize.width, yu12Buffer +
        imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToNV21(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width, *destBuffer + imageSize.width * imageSize.height,
        ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
#else
    int32_t frameSize = imageSize.width * imageSize.height;
    int32_t uvSize = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    memcpy_s(*destBuffer, frameSize, srcBuffer, frameSize);
    const uint8_t* srcUV = srcBuffer + frameSize;
    uint8_t* destUV = *destBuffer + frameSize;
    for (int i = 0; i < uvSize; i++) {
        destUV[i * NUM_2] = srcUV[i * NUM_2 + NUM_1];
        destUV[NUM_2 * i + NUM_1] = srcUV[i * NUM_2];
    }
#endif
    return true;
}

bool BGRAToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0)
    {
        return false;
    }
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    size_t srcPlaneSizeUV = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + srcPlaneSizeUV * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        std::cerr << "apply space for dest buffer failed!" << std::endl;
        return false;
    }
#ifdef LIBYUV
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + destPlaneSizeY;
    libyuv::ARGBToNV12(srcBuffer, imageSize.width * NUM_4, nv12Y, imageSize.width, nv12UV,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    BGRAToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGB565ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    libyuv::RGB565ToI420(srcBuffer, imageSize.width * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1)/ NUM_2, yu12Buffer +
        imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToNV12(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
        (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width, *destBuffer + imageSize.width *
        imageSize.height, ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
#else
    RGB565ToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGB565ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2]());
    if(yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    libyuv::RGB565ToI420(srcBuffer, imageSize.width * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2, yu12Buffer +
        imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2), (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToNV21(yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2, yu12Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
        (imageSize.width + NUM_1) / NUM_2, *destBuffer, imageSize.width, *destBuffer + imageSize.width *
        imageSize.height, ((imageSize.width + NUM_1) / NUM_2) * NUM_2, imageSize.width, imageSize.height);
#else
    RGB565ToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToRGBAF16(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < 0 || imageSize.height < 0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize * sizeof(uint64_t);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    for (int h = 0; h < imageSize.height; h++) {
        for (int w = 0; w < imageSize.width; w++) {
            uint8_t y = srcBuffer[h * imageSize.width + w];
            uint8_t v = srcBuffer[frameSize + (h / NUM_2) * ((imageSize.width + NUM_1) / NUM_2) + (w / NUM_2) * NUM_2];
            uint8_t u = srcBuffer[frameSize + (h / NUM_2) * ((imageSize.width + NUM_1) / NUM_2) + 
                                 (w / NUM_2) * NUM_2 + NUM_1];
            float yy = static_cast<float>(y);
            float uu = static_cast<float>(u) - 128.0f;
            float vv = static_cast<float>(v) - 128.0f;
            float r = yy + 1.13983f * vv;
            float g = yy - 0.39465f * uu - 0.5806f * vv;
            float b = yy + 2.03211f * uu;
            uint16_t r16 = static_cast<uint16_t>(r * 65535.0f);
            uint16_t g16 = static_cast<uint16_t>(g * 65535.0f);
            uint16_t b16 = static_cast<uint16_t>(b * 65535.0f);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w)] = static_cast<uint8_t>(r16 & 0xFF);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w) + NUM_1] = static_cast<uint8_t>(g16 & 0xFF);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w) + NUM_2] = static_cast<uint8_t>(b16 & 0xFF);
            (*destBuffer)[NUM_4 * (h * imageSize.width + w) + NUM_3] = NUM_255;
        }
    }
    return true;
}

bool YU12ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer, size_t &destBufferSize,
                [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const int32_t destPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t destPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcU = srcBuffer + destPlaneSizeY;
    const uint8_t *srcV = srcBuffer + destPlaneSizeY + destPlaneSizeVU;
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + destPlaneSizeY;
    libyuv::I420ToNV21(srcY, imageSize.width, srcU, (imageSize.width + NUM_1) / NUM_2, srcV,
        (imageSize.width + NUM_1) / NUM_2, nv21Y, imageSize.width, nv21VU, 
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    YU12ToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV21ToYU12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer, size_t &destBufferSize,
                [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const int32_t srcplaneSizeY = imageSize.width * imageSize.height;
    const int32_t srcPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(srcplaneSizeY + srcPlaneSizeVU * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcVU = srcBuffer + imageSize.width * imageSize.height;
    uint8_t *yuv420Y = *destBuffer;
    uint8_t *yuv420U = *destBuffer + srcplaneSizeY;
    uint8_t *yuv420V = *destBuffer + srcplaneSizeY + srcPlaneSizeVU;
    libyuv::NV21ToI420(srcY, imageSize.width, srcVU, (imageSize.width + NUM_1) / NUM_2 * NUM_2, yuv420Y, 
        imageSize.width, yuv420U, (imageSize.width + NUM_1) / NUM_2, yuv420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
#else
    NV21ToYU12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool BGRAToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer, size_t &destBufferSize,
                [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const int32_t destPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t destPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + destPlaneSizeY;
    libyuv::ARGBToNV21(srcBuffer, imageSize.width * NUM_4, nv21Y, imageSize.width, nv21VU,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    BGRAToNV21Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToYV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer, size_t &destBufferSize,
                [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const int32_t srcPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t srcPlaneSizeUV = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);

    destBufferSize = static_cast<size_t>(srcPlaneSizeY + srcPlaneSizeUV * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcVU = srcBuffer + srcPlaneSizeY;
    uint8_t *yuv420Y = *destBuffer;
    uint8_t *yuv420V = *destBuffer + srcPlaneSizeY;
    uint8_t *yuv420U = *destBuffer + srcPlaneSizeY + srcPlaneSizeUV;
    libyuv::NV12ToI420(srcY, imageSize.width, srcVU, (imageSize.width + NUM_1) / NUM_2 * NUM_2, yuv420Y, 
        imageSize.width, yuv420U, (imageSize.width + NUM_1) / NUM_2, yuv420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
#else
    NV12ToYV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV21ToYV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer, size_t &destBufferSize,
                [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const int32_t srcPlaneSizeY = imageSize.width * imageSize.height;
    const int32_t srcPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(srcPlaneSizeY + srcPlaneSizeVU * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcVU = srcBuffer + srcPlaneSizeY;
    uint8_t *yuv420Y = *destBuffer;
    uint8_t *yuv420V = *destBuffer + srcPlaneSizeY;
    uint8_t *yuv420U = *destBuffer + srcPlaneSizeY + srcPlaneSizeVU;
    libyuv::NV21ToI420(srcY, imageSize.width, srcVU, (imageSize.width + NUM_1) / NUM_2 * NUM_2, yuv420Y,
        imageSize.width, yuv420U, (imageSize.width + NUM_1) / NUM_2, yuv420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
#else
    NV21ToYV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV12ToRGBA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

#ifdef LIBYUV
    const int32_t yStride= imageSize.width;
    const uint8_t *yPlane = srcBuffer;
    const uint8_t *uvPlane = srcBuffer + yStride* imageSize.height;
    const int32_t uvStride = imageSize.width;
    int32_t i420Buffer_size = imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2) * NUM_2;
    std::unique_ptr<uint8_t[]> i420Buffer(new(std::nothrow) uint8_t[i420Buffer_size]());
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    } 
    const struct YuvConstants * YuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    if (imageSize.width % NUM_2 ==NUM_0) {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, uvStride, i420Buffer,
            uvStride, i420Buffer + imageSize.width * imageSize.height, yStride / NUM_2,
            i420Buffer + imageSize.width * imageSize.height + imageSize.width * imageSize.height / NUM_4,
            (yStride + NUM_1) / NUM_2, imageSize.width, imageSize.height);
        libyuv::I420ToRGBAMatrix(i420Buffer, yStride, i420Buffer + imageSize.width * imageSize.height, yStride / NUM_2,
            i420Buffer + imageSize.width * imageSize.height + imageSize.width * imageSize.height / NUM_4,
            yStride / NUM_2, *destBuffer, imageSize.width * NUM_4, YuvConstants, imageSize.width, imageSize.height);
    } else {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, (uvStride + NUM_1) / NUM_2 * NUM_2, i420Buffer, yStride,
            i420Buffer + imageSize.width * imageSize.height, (yStride + NUM_1) / NUM_2,
            i420Buffer + imageSize.width* imageSize.height +
            ((imageSize.width+ NUM_1) / NUM_2)* ((imageSize.height+ NUM_1) / NUM_2),
            (yStride + NUM_1) / NUM_2 , imageSize.width, imageSize.height);
        libyuv::I420ToRGBAMatrix(i420Buffer, yStride, i420Buffer + yStride * imageSize.height,
            (yStride + NUM_1) / NUM_2, i420Buffer + imageSize.height * yStride +
            (imageSize.height + NUM_1) / NUM_2 * (yStride + NUM_1) / NUM_2,
            (yStride + NUM_1) / NUM_2, *destBuffer, yStride * NUM_4, YuvConstants, imageSize.width, imageSize.height);
    }
#else
    NV12ToRGBAManual(srcBuffer, imageSize, *destBuffer);
#endif
    return true;
}

bool NV12ToBGRA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

#ifdef LIBYUV
    const int32_t yStride= imageSize.width;
    const int32_t uvStride = imageSize.width;
    const uint8_t *yPlane = srcBuffer;
    const uint8_t *uvPlane = srcBuffer + imageSize.width * imageSize.height;
    int32_t i420Buffer_size = ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2) * NUM_2;
    std::unique_ptr<uint8_t[]> i420Buffer(new(std::nothrow) uint8_t[i420Buffer_size]);
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    } 
    if (imageSize.width % NUM_2 == NUM_0) {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, uvStride, i420Buffer, yStride,
            i420Buffer + imageSize.width * imageSize.height, yStride / NUM_2,
            i420Buffer + imageSize.width * imageSize.height + imageSize.width * imageSize.height / NUM_4,
            yStride/ NUM_2, imageSize.width, imageSize.height);
        libyuv::I420ToBGRA(i420Buffer, yStride, i420Buffer + imageSize.width * imageSize.height, yStride / NUM_2,
            i420Buffer + imageSize.width * imageSize.height + imageSize.width * imageSize.height / NUM_4,
            yStride / NUM_2, *destBuffer, imageSize.width * NUM_4, imageSize.width, imageSize.height);
    } 
    else {
        libyuv::NV12ToI420(srcBuffer, yStride, uvPlane, (uvStride + NUM_1) / NUM_2 * NUM_2, i420Buffer, yStride,
            i420Buffer + imageSize.width * imageSize.height, (yStride + NUM_1) / NUM_2,
            i420Buffer + imageSize.width * imageSize.height +
            ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2),
            (yStride + NUM_1) / NUM_2 , imageSize.width, imageSize.height);
        destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
        *destBuffer = new uint8_t[destBufferSize]();
        libyuv::I420ToBGRA(i420Buffer, yStride, i420Buffer + yStride * imageSize.height, (yStride + NUM_1) / NUM_2,
            i420Buffer + imageSize.height * yStride + (imageSize.height + NUM_1) / NUM_2 * (yStride + NUM_1) / NUM_2,
            (yStride + NUM_1) / NUM_2, *destBuffer, yStride * NUM_4, mageSize.width, imageSize.height);
    }
#else
    NV12ToBGRAManual(srcBuffer, imageSize, *destBuffer);
#endif
    return true;
}

bool RGBAF16ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                   size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    uint32_t frameSize = imageSize.width * imageSize.height;
    destBufferSize = frameSize + (((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 ) * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    for (int h = 0; h < imageSize.height; h++) {
        for (int w = 0; w < imageSize.width; w++) {
            float r = static_cast<float>(srcBuffer[NUM_4 * (h * imageSize.width + w)]);
            float g = static_cast<float>(srcBuffer[NUM_4 * (h * imageSize.width + w) + NUM_1]);
            float b = static_cast<float>(srcBuffer[NUM_4 * (h * imageSize.width + w) + NUM_2]);
            float y = 0.299f * r + 0.587f * g + 0.114f * b;
            int currentIndex = h * imageSize.width + w;
            (*destBuffer)[currentIndex] = static_cast<uint8_t> (y < NUM_0) ? NUM_0 : ((y > NUM_255) ? NUM_255 : y);
            if (h % NUM_2 == 0 && w % NUM_2 == 0) {
                int width_even = (imageSize.width % NUM_2 == 0) ? (imageSize.width) : (imageSize.width + NUM_1);
                int uIndex = frameSize + (h / NUM_2) * (width_even / NUM_2) + (w / NUM_2) * NUM_2 + NUM_1;
                int vIndex = frameSize + (h / NUM_2) * (width_even / NUM_2) + (w / NUM_2) * NUM_2;
                float u = -0.14713f * r - 0.28886f * g + 0.436f * b + 128;
                float v = 0.615f * r - 0.51499f * g - 0.10001f * b + 128;
                (*destBuffer)[uIndex] = static_cast<uint8_t>(u < NUM_0) ? NUM_0 : ((u > NUM_255) ? NUM_255 : u);
                (*destBuffer)[vIndex] = static_cast<uint8_t>(v < NUM_0) ? NUM_0 : ((v > NUM_255) ? NUM_255 : v);
            }
        }
    }
    return true;
}

bool RGBAToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
				size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    size_t destPlaneSizeUV = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = destPlaneSizeY + destPlaneSizeUV * NUM_2;
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint32_t i420BufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    std::unique_ptr<uint8[]> i420Buffer(new(std::nothrow) uint8_t[i420BufferSize]());
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *i420Y = i420Buffer;
    uint8_t *i420U = i420Buffer + imageSize.width * imageSize.height;
    uint8_t *i420V = i420Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    libyuv::ARGBToI420(srcBuffer, NUM_4 * imageSize.width, i420Y, imageSize.width, i420U,
    	(imageSize.width + NUM_1) / NUM_2, i420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
    uint8_t *nv12Y = *destBuffer;
    uint8_t *nv12UV = *destBuffer + imageSize.width * imageSize.height;
    libyuv::I420ToNV12(i420Y, imageSize.width, i420V, (imageSize.width + NUM_1) / NUM_2, i420U,
		(imageSize.width + NUM_1) / NUM_2, nv12Y, imageSize.width, nv12UV,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBAToNV12Manual(srcBuffer,imageSize, destBuffer);
#endif
    return true;
}

bool RGBAToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
				size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    size_t destPlaneSizeY = imageSize.width * imageSize.height;
    size_t destPlaneSizeVU = ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    destBufferSize = static_cast<size_t>(destPlaneSizeY + destPlaneSizeVU * NUM_2);
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

#ifdef LIBYUV
    const uint32_t i420BufferSize = static_cast<size_t>(imageSize.width * imageSize.height + 
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    std::unique_ptr<uint8_t[]> i420Buffer(new(std::nothrow) uint8_t[i420BufferSize]());
    if (i420Buffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    uint8_t *i420Y = i420Buffer;
    uint8_t *i420U = i420Buffer + imageSize.width * imageSize.height;
    uint8_t *i420V = i420Buffer + imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    libyuv::ARGBToI420(srcBuffer, NUM_4 * imageSize.width, i420Y, imageSize.width, i420U,
    	(imageSize.width + NUM_1) / NUM_2, i420V, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);
    uint8_t *nv21Y = *destBuffer;
    uint8_t *nv21VU = *destBuffer + imageSize.width * imageSize.height;
    libyuv::I420ToNV21(i420Y, imageSize.width, i420V, (imageSize.width + NUM_1) / NUM_2, i420U,
        (imageSize.width + NUM_1) / NUM_2, nv21Y, imageSize.width, nv21VU,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBAToNV21Manual(srcBuffer,imageSize, destBuffer);
#endif
    return true;
}

bool YV12ToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcV = srcY + imageSize.width * imageSize.height;
    const uint8_t *srcU = srcV + (((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2));
    destBufferSize = static_cast<size_t>
        (imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2 *
        ((imageSize.height + NUM_1) / NUM_2) * NUM_2));
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
    for (int h = NUM_0; h < imageSize.height; h++) {
        for (int w = NUM_0; w < imageSize.width; w++) {
            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
            uint8_t yv12V = srcV[(h / NUM_2) * (widthEven / NUM_2) + (w / NUM_2)];
            uint8_t yv12U = srcU[(h / NUM_2) * (widthEven / NUM_2) + (w / NUM_2)];
            size_t nv12UVIndex = imageSize.width * imageSize.height + (h / NUM_2) * widthEven + (w / NUM_2) * NUM_2;
            (*destBuffer)[h * imageSize.width + w] = srcY[h * imageSize.width + w];
            (*destBuffer)[nv12UVIndex] = yv12V;
            (*destBuffer)[nv12UVIndex + NUM_1] = yv12U;
        }
    }
    return true;
}

bool RGBToNV21(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr ||imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>
        (imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2 *
        ((imageSize.height + NUM_1) / NUM_2) * NUM_2));
    *destBuffer = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const uint32_t yu12BufferSize = static_cast<size_t>(imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[yu12BufferSize]());
    if (yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    uint8_t *I420Y = yu12Buffer;
    uint8_t *I420U = yu12Buffer + imageSize.height * imageSize.width;
    uint8_t *I420V = yu12Buffer + imageSize.height * imageSize.width +
        ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2);
    libyuv::RGB24ToI420(srcBuffer, imageSize.width * NUM_3, I420Y, imageSize.width, I420U,
        (imageSize.width + NUM_1 ) / NUM_2, I420V, (imageSize.width + NUM_1 ) / NUM_2,
        imageSize.width, imageSize.height);
    libyuv::I420ToNV21(I420Y, imageSize.width,I420U, (imageSize.width + NUM_1 )/ NUM_2,
        I420V, (imageSize.width + NUM_1 )/ NUM_2, *destBuffer, imageSize.width,
        *destBuffer + imageSize.width * imageSize.height + NUM_1,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBToNV21Manual(srcBuffer, imageSize, *destBuffer);
#endif
    return true;
}

bool NV12ToRGB(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcUV = srcBuffer + imageSize.width * imageSize.height;
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_3);
    (*destBuffer) = new(std::nothrow)uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    libyuv:: NV12ToRGB24Matrix(srcBuffer, imageSize.width, srcUV + NUM_1, (imageSize.width + NUM_1) / NUM_2 * NUM_2,
        *destBuffer, imageSize.width * NUM_3, &kYuvV2020Constants, imageSize.width, imageSize.height);
#else
    for (int h = NUM_0;h < imageSize.height;++h) {
        for (int w = NUM_0;w < imageSize.width;++w) {
            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? (imageSize.width) : (imageSize.width + NUM_1);
            int yIndex = h * imageSize.width + w;
            uint8_t y = srcY[yIndex];
            uint8_t u = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2];
            uint8_t v = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2 + NUM_1];
            uint8_t r = static_cast<uint8_t>(y + 1.402f * (v - NUM_128));
            uint8_t g = static_cast<uint8_t>(y - 0.344136f * (u - NUM_128) - 0.714136f * (v - NUM_128));
            uint8_t b = static_cast<uint8_t>(y + 1.772f * (u - NUM_128));
            r = (r < NUM_0) ? NUM_0 : (r > NUM_255) ? NUM_255 : r;
            g = (g < NUM_0) ? NUM_0 : (g > NUM_255) ? NUM_255 : g;
            b = (b < NUM_0) ? NUM_0 : (b > NUM_255) ? NUM_255 : b;
            (*destBuffer)[(h * imageSize.width + w) * NUM_3] = r;
            (*destBuffer)[(h * imageSize.width + w) * NUM_3 + NUM_1] = g;
            (*destBuffer)[(h * imageSize.width + w) * NUM_3 + NUM_2] = b;
        }
    }
#endif
    return true;
}

bool NV12ToYU12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcUV = srcBuffer + imageSize.width * imageSize.height;
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2) * NUM_2);
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

    uint8_t *destU = *destBuffer + imageSize.width * imageSize.height;
    uint8_t *destV = *destBuffer + imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2);

#ifdef LIBYUV
    libyuv::NV12ToI420(
        srcBuffer, imageSize.width,
        srcUV, (imageSize.width + NUM_1) / NUM_2 * NUM_2,
        *destBuffer, imageSize.width,
        *destBuffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        *destBuffer + imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
        ((imageSize.height + NUM_1) / NUM_2),
        (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
#else
    for (int i = NUM_0; i < imageSize.width * imageSize.height; i++) {
        (*destBuffer)[i] = srcY[i];
    }

    for (size_t i = NUM_0; i < ((imageSize.width + NUM_1) / NUM_2) * ((imageSize.height + NUM_1) / NUM_2); i++) {
        destU[i] = srcUV[i * NUM_2];
        destV[i] = srcUV[i * NUM_2 + NUM_1];
    }
#endif
    return true;
}

bool NV21ToRGB(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    const uint8_t *srcY = srcBuffer;
    const uint8_t *srcUV = srcBuffer + imageSize.width * imageSize.height;

    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_3);
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    libyuv::NV21ToRGB24Matrix(
        srcBuffer, imageSize.width,
        srcBuffer + imageSize.width * imageSize.height + NUM_1, (imageSize.width + NUM_1) / NUM_2 * NUM_2,
        *destBuffer, imageSize.width * NUM_3, yuvConstants, imageSize.width, imageSize.height);
#else
    for (int h = NUM_0; h < imageSize.height; h++) {
        for (int w = NUM_0; w < imageSize.width; w++) {
            int yIndex = h * imageSize.width + w;
            int widthEven = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width :
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2);
            uint8_t y = srcY[yIndex];
            uint8_t u = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2 + NUM_1];
            uint8_t v = srcUV[(h / NUM_2) * widthEven + (w / NUM_2) * NUM_2];
            uint32_t r = static_cast<uint32_t>(y + 1.402 * (v - NUM_128));
            uint32_t g = static_cast<uint32_t>(y - 0.344136 * (u - NUM_128) - 0.714136 * (v - NUM_128));
            uint32_t b = static_cast<uint32_t>(y + 1.772 * (u - NUM_128));
            (*destBuffer)[NUM_3 * yIndex] = r < NUM_0 ? NUM_0 : (r > NUM_255 ? NUM_255 : r);
            (*destBuffer)[NUM_3 * yIndex + NUM_1] = g < NUM_0 ? NUM_0 : (g > NUM_255 ? NUM_255 : g);
            (*destBuffer)[NUM_3 * yIndex + NUM_2] = b < NUM_0 ? NUM_0 : (b > NUM_255 ? NUM_255 : b);
        }
    }
#endif
    return true;
}

bool NV21ToRGBA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }

    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2)]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::NV21ToI420(
        srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, yu12Buffer, imageSize.width, yu12Buffer +
        imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2), (imageSize.width+NUM_1) / NUM_2, imageSize.width, imageSize.height);
    libyuv::I420ToRGBAMatrix(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height, (imageSize.width+NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width+NUM_1) / NUM_2, *destBuffer, imageSize.width * NUM_4,
        yuvConstants, imageSize.width, imageSize.height);
#else
    NV21ToRGBAManual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV21ToBGRA(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_4);
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2)]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::NV21ToI420(
        srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2,
        (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);

    libyuv::I420ToBGRA(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width * NUM_4, imageSize.width, imageSize.height);
#else
    NV21ToBGRAManual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool NV21ToRGB565(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                  size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height * NUM_2);
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    const struct YuvConstants* yuvConstants = mapColorSPaceToYuvConstants(colorSpace);
    std::unique_ptr<uint8_t[]> yu12Buffer(new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        ((imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2)]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::NV21ToI420(
        srcBuffer, imageSize.width, srcBuffer + imageSize.width * imageSize.height + NUM_1,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2 * NUM_2, (imageSize.width + NUM_1) / NUM_2,
        imageSize.width, imageSize.height);

    libyuv::I420ToRGB565Matrix(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2 * NUM_2, (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width * NUM_2, yuvConstants, imageSize.width, imageSize.height);
#else
    NV21ToRGB565Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool RGBToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
               size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2 * NUM_2);
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }
#ifdef LIBYUV
    uint8_t *yu12Buffer (new(std::nothrow) uint8_t[imageSize.width * imageSize.height +
        (imageSize.width + NUM_1) / NUM_2 * (imageSize.height + NUM_1) / NUM_2 * NUM_2]());
    if (*yu12Buffer == nullptr) {
        IMAGE_LOGD("apply space for I420 buffer failed!");
        return false;
    }
    libyuv::RGB24ToI420(
        srcBuffer, imageSize.width * NUM_3, yu12Buffer, imageSize.width,
        yu12Buffer + imageSize.width * imageSize.height, (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) / NUM_2, imageSize.width, imageSize.height);
        
    libyuv::I420ToNV12(
        yu12Buffer, imageSize.width, yu12Buffer + imageSize.width * imageSize.height,
        (imageSize.width + NUM_1) / NUM_2,
        yu12Buffer + imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2, (imageSize.width + NUM_1) / NUM_2,
        *destBuffer, imageSize.width, *destBuffer + imageSize.width * imageSize.height + NUM_1,
        (imageSize.width + NUM_1) / NUM_2 * NUM_2, imageSize.width, imageSize.height);
#else
    RGBToNV12Manual(srcBuffer, imageSize, destBuffer);
#endif
    return true;
}

bool YV12ToNV12(const uint8_t *srcBuffer, const Size &imageSize, uint8_t **destBuffer,
                size_t &destBufferSize, [[maybe_unused]]ColorSpace colorSpace)
{
    if (srcBuffer == nullptr || destBuffer == nullptr || imageSize.width < NUM_0 || imageSize.height < NUM_0) {
        return false;
    }
    destBufferSize = static_cast<size_t>(imageSize.width * imageSize.height + (imageSize.width + NUM_1) / NUM_2 *
        (imageSize.height + NUM_1) / NUM_2 * NUM_2);
    (*destBuffer) = new(std::nothrow) uint8_t[destBufferSize]();
    if (*destBuffer == nullptr) {
        IMAGE_LOGD("apply space for dest buffer failed!");
        return false;
    }

    for (int h = NUM_0; h < imageSize.height; ++h) {
        for (int w = NUM_0; w < imageSize.width; ++w) {
            int widthSrcW = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width : (imageSize.width + NUM_1);
            int widthSrcH = (imageSize.width % NUM_2 == NUM_0) ? imageSize.height : (imageSize.height + NUM_1);
            int widthEvenDest = (imageSize.width % NUM_2 == NUM_0) ? imageSize.width :
                ((imageSize.width + NUM_1) / NUM_2 * NUM_2);

            const int srcIndexY = h * imageSize.width + w;
            const int srcIndexV = imageSize.height * imageSize.width + (h / NUM_2) * widthSrcW / NUM_2 + (w / NUM_2);
            const int srcIndexU = srcIndexV + widthSrcW / NUM_2 * widthSrcH / NUM_2;

            const int destIndexY = h * imageSize.width + w;
            const int destIndexUV = imageSize.width * imageSize.height + (h / NUM_2) * widthEvenDest +
                (w / NUM_2) * NUM_2;

            (*destBuffer)[destIndexY] = srcBuffer[srcIndexY];
            (*destBuffer)[destIndexUV] = srcBuffer[srcIndexU];
            (*destBuffer)[destIndexUV + NUM_1] = srcBuffer[srcIndexV];
        }
    }
    return true;
}
} // namespace Media
} // namespace OHOS