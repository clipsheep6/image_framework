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
constexpr int32_t TREE_ORIGINAL_WIDTH = 800;
constexpr int32_t TREE_ORIGINAL_HEIGHT = 500;
constexpr int32_t ODDTREE_ORIGINAL_WIDTH = 951;
constexpr int32_t ODDTREE_ORIGINAL_HEIGHT = 595;
constexpr int32_t P010_ORIGINAL_WIDTH = 1920;
constexpr int32_t P010_ORIGINAL_HEIGHT = 1080;
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

static const std::string IMAGE_INPUT_JPG_PATH1 = "/data/local/tmp/image/800-500.jpg";
static const std::string IMAGE_INPUT_JPG_PATH2 = "/data/local/tmp/image/951-595.jpg";
static const std::string IMAGE_OUTPUT_JPG_PATH = "/data/local/tmp/";
static const std::string IMAGE_INPUT_YUV_PATH3 = "/data/local/tmp/image/P010.yuv";
static const std::string IMAGE_INPUT_RGBA_PATH4 = "/data/local/tmp/image/RGBA1010102.rgba";
class ImageFormatConvertTest : public testing::Test {
public:
    ImageFormatConvertTest() {}
    ~ImageFormatConvertTest() {}
    static ConvertFunction TestGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat);
    void WriteToFile(const std::string &outpath, Size &imageSize, const std::string &outname, const uint8_t *data,
        const uint32_t size);
    bool ReadFile(void *ch_org, std::string path, int32_t totalsize, int32_t src_num);
    void RgbConvertToYuv(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize);
    void RgbConvertToYuvP010(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize);
    void Rgba1010102ConvertToYuvP010(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize,
        uint32_t destBuffersize);
    void YuvConvertToRgb(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize, uint32_t destBuffersize);
    void YuvP010ConvertToRgb(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize, uint32_t destBuffersize);
    void RgbConvertToYuvByPixelMap(PixelFormat &tempFormat, PixelFormat &srcFormat,
        PixelFormat &destFormat, Size &srcSize);
    void RgbConvertToYuvP010ByPixelMap(PixelFormat &tempFormat, PixelFormat &srcFormat,
        PixelFormat &destFormat, Size &srcSize);
};

ConvertFunction ImageFormatConvertTest::TestGetConvertFuncByFormat(PixelFormat srcFormat, PixelFormat destFormat)
{
    return ImageFormatConvert::GetConvertFuncByFormat(srcFormat, destFormat);
}

static const string GetNamedPixelFormat(const PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::UNKNOWN:
            return "UNKNOWN";
        case PixelFormat::RGB_565:
            return "RGB_565";
        case PixelFormat::RGB_888:
            return "RGB_888";
        case PixelFormat::NV21:
            return "NV21";
        case PixelFormat::NV12:
            return "NV12";
        case PixelFormat::RGBA_8888:
            return "RGBA_8888";
        case PixelFormat::BGRA_8888:
            return "BGRA_8888";
        case PixelFormat::RGBA_F16:
            return "RGBA_F16";
        case PixelFormat::RGBA_1010102:
            return "RGBA_1010102";
        case PixelFormat::YCBCR_P010:
            return "YCBCR_P010";
        case PixelFormat::YCRCB_P010:
            return "YCRCB_P010";
        default:
            return "UNKNOWN";
    }
    return "UNKNOWN";
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
    std::string formatName = GetNamedPixelFormat(srcFormat) + "To" + GetNamedPixelFormat(destFormat) + ".yuv";
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + formatName : "Odd_" + formatName;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUV/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

void ImageFormatConvertTest::YuvConvertToRgb(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize,
    uint32_t destBuffersize)
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
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();
    std::string formatName = GetNamedPixelFormat(srcFormat) + "To" + GetNamedPixelFormat(destFormat) + ".rgb";
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + formatName : "Odd_" + formatName;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "YUVToRGB/";
    WriteToFile(outpath, size, outname, data, destBuffersize);
}

