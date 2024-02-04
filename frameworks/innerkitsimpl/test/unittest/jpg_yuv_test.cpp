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
#include <fstream>
#include <fcntl.h>
#include <string>
#include <chrono>
#include "buffer_packer_stream.h"
#include "image_type.h"
#include "image_utils.h"
#include "image_source.h"
#include "image_source_util.h"
#include "media_errors.h"
#include "pixel_map.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImageSourceUtil;

namespace OHOS {
namespace Multimedia {
static const std::string IMAGE_INPUT_JPG_PATH = "/data/local/tmp/image/";
static const std::string IMAGE_OUTPUT_JPG_PATH = "/data/local/tmp/";
static constexpr uint32_t MAXSIZE = 10000;
#define TREE_ORIGINAL_WIDTH 480
#define TREE_ORIGINAL_HEIGHT 360
#define ODDTREE_ORIGINAL_WIDTH 481
#define ODDTREE_ORIGINAL_HEIGHT 361
static const uint8_t NUM_2 = 2;
static const uint8_t NUM_3 = 3;
class JpgYUVTest : public testing::Test {
public:
    JpgYUVTest() {}
    ~JpgYUVTest() {}

    void TestDecodeToSize(int width, int height);
    uint64_t GetNowTimeMicroSeconds();
    void DoTimeTest(std::string jpgname);
    void DecodeToFormat(std::string srcjpg, PixelFormat outfmt, int width, int height);
    void DecodeToYUV(std::string srcjpg, PixelFormat outfmt, std::string outname, int& width, int& height);
    void YUVCrop(std::string srcjpg, PixelFormat outfmt, std::string outname, int &width, int &height);
    void YUVRotate(std::string srcjpg, PixelFormat outfmt, std::string outname, int &width, int &height, float degrees);
    void YUVWriteConvert(std::string srcjpg, PixelFormat outfmt, std::string outname, int &width, int &height);
    void ScaleYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname, int width, int height,
                        float dstWidth, float dstHeight, AntiAliasingOption option);
    void ResizeYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname,
                        int width, int height, float dstWidth, float dstHeight);
    void FlipYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname,
                    int width, int height, bool xAxis, bool yAxis);
    void ApplyColorSpaceYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname,
                                int width, int height, const OHOS::ColorManager::ColorSpace &grColorSpace);
};

void JpgYUVTest::TestDecodeToSize(int width, int height)
{
    const char* srcjpg[] = { "test-tree-444.jpg", "test-tree-422.jpg", "test-tree-420.jpg",
        "test-tree-400.jpg", "test-tree-440.jpg", "test-tree-411.jpg"};
    const char* outNamePart1[] = { "tree-444", "tree-422", "tree-420", "tree-400", "tree-440", "tree-411"};
    const char* outNamePart2[] = { "-nv12.yuv", "-nv21.yuv"};
    PixelFormat outfmt[] = { PixelFormat::NV12, PixelFormat::NV21};
    for (uint32_t k = 0; k < sizeof(srcjpg) / sizeof(char*); k++) {
        for (uint32_t j = 0; j < sizeof(outfmt) / sizeof(PixelFormat); j++) {
            std::string jpgpath = IMAGE_INPUT_JPG_PATH;
            jpgpath.append(srcjpg[k]);
            std::string outname;
            outname.append(outNamePart1[k]);
            outname.append(outNamePart2[j]);
            int outWidth = width;
            int outHeight = height;
            DecodeToYUV(jpgpath, outfmt[j], outname, outWidth, outHeight);
        }
    }
}

uint64_t JpgYUVTest::GetNowTimeMicroSeconds()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

void JpgYUVTest::DecodeToFormat(std::string srcjpg, PixelFormat outfmt, int width, int height)
{
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
}

