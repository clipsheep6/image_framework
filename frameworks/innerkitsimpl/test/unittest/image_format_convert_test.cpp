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

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <gtest/hwext/gtest-multithread.h>
#include <iostream>
#include <unistd.h>

#include "buffer_packer_stream.h"
#include "hilog/log.h"
#include "hilog/log_cpp.h"
#include "image_format_convert.h"
#include "image_format_convert_utils.h"
#include "image_log.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "media_errors.h"

using namespace testing::mt;
using namespace testing::ext;
namespace OHOS {
namespace Media {
constexpr uint8_t TARGET_IMAGE_WIDTH = 8;
constexpr uint8_t TARGET_IMAGE_HEIGHT = 5;
constexpr int32_t TREE_ORIGINAL_WIDTH = 800;
constexpr int32_t TREE_ORIGINAL_HEIGHT = 500;
constexpr int32_t ODDTREE_ORIGINAL_WIDTH = 951;
constexpr int32_t ODDTREE_ORIGINAL_HEIGHT = 595;
constexpr uint32_t BYTES_PER_PIXEL_RGB565 = 2;
constexpr uint32_t BYTES_PER_PIXEL_RGB = 3;
constexpr uint32_t BYTES_PER_PIXEL_RGBA = 4;
constexpr uint32_t BYTES_PER_PIXEL_BGRA = 4;
constexpr uint32_t EVEN_ODD_DIVISOR = 2;
constexpr uint32_t TWO_SLICES = 2;

struct ImageSize {
    int32_t width = 0;
    int32_t height = 0;
    float dstWidth = 0;
    float dstHeight = 0;
    const uint32_t color = 0;
    uint32_t dst = 0;
};

using namespace OHOS::HiviewDFX;
static const std::string IMAGE_INPUT_JPG_PATH = "/data/local/tmp/jpeg_include_icc_profile.jpg";
static const std::string IMAGE_INPUT_JPG_PATH1 = "/data/local/tmp/image/800-500.jpg";
static const std::string IMAGE_INPUT_JPG_PATH2 = "/data/local/tmp/image/951-595.jpg";
static const std::string IMAGE_OUTPUT_JPG_PATH = "/data/local/tmp/";
static std::map<std::pair<PixelFormat, PixelFormat>, std::string>  convertOutName = {};

class ImageFormatConvertTest : public testing::Test {
public:
    static bool TestIsSupport(PixelFormat format);
    static ConvertFunction TestGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
    static YUVConvertFunction TestYUVGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
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
    void RgbConvertToYuv(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize);
};

void ImageFormatConvertTest::SetUpTestCase(void)
{
    convertOutName = {
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV21), "rgb565tonv21.yuv"},
        {std::make_pair(PixelFormat::RGB_565, PixelFormat::NV12), "rgb565tonv12.yuv"},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV21), "rgb8888tonv21.yuv"},
        {std::make_pair(PixelFormat::RGBA_8888, PixelFormat::NV12), "rgb8888tonv12.yuv"},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV21), "bgr8888tonv21.yuv"},
        {std::make_pair(PixelFormat::BGRA_8888, PixelFormat::NV12), "bgr8888tonv12.yuv"},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV21), "rgb888tonv21.yuv"},
        {std::make_pair(PixelFormat::RGB_888, PixelFormat::NV12), "rgb888tonv12.yuv"},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV21), "rgbaf16tonv21.yuv"},
        {std::make_pair(PixelFormat::RGBA_F16, PixelFormat::NV12), "rgbaf16tonv12.yuv"},
    };
}

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

