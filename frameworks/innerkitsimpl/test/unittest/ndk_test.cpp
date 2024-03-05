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
#include "image_pixel_map_mdk.h"
#include "image_pixel_map_napi.h"
#include "common_utils.h"
#include "image_pixel_map_napi_kits.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Media {
class NdkTest : public testing::Test {
public:
    NdkTest() {}
    ~NdkTest() {}
};

/**
 * @tc.name: NdkTest001
 * @tc.desc: OH_PixelMap_GetBytesNumberPerRow
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest001 start";
    const NativePixelMap *p = nullptr;
    int* num = nullptr;
    int32_t res = OH_PixelMap_GetBytesNumberPerRow(p, num);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest001 end";
}

/**
 * @tc.name: NdkTest002
 * @tc.desc: OH_PixelMap_GetIsEditable
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest002 start";
    const NativePixelMap *p = nullptr;
    int* num = nullptr;
    int32_t res = OH_PixelMap_GetIsEditable(p, num);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest002 end";
}

/**
 * @tc.name: NdkTest003
 * @tc.desc: OH_PixelMap_IsSupportAlpha
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest003 start";
    const NativePixelMap *p = nullptr;
    int* num = nullptr;
    int32_t res = OH_PixelMap_IsSupportAlpha(p, num);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest003 end";
}

/**
 * @tc.name: NdkTest004
 * @tc.desc: OH_PixelMap_SetAlphaAble
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest004 start";
    const NativePixelMap *p = nullptr;
    int num = 0;
    int32_t res = OH_PixelMap_SetAlphaAble(p, num);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest004 end";
}

/**
 * @tc.name: NdkTest005
 * @tc.desc: GetFilterRowType
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest005 start";
    const NativePixelMap *p = nullptr;
    int* num = nullptr;
    int32_t res = OH_PixelMap_GetDensity(p, num);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest005 end";
}

/**
 * @tc.name: NdkTest006
 * @tc.desc: OH_PixelMap_SetDensity
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest006 start";
    const NativePixelMap *p = nullptr;
    int num = 0;
    int32_t res = OH_PixelMap_SetDensity(p, num);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest006 end";
}

/**
 * @tc.name: NdkTest007
 * @tc.desc: OH_PixelMap_SetOpacity
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest007 start";
    const NativePixelMap *p = nullptr;
    float num = 0.5;
    int32_t res = OH_PixelMap_SetOpacity(p, num);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest007 end";
}

/**
 * @tc.name: NdkTest008
 * @tc.desc: OH_PixelMap_Scale
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest008 start";
    const NativePixelMap *p = nullptr;
    float x = 0.5;
    float y = 0.5;
    int32_t res = OH_PixelMap_Scale(p, x, y);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest008 end";
}

/**
 * @tc.name: NdkTest009
 * @tc.desc: OH_PixelMap_Translate
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest009, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest009 start";
    const NativePixelMap *p = nullptr;
    float x = 0.5;
    float y = 0.5;
    int32_t res = OH_PixelMap_Translate(p, x, y);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest009 end";
}

/**
 * @tc.name: NdkTest0010
 * @tc.desc: OH_PixelMap_Rotate
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest0010, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest0010 start";
    const NativePixelMap *p = nullptr;
    float x = 0.5;
    int32_t res = OH_PixelMap_Rotate(p, x);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest0010 end";
}

/**
 * @tc.name: NdkTest0011
 * @tc.desc: OH_PixelMap_Flip
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest0011, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest0011 start";
    const NativePixelMap *p = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t res = OH_PixelMap_Flip(p, x, y);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest0011 end";
}

/**
 * @tc.name: NdkTest0012
 * @tc.desc: OH_PixelMap_Crop
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest0012, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest0012 start";
    const NativePixelMap *p = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t res = OH_PixelMap_Crop(p, x, y, width, height);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest0012 end";
}

/**
 * @tc.name: NdkTest0013
 * @tc.desc: OH_PixelMap_Crop
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest0013, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest0013 start";
    const NativePixelMap *p = nullptr;
    OhosPixelMapInfos *info = nullptr;
    int32_t res = OH_PixelMap_GetImageInfo(p, info);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest0013 end";
}

/**
 * @tc.name: NdkTest0014
 * @tc.desc: OH_PixelMap_AccessPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest0014, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest0014 start";
    const NativePixelMap *p = nullptr;
    void **info = nullptr;
    int32_t res = OH_PixelMap_AccessPixels(p, info);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest0014 end";
}

/**
 * @tc.name: NdkTest0015
 * @tc.desc: OH_PixelMap_UnAccessPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, NdkTest0015, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: NdkTest0015 start";
    const NativePixelMap *p = nullptr;
    int32_t res = OH_PixelMap_UnAccessPixels(p);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: NdkTest0015 end";
}

/**
 * @tc.name: OH_PixelMap_InitNativePixelMapTest
 * @tc.desc: OH_PixelMap_InitNativePixelMap
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMap_InitNativePixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMap_InitNativePixelMapTest start";
    napi_env env = nullptr;
    napi_value source = nullptr;
    NativePixelMap* res = OH_PixelMap_InitNativePixelMap(env, source);
    ASSERT_EQ(res, nullptr);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMap_InitNativePixelMapTest end";
}

/**
 * @tc.name: OH_PixelMap_CreatePixelMapTest
 * @tc.desc: OH_PixelMap_CreatePixelMap
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMap_CreatePixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMap_CreatePixelMapTest start";
    napi_env env = nullptr;
    OhosPixelMapCreateOps info;
    void* buf = nullptr;
    size_t len = 0;
    napi_value* res = nullptr;
    int32_t ret = OH_PixelMap_CreatePixelMap(env, info, buf, len, res);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMap_CreatePixelMapTest end";
}

/**
 * @tc.name: OH_PixelMap_CreateAlphaPixelMapTest
 * @tc.desc: OH_PixelMap_CreateAlphaPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMap_CreateAlphaPixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMap_CreateAlphaPixelMapTest start";
    napi_env env = nullptr;
    napi_value source = nullptr;
    napi_value* alpha = nullptr;
    int32_t res = OH_PixelMap_CreateAlphaPixelMap(env, source, alpha);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMap_CreateAlphaPixelMapTest end";
}

/**
 * @tc.name: OH_AccessPixelsTest
 * @tc.desc: OH_AccessPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_AccessPixelsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_AccessPixelsTest start";
    napi_env env = nullptr;
    napi_value value = nullptr;
    void** addr = nullptr;
    int32_t res = OH_AccessPixels(env, value, addr);
    ASSERT_EQ(res, OHOS_IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_AccessPixelsTest end";
}

/**
 * @tc.name: OH_UnAccessPixelsTest
 * @tc.desc: OH_UnAccessPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_UnAccessPixelsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_UnAccessPixelsTest start";
    napi_env env = nullptr;
    napi_value value = nullptr;
    int32_t res = OH_UnAccessPixels(env, value);
    ASSERT_EQ(res, OHOS_IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_UnAccessPixelsTest end";
}

/**
 * @tc.name: OH_GetImageInfoTest
 * @tc.desc: OH_GetImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_GetImageInfoTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_GetImageInfoTest start";
    napi_env env = nullptr;
    napi_value value = nullptr;;
    OhosPixelMapInfo *info = nullptr;
    int32_t res = OH_GetImageInfo(env, value, info);
    ASSERT_EQ(res, OHOS_IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_GetImageInfoTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_CreatePixelMapTest
 * @tc.desc: OH_PixelMapCapi_CreatePixelMap
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_CreatePixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CreatePixelMapTest start";
    uint32_t *colors = nullptr;
    uint32_t colorLength = 0;
    OhosPixelMapCreateOps *opts = nullptr;
    PixelMapCapi *pixelmap = nullptr;
    int32_t res = OH_PixelMapCapi_CreatePixelMap(colors, colorLength, opts, &pixelmap);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CreatePixelMapTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_CreatePixelMapFromSurfaceTest
 * @tc.desc: OH_PixelMapCapi_CreatePixelMapFromSurface
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_CreatePixelMapFromSurfaceTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CreatePixelMapFromSurfaceTest start";
    uint64_t surfaceId = 0;
    OhosImageRect rect;
    int32_t value = 0;
    PixelMapCapi *pixelmap = nullptr;
    int32_t res = OH_PixelMapCapi_CreatePixelMapFromSurface(surfaceId, rect, &pixelmap);
    ASSERT_EQ(res, IMAGE_RESULT_ADD_PIXEL_MAP_FAILED);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CreatePixelMapFromSurfaceTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_ReadPixelsToBufferTest
 * @tc.desc: OH_PixelMapCapi_ReadPixelsToBuffer
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_ReadPixelsToBufferTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ReadPixelsToBufferTest start";
    PixelMapCapi *pixelmap = nullptr;
    uint64_t *bufferSize = nullptr;
    uint8_t *dst = nullptr;
    int32_t res = OH_PixelMapCapi_ReadPixelsToBuffer(pixelMap, bufferSize, dst);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ReadPixelsToBufferTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_ReadPixelsTest
 * @tc.desc: OH_PixelMapCapi_ReadPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_ReadPixelsToBufferTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ReadPixelsTest start";
    PixelMapCapi *pixelmap = nullptr;
    OhosPositionArea positionArea
    uint8_t *dst = nullptr;
    int32_t res = OH_PixelMapCapi_ReadPixels(pixelMap, positionArea);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ReadPixelsTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_WritePixelsTest
 * @tc.desc: OH_PixelMapCapi_WritePixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_WritePixelsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_WritePixelsTest start";
    PixelMapCapi *pixelmap = nullptr;
    OhosPositionArea positionArea
    uint8_t *dst = nullptr;
    int32_t res = OH_PixelMapCapi_WritePixels(pixelMap, positionArea);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_WritePixelsTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_WriteBufferToPixelsTest
 * @tc.desc: OH_PixelMapCapi_WriteBufferToPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_WriteBufferToPixelsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_WriteBufferToPixelsTest start";
    PixelMapCapi *pixelmap = nullptr;
    uint8_t *source = nullptr;
    uint64_t *bufferSize = nullptr;
    int32_t res = OH_PixelMapCapi_WriteBufferToPixels(pixelMap, source, bufferSize);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_WriteBufferToPixelsTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_GetImageInfoTest
 * @tc.desc: OH_PixelMapCapi_GetImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_GetImageInfoTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetImageInfoTest start";
    PixelMapCapi *pixelmap = nullptr;
    OhosPixelMapInfos *imageInfo = nullptr;
    int32_t res = OH_PixelMapCapi_GetImageInfo(pixelMap, imageInfo);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetImageInfoTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_GetBytesNumberPerRowTest
 * @tc.desc: OH_PixelMapCapi_GetBytesNumberPerRow
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_GetBytesNumberPerRowTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetBytesNumberPerRowTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t *bytesNumberPerRow = nullptr;
    int32_t res = OH_PixelMapCapi_GetBytesNumberPerRow(pixelMap, bytesNumberPerRow);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetBytesNumberPerRowTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_GetPixelBytesNumberTest
 * @tc.desc: OH_PixelMapCapi_GetPixelBytesNumber
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_GetPixelBytesNumberTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetPixelBytesNumberTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t *pixelBytesNumber = nullptr;
    int32_t res = OH_PixelMapCapi_GetPixelBytesNumber(pixelMap, pixelBytesNumber);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetPixelBytesNumberTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_GetDensityTest
 * @tc.desc: OH_PixelMapCapi_GetDensity
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_GetDensityTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetDensityTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t *density = nullptr;
    int32_t res = OH_PixelMapCapi_GetDensity(pixelMap, density);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetDensityTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_OpacityTest
 * @tc.desc: OH_PixelMapCapi_Opacity
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_OpacityTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_OpacityTest start";
    PixelMapCapi *pixelmap = nullptr;
    float rate = 0.0f;
    int32_t res = OH_PixelMapCapi_Opacity(pixelMap, rate);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_OpacityTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_CreateAlphaPixelmapTest
 * @tc.desc: OH_PixelMapCapi_CreateAlphaPixelmap
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_CreateAlphaPixelmapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CreateAlphaPixelmapTest start";
    PixelMapCapi *pixelmap = nullptr;
    void *pixelMapAlpha = nullptr;
    int32_t res = OH_PixelMapCapi_CreateAlphaPixelmap(pixelMap, &pixelMapAlpha);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CreateAlphaPixelmapTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_ScaleTest
 * @tc.desc: OH_PixelMapCapi_Scale
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_ScaleTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ScaleTest start";
    PixelMapCapi *pixelmap = nullptr;
    float x = 0.0f;
    float y = 0.0f;
    int32_t res = OH_PixelMapCapi_Scale(pixelMap, x, y);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ScaleTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_TranslateTest
 * @tc.desc: OH_PixelMapCapi_Translate
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_TranslateTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_TranslateTest start";
    PixelMapCapi *pixelmap = nullptr;
    float x = 0.0f;
    float y = 0.0f;
    int32_t res = OH_PixelMapCapi_Translate(pixelMap, x, y);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_TranslateTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_RotateTest
 * @tc.desc: OH_PixelMapCapi_Rotate
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_RotateTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_RotateTest start";
    PixelMapCapi *pixelmap = nullptr;
    float angle = 0.0f;
    int32_t res = OH_PixelMapCapi_Rotate(pixelMap, angle);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_RotateTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_FlipTest
 * @tc.desc: OH_PixelMapCapi_Flip
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_FlipTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_FlipTest start";
    PixelMapCapi *pixelmap = nullptr;
    bool horizontal = true;
    bool vertical = true;
    int32_t res = OH_PixelMapCapi_Flip(pixelMap, horizontal, vertical);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_FlipTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_CropTest
 * @tc.desc: OH_PixelMapCapi_Crop
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_CropTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CropTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t res = OH_PixelMapCapi_Crop(pixelMap, x, y, width, height);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_CropTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_GetColorSpaceTest
 * @tc.desc: OH_PixelMapCapi_GetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_GetColorSpaceTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetColorSpaceTest start";
    PixelMapCapi *pixelmap = nullptr;
    void colorSpace = nullptr;
    int32_t res = OH_PixelMapCapi_GetColorSpace(pixelMap, colorSpace);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetColorSpaceTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_SetColorSpaceTest
 * @tc.desc: OH_PixelMapCapi_SetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_SetColorSpaceTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_SetColorSpaceTest start";
    PixelMapCapi *pixelmap = nullptr;
    void *colorSpace = nullptr;
    int32_t res = OH_PixelMapCapi_SetColorSpace(pixelMap, colorSpace);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_SetColorSpaceTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_ApplyColorSpaceTest
 * @tc.desc: OH_PixelMapCapi_ApplyColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_ApplyColorSpaceTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ApplyColorSpaceTest start";
    PixelMapCapi *pixelmap = nullptr;
    void *targetColorSpace = nullptr;
    int32_t res = OH_PixelMapCapi_ApplyColorSpace(pixelMap, targetColorSpace);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ApplyColorSpaceTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_GetIsEditableTest
 * @tc.desc: OH_PixelMapCapi_GetIsEditable
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_GetIsEditableTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetIsEditableTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t* editable = nullptr;
    int32_t res = OH_PixelMapCapi_GetIsEditable(pixelMap, editable);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_GetIsEditableTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_IsSupportAlphaTest
 * @tc.desc: OH_PixelMapCapi_IsSupportAlpha
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_IsSupportAlphaTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_IsSupportAlphaTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t* alpha = nullptr;
    int32_t res = OH_PixelMapCapi_IsSupportAlpha(pixelMap, alpha);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_IsSupportAlphaTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_SetAlphaAbleTest
 * @tc.desc: OH_PixelMapCapi_SetAlphaAble
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_SetAlphaAbleTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_SetAlphaAbleTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t alpha = 0;
    int32_t res = OH_PixelMapCapi_SetAlphaAble(pixelMap, alpha);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_SetAlphaAbleTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_AccessPixelsTest
 * @tc.desc: OH_PixelMapCapi_AccessPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_AccessPixelsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_AccessPixelsTest start";
    PixelMapCapi *pixelmap = nullptr;
    void* addr = null;
    int32_t res = OH_PixelMapCapi_AccessPixels(pixelMap, &addr);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_AccessPixelsTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_UnAccessPixelsTest
 * @tc.desc: OH_PixelMapCapi_UnAccessPixels
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_UnAccessPixelsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_UnAccessPixelsTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t res = OH_PixelMapCapi_UnAccessPixels(pixelMap);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_UnAccessPixelsTest end";
}

/**
 * @tc.name: OH_PixelMapCapi_ReleaseTest
 * @tc.desc: OH_PixelMapCapi_Release
 * @tc.type: FUNC
 */
HWTEST_F(NdkTest, OH_PixelMapCapi_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ReleaseTest start";
    PixelMapCapi *pixelmap = nullptr;
    int32_t res = OH_PixelMapCapi_Release(pixelMap);
    ASSERT_EQ(res, IMAGE_RESULT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "NdkTest: OH_PixelMapCapi_ReleaseTest end";
}
}
}