bool ImageFormatConvertTest::ReadFile(void *ch_org, std::string path, int32_t totalsize, int32_t src_num)
{
    FILE* const file_org = fopen(path.c_str(), "rb");
    if (file_org == NULL) {
        GTEST_LOG_(INFO) << "Cannot open" << path.c_str();
        GTEST_LOG_(INFO) << "fopen";
        exit(1);
        return false;
    }
    if (src_num == 0)
    {
        size_t bytes_org = fread(ch_org, sizeof(uint8_t), static_cast<size_t>(totalsize), file_org);
        if (bytes_org < static_cast<size_t>(totalsize)) {
            GTEST_LOG_(INFO) << "Read fail";
            exit(1);
            return false;
        }
    } else {
        size_t bytes_org = fread(ch_org, sizeof(uint16_t), static_cast<size_t>(totalsize), file_org);
        if (bytes_org < static_cast<size_t>(totalsize)) {
            GTEST_LOG_(INFO) << "Read fail " << bytes_org << " totalsize" << totalsize;
            exit(1);
            return false;
        }
    }
    return true;
}

void ImageFormatConvertTest::YuvP010ConvertToRgb(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize,
    uint32_t destBuffersize)
{
    ImageSize imageSize;
    imageSize.width = srcSize.width;
    imageSize.height = srcSize.height;
    int32_t y_size = imageSize.width * imageSize.height;
    int32_t uv_size = ((imageSize.width + 1) / 2) * ((imageSize.height + 1) / 2);
    const size_t total_size = (y_size + 2 * uv_size);
    uint16_t* const ch_org = new uint16_t[total_size];
    bool result = ReadFile(ch_org, IMAGE_INPUT_YUV_PATH3, total_size, 1);
    ASSERT_NE(result, true);

    const uint32_t dataLength = total_size * 2;
    uint32_t *data = (uint32_t *)ch_org;
    InitializationOptions opts;
    opts.srcPixelFormat = srcFormat;
    opts.pixelFormat = srcFormat;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.size.width = imageSize.width;
    opts.size.height = imageSize.height;
    std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(data, dataLength, opts);
    ASSERT_NE(pixelMap.get(), nullptr);

    std::shared_ptr<PixelMap> srcPixelMap = std::move(pixelMap);
    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();

    std::string formatName = GetNamedPixelFormat(srcFormat) + "To" + GetNamedPixelFormat(destFormat) + ".yuv";
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + formatName : "Odd_" + formatName;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "P010ToRgba1010102/";
    uint8_t *data8 = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data8, nullptr);
    WriteToFile(outpath, size, outname, data8, destBuffersize);
}

void ImageFormatConvertTest::Rgba1010102ConvertToYuvP010(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize,
    uint32_t destBuffersize)
{
    ImageSize imageSize;
    imageSize.width = srcSize.width;
    imageSize.height = srcSize.height;
    int32_t y_size = imageSize.width * imageSize.height;
    const size_t total_size = y_size * 4;
    uint8_t* const ch_org = new uint8_t[total_size];
    bool result = ReadFile(ch_org, IMAGE_INPUT_RGBA_PATH4, total_size, 0);
    ASSERT_NE(result, true);

    const uint32_t dataLength = total_size;
    uint32_t *data = (uint32_t *)ch_org;
    InitializationOptions opts;
    opts.srcPixelFormat = srcFormat;
    opts.pixelFormat = srcFormat;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    opts.size.width = imageSize.width;
    opts.size.height = imageSize.height;
    std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(data, dataLength, opts);
    ASSERT_NE(pixelMap.get(), nullptr);
    std::shared_ptr<PixelMap> srcPixelMap = std::move(pixelMap);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size;
    size.width = srcPixelMap->GetWidth();
    size.height = srcPixelMap->GetHeight();

    std::string formatName = GetNamedPixelFormat(srcFormat) + "To" + GetNamedPixelFormat(destFormat) + ".yuv";
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + formatName : "Odd_" + formatName;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "Rgba1010102ToP010/";
    uint8_t *data8 = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data8, nullptr);
    WriteToFile(outpath, size, outname, data8, destBuffersize);
}