YUVConvertFunction ImageFormatConvertTest::TestYUVGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat)
{
    return ImageFormatConvert::YUVGetConvertFuncByFormat(srcFormat, destFormat);
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

void ImageFormatConvertTest::RgbConvertToYuv(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize)
{
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::string jpgPath = srcSize.width % EVEN_ODD_DIVISOR == 0 ? IMAGE_INPUT_JPG_PATH1 : IMAGE_INPUT_JPG_PATH2;
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(jpgPath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    decodeOpts.desiredSize.width = srcSize.width;
    decodeOpts.desiredSize.height = srcSize.height;
    std::shared_ptr<PixelMap> srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    ImageInfo destImageInfo;
    srcPixelMap->GetImageInfo(destImageInfo);
    uint32_t buffersize = static_cast<size_t>(destImageInfo.size.width * destImageInfo.size.height +
        ((destImageInfo.size.width + 1) / TWO_SLICES) * ((destImageInfo.size.height + 1) / TWO_SLICES) * TWO_SLICES);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size = destImageInfo.size;
    auto iter = convertOutName.find(std::make_pair(srcFormat, destFormat));
    ASSERT_NE(iter, convertOutName.end());
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + iter->second : "Odd_" + iter->second;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUV/";
    WriteToFile(outpath, size, outname, data, buffersize);
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
    Size size = { TARGET_IMAGE_WIDTH, TARGET_IMAGE_HEIGHT };
    const uint8_t *srcBuffer = new uint8_t[size.width * size.height * BYTES_PER_PIXEL_RGB]();
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
    YUVConvertFunction cvtFunc = ImageFormatConvertTest::TestYUVGetConvertFuncByFormat(srcFormat, destFormat);
    ASSERT_NE(cvtFunc, nullptr);
    
    YUVDataInfo yuvDataInfo = { TARGET_IMAGE_WIDTH,
                                TARGET_IMAGE_HEIGHT,
                                (TARGET_IMAGE_WIDTH + 1) / EVEN_ODD_DIVISOR,
                                (TARGET_IMAGE_HEIGHT + 1) / EVEN_ODD_DIVISOR,
                                TARGET_IMAGE_WIDTH,
                                (TARGET_IMAGE_WIDTH + 1) / EVEN_ODD_DIVISOR,
                                (TARGET_IMAGE_WIDTH + 1) / EVEN_ODD_DIVISOR,
                                (TARGET_IMAGE_WIDTH + 1) / EVEN_ODD_DIVISOR * TWO_SLICES,
                                0, 0, 0,
                                TARGET_IMAGE_WIDTH * TARGET_IMAGE_HEIGHT };
    const uint8_t *srcBuffer = new uint8_t[yuvDataInfo.yWidth * yuvDataInfo.yHeight +
        ((yuvDataInfo.yWidth + 1) / BYTES_PER_PIXEL_RGB565) *
        ((yuvDataInfo.yHeight + 1) / BYTES_PER_PIXEL_RGB565) * BYTES_PER_PIXEL_RGB565]();
    uint8_buffer_type destBuffer = nullptr;
    size_t destBufferSize = 0;
    ColorSpace colorspace = ColorSpace::UNKNOWN;
    EXPECT_EQ(cvtFunc(srcBuffer, yuvDataInfo, &destBuffer, destBufferSize, colorspace), true);
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
        ((destInfo.size.width + 1) / BYTES_PER_PIXEL_RGB565) *
        ((destInfo.size.height + 1) / BYTES_PER_PIXEL_RGB565) * BYTES_PER_PIXEL_RGB565;
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
        ((destInfo.size.width + 1) / BYTES_PER_PIXEL_RGB565) *
        ((destInfo.size.height + 1) / BYTES_PER_PIXEL_RGB565) * BYTES_PER_PIXEL_RGB565;
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

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV21_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV21_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV12_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV12_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV21_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV21_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV21_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV21_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV12_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV12_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV12_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV12_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV21_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV21_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV12_001, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV12_002, TestSize.Level3)
{
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
}

HWTEST_F(ImageFormatConvertTest, RGBToNV21_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_001: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat tmpFormat = PixelFormat::RGB_888;
    uint32_t tmpRet = ImageFormatConvert::ConvertImageFormat(srcPixelMap, tmpFormat);
    ASSERT_EQ(tmpRet, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), tmpFormat);

    PixelFormat destFormat = PixelFormat::NV21;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    ImageInfo destImageInfo;
    srcPixelMap->GetImageInfo(destImageInfo);
    uint32_t buffersize = static_cast<size_t>(destImageInfo.size.width * destImageInfo.size.height +
        ((destImageInfo.size.width + 1) / TWO_SLICES) * ((destImageInfo.size.height + 1) / TWO_SLICES) * TWO_SLICES);

    Size size = destImageInfo.size;
    auto iter = convertOutName.find(std::make_pair(tmpFormat, destFormat));
    ASSERT_NE(iter, convertOutName.end());
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + iter->second : "Odd_" + iter->second;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUV/";
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV21_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_002: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat tmpFormat = PixelFormat::RGB_888;
    uint32_t tmpRet = ImageFormatConvert::ConvertImageFormat(srcPixelMap, tmpFormat);
    ASSERT_EQ(tmpRet, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), tmpFormat);

    PixelFormat destFormat = PixelFormat::NV21;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    ImageInfo destImageInfo;
    srcPixelMap->GetImageInfo(destImageInfo);
    uint32_t buffersize = static_cast<size_t>(destImageInfo.size.width * destImageInfo.size.height +
        ((destImageInfo.size.width + 1) / TWO_SLICES) * ((destImageInfo.size.height + 1) / TWO_SLICES) * TWO_SLICES);

    Size size = destImageInfo.size;
    auto iter = convertOutName.find(std::make_pair(tmpFormat, destFormat));
    ASSERT_NE(iter, convertOutName.end());
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + iter->second : "Odd_" + iter->second;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUV/";
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV12_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_001: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat tmpFormat = PixelFormat::RGB_888;
    uint32_t tmpRet = ImageFormatConvert::ConvertImageFormat(srcPixelMap, tmpFormat);
    ASSERT_EQ(tmpRet, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), tmpFormat);

    PixelFormat destFormat = PixelFormat::NV12;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    ImageInfo destImageInfo;
    srcPixelMap->GetImageInfo(destImageInfo);
    uint32_t buffersize = static_cast<size_t>(destImageInfo.size.width * destImageInfo.size.height + 
        ((destImageInfo.size.width + 1) / TWO_SLICES) * ((destImageInfo.size.height + 1) / TWO_SLICES) * TWO_SLICES);

    Size size = destImageInfo.size;
    auto iter = convertOutName.find(std::make_pair(tmpFormat, destFormat));
    ASSERT_NE(iter, convertOutName.end());
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + iter->second : "Odd_" + iter->second;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUV/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_001: start";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV12_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_002: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat tmpFormat = PixelFormat::RGB_888;
    uint32_t tmpRet = ImageFormatConvert::ConvertImageFormat(srcPixelMap, tmpFormat);
    ASSERT_EQ(tmpRet, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), tmpFormat);

    PixelFormat destFormat = PixelFormat::NV12;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    ImageInfo destImageInfo;
    srcPixelMap->GetImageInfo(destImageInfo);
    uint32_t buffersize = static_cast<size_t>(destImageInfo.size.width * destImageInfo.size.height + 
        ((destImageInfo.size.width + 1) / TWO_SLICES) * ((destImageInfo.size.height + 1) / TWO_SLICES) * TWO_SLICES);

    Size size = destImageInfo.size;
    auto iter = convertOutName.find(std::make_pair(tmpFormat, destFormat));
    ASSERT_NE(iter, convertOutName.end());
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + iter->second : "Odd_" + iter->second;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUV/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_002: start";
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
    std::string outname = "Tree_n21torgb565.yuv";
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
    std::string outname = "Odd_n21torgb565.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGB565/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, NV21ToNV12_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_Test_001 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH1;
    ImageSize imageSize = {TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV21;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::NV12;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);

    uint32_t buffersize = pixelMap->GetWidth() * pixelMap->GetHeight() + ((pixelMap->GetWidth() + 1) / 2) *
        ((pixelMap->GetHeight() + 1) / 2) * 2;
    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Tree_n21tonv12.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToNV12/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToNV12_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_Test_002 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH2;
    ImageSize imageSize = {ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV21;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::NV12;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    uint32_t buffersize = pixelMap->GetWidth() * pixelMap->GetHeight() +
        ((pixelMap->GetWidth() + 1) / EVEN_ODD_DIVISOR) * ((pixelMap->GetHeight() + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Odd_n21tonv12.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToNV12/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToNV21_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_Test_001 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH1;
    ImageSize imageSize = {TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::NV21;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    uint32_t buffersize = pixelMap->GetWidth() * pixelMap->GetHeight() +
        ((pixelMap->GetWidth() + 1) / EVEN_ODD_DIVISOR) * ((pixelMap->GetHeight() + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Tree_n12tonv21.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ToNV21/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToNV21_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_Test_002 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH2;
    ImageSize imageSize = {ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::NV21;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    uint32_t buffersize = pixelMap->GetWidth() * pixelMap->GetHeight() +
        ((pixelMap->GetWidth() + 1) / EVEN_ODD_DIVISOR) * ((pixelMap->GetHeight() + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Odd_n12tonv21.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ToNV21/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGB565_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_Test_001 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH1;

    ImageSize imageSize = {TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::RGB_565;

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);
    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    uint32_t buffersize = size.width * size.height * TWO_SLICES;
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Tree_n12torgb565.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ToRGB565/";
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGB565_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_Test_002 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH2;
    ImageSize imageSize = {ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::RGB_565;

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);
    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    uint32_t buffersize = size.width * size.height * TWO_SLICES;
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Odd_n12torgb565.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ToRGB565/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_Test_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBAF16_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_Test_001 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH1;
    ImageSize imageSize = {TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV21;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::RGBA_F16;

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);
    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    uint32_t buffersize = size.width * size.height * sizeof(uint64_t);
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Tree_n21torgbaf16.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGBAF16/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBAF16_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_Test_002 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH2;
    ImageSize imageSize = {ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT};
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: request size(" << imageSize.width << ", " << imageSize.height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: srcjpg" << jpgpath;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV21;
    decodeOpts.desiredSize.width = imageSize.width;
    decodeOpts.desiredSize.height = imageSize.height;
    std::shared_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    YUVDataInfo yuvInfo;
    pixelMap.get()->GetImageYUVInfo(yuvInfo);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: created size(" << yuvInfo.yWidth << ", " << yuvInfo.yHeight << ")";
    PixelFormat destFormat = PixelFormat::RGBA_F16;

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(pixelMap, destFormat);
    EXPECT_EQ(ret, SUCCESS);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(pixelMap->GetPixelFormat(), destFormat);
    Size size;
    size.width = pixelMap->GetWidth();
    size.height = pixelMap->GetHeight();
    ASSERT_EQ(imageSize.width, size.width);
    ASSERT_EQ(imageSize.height, size.height);
    uint32_t buffersize = size.width * size.height * sizeof(uint64_t);
    std::cout << "width: " << size.width << " height: " << size.height << std::endl;
    std::string outname = "Odd_n21torgbaf16.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV21ToRGBAF16/";
    WriteToFile(outpath, size, outname, data, buffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_Test_002 end";
}
HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), srcFormat);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::RGBA_F16;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToRGBAF16: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToRGBAF16.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::RGBA_8888;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToRGBA: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToRGBA.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::BGRA_8888;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToBGRA: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToBGRA.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::RGB_888;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToRGB: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToRGB.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), srcFormat);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::RGBA_F16;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToRGBAF16: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToRGBAF16.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::RGBA_8888;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToRGBA: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToRGBA.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_0010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::BGRA_8888;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToBGRA: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToBGRA.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}

HWTEST_F(ImageFormatConvertTest, ConvertImageFormat_Test_0011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: start";
    PixelFormat srcFormat = PixelFormat::NV12;
    uint32_t errorCode = 0;

    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    auto rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH2, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = srcFormat;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    PixelFormat destFormat = PixelFormat::RGB_888;
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    GTEST_LOG_(INFO) << "NV12ToRGB: size(" << srcPixelMap->GetWidth() << ", " << srcPixelMap->GetHeight() << ")";
    std::string outname = "NV12ToRGB.yuv";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "NV12ConvertRGB/";
    uint32_t buffersize = srcPixelMap->GetWidth() * srcPixelMap->GetHeight() * srcPixelMap->GetPixelBytes();
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    WriteToFile(outpath, size, outname, data, buffersize);

    GTEST_LOG_(INFO) << "ImageFormatConvertTest: end";
}
} // namespace Media
} // namespace OHOS