void JpgYUVTest::DoTimeTest(std::string jpgname)
{
    GTEST_LOG_(INFO) << "JpgYUVTest:DoTimeTest for: " << jpgname.c_str();
    const int testCount = 100;
    uint64_t startTick = GetNowTimeMicroSeconds();
    for (uint32_t k = 0; k < testCount; k++) {
        std::string jpgpath = IMAGE_INPUT_JPG_PATH;
        jpgpath.append(jpgname);
        DecodeToFormat(jpgpath, PixelFormat::RGBA_8888, 0, 0);
    }
    uint64_t endTick = GetNowTimeMicroSeconds();
    uint64_t argbCost = endTick - startTick;
    GTEST_LOG_(INFO) << "JpgYUVTest:DoTimeTest time argbCost: " << argbCost;

    startTick = GetNowTimeMicroSeconds();
    for (uint32_t k = 0; k < testCount; k++) {
        std::string jpgpath = IMAGE_INPUT_JPG_PATH;
        jpgpath.append(jpgname);
        DecodeToFormat(jpgpath, PixelFormat::NV12, 0, 0);
    }
    endTick = GetNowTimeMicroSeconds();
    uint64_t nv12Cost = endTick - startTick;
    GTEST_LOG_(INFO) << "JpgYUVTest:DoTimeTest time nv12Cost: " << nv12Cost;
}

void JpgYUVTest::DecodeToYUV(std::string srcjpg, PixelFormat outfmt, std::string outname, int& width, int& height)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetCapacity();
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "JpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = "/tmp/";
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE* outfile = fopen(outpath.c_str(), "wb");
    if (outfile) {
        fwrite(data, 1, size, outfile);
        int fret = fclose(outfile);
        if (fret != 0) {
            ASSERT_TRUE(false);
        }
    }
}

void jpgYUVTest::YUVRotate(std::string srcjpg, PixelFormat outfmt, std::string outname, int &width, int &height, float degrees)
{

    GTEST_LOG_(INFO) << "jpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "jpgYUVTest: srcjpg" << srcjpg;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    pixelMap->rotate(degrees);

    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetWidth() * pixelMap->GetHeight() + ((pixelMap->GetWidth() + 1) / NUM_2) * ((pixelMap->GetHeight() + 1) / NUM_2) * NUM_2;
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "jpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "YUVRotate/";
    std::filesystem::path dir(outpath);
    std::filesystem::create_directory(dir);
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE *outfile = fopen(outpath.c_str(), "wb");
    if (outfile)
    {
        fwrite(data, 1, size, outfile);
        fclose(outfile);
    }
}

void jpgYUVTest::YUVCrop(std::string srcjpg, PixelFormat outfmt, std::string outname, int &width, int &height)
{
    GTEST_LOG_(INFO) << "jpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "jpgYUVTest: srcjpg" << srcjpg;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    Rect rect = {0, 0, 100, 100};
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(pixelMap->crop(rect), SUCCESS);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);

    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetWidth() * pixelMap->GetHeight() + ((pixelMap->GetWidth() + 1) / NUM_2) * ((pixelMap->GetHeight() + 1) / NUM_2) * NUM_2;
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "jpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "YUVCrop/";
    std::filesystem::path dir(outpath);
    std::filesystem::create_directory(dir);
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE *outfile = fopen(outpath.c_str(), "wb");
    if (outfile)
    {
        fwrite(data, 1, size, outfile);
        fclose(outfile);
    }
}

void jpgYUVTest::YUVWriteConvert(std::string srcjpg, PixelFormat outfmt, std::string outname, int &width, int &height)
{
    GTEST_LOG_(INFO) << "jpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "jpgYUVTest: srcjpg" << srcjpg;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.editable = true;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    Rect region = {10, 10, 100, 100}; // The size of the crop
    size_t bufferSize = (region.width * region.height + ((region.width + 1) / NUM_2) * ((region.height + 1) / NUM_2) * NUM_2);
    uint8_t *dst = (uint8_t *)malloc(bufferSize);
    int stride = static_cast<uint32_t>(region.width) * NUM_3 / NUM_2;
    ASSERT_EQ(pixelMap->ReadPixels(bufferSize, 0, stride, region, dst), SUCCESS);

    Rect rect = {50, 50, 100, 100}; // The location and size of the write
    stride = static_cast<uint32_t>(rect.width) * NUM_3 / NUM_2;
    ASSERT_EQ(pixelMap->WritePixels(dst, bufferSize, 0, stride, rect), SUCCESS);
    free(dst);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);

    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetWidth() * pixelMap->GetHeight() + ((pixelMap->GetWidth() + 1) / NUM_2) * ((pixelMap->GetHeight() + 1) / NUM_2) * NUM_2;
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "jpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "WriteConvert/";
    std::filesystem::path dir(outpath);
    std::filesystem::create_directory(dir);
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE *outfile = fopen(outpath.c_str(), "wb");
    if (outfile)
    {
        fwrite(data, 1, size, outfile);
        fclose(outfile);
    }
}

