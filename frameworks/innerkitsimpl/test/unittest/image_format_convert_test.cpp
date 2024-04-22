/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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


namespace OHOS {
namespace Media {
constexpr uint8_t NUM_5 = 5;
constexpr uint8_t NUM_8 = 8;
constexpr int32_t TREE_ORIGINAL_WIDTH = 800;
constexpr int32_t TREE_ORIGINAL_HEIGHT = 500;
constexpr int32_t ODDTREE_ORIGINAL_WIDTH = 951;
constexpr int32_t ODDTREE_ORIGINAL_HEIGHT = 595;
constexpr uint32_t BYTES_PER_PIXEL_RGB565 = 2;
constexpr uint32_t BYTES_PER_PIXEL_RGB = 3;
constexpr uint32_t BYTES_PER_PIXEL_RGBA = 4;
constexpr uint32_t BYTES_PER_PIXEL_BGRA = 4;

using namespace OHOS::HiviewDFX;
static const std::string IMAGE_INPUT_JPG_PATH = "/data/local/tmp/jpeg_include_icc_profile.jpg";
static const std::string IMAGE_INPUT_JPG_PATH1 = "/data/local/tmp/image/800-500.jpg";
static const std::string IMAGE_INPUT_JPG_PATH2 = "/data/local/tmp/image/951-595.jpg";
static const std::string IMAGE_OUTPUT_JPG_PATH = "/data/local/tmp/";

class ImageFormatConvertTest : public testing::Test {
public:
    static bool TestIsSupport(PixelFormat format);
    static ConvertFunction TestGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
    static bool TestMakeDestPixelMap(std::shared_ptr<PixelMap> &destPixelMap, uint8_buffer_type destBuffer,
        const size_t destBufferSize, ImageInfo &info, AllocatorType allcatorType);

public:
    ImageFormatConvertTest() {}
    ~ImageFormatConvertTest() {}
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void WriteToFile(const std::string &outpath, Size &imageSize, const std::string &outname, const uint8_t *data,
        const uint32_t size);
};

void ImageFormatConvertTest::SetUpTestCase(void) {}

void ImageFormatConvertTest::TearDownTestCase(void) {}

void ImageFormatConvertTest::SetUp(void) {}

void ImageFormatConvertTest::TearDown(void) {}

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

void ImageFormatConvertTest::WriteToFile(const std::string &outpath, Size &imageSize, const std::string &outname,
    const uint8_t *data, const uint32_t size)
{
    std::filesystem::path dir(outpath);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directory(dir);
    }

    std::string filepath =
        outpath + "/" + std::to_string(imageSize.width) + "-" + std::to_string(imageSize.height) + "-" + outname;

    std::ofstream outfile(filepath, std::ios::out | std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char *>(data), size);
        outfile.close();
    } else {
        ASSERT_TRUE(false);
    }
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
    Size size = { 800, 500 };
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
    Size size = { NUM_8, NUM_5 };
    const uint8_t *srcBuffer = new uint8_t[NUM_8 * NUM_5 * BYTES_PER_PIXEL_RGB]();
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
    Size size = { NUM_8, NUM_5 };
    const uint8_t *srcBuffer = new uint8_t[size.width * size.height +
        ((size.width + 1) / BYTES_PER_PIXEL_RGB565) * ((size.height + 1) / BYTES_PER_PIXEL_RGB565) * BYTES_PER_PIXEL_RGB565]();
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
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    ImageInfo srcInfo;
    srcPixelMap->GetImageInfo(srcInfo);
    ImageInfo destInfo;
    destInfo.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    destInfo.baseDensity = srcInfo.baseDensity;
    destInfo.colorSpace = srcInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = srcInfo.size;
    destInfo.size.width = 0;
    destInfo.size.height = 0;

    size_t destBufferSize = destInfo.size.width * destInfo.size.height +
        ((destInfo.size.width + 1) / BYTES_PER_PIXEL_RGB565) * ((destInfo.size.height + 1) / BYTES_PER_PIXEL_RGB565) * BYTES_PER_PIXEL_RGB565;
    uint8_buffer_type destBuffer = new uint8_t[destBufferSize]();
    bool ret = ImageFormatConvertTest::TestMakeDestPixelMap(srcPixelMap, destBuffer, destBufferSize, destInfo,
        srcPixelMap->GetAllocatorType());

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
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    ImageInfo srcInfo;
    srcPixelMap->GetImageInfo(srcInfo);
    ImageInfo destInfo;
    destInfo.alphaType = srcInfo.alphaType;
    destInfo.baseDensity = srcInfo.baseDensity;
    destInfo.colorSpace = srcInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = srcInfo.size;
    bool ret = ImageFormatConvertTest::TestMakeDestPixelMap(srcPixelMap, destBuffer, destBufferSize, destInfo,
        srcPixelMap->GetAllocatorType());
    EXPECT_EQ(ret, false);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, MakeDestPixelMap_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_003 start";
    PixelFormat destFormat = PixelFormat::NV12;
    std::shared_ptr<ImageSource> rImageSource;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    ImageInfo srcInfo;
    srcPixelMap->GetImageInfo(srcInfo);
    ImageInfo destInfo;
    destInfo.alphaType = srcInfo.alphaType;
    destInfo.baseDensity = srcInfo.baseDensity;
    destInfo.colorSpace = srcInfo.colorSpace;
    destInfo.pixelFormat = destFormat;
    destInfo.size = srcInfo.size;

    size_t destBufferSize = destInfo.size.width * destInfo.size.height +
        ((destInfo.size.width + 1) / BYTES_PER_PIXEL_RGB565) * ((destInfo.size.height + 1) / BYTES_PER_PIXEL_RGB565) * BYTES_PER_PIXEL_RGB565;
    uint8_buffer_type destBuffer = new uint8_t[destBufferSize]();

    bool ret = ImageFormatConvertTest::TestMakeDestPixelMap(srcPixelMap, destBuffer, destBufferSize, destInfo,
        srcPixelMap->GetAllocatorType());
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: MakeDestPixelMap_Test_003 end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_001 start";
    PixelFormat destFormat = PixelFormat::NV12;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    EXPECT_EQ(ret, ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_002 start";
    PixelFormat destFormat = PixelFormat::UNKNOWN;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    EXPECT_EQ(ret, ERR_MEDIA_FORMAT_UNSUPPORT);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_003 start";
    PixelFormat destFormat = PixelFormat::RGB_888;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    uint32_t errorCode = 0;
    DecodeOptions decodeOpts;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    EXPECT_EQ(ret, ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_888;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = TREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = TREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_RGB;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Tree_n21torgb.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGB/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_888;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = ODDTREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = ODDTREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_RGB;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Odd_n21torgb.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGB/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBA_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = TREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = TREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_RGBA;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Tree_n21torgba.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGBA/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBA_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = ODDTREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = ODDTREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_RGBA;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Odd_n21torgba.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGBA/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToBGRA_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = TREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = TREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_BGRA;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Tree_n21tobgra.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToBGRA/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToBGRA_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = ODDTREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = ODDTREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_BGRA;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Odd_n21tobgra.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToBGRA/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB565_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_565;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = TREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = TREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_RGB565;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Tree_n21torgb656.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGB565/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB565_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_565;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);

    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = ODDTREE_ORIGINAL_WIDTH;
    decodeOpts.desiredSize.height = ODDTREE_ORIGINAL_HEIGHT;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * BYTES_PER_PIXEL_RGB565;
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string outname = "Odd_n21torgb656.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGB565/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

}   //namespace
}