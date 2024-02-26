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
#include "jpeg_decoder_yuv.h"
#include "media_errors.h"
#include "abs_image_decoder.h"

using namespace testing::ext;
using namespace OHOS::ImagePlugin;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {
static const std::string IMAGE_INPUT_JPG_PATH = "/data/local/tmp/image/";
#define TREE_ORIGINAL_WIDTH 480
#define TREE_ORIGINAL_HEIGHT 360
#define ODDTREE_ORIGINAL_WIDTH 481
#define ODDTREE_ORIGINAL_HEIGHT 361
class JpgYUVDecoderTest : public testing::Test {
public:
    JpgYUVDecoderTest() {}
    ~JpgYUVDecoderTest() {}

    bool ReadImageData(std::string jpgpath, uint8_t*& jpegBuffer, uint32_t& jpegBufferSize);
    void DecodeToYUV(std::string srcjpg, int width, int height, JpegYUVFmt outfmt);
};

bool JpgYUVDecoderTest::ReadImageData(std::string jpgpath, uint8_t*& jpegBuffer, uint32_t& jpegBufferSize)
{
    FILE* jpgFile = fopen(jpgpath.c_str(), "rb");
    if (jpgFile == nullptr) {
        return false;
    }
    int ret = fseek(jpgFile, 0, SEEK_END);
    if (ret != 0) {
        return false;
    }
    jpegBufferSize = (int)ftell(jpgFile);
    ret = fseek(jpgFile, 0, SEEK_SET);
    if (ret != 0) {
        return false;
    }
    if (jpegBufferSize == 0 || jpegBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        return false;
    } else {
        jpegBuffer = new uint8_t[jpegBufferSize];
    }
    if (jpegBuffer == nullptr) {
        return false;
    }
    jpegBufferSize = fread(jpegBuffer, 1, jpegBufferSize, jpgFile);
    if (jpegBufferSize == 0) {
        return false;
    }
    ret = fclose(jpgFile);
    if (ret != 0) {
        return false;
    }
    return true;
}

void JpgYUVDecoderTest::DecodeToYUV(std::string srcjpg, int width, int height, JpegYUVFmt outfmt)
{
    std::string jpgpath = IMAGE_INPUT_JPG_PATH;
    jpgpath.append(srcjpg);
    uint32_t jpegBufferSize = 0;
    uint8_t* jpegBuffer = nullptr;
    bool readret = ReadImageData(jpgpath, jpegBuffer, jpegBufferSize);
    ASSERT_TRUE(readret);
    ASSERT_NE(jpegBufferSize, 0);
    ASSERT_NE(jpegBuffer, nullptr);

    uint32_t yuvBufferSize = JpegDecoderYUV::GetYUVOutSize(width, height);
    ASSERT_NE(yuvBufferSize, 0);
    if (yuvBufferSize == 0 || yuvBufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        ASSERT_TRUE(false);
        return;
    }
    uint8_t* yuvBuffer = new uint8_t[yuvBufferSize];
    if (yuvBuffer == nullptr) {
        ASSERT_TRUE(false);
        return;
    }
    std::unique_ptr<JpegDecoderYUV> decoderPtr = std::make_unique<JpegDecoderYUV>();
    JpegDecoderYUVParameter para = { 0, 0, jpegBuffer, jpegBufferSize, yuvBuffer, yuvBufferSize,
        outfmt, width, height };
    DecodeContext context;
    int ret = decoderPtr->DoDecode(context, para);
    ASSERT_EQ(ret, SUCCESS);
    ASSERT_NE(context.yuvInfo.imageSize.width, 0);
    ASSERT_NE(context.yuvInfo.imageSize.height, 0);
    ASSERT_NE(context.yuvInfo.y_width, 0);
    ASSERT_NE(context.yuvInfo.y_height, 0);
    ASSERT_NE(context.yuvInfo.uv_width, 0);
    ASSERT_NE(context.yuvInfo.uv_height, 0);
    if (outfmt == JpegYUVFmt::OutFmt_YU12 || outfmt == JpegYUVFmt::OutFmt_YV12) {
        ASSERT_NE(context.yuvInfo.y_stride, 0);
        ASSERT_NE(context.yuvInfo.u_stride, 0);
        ASSERT_NE(context.yuvInfo.v_stride, 0);
    } else {
        ASSERT_NE(context.yuvInfo.y_stride, 0);
        ASSERT_NE(context.yuvInfo.uv_stride, 0);
    }
    delete[] yuvBuffer;
    delete[] jpegBuffer;
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest001, TestSize.Level3)
{
    uint32_t jpegwidth = 480;
    uint32_t jpegheight = 360;
    uint32_t width = jpegwidth;
    uint32_t height = jpegheight;
    bool ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
    ASSERT_EQ(ret, true);

    ret = JpegDecoderYUV::GetScaledSize(0, jpegheight, width, height);
    ASSERT_EQ(ret, false);
    ret = JpegDecoderYUV::GetScaledSize(jpegwidth, 0, width, height);
    ASSERT_EQ(ret, false);

    width = 0;
    height = jpegheight;
    ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
    ASSERT_EQ(ret, true);
    width = jpegwidth;
    height = 0;
    ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
    ASSERT_EQ(ret, true);

    int testScale = 4;
    width = jpegwidth * testScale;
    height = jpegheight * testScale;
    ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
    ASSERT_EQ(ret, true);
    width = jpegwidth / testScale;
    height = jpegheight / testScale;
    ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
    ASSERT_EQ(ret, true);

    int offset = 11;
    width = jpegwidth + offset;
    height = jpegheight + offset;
    ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
    ASSERT_EQ(ret, true);
    width = jpegwidth - offset;
    height = jpegheight - offset;
    ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
    ASSERT_EQ(ret, true);
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest002, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-tree-444.jpg", TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest003, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-tree-422.jpg", TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest004, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-tree-420.jpg", TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest005, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-tree-400.jpg", TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest006, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-tree-440.jpg", TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest007, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-tree-411.jpg", TREE_ORIGINAL_WIDTH, TREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest008, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-treeodd-444.jpg", ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest009, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-treeodd-422.jpg", ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest010, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-treeodd-420.jpg", ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest011, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-treeodd-400.jpg", ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest012, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-treeodd-440.jpg", ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest013, TestSize.Level3)
{
    JpegYUVFmt outfmtList[] = { JpegYUVFmt::OutFmt_YU12, JpegYUVFmt::OutFmt_YV12,
        JpegYUVFmt::OutFmt_NV12, JpegYUVFmt::OutFmt_NV21};
    for (uint32_t j = 0; j < sizeof(outfmtList) / sizeof(JpegYUVFmt); j++) {
        DecodeToYUV("test-treeodd-411.jpg", ODDTREE_ORIGINAL_WIDTH, ODDTREE_ORIGINAL_HEIGHT, outfmtList[j]);
    }
}

HWTEST_F(JpgYUVDecoderTest, JpgYUVDecoderTest014, TestSize.Level3)
{
    uint32_t jpegwidth = 480;
    uint32_t jpegheight = 360;
    float maxScale = 2.5;
    float minScale = 0.05;
    float step = 0.01;
    for (float scaleFactor = maxScale; scaleFactor > minScale; scaleFactor -= step) {
        uint32_t width = jpegwidth * scaleFactor;
        uint32_t height = jpegheight * scaleFactor;
        GTEST_LOG_(INFO) << "JpgYUVDecoderTest:JpgYUVDecoderTest014 request size(" << width << ", " << height << ")";
        bool ret = JpegDecoderYUV::GetScaledSize(jpegwidth, jpegheight, width, height);
        ASSERT_EQ(ret, true);
        GTEST_LOG_(INFO) << "JpgYUVDecoderTest:JpgYUVDecoderTest014 ret size(" << width << ", " << height << ")";
        DecodeToYUV("test-tree-444.jpg", width, height, JpegYUVFmt::OutFmt_NV21);
    }
}

}
}