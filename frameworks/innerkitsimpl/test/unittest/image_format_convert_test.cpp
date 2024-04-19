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
#include "buffer_packer_stream.h"
#include "image_source.h"
#include "image_type.h"
#include <gtest/gtest.h>
#include "media_errors.h"
#include "image_utils.h"
#include "hilog/log.h"
#include "hilog/log_cpp.h"
#include "image_log.h"
// #include "memory_manager.h"
// #include "pixel_yuv.h"
#include <gtest/hwext/gtest-multithread.h>
#include <unistd.h>
#include <fstream>
#include <filesystem>
#include <iostream>

using namespace testing::mt;
using namespace testing::ext;

namespace {
    //constexpr uint8_t NUM_0 = 0;
    constexpr uint8_t NUM_1 = 1;
    constexpr uint8_t NUM_2 = 2;
    constexpr uint8_t NUM_3 = 3;
    constexpr uint8_t NUM_8 = 8;
    //constexpr uint8_t NUM_10 = 10;
    //constexpr uint8_t NUM_16 = 16;
    constexpr uint8_t NUM_5 = 5;
}


namespace OHOS {
namespace Media {

using namespace OHOS::HiviewDFX;
static const std::string IMAGE_INPUT_JPG_PATH = "/data/local/tmp/jpeg_include_icc_profile.jpg";
// static const std::string IMAGE_OUTPUT_JPG_PATH = "/data/local/tmp/";

// static constexpr uint32_t MAXSIZE = 10000;

class ImageFormatConvertTest : public testing::Test {
public:
    static bool TestIsSupport(PixelFormat format);
    static ConvertFunction TestGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
    static bool TestMakeDestPixelMap(std::shared_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
                                 const size_t destBufferSize, ImageInfo &info, AllocatorType allcatorType);
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

bool ImageFormatConvertTest::TestIsSupport(PixelFormat format)
{
    return ImageFormatConvert::IsSupport(format);
}

ConvertFunction ImageFormatConvertTest::TestGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat)
{
    return ImageFormatConvert::GetConvertFuncByFormat(srcFormat, destFormat);
}

bool ImageFormatConvertTest::TestMakeDestPixelMap(std::shared_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
                                const size_t destBufferSize, ImageInfo &info, AllocatorType allcatorType)
{
    return ImageFormatConvert::MakeDestPixelMap(destPixelMap, destBuffer, destBufferSize, info, allcatorType);
}

HWTEST_F(ImageFormatConvertTest, IsSupport_Test_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: IsSupport_Test_001 start";
    PixelFormat destFormat = PixelFormat::RGB_888;
    bool ret = ImageFormatConvertTest::TestIsSupport(destFormat);
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: IsSupport_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, IsSupport_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: IsSupport_Test_002 start";
    PixelFormat destFormat = PixelFormat::UNKNOWN;
    bool ret = ImageFormatConvertTest::TestIsSupport(destFormat);
    EXPECT_EQ(ret, false);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: IsSupport_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, GetConvertFuncByFormat_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_001 start";
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::NV12;
    ConvertFunction cvtFunc = ImageFormatConvertTest::TestGetConvertFuncByFormat(srcFormat, destFormat);

    const_uint8_buffer_type srcBuffer = nullptr;
    Size size = {800, 500};
    uint8_buffer_type destBuffer = nullptr;
    size_t destBufferSize = 0;
    ColorSpace colorspace = ColorSpace::UNKNOWN;

    EXPECT_EQ(cvtFunc(srcBuffer, size, &destBuffer, destBufferSize, colorspace), false);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, GetConvertFuncByFormat_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_002 start";
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::NV12;
    ConvertFunction cvtFunc = ImageFormatConvertTest::TestGetConvertFuncByFormat(srcFormat, destFormat);
    Size size = {NUM_8, NUM_5};
    const uint8_t *srcBuffer = new uint8_t[NUM_8 * NUM_5 * NUM_3]();
    uint8_buffer_type destBuffer = nullptr;
    size_t destBufferSize = 0;
    ColorSpace colorspace = ColorSpace::UNKNOWN;
    EXPECT_EQ(cvtFunc(srcBuffer, size, &destBuffer, destBufferSize, colorspace), true);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, GetConvertFuncByFormat_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_003 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGB_565;
    ConvertFunction cvtFunc = ImageFormatConvertTest::TestGetConvertFuncByFormat(srcFormat, destFormat);
    Size size = {NUM_8, NUM_5};
    const uint8_t *srcBuffer = new uint8_t[size.width * size.height + ((size.width + NUM_1) / NUM_2) *
                ((size.height + NUM_1) / NUM_2) * NUM_2]();
    uint8_buffer_type destBuffer = nullptr;
    size_t destBufferSize = 0;
    ColorSpace colorspace = ColorSpace::UNKNOWN;
    EXPECT_EQ(cvtFunc(srcBuffer, size, &destBuffer, destBufferSize, colorspace), true);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_003 end";
}

HWTEST_F(ImageFormatConvertTest, GetConvertFuncByFormat_Test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_004 start";
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::UNKNOWN;
    ConvertFunction cvtFunc = ImageFormatConvertTest::TestGetConvertFuncByFormat(srcFormat, destFormat);

    EXPECT_EQ(cvtFunc, nullptr);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_004 end";
}

HWTEST_F(ImageFormatConvertTest, MakeDestPixelMap_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_001 start";
    PixelFormat destFormat = PixelFormat::NV12;
    std::shared_ptr<ImageSource> rImageSource;
    std::shared_ptr<PixelMap> srcPiexlMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPiexlMap = rImageSource ->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPiexlMap.get(), nullptr);