void jpgYUVTest::ScaleYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname, int width, int height, float dstWidth, float dstHeight, AntiAliasingOption option)
{
    GTEST_LOG_(INFO) << "jpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "jpgYUVTest: srcjpg" << srcjpg;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    pixelMap->scale(dstWidth, dstHeight, option);

    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetCapacity();
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "jpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "scale/";
    std::filesystem::path dir(outpath);
    std::filesystem::create_directory(dir);
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE *outfile = fopen(outpath.c_str(), "wb");
    if (outfile)
    {
        fwrite(data, 1, size, outfile);
        fclose(outfile);
    }
}

void jpgYUVTest::ResizeYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname, int width, int height, float dstWidth, float dstHeight)
{
    GTEST_LOG_(INFO) << "jpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "jpgYUVTest: srcjpg" << srcjpg;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    pixelMap->resize(dstWidth, dstHeight);

    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetCapacity();
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "jpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "resize/";
    std::filesystem::path dir(outpath);
    std::filesystem::create_directory(dir);
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE *outfile = fopen(outpath.c_str(), "wb");
    if (outfile)
    {
        fwrite(data, 1, size, outfile);
        fclose(outfile);
    }
}

void jpgYUVTest::FlipYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname, int width, int height, bool xAxis, bool yAxis)
{
    GTEST_LOG_(INFO) << "jpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "jpgYUVTest: srcjpg" << srcjpg;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    pixelMap->flip(xAxis, yAxis);

    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetCapacity();
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "jpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "flip/";
    std::filesystem::path dir(outpath);
    std::filesystem::create_directory(dir);
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE *outfile = fopen(outpath.c_str(), "wb");
    if (outfile)
    {
        fwrite(data, 1, size, outfile);
        fclose(outfile);
    }
}

