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

#include <gtest/gtest.h>
#include "pixelmap2.h"
#include "pixelmap2_impl.h"
#include "common_utils.h"
// #include "image_pixel_map_napi_kits.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Media {
class PixelMapNdk2Test : public testing::Test {
public:
    PixelMapNdk2Test() {}
    ~PixelMapNdk2Test() {}
};

/**
 * @tc.name: OH_Pixelmap2_CreateInitializationOptions
 * @tc.desc: OH_Pixelmap2_CreateInitializationOptions
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_CreateInitializationOptions, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_CreateInitializationOptions start";
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    Image_ErrorCode res = OH_Pixelmap2_CreateInitializationOptions(&ops);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_CreateInitializationOptions end";
}

/**
 * @tc.name: OH_Pixelmap2_InitializationOptionsSetGetWidth
 * @tc.desc: OH_Pixelmap2_InitializationOptionsSetGetWidth
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_InitializationOptionsSetGetWidth, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationOptionsGetWidth start";
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    OH_Pixelmap2_CreateInitializationOptions(&ops);
    uint32_t width = 0;
    OH_Pixelmap2_InitializationOptionsSetWidth(ops, 1);
    OH_Pixelmap2_InitializationOptionsGetWidth(ops, &width);
    ASSERT_EQ(width, 1);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationOptionsGetWidth end";
}

/**
 * @tc.name: OH_Pixelmap2_InitializationOptionsSetGetHeight
 * @tc.desc: OH_Pixelmap2_InitializationOptionsSetGetHeight
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_InitializationOptionsSetGetHeight, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationOptionsSetGetHeight start";
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    OH_Pixelmap2_CreateInitializationOptions(&ops);
    uint32_t height = 0;
    OH_Pixelmap2_InitializationOptionsSetHeight(ops, 1);
    OH_Pixelmap2_InitializationOptionsGetHeight(ops, &height);
    ASSERT_EQ(height, 1);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationOptionsSetGetHeight end";
}

/**
 * @tc.name: OH_Pixelmap2_InitializationOptionsSetGetPixelFormat
 * @tc.desc: OH_Pixelmap2_InitializationOptionsSetGetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_InitializationOptionsSetGetPixelFormat, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationSetOptionsGetPixelFormat start";
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    OH_Pixelmap2_CreateInitializationOptions(&ops);
    int32_t pixelFormat = 0;
    OH_Pixelmap2_InitializationOptionsSetPixelFormat(ops, 1);
    OH_Pixelmap2_InitializationOptionsGetPixelFormat(ops, &pixelFormat);
    ASSERT_EQ(pixelFormat, 1);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationSetOptionsGetPixelFormat end";
}

HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_InitializationOptionsSetGetAlphaType, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationOptionsSetGetAlphaType start";
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    OH_Pixelmap2_CreateInitializationOptions(&ops);
    int32_t alphaType = 0;
    OH_Pixelmap2_InitializationOptionsSetAlphaType(ops, 1);
    OH_Pixelmap2_InitializationOptionsGetAlphaType(ops, &alphaType);
    ASSERT_EQ(alphaType, 1);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_InitializationOptionsSetGetAlphaType end";
}

/**
 * @tc.name: OH_Pixelmap2_ReleaseInitializationOptions
 * @tc.desc: OH_Pixelmap2_ReleaseInitializationOptions
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ReleaseInitializationOptions, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ReleaseInitializationOptions start";
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_ReleaseInitializationOptions(ops);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ReleaseInitializationOptions end";
}

/**
 * @tc.name: OH_Pixelmap2_CreateImageInfo
 * @tc.desc: OH_Pixelmap2_CreateImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_CreateImageInfo, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_CreateImageInfo start";
    OH_Pixelmap_ImageInfo *ImageInfo = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_CreateImageInfo(&ImageInfo);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_CreateImageInfo end";
}

/**
 * @tc.name: OH_Pixelmap2_ImageInfoGetWidth
 * @tc.desc: OH_Pixelmap2_ImageInfoGetWidth
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ImageInfoGetWidth, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetWidth start";
    OH_Pixelmap_ImageInfo *ImageInfo = nullptr;
    uint32_t width = 0;
    Image_ErrorCode ret = OH_Pixelmap2_ImageInfoGetWidth(ImageInfo, &width);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetWidth end";
}

/**
 * @tc.name: OH_Pixelmap2_ImageInfoGetHeight
 * @tc.desc: OH_Pixelmap2_ImageInfoGetHeight
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ImageInfoGetHeight, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetHeight start";
    OH_Pixelmap_ImageInfo *ImageInfo = nullptr;
    uint32_t height = 0;
    Image_ErrorCode ret = OH_Pixelmap2_ImageInfoGetHeight(ImageInfo, &height);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetHeight end";
}

/**
 * @tc.name: OH_Pixelmap2_ImageInfoGetPixelRowStride
 * @tc.desc: OH_Pixelmap2_ImageInfoGetPixelRowStride
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ImageInfoGetPixelRowStride, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetPixelRowStride start";
    OH_Pixelmap_ImageInfo *ImageInfo = nullptr;
    uint32_t rowSize = 0;
    Image_ErrorCode ret = OH_Pixelmap2_ImageInfoGetPixelRowStride(ImageInfo, &rowSize);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetPixelRowStride end";
}

/**
 * @tc.name: OH_Pixelmap2_ImageInfoGetPixelFormat
 * @tc.desc: OH_Pixelmap2_ImageInfoGetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ImageInfoGetPixelFormat, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetPixelFormat start";
    OH_Pixelmap_ImageInfo *ImageInfo = nullptr;
    int32_t pixelFormat = 0;
    Image_ErrorCode ret = OH_Pixelmap2_ImageInfoGetPixelFormat(ImageInfo, &pixelFormat);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetPixelFormat end";
}

/**
 * @tc.name: OH_Pixelmap2_ImageInfoGetAlphaType
 * @tc.desc: OH_Pixelmap2_ImageInfoGetAlphaType
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ImageInfoGetAlphaType, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetAlphaType start";
    OH_Pixelmap_ImageInfo *ImageInfo = nullptr;
    int32_t density = 0;
    Image_ErrorCode ret = OH_Pixelmap2_ImageInfoGetAlphaType(ImageInfo, &density);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ImageInfoGetAlphaType end";
}

/**
 * @tc.name: OH_Pixelmap2_ReleaseImageInfo
 * @tc.desc: OH_Pixelmap2_ReleaseImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ReleaseImageInfo, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ReleaseImageInfo start";
    OH_Pixelmap_ImageInfo *ImageInfo = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_ReleaseImageInfo(ImageInfo);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ReleaseImageInfo end";
}

/**
 * @tc.name: OH_Pixelmap2_CreatePixelMap
 * @tc.desc: OH_Pixelmap2_CreatePixelMap
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_CreatePixelMap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_CreatePixelMap start";
    uint8_t *colors = nullptr;
    size_t colorLength = 0;
    OH_Pixelmap_InitializationOptions *opts = nullptr;
    OH_Pixelmap *pixelMap = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_CreatePixelMap(colors, colorLength, opts, &pixelMap);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_CreatePixelMap end";
}

/**
 * @tc.name: OH_Pixelmap2_ReadPixels
 * @tc.desc: OH_Pixelmap2_ReadPixels
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_ReadPixels, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ReadPixels start";
    OH_Pixelmap *pixelMap = nullptr;
    uint8_t *buffer = nullptr;
    size_t *bufferSize = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_ReadPixels(pixelMap, buffer, bufferSize);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_ReadPixels end";
}

/**
 * @tc.name: OH_Pixelmap2_WritePixels
 * @tc.desc: OH_Pixelmap2_WritePixels
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_WritePixels, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_WritePixels start";
    OH_Pixelmap *pixelMap = nullptr;
    uint8_t *source = nullptr;
    size_t *bufferSize = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_WritePixels(pixelMap, source, bufferSize);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_WritePixels end";
}

/**
 * @tc.name: OH_Pixelmap2_GetImageInfo
 * @tc.desc: OH_Pixelmap2_GetImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_GetImageInfo, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_GetImageInfo start";
    OH_Pixelmap *pixelMap = nullptr;
    OH_Pixelmap_ImageInfo *imageInfo = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_GetImageInfo(pixelMap, imageInfo);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_GetImageInfo end";
}

/**
 * @tc.name: OH_Pixelmap2_Opacity
 * @tc.desc: OH_Pixelmap2_Opacity
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_Opacity, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Opacity start";
    OH_Pixelmap *pixelMap = nullptr;
    float rate = 0;
    Image_ErrorCode ret = OH_Pixelmap2_Opacity(pixelMap, rate);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Opacity end";
}

/**
 * @tc.name: OH_Pixelmap2_Scale
 * @tc.desc: OH_Pixelmap2_Scale
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_Scale, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Scale start";
    OH_Pixelmap *pixelMap = nullptr;
    float x = 0;
    float y = 0;
    Image_ErrorCode ret = OH_Pixelmap2_Scale(pixelMap, x, y);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Scale end";
}

/**
 * @tc.name: OH_Pixelmap2_Translate
 * @tc.desc: OH_Pixelmap2_Translate
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_Translate, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Translate start";
    OH_Pixelmap *pixelMap = nullptr;
    float x = 0;
    float y = 0;
    Image_ErrorCode ret = OH_Pixelmap2_Translate(pixelMap, x, y);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Translate end";
}

/**
 * @tc.name: OH_Pixelmap2_Rotate
 * @tc.desc: OH_Pixelmap2_Rotate
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_Rotate, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Rotate start";
    OH_Pixelmap *pixelMap = nullptr;
    float angle = 0;
    Image_ErrorCode ret = OH_Pixelmap2_Rotate(pixelMap, angle);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Rotate end";
}

/**
 * @tc.name: OH_Pixelmap2_Flip
 * @tc.desc: OH_Pixelmap2_Flip
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_Flip, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Flip start";
    OH_Pixelmap *pixelMap = nullptr;
    bool horizontal = 0;
    bool vertical = 0;
    Image_ErrorCode ret = OH_Pixelmap2_Flip(pixelMap, horizontal, vertical);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Flip end";
}

/**
 * @tc.name: OH_Pixelmap2_Crop
 * @tc.desc: OH_Pixelmap2_Crop
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_Crop, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Crop start";
    OH_Pixelmap *pixelMap = nullptr;
    Image_Region *region = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_Crop(pixelMap, region);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Crop end";
}

/**
 * @tc.name: OH_Pixelmap2_Release
 * @tc.desc: OH_Pixelmap2_Release
 * @tc.type: FUNC
 */
HWTEST_F(PixelMapNdk2Test, OH_Pixelmap2_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Release start";
    OH_Pixelmap *pixelMap = nullptr;
    Image_ErrorCode ret = OH_Pixelmap2_Release(pixelMap);
    ASSERT_EQ(ret , IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "PixelMapNdk2Test: OH_Pixelmap2_Release end";
}

}
}
