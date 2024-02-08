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
    //constexpr uint8_t NUM_5 = -5;
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
 * @tc.name:ConvertImageFormat_Test_001
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_001 start";
    ImageFormatConvert converter;
    const std::shared_ptr<PixelMap> srcPiexlMap = nullptr;
    bool result = converter.SetSourcePixelMap(srcPiexlMap);
    EXPECT_EQ(result, false);
    std::unique_ptr<PixelMap> destPixelMap;
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_001 end";
}

/**
 * @tc.name:ConvertImageFormat_Test_002
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_002 start";
    ImageFormatConvert converter;
    bool result = converter.SetDestinationFormat(PixelFormat::UNKNOWN);
    EXPECT_EQ(result, false);
    std::unique_ptr<PixelMap> destPixelMap;
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_002 end";
}

/**
 * @tc.name:ConvertImageFormat_Test_003
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_003 start";
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    std::unique_ptr<PixelMap> destPixelMap;
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
    EXPECT_NE(converter.ConvertImageFormat(destPixelMap), IMAGE_RESULT_FORMAT_CONVERT_FAILED);
    delete[] destBuffer;
    delete[] srcBuffer;
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_003 end";
}

/**
 * @tc.name:ConvertImageFormat_Test_004
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_004 start";
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
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_004 end";
}

/**
 * @tc.name:ConvertImageFormat_Test_005
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_005 start";
    ImageFormatConvert converter;
    std::unique_ptr<PixelMap> destPixelMap;
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    const uint8_t *srcBuffer = new uint8_t[NUM_8];
    const size_t srcBufferSize = NUM_8;
    PixelFormat srcFormat = PixelFormat::NV12;
    converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    bool result = converter.SetPlInfo(imageSize);
    EXPECT_EQ(result, true);
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_005 end";
}

/**
 * @tc.name:ConvertImageFormat_Test_006
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_006 start";
    ImageFormatConvert converter;
    std::unique_ptr<PixelMap> destPixelMap;
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    const uint8_t *srcBuffer = new uint8_t[NUM_8];
    const size_t srcBufferSize = NUM_8;
    PixelFormat srcFormat = PixelFormat::NV12;
    converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    uint8_buffer_type destBuffer = new uint8_t[NUM_8]();
    size_t destBufferSize = NUM_8;
    Size imageSizes;
    imageSizes.width = NUM_2;
    imageSizes.height = NUM_0;
    bool result = converter.SetAddr(destBuffer, destBufferSize);
    EXPECT_EQ(result, true);
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_006 end";
}

/**
 * @tc.name:ConvertImageFormat_Test_007
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_007 start";
    ImageFormatConvert converter;
    std::unique_ptr<PixelMap> destPixelMap;
    PixelFormat destFormat_ = PixelFormat::NV21;
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    const uint8_t *srcBuffer = new uint8_t[NUM_8];
    const size_t srcBufferSize = NUM_8;
    PixelFormat srcFormat = PixelFormat::NV12;
    converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    bool result = converter.CreateSource(destFormat_, imageSize);
    EXPECT_EQ(result, true);
    EXPECT_EQ(converter.ConvertImageFormat(destPixelMap), ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_007 end";
}

/**
 * @tc.name:ConvertImageFormat_Test_008
 * @tc.desc: ImageFormatConvertTest Test
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_008 start";
    ImageFormatConvert converter;
    std::unique_ptr<PixelMap> destPixelMap;
    Size imageSize;
    imageSize.width = NUM_2;
    imageSize.height = NUM_2;
    const uint8_t *srcBuffer = new uint8_t[NUM_8];
    const size_t srcBufferSize = NUM_8;
    PixelFormat srcFormat = PixelFormat::NV12;
    bool result = converter.SetSourceBuffer(srcBuffer, srcBufferSize, imageSize, srcFormat);
    EXPECT_EQ(result, false);
    EXPECT_NE(converter.ConvertImageFormat(destPixelMap), ERR_MEDIA_FORMAT_UNSUPPORT);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_008 end";
}

}   //namespace 
}