void ImageFormatConvertTest::RgbConvertToYuvByPixelMap(PixelFormat &tempFormat, PixelFormat &srcFormat,
    PixelFormat &destFormat, Size &srcSize)
{
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::string jpgPath = srcSize.width % EVEN_ODD_DIVISOR == 0 ? IMAGE_INPUT_JPG_PATH1 : IMAGE_INPUT_JPG_PATH2;
    auto rImageSource = ImageSource::CreateImageSource(jpgPath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = tempFormat;
    decodeOpts.desiredSize.width = srcSize.width;
    decodeOpts.desiredSize.height = srcSize.height;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t tmpRet = ImageFormatConvert::ConvertImageFormat(srcPixelMap, srcFormat);
    ASSERT_EQ(tmpRet, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), srcFormat);

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
    std::string formatName = GetNamedPixelFormat(srcFormat) + "To" + GetNamedPixelFormat(destFormat) + ".yuv";
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + formatName : "Odd_" + formatName;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUV/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

void ImageFormatConvertTest::RgbConvertToYuvP010ByPixelMap(PixelFormat &tempFormat, PixelFormat &srcFormat,
    PixelFormat &destFormat, Size &srcSize)
{
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::string jpgPath = srcSize.width % EVEN_ODD_DIVISOR == 0 ? IMAGE_INPUT_JPG_PATH1 : IMAGE_INPUT_JPG_PATH2;
    auto rImageSource = ImageSource::CreateImageSource(jpgPath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = tempFormat;
    decodeOpts.desiredSize.width = srcSize.width;
    decodeOpts.desiredSize.height = srcSize.height;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t tmpRet = ImageFormatConvert::ConvertImageFormat(srcPixelMap, srcFormat);
    ASSERT_EQ(tmpRet, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), srcFormat);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);
    uint8_t *data = const_cast<uint8_t *>(srcPixelMap->GetPixels());
    ASSERT_NE(data, nullptr);

    ImageInfo destImageInfo;
    srcPixelMap->GetImageInfo(destImageInfo);
    uint32_t buffersize = static_cast<size_t>((destImageInfo.size.width * destImageInfo.size.height +
        ((destImageInfo.size.width + 1) / TWO_SLICES) * ((destImageInfo.size.height + 1) / TWO_SLICES)
        * TWO_SLICES) * TWO_SLICES);
    Size size = destImageInfo.size;
    std::string formatName = GetNamedPixelFormat(srcFormat) + "To" + GetNamedPixelFormat(destFormat) + "P010.yuv";
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + formatName : "Odd_" + formatName;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUVP010/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

void ImageFormatConvertTest::RgbConvertToYuvP010(PixelFormat &srcFormat, PixelFormat &destFormat, Size &srcSize)
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
    uint32_t buffersize = static_cast<size_t>((destImageInfo.size.width * destImageInfo.size.height +
        ((destImageInfo.size.width + 1) / TWO_SLICES) * ((destImageInfo.size.height + 1) / TWO_SLICES)
        * TWO_SLICES) * TWO_SLICES);
    ASSERT_EQ(srcPixelMap->GetPixelFormat(), destFormat);

    Size size = destImageInfo.size;
    std::string formatName = GetNamedPixelFormat(srcFormat) + "To" + GetNamedPixelFormat(destFormat) + "P010.yuv";
    std::string outname = size.width % EVEN_ODD_DIVISOR == 0 ? "Tree_" + formatName : "Odd_" + formatName;
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "RGBToYUVP010/";
    WriteToFile(outpath, size, outname, data, buffersize);
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV21_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21_001: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV21_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21_002: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV12_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12_001: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV12_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12_002: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV21_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV21_001: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV21_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV21_002: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV12_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV12_001: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV12_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV12_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV12_002: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV12_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV21_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_001: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV21_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_002: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV12_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_001: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV12_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_002: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV12_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12_001: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV12_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12_002: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV21_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21_001: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV21_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21_002: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21_002: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV21_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21_001: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21_001: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV21_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21_002: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21_002: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV12_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12_001: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12_001: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV12_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12_002: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuv(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12_002: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_001: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_888;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_001: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_002: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_888;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12_002: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBA_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGBA_001: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGBA_001: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBA_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGBA_002: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGBA_002: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToBGRA_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToBGRA_001: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_BGRA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToBGRA_001: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToBGRA_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToBGRA_002: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_BGRA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToBGRA_002: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB565_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGB565_001: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_565;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB565;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGB565_001: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGB565_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGB565_002: start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGB_565;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height * BYTES_PER_PIXEL_RGB565;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV21ToRGB565_002: end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToNV12_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_001 start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToNV12_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_002 start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToNV21_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_001 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToNV21_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_002 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGB565_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_001 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGB_565;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB565;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGB565_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_002 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGB_565;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB565;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB565_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBAF16_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_Test_001 start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGBA_F16;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * sizeof(uint64_t);
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToRGBAF16_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_002 start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::RGBA_F16;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * sizeof(uint64_t);
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToRGBAF16_002 end";
}
HWTEST_F(ImageFormatConvertTest, NV12ToRGBAF16_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBAF16_001 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGBA_F16;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * sizeof(uint64_t);
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBAF16_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGBAF16_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBAF16_002 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGBA_F16;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * sizeof(uint64_t);
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBAF16_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGBA_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBA_001 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBA_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGBA_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBA_002 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGBA_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToBGRA_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToBGRA_001 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_BGRA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToBGRA_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToBGRA_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToBGRA_002 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_BGRA;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToBGRA_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGB_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB_001 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGB_888;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = TREE_ORIGINAL_WIDTH * TREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToRGB_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB_002 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::RGB_888;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = ODDTREE_ORIGINAL_WIDTH * ODDTREE_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToRGB_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToNV12P010_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12P010_001 start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12P010_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToNV12P010_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12P010_002 start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV12P010_002 end";
}

HWTEST_F(ImageFormatConvertTest, NV12P010ToNV12_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV12_003 start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV12_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToNV12P010_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV12P010_001 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES)  * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV12P010_001 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToNV12P010_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV12P010_002 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV12P010_002 end";
}