HWTEST_F(JpgYUVTest, JpgYUVTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest001 start";
    TestDecodeToSize(TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest001 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest002 start";
    int testWidth = 510;
    int testHeight = 460;
    TestDecodeToSize(testWidth, testHeight);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest002 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest003 start";
    int testWidth = 380;
    int testHeight = 211;
    TestDecodeToSize(testWidth, testHeight);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest003 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest004 start";
    int testWidth = 100;
    int testHeight = 100;
    TestDecodeToSize(testWidth, testHeight);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest004 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest005 start";
    int testWidth = 2000;
    int testHeight = 2000;
    TestDecodeToSize(testWidth, testHeight);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest005 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest006 start";
    PixelFormat outfmt[] = { PixelFormat::NV12, PixelFormat::NV21};
    const char* outNamePart2[] = { "-nv12.yuv", "-nv21.yuv"};
    for (uint32_t j = 0; j < sizeof(outfmt) / sizeof(PixelFormat); j++) {
        std::string jpgpath = IMAGE_INPUT_JPG_PATH;
        jpgpath.append("test_hw.jpg");
        std::string outname;
        outname.append("testhw");
        outname.append(outNamePart2[j]);
        int outWidth = 0;
        int outHeight = 0;
        DecodeToYUV(jpgpath, outfmt[j], outname, outWidth, outHeight);
    }
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest006 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest007 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH;
    jpgpath.append("test-tree-311.jpg");

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = 0;
    decodeOpts.desiredSize.height = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_NE(errorCode, SUCCESS);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest007 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest008 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH;
    jpgpath.append("test-bad.jpg");

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = 0;
    decodeOpts.desiredSize.height = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_NE(errorCode, SUCCESS);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest008 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest009, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest009 start";
    std::string jpgpath = IMAGE_INPUT_JPG_PATH;
    jpgpath.append("test_null.jpg");

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = 0;
    decodeOpts.desiredSize.height = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_NE(errorCode, SUCCESS);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest009 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest010, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest010 start";
    const char* srcjpg[] = { "test-treeodd-444.jpg", "test-treeodd-422.jpg", "test-treeodd-420.jpg",
        "test-treeodd-400.jpg", "test-treeodd-440.jpg", "test-treeodd-411.jpg"};
    const char* outNamePart1[] = { "treeodd-444", "treeodd-422", "treeodd-420",
        "treeodd-400", "treeodd-440", "treeodd-411"};
    const char* outNamePart2[] = { "-nv12.yuv", "-nv21.yuv"};
    PixelFormat outfmt[] = { PixelFormat::NV12, PixelFormat::NV21};
    for (uint32_t k = 0; k < sizeof(srcjpg) / sizeof(char*); k++) {
        for (uint32_t j = 0; j < sizeof(outfmt) / sizeof(PixelFormat); j++) {
            std::string jpgpath = IMAGE_INPUT_JPG_PATH;
            jpgpath.append(srcjpg[k]);
            std::string outname;
            outname.append(outNamePart1[k]);
            outname.append(outNamePart2[j]);
            int outWidth = ODDTREE_ORIGINAL_WIDTH;
            int outHeight = ODDTREE_ORIGINAL_HEIGHT;
            DecodeToYUV(jpgpath, outfmt[j], outname, outWidth, outHeight);
        }
    }
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest010 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest011, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest011 start";
    int testWidth = 1;
    int testHeight = 1;
    TestDecodeToSize(testWidth, testHeight);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest011 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest012, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest012 start";
    int testWidth = 1;
    int testHeight = 100;
    TestDecodeToSize(testWidth, testHeight);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest012 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest013, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest013 start";
    float scale = 0.875;
    int outwidth = TREE_ORIGINAL_WIDTH * scale;
    int outheight = TREE_ORIGINAL_HEIGHT * scale;
    TestDecodeToSize(outwidth, outheight);
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest013 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest014, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest014 start";
    float scalFactor = 2.5;
    float minscale = 0.05;
    float step = 0.01;
    for (; scalFactor >= minscale; scalFactor -= step) {
        std::string jpgpath = IMAGE_INPUT_JPG_PATH;
        jpgpath.append("test-tree-444.jpg");

        uint32_t errorCode = 0;
        SourceOptions opts;
        opts.formatHint = "image/jpeg";
        std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(jpgpath, opts, errorCode);
        ASSERT_EQ(errorCode, SUCCESS);
        ASSERT_NE(imageSource.get(), nullptr);
        DecodeOptions decodeOpts;
        decodeOpts.desiredPixelFormat = PixelFormat::NV12;
        decodeOpts.desiredSize.width = TREE_ORIGINAL_WIDTH * scalFactor;
        decodeOpts.desiredSize.height = TREE_ORIGINAL_HEIGHT * scalFactor;
        std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
        ASSERT_EQ(errorCode, SUCCESS);
    }
    GTEST_LOG_(INFO) << "JpgYUVTest: JpgYUVTest014 end";
}

HWTEST_F(JpgYUVTest, JpgYUVTest015, TestSize.Level3)
{
    DoTimeTest("test-tree-444.jpg");
}

HWTEST_F(JpgYUVTest, JpgYUVTest016, TestSize.Level3)
{
    DoTimeTest("test-tree-440.jpg");
}

HWTEST_F(JpgYUVTest, JpgYUVTest017, TestSize.Level3)
{
    DoTimeTest("test-tree-422.jpg");
}

HWTEST_F(JpgYUVTest, JpgYUVTest018, TestSize.Level3)
{
    DoTimeTest("test-tree-420.jpg");
    DoTimeTest("test-treeodd-420.jpg");
}

HWTEST_F(JpgYUVTest, JpgYUVTest019, TestSize.Level3)
{
    DoTimeTest("test-tree-411.jpg");
}

HWTEST_F(JpgYUVTest, JpgYUVTest020, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest:JpgYUVTest020";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/png";
    std::string pngpath = IMAGE_INPUT_JPG_PATH;
    pngpath.append("test.png");
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(pngpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV12;
    decodeOpts.desiredSize.width = 0;
    decodeOpts.desiredSize.height = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    uint32_t size = pixelMap->GetCapacity();
    ASSERT_EQ(pixelMap->GetPixelFormat(), PixelFormat::NV12);
    int32_t width = pixelMap->GetWidth();
    int32_t height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "JpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = "/tmp/";
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append("NV12-png");
    FILE* outfile = fopen(outpath.c_str(), "wb");
    if (outfile) {
        fwrite(data, 1, size, outfile);
        int fret = fclose(outfile);
        if (fret != 0) {
            ASSERT_TRUE(false);
        }
    }
}

HWTEST_F(JpgYUVTest, JpgYUVTest021, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYUVTest:JpgYUVTest021";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/png";
    std::string pngpath = IMAGE_INPUT_JPG_PATH;
    pngpath.append("test.png");
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(pngpath, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = PixelFormat::NV21;
    decodeOpts.desiredSize.width = 0;
    decodeOpts.desiredSize.height = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    uint32_t size = pixelMap->GetCapacity();
    ASSERT_EQ(pixelMap->GetPixelFormat(), PixelFormat::NV21);
    int32_t width = pixelMap->GetWidth();
    int32_t height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "JpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = "/tmp/";
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append("NV21-png");
    FILE* outfile = fopen(outpath.c_str(), "wb");
    if (outfile) {
        fwrite(data, 1, size, outfile);
        int fret = fclose(outfile);
        if (fret != 0) {
            ASSERT_TRUE(false);
        }
    }
}

HWTEST_F(jpgYUVTest, jpgYUVTest022, TestSize.Level3)
{
    const char *srcjpg[] = {"test-treeodd-444.jpg"};
    PixelFormat outfmt[] = {PixelFormat::NV12, PixelFormat::NV21};
    std::string jpgpath = IMAGE_INPUT_JPG_PATH;
    jpgpath.append(srcjpg[0]);
    float degrees[] = {90, 180, 270};
    int k = 0;
    const char *outFileName[] = {"90-nv12.yuv", "180-nv12.yuv", "270-nv12.yuv",
                                    "90-nv21.yuv", "180-nv21.yuv", "270-nv21.yuv"};
    for (int i = 0; i < sizeof(outfmt) / sizeof(PixelFormat); ++i)
    {
        for (int j = 0; j < sizeof(degrees) / sizeof(float); ++j)
        {
            std::string outname;
            outname.append(outFileName[k++]);
            GTEST_LOG_(INFO) << "start outname" << outname;
            int outWidth = 0;
            int outHeight = 0;
            YUVRotate(jpgpath, outfmt[i], outname, outWidth, outHeight, degrees[j]);
            GTEST_LOG_(INFO) << "end outname" << outname;
        }
    }
}

HWTEST_F(jpgYUVTest, jpgYUVTest023, TestSize.Level3)
{
    const char *srcjpg[] = {"test-treeodd-444.jpg"};
    PixelFormat outfmt[] = {PixelFormat::NV12, PixelFormat::NV21};
    std::string jpgpath = IMAGE_INPUT_JPG_PATH;
    jpgpath.append(srcjpg[0]);
    const char *outFileName[] = {"-yu12.yuv", "-yv12.yuv", "-nv12.yuv", "-nv21.yuv"};
    for (int i = 0; i < sizeof(outfmt) / sizeof(outfmt[0]); ++i)
    {
        std::string outname;
        outname.append(outFileName[i]);
        GTEST_LOG_(INFO) << "start outname" << outname;
        int width = 0;
        int height = 0;
        YUVCrop(jpgpath, outfmt[i], outname, width, height);
        GTEST_LOG_(INFO) << "end outname" << outname;
    }
}

HWTEST_F(jpgYUVTest, jpgYUVTest024, TestSize.Level3)
{
    const char *srcjpg[] = {"test-treeodd-444.jpg"};
    PixelFormat outfmt[] = {PixelFormat::NV12, PixelFormat::NV21};
    std::string jpgpath = IMAGE_INPUT_JPG_PATH;
    jpgpath.append(srcjpg[0]);
    const char *outFileName[] = {"-yu12.yuv", "-yv12.yuv", "-nv12.yuv", "-nv21.yuv"};
    for (int i = 0; i < sizeof(outfmt) / sizeof(outfmt[0]); ++i)
    {
        std::string outname;
        outname.append(outFileName[i]);
        GTEST_LOG_(INFO) << "start outname" << outname;
        int width = 0;
        int height = 0;
        YUVWriteConvert(jpgpath, outfmt[i], outname, width, height);
        GTEST_LOG_(INFO) << "end outname" << outname;
    }
}

HWTEST_F(jpgYUVTest, jpgYUVTest025, TestSize.Level3)
{
    PixelFormat outfmt[] = {PixelFormat::NV12, PixelFormat::NV21};
    const char *outNamePart2[] = {"-nv12.yuv", "-nv21.yuv"};
    AntiAliasingOption options[] = {AntiAliasingOption::NONE, AntiAliasingOption::LOW, AntiAliasingOption::MEDIUM, AntiAliasingOption::HIGH};
    std::filesystem::path dir("scale");
    if (!std::filesystem::exists(dir))
    {
        ASSERT_EQ(std::filesystem::create_directory(dir), true);
    }
    for (size_t j = 0; j < sizeof(outfmt) / sizeof(PixelFormat); j++)
    {
        for (size_t i = 0; i < sizeof(options) / sizeof(AntiAliasingOption); i++)
        {
            std::string jpgpath = IMAGE_INPUT_JPG_PATH;
            jpgpath.append("test-treeodd-444.jpg");
            std::string outname;
            outname.append("testhw");
            switch (options[i])
            {
            case AntiAliasingOption::NONE:
                outname.append("-NONE-");
                break;
            case AntiAliasingOption::LOW:
                outname.append("-LOW-");
                break;
            case AntiAliasingOption::MEDIUM:
                outname.append("-MEDIUM-");
                break;
            case AntiAliasingOption::HIGH:
                outname.append("-HIGH-");
                break;
            default:
                break;
            }
            outname.append(outNamePart2[j]);
            int outWidth = 480;
            int outHeight = 360;
            ScaleYUV420(jpgpath, outfmt[j], outname, outWidth, outHeight, 2, 2, options[i]);
        }
    }
}

HWTEST_F(jpgYUVTest, jpgYUVTest026, TestSize.Level3)
{
    PixelFormat outfmt[] = {PixelFormat::NV12, PixelFormat::NV21};
    const char *outNamePart2[] = {"-nv12.yuv", "-nv21.yuv"};
    std::filesystem::path dir("resize");
    if (!std::filesystem::exists(dir))
    {
        ASSERT_EQ(std::filesystem::create_directory(dir), true);
    }
    for (size_t j = 0; j < sizeof(outfmt) / sizeof(PixelFormat); j++)
    {
        std::string jpgpath = IMAGE_INPUT_JPG_PATH;
        jpgpath.append("test-treeodd-444.jpg");
        std::string outname;
        outname.append("testhw");
        outname.append(outNamePart2[j]);
        int outWidth = 480;
        int outHeight = 360;
        ResizeYUV420(jpgpath, outfmt[j], outname, outWidth, outHeight, 2, 2);
    }
}

HWTEST_F(jpgYUVTest, jpgYUVTest027, TestSize.Level3)
{
    PixelFormat outfmt[] = {PixelFormat::NV12, PixelFormat::NV21};
    const char *outNamePart2[] = {"-nv12.yuv", "-nv21.yuv"};
    bool flips[4][2] = {{true, true}, {true, false}, {false, true}, {false, false}};
    std::filesystem::path dir("flip");
    if (!std::filesystem::exists(dir))
    {
        ASSERT_EQ(std::filesystem::create_directory(dir), true);
    }
    for (size_t j = 0; j < sizeof(outfmt) / sizeof(PixelFormat); j++)
    {
        for (size_t i = 0; i < sizeof(flips) / (sizeof(bool) * 2); i++)
        {
            std::string jpgpath = IMAGE_INPUT_JPG_PATH;
            jpgpath.append("test-treeodd-444.jpg");
            std::string outname;
            outname.append("testhw");
            switch (i)
            {
            case 0:
                outname.append("-xy-");
                break;
            case 1:
                outname.append("-x-");
                break;
            case 2:
                outname.append("-y-");
                break;
            case 3:
                outname.append("-no-");
                break;
            default:
                break;
            }
            outname.append(outNamePart2[j]);
            int outWidth = 0;
            int outHeight = 0;
            FlipYUV420(jpgpath, outfmt[j], outname, outWidth, outHeight, flips[i][0], flips[i][1]);
        }
    }
}

#ifdef IMAGE_COLORSPACE_FLAG
// using namespace OHOS::ColorManager;
void jpgYUVTest::ApplyColorSpaceYUV420(std::string &srcjpg, PixelFormat outfmt, std::string &outname,
                                        int width, int height, const OHOS::ColorManager::ColorSpace &grColorSpace)
{
    GTEST_LOG_(INFO) << "jpgYUVTest: request size(" << width << ", " << height << ")";
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    GTEST_LOG_(INFO) << "jpgYUVTest: srcjpg" << srcjpg;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(srcjpg, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = outfmt;
    decodeOpts.desiredSize.width = width;
    decodeOpts.desiredSize.height = height;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    pixelMap->ApplyColorSpace(grColorSpace);

    uint8_t *data = const_cast<uint8_t *>(pixelMap->GetPixels());
    const uint8_t *buffer = nullptr;
    uint32_t size = pixelMap->GetCapacity();
    uint32_t maxSize = MAXSIZE;
    BufferPackerStream bufferPackerStream(data, maxSize);
    bool ret = bufferPackerStream.Write(buffer, size);
    ASSERT_EQ(pixelMap->GetPixelFormat(), outfmt);
    ASSERT_EQ(ret, false);
    width = pixelMap->GetWidth();
    height = pixelMap->GetHeight();
    GTEST_LOG_(INFO) << "jpgYUVTest: ret size(" << width << ", " << height << ")";
    std::string outpath = IMAGE_OUTPUT_JPG_PATH + "applyCorlorSpace/";
    std::filesystem::path dir(outpath);
    std::filesystem::create_directory(dir);
    outpath.append(std::to_string(width));
    outpath.append("-");
    outpath.append(std::to_string(height));
    outpath.append("-");
    outpath.append(outname);
    FILE *outfile = fopen(outpath.c_str(), "wb");
    if (outfile)
    {
        fwrite(data, 1, size, outfile);
        fclose(outfile);
    }
}

HWTEST_F(jpgYUVTest, jpgYUVTest028, TestSize.Level3)
{
    PixelFormat outfmt[] = {PixelFormat::NV12, PixelFormat::NV21};
    const char *outNamePart2[] = {"-nv12.yuv", "-nv21.yuv"};
    OHOS::ColorManager::ColorSpace colorSpaces[] = {
        OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::SRGB),
        OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::ADOBE_RGB),
        OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::DISPLAY_P3),
        OHOS::ColorManager::ColorSpace(OHOS::ColorManager::ColorSpaceName::BT2020)};
    std::filesystem::path dir("applyCorlorSpace");
    if (!std::filesystem::exists(dir))
    {
        ASSERT_EQ(std::filesystem::create_directory(dir), true);
    }
    for (size_t j = 0; j < sizeof(outfmt) / sizeof(PixelFormat); j++)
    {
        for (size_t i = 0; i < sizeof(colorSpaces) / sizeof(OHOS::ColorManager::ColorSpace); i++)
        {
            std::string jpgpath = IMAGE_INPUT_JPG_PATH;
            jpgpath.append("test-treeodd-444.jpg");
            std::string outname;
            outname.append("testhw");
            switch (colorSpaces[i].GetColorSpaceName())
            {
            case OHOS::ColorManager::ColorSpaceName::SRGB:
                outname.append("-SRGB-");
                break;
            case OHOS::ColorManager::ColorSpaceName::ADOBE_RGB:
                outname.append("-ADOBE_RGB-");
                break;
            case OHOS::ColorManager::ColorSpaceName::DISPLAY_P3:
                outname.append("-DISPLAY_P3-");
                break;
            case OHOS::ColorManager::ColorSpaceName::BT2020:
                outname.append("-BT2020-");
                break;
            default:
                break;
            }
            outname.append(outNamePart2[j]);
            int outWidth = 480;
            int outHeight = 360;
            ApplyColorSpaceYUV420(jpgpath, outfmt[j], outname, outWidth, outHeight, colorSpaces[i]);
        }
    }
}
#endif

}
}