    ImageInfo srcInfo;
    srcPiexlMap->GetImageInfo(srcInfo);
    ImageInfo destInfo;
    destInfo.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    destInfo.baseDensity = srcInfo.baseDensity;
    destInfo.colorSpace = srcInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = srcInfo.size;
    destInfo.size.width = 0;
    destInfo.size.height = 0;

    size_t destBufferSize = destInfo.size.width * destInfo.size.height + ((destInfo.size.width + NUM_1) / NUM_2) *
                ((destInfo.size.height + NUM_1) / NUM_2) * NUM_2;
    uint8_buffer_type destBuffer = new uint8_t[destBufferSize]();
     bool ret = ImageFormatConvertTest::TestMakeDestPixelMap(
        srcPiexlMap, destBuffer, destBufferSize, destInfo, srcPiexlMap->GetAllocatorType());

    EXPECT_EQ(ret, false);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, MakeDestPixelMap_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_002 start";
    PixelFormat destFormat = PixelFormat::UNKNOWN;
    size_t destBufferSize = 0;
    uint8_buffer_type destBuffer = new uint8_t[destBufferSize]();
    std::shared_ptr<ImageSource> rImageSource;
    std::shared_ptr<PixelMap> srcPiexlMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPiexlMap = rImageSource ->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPiexlMap.get(), nullptr);

    ImageInfo srcInfo;
    srcPiexlMap->GetImageInfo(srcInfo);
    ImageInfo destInfo;
    destInfo.alphaType = srcInfo.alphaType;
    destInfo.baseDensity = srcInfo.baseDensity;
    destInfo.colorSpace = srcInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = srcInfo.size;
    bool ret = ImageFormatConvertTest::TestMakeDestPixelMap(
        srcPiexlMap, destBuffer, destBufferSize, destInfo, srcPiexlMap->GetAllocatorType());
    EXPECT_EQ(ret, false);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, MakeDestPixelMap_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_003 start";
    PixelFormat destFormat = PixelFormat::NV12;
    std::shared_ptr<ImageSource> rImageSource;
    std::shared_ptr<PixelMap> srcPiexlMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPiexlMap = rImageSource ->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPiexlMap.get(), nullptr);

    ImageInfo srcInfo;
    srcPiexlMap->GetImageInfo(srcInfo);
    ImageInfo destInfo;
    destInfo.alphaType = srcInfo.alphaType;
    destInfo.baseDensity = srcInfo.baseDensity;
    destInfo.colorSpace = srcInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = srcInfo.size;

    size_t destBufferSize = destInfo.size.width * destInfo.size.height + ((destInfo.size.width + NUM_1) / NUM_2) *
                ((destInfo.size.height + NUM_1) / NUM_2) * NUM_2;
    uint8_buffer_type destBuffer = new uint8_t[destBufferSize]();

     bool ret = ImageFormatConvertTest::TestMakeDestPixelMap(
        srcPiexlMap, destBuffer, destBufferSize, destInfo, srcPiexlMap->GetAllocatorType());
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_003 end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_001 start";
    PixelFormat destFormat = PixelFormat::NV12;
    std::shared_ptr<PixelMap> srcPiexlMap = nullptr;

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPiexlMap, destFormat);
    EXPECT_EQ(ret, ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_002 start";
    PixelFormat destFormat = PixelFormat::UNKNOWN;
    std::shared_ptr<PixelMap> srcPiexlMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource =
        ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPiexlMap = rImageSource -> CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPiexlMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPiexlMap, destFormat);
    EXPECT_EQ(ret, ERR_MEDIA_FORMAT_UNSUPPORT);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_003 start";
    PixelFormat destFormat = PixelFormat::RGB_888;
    std::shared_ptr<PixelMap> srcPiexlMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource =
        ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPiexlMap = rImageSource ->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPiexlMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPiexlMap, destFormat);
    EXPECT_EQ(ret, ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_003 end";
}

}   //namespace
}