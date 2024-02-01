/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "image_format_convert.h"
#include "image_format_convert_utils.h"
#include "image_type.h"
#include <gtest/gtest.h>
#include "media_errors.h"
#include "image_utils.h"
#include "hilog/log.h"
#include "hilog/log_cpp.h"
#include <gtest/hwext/gtest-multithread.h>
#include <unistd.h>

using namespace testing::mt;
using namespace testing::ext;

namespace {
    constexpr uint8_t NUM_0 = 0;
    constexpr uint8_t NUM_1 = 1;
    constexpr uint8_t NUM_2 = 2;
    constexpr uint8_t NUM_8 = 8;
    constexpr uint8_t NUM_10 = 10;
}

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;

class ImageFormatConvertTest : public testing::Test {
public:
    ImageFormatConvertTest()
    {}
    ~ImageFormatConvertTest()
    {}
	static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ImageFormatConvertTest::SetUpTestCase(void)
{
}

void ImageFormatConvertTest::TearDownTestCase(void)
{
}

void ImageFormatConvertTest::SetUp(void)
{
}

void ImageFormatConvertTest::TearDown(void)
{
}

/**
 * @tc.name:ConvertImageFormatTest_001
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormatTest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_001 start";
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_0;
    uint8_t *destBuffer = new uint8_t[NUM_10];
    size_t destBufferSize = NUM_10;
    ImageFormatConvert converter;
    EXPECT_EQ(converter.ConvertImageFormat(destBuffer, destBufferSize), ERR_IMAGE_INVALID_PARAMETER);
    delete[] destBuffer;
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_001 end";
}

/**
 * @tc.name:ConvertImageFormatTest_002
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormatTest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_002 start";
    Size imageSize;
    imageSize.width = NUM_0;
    imageSize.height = NUM_2;
    uint8_t *destBuffer = new uint8_t[NUM_10];
    size_t destBufferSize = NUM_10;
    ImageFormatConvert converter;
    EXPECT_EQ(converter.ConvertImageFormat(destBuffer, destBufferSize), ERR_IMAGE_INVALID_PARAMETER);
    delete[] destBuffer;
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_002 end";
}

/**
 * @tc.name:ConvertImageFormatTest_003
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormatTest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_003 start";
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    uint8_t *destBuffer = new uint8_t[NUM_10];
    size_t destBufferSize = NUM_10;
    const uint8_t *srcBuffer = nullptr;
    const size_t srcBufferSize = NUM_10;
    PixelFormat srcFormat = PixelFormat::NV12;
    ImageFormatConvert converter;
    bool result = converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    EXPECT_EQ(result, false);
    bool results = converter.SetColorSapace(ColorSpace::UNKNOWN);
    EXPECT_EQ(results, false);
    EXPECT_EQ(converter.ConvertImageFormat(destBuffer, destBufferSize), ERR_IMAGE_INVALID_PARAMETER);
    delete[] destBuffer;
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_003 end";
}

/**
 * @tc.name:ConvertImageFormatTest_004
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormatTest_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_004 start";
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    size_t destBufferSize = NUM_10;
    uint8_t *destBuffer = new uint8_t[destBufferSize];
    const size_t srcBufferSize = NUM_10;
    const uint8_t *srcBuffer = new uint8_t[srcBufferSize];
    PixelFormat srcFormat = PixelFormat::NV12;
    ImageFormatConvert converter;
    bool result = converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    EXPECT_EQ(result, false);
    bool results = converter.SetColorSapace(ColorSpace::DCI_P3);
    EXPECT_EQ(results, true);
    EXPECT_EQ(converter.ConvertImageFormat(destBuffer, destBufferSize), ERR_IMAGE_INVALID_PARAMETER);
    delete[] destBuffer;
    delete[] srcBuffer;
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_004 end";
}

/**
 * @tc.name:ConvertImageFormatTest_005
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormatTest_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_005 start";
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    size_t destBufferSize = NUM_10;
    uint8_t *destBuffer = new uint8_t[destBufferSize];
    const uint8_t *srcBuffer = new uint8_t[NUM_8];
    const size_t srcBufferSize = NUM_8;
    PixelFormat srcFormat = PixelFormat::UNKNOWN;
    ImageFormatConvert converter;
    bool result = converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    EXPECT_EQ(result, false);
    bool results = converter.SetColorSapace(ColorSpace::DCI_P3);
    EXPECT_EQ(results, true);
    EXPECT_NE(converter.ConvertImageFormat(destBuffer, destBufferSize), IMAGE_RESULT_FORMAT_CONVERT_FAILED);
    delete[] destBuffer;
    delete[] srcBuffer;
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_005 end";
}

/**
 * @tc.name:ConvertImageFormatTest_006
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormatTest_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_006 start";
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    size_t destBufferSize = NUM_10;
    uint8_t *destBuffer = new uint8_t[destBufferSize];
    const size_t srcBufferSize = imageSize.width * imageSize.height + ((imageSize.width + NUM_1) / NUM_2) *
                                ((imageSize.height + NUM_1) / NUM_2) * NUM_2;
    const uint8_t *srcBuffer = new uint8_t[srcBufferSize];
    PixelFormat srcFormat = PixelFormat::NV12;
    ImageFormatConvert converter;
    bool result = converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    EXPECT_EQ(result, true);
    bool results = converter.SetColorSapace(ColorSpace::DCI_P3);
    EXPECT_EQ(results, true);
    bool resultss = converter.SetDestinationFormat(PixelFormat::UNKNOWN);
    EXPECT_EQ(resultss, false);
    EXPECT_NE(converter.ConvertImageFormat(destBuffer, destBufferSize), IMAGE_RESULT_FORMAT_CONVERT_FAILED);
    delete[] destBuffer;
    delete[] srcBuffer;
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormatTest_006 end";
}


/**
 * @tc.name:ConvertImageFormat_2_Test_001
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_2_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_2_Test_001 start";
    ImageFormatConvert converter;
    bool result = converter.SetDestinationFormat(PixelFormat::UNKNOWN);
    EXPECT_EQ(result, false);
    std::unique_ptr<PixelMap> destPixelMap;
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_MEDIA_FORMAT_UNSUPPORT);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_2_Test_001 end";
}

/**
 * @tc.name:ConvertImageFormat_2_Test_002
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_2_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_2_Test_001 start";
    ImageFormatConvert converter;
    std::unique_ptr<PixelMap> destPixelMap;
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    const uint8_t *srcBuffer = new uint8_t[NUM_8];
    const size_t srcBufferSize = NUM_8;
    PixelFormat srcFormat = PixelFormat::UNKNOWN;
    bool result = converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    EXPECT_EQ(result, false);
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_MEDIA_FORMAT_UNSUPPORT);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_2_Test_001 end";
}

}   //namespace 
}