HWTEST_F(ImageFormatConvertTest,NV12P010ToNV21_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV21_003 start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV21_003 end";
}

HWTEST_F(ImageFormatConvertTest,NV12P010ToNV21P010_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV21P010_003 start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV21P010_003 end";
}

HWTEST_F(ImageFormatConvertTest,NV12P010ToRGB565_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV12P010ToRGB565_003: start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::RGB_565;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height * BYTES_PER_PIXEL_RGB565;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV12P010ToRGB565_003: end";
}

HWTEST_F(ImageFormatConvertTest, NV12P010ToRGBA_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV12P010ToRGBA_003: start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV12P010ToRGBA_003: end";
}

HWTEST_F(ImageFormatConvertTest, NV12P010ToBGRA_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV12P010ToBGRA_003: start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_BGRA;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.NV12P010ToBGRA_003: end";
}

HWTEST_F(ImageFormatConvertTest, NV12P010ToRGB_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToRGB_003 start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::RGB_888;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToRGB_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV12P010ToRGBAF16_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToRGBAF16_003 start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::RGBA_F16;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * sizeof(uint64_t);
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToRGBAF16_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV21ToNV21P010_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV21P010_003 start";
    PixelFormat srcFormat = PixelFormat::NV21;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21ToNV21P010_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToNV12_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToNV12_003 start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToNV12_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV12ToNV21P010_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21P010_003 start";
    PixelFormat srcFormat = PixelFormat::NV12;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    YuvConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12ToNV21P010_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToNV21_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToNV21_003 start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToNV21_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV12P010ToNV12P010_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV12P010_003 start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToNV12P010_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToRGB565_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGB565_003: start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::RGB_565;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height * BYTES_PER_PIXEL_RGB565;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGB565_003: end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToRGBA_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGBA_003: start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::RGBA_8888;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGBA_003: end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToBGRA_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToBGRA_003: start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::BGRA_8888;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_BGRA;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToBGRA_003: end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToRGB_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGB_003 start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::RGB_888;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGB;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGB_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToRGBAF16_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGBAF16_003 start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::RGBA_F16;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * sizeof(uint64_t);
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGBAF16_003 end";
}

HWTEST_F(ImageFormatConvertTest, NV12P010ToRGBA_1010102_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToRGBA_1010102_004 start";
    PixelFormat srcFormat = PixelFormat::YCBCR_P010;
    PixelFormat destFormat = PixelFormat::RGBA_1010102;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV12P010ToRGBA_1010102_004 end";
}

