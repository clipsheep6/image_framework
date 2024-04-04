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

#include <gtest/gtest.h>
#include "common_utils.h"
#include "image.h"
#include "image/image_receiver.h"
#include "image_kits.h"
#include "receiver/include/image_receiver.h"

using namespace testing::ext;
namespace OHOS {
namespace Media {

class Image2Test : public testing::Test {
public:
    Image2Test() {}
    ~Image2Test() {}
};

/**
 * @tc.name: OH_Image2_ReleaseTest
 * @tc.desc: OH_Image2_Release
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_ReleaseTest start";
    OH_Image* pImg = nullptr;
    Image_ErrorCode nRst = OH_Image2_Release(pImg);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_ReleaseTest end";
}

/**
 * @tc.name: OH_Image2_GetByteBufferTest
 * @tc.desc: OH_Image2_GetByteBuffer
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_GetByteBufferTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetByteBufferTest start";
    OH_Image* pImg = nullptr;
    OH_NativeBuffer* pBuffer = nullptr;
    Image_ErrorCode nRst = OH_Image2_GetByteBuffer(pImg, 0, &pBuffer);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetByteBufferTest end";
}

/**
 * @tc.name: OH_Image2_GetBufferSizeTest
 * @tc.desc: OH_Image2_GetBufferSize
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_GetBufferSizeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetBufferSizeTest start";
    OH_Image* pImg = nullptr;
    size_t nSize = 0;
    Image_ErrorCode nRst = OH_Image2_GetBufferSize(pImg, 0, &nSize);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetBufferSizeTest end";
}

/**
 * @tc.name: OH_Image2_GetComponentTypesTest
 * @tc.desc: OH_Image2_GetComponentTypes
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_GetComponentTypesTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetComponentTypesTest start";
    OH_Image* pImg = nullptr;
    uint32_t* pTypes = nullptr;
    size_t nTypeSize = 0;
    Image_ErrorCode nRst = OH_Image2_GetComponentTypes(pImg, &pTypes, &nTypeSize);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetComponentTypesTest end";
}

/**
 * @tc.name: OH_Image2_GetRowStrideTest
 * @tc.desc: OH_Image2_GetRowStride
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_GetRowStrideTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetRowStrideTest start";
    OH_Image* pImg = nullptr;
    int32_t nRowStride = 0;
    Image_ErrorCode nRst = OH_Image2_GetRowStride(pImg, 0, &nRowStride);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetRowStrideTest end";
}

/**
 * @tc.name: OH_Image2_GetPixelStrideTest
 * @tc.desc: OH_Image2_GetPixelStride
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_GetPixelStrideTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetPixelStrideTest start";
    OH_Image* pImg = nullptr;
    int32_t nPixelStride = 0;
    Image_ErrorCode nRst = OH_Image2_GetPixelStride(pImg, 0, &nPixelStride);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetPixelStrideTest end";
}

/**
 * @tc.name: OH_Image2_GetImageSizeTest
 * @tc.desc: OH_Image2_GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_GetImageSizeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetImageSizeTest start";
    OH_Image* pImg = nullptr;
    Image_Size szImg = { 0 };
    Image_ErrorCode nRst = OH_Image2_GetImageSize(pImg, &szImg);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetImageSizeTest end";
}

/**
 * @tc.name: OH_Image2_GetFormatTest
 * @tc.desc: OH_Image2_GetFormat
 * @tc.type: FUNC
 */
HWTEST_F(Image2Test, OH_Image2_GetFormatTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetFormatTest start";
    OH_Image* pImg = nullptr;
    uint32_t nFormat = 0;
    Image_ErrorCode nRst = OH_Image2_GetFormat(pImg, &nFormat);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "Image2Test: OH_Image2_GetFormatTest end";
}
} // namespace Media
} // namespace OHOS