HWTEST_F(ImageFormatConvertTest, NV21P010ToRGBA_1010102_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGBA_1010102_004 start";
    PixelFormat srcFormat = PixelFormat::YCRCB_P010;
    PixelFormat destFormat = PixelFormat::RGBA_1010102;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = P010_ORIGINAL_WIDTH * P010_ORIGINAL_HEIGHT * BYTES_PER_PIXEL_RGBA;
    YuvP010ConvertToRgb(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: NV21P010ToRGBA_1010102_004 end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV12P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12P010_001: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV12P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12P010_002: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV12P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV21P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21P010_001: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGB565ToNV21P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21P010_002: start";
    PixelFormat srcFormat = PixelFormat::RGB_565;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGB565ToNV21P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV12P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV12P010_001: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV12P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV12P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV21_002: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV21P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV21P010_001: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV21P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAToNV21P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV21P010_002: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_8888;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAToNV21P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV12P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12P010_001: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV12P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12P010_002: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV12P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV21P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21P010_001: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, BGRAToNV21P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21P010_002: start";
    PixelFormat srcFormat = PixelFormat::BGRA_8888;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010(srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.BGRAToNV21P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV12P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12P010_001: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV12P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12P010_002: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV12P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV21P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21P010_001: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBToNV21P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21P010_002: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBToNV21P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV12P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12P010_001: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV12P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12P010_002: start";
    PixelFormat tempFormat = PixelFormat::NV12;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV12P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV21P010_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21P010_001: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21P010_001: end";
}

HWTEST_F(ImageFormatConvertTest, RGBAF16ToNV21P010_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21P010_002: start";
    PixelFormat tempFormat = PixelFormat::NV21;
    PixelFormat srcFormat = PixelFormat::RGBA_F16;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT };
    RgbConvertToYuvP010ByPixelMap(tempFormat, srcFormat, destFormat, srcSize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21P010_002: end";
}

HWTEST_F(ImageFormatConvertTest, RGBA_1010102ToNV12_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBA_1010102ToNV12_003: start";
    PixelFormat srcFormat = PixelFormat::RGBA_1010102;
    PixelFormat destFormat = PixelFormat::NV12;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    Rgba1010102ConvertToYuvP010(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBA_1010102ToNV12_003: end";
}

HWTEST_F(ImageFormatConvertTest, RGBA_1010102ToNV21_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBA_1010102ToNV21_003: start";
    PixelFormat srcFormat = PixelFormat::RGBA_1010102;
    PixelFormat destFormat = PixelFormat::NV21;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES;
    Rgba1010102ConvertToYuvP010( srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBAF16ToNV21P010_003: end";
}

HWTEST_F(ImageFormatConvertTest, RGBA_1010102ToNV12P010_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBA_1010102ToNV12P010_003: start";
    PixelFormat srcFormat = PixelFormat::RGBA_1010102;
    PixelFormat destFormat = PixelFormat::YCBCR_P010;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    Rgba1010102ConvertToYuvP010( srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBA_1010102ToNV12P010_003: end";
}

HWTEST_F(ImageFormatConvertTest, RGBA_1010102ToNV21P010_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBA_1010102ToNV21P010_003: start";
    PixelFormat srcFormat = PixelFormat::RGBA_1010102;
    PixelFormat destFormat = PixelFormat::YCRCB_P010;
    Size srcSize = { P010_ORIGINAL_WIDTH, P010_ORIGINAL_HEIGHT };
    uint32_t destBuffersize = (srcSize.width * srcSize.height + ((srcSize.width + 1) / EVEN_ODD_DIVISOR) *
        ((srcSize.height + 1) / EVEN_ODD_DIVISOR) * TWO_SLICES) * TWO_SLICES;
    Rgba1010102ConvertToYuvP010(srcFormat, destFormat, srcSize, destBuffersize);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest.RGBA_1010102ToNV21P010_003: end";
}

HWTEST_F(ImageFormatConvertTest, GetConvertFuncByFormat_Test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_001 start";
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::NV12;
    ConvertFunction cvtFunc = ImageFormatConvertTest::TestGetConvertFuncByFormat(srcFormat, destFormat);

    const_uint8_buffer_type srcBuffer = nullptr;
    Size size = { TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT };
    uint8_buffer_type destBuffer = nullptr;
    size_t destBufferSize = 0;
    ColorSpace colorspace = ColorSpace::UNKNOWN;

    EXPECT_EQ(cvtFunc(srcBuffer, size, &destBuffer, destBufferSize, colorspace), false);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_001 end";
}

HWTEST_F(ImageFormatConvertTest, GetConvertFuncByFormat_Test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_004 start";
    PixelFormat srcFormat = PixelFormat::RGB_888;
    PixelFormat destFormat = PixelFormat::UNKNOWN;
    ConvertFunction cvtFunc = ImageFormatConvertTest::TestGetConvertFuncByFormat(srcFormat, destFormat);

    EXPECT_EQ(cvtFunc, nullptr);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: GetConvertFuncByFormat_Test_004 end";
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
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
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
    std::shared_ptr<ImageSource> rImageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH1, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(rImageSource.get(), nullptr);

    srcPixelMap = rImageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(srcPixelMap.get(), nullptr);

    uint32_t ret = ImageFormatConvert::ConvertImageFormat(srcPixelMap, destFormat);
    EXPECT_EQ(ret, ERR_IMAGE_INVALID_PARAMETER);
    GTEST_LOG_(INFO) << "ImageFormatConvertTest: ConvertImageFormat_Test_003 end";
}
} // namespace Media
} // namespace OHOS