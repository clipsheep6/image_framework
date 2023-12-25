/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include <cstdlib>
#include "astc_codec.h"
#include "image_compressor.h"
#include "image_system_properties.h"
#include "securec.h"
#include "media_errors.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "image_source_util.h"
#include "buffer_packer_stream.h"
#include "image_utils.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImagePlugin;
namespace OHOS {
namespace Media {
static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test.jpg";
static constexpr int32_t RGBA_TEST0001_WIDTH = 256;
static constexpr int32_t RGBA_TEST0001_HEIGHT = 256;
static constexpr uint8_t RGBA_BYTES_PIXEL_LOG2 = 2;
static constexpr int32_t PIXEL_VALUE_MAX = 256;
static constexpr int32_t TEXTURE_BLOCK_BYTES = 16;
static constexpr int32_t ASTC_BLOCK_WIDTH = 4;
static constexpr int32_t ASTC_BLOCK_HEIGHT = 4;
static constexpr int32_t OUTPUT_SIZE_MAX = 200000;
class PluginTextureEncodeTest : public testing::Test {
public:
    PluginTextureEncodeTest() {}
    ~PluginTextureEncodeTest() {}
};

static TextureEncodeOptions* SetDefaultOption()
{
    TextureEncodeOptions *param = new TextureEncodeOptions;
    if (param == nullptr) {
        return nullptr;
    }
    param->width_ = RGBA_TEST0001_WIDTH;
    param->height_ = RGBA_TEST0001_HEIGHT;
    param->stride_ = RGBA_TEST0001_WIDTH;
    param->blockX_ = ASTC_BLOCK_WIDTH;
    param->blockY_ = ASTC_BLOCK_HEIGHT;
    return param;
}

/**
 * @tc.name: ASTCEncode001
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode001 start";
    size_t bufferSize = 0;
    bool ret = ImageUtils::GetFileSize(IMAGE_INPUT_JPEG_PATH, bufferSize);
    ASSERT_EQ(ret, true);
    uint8_t *buffer = static_cast<uint8_t *>(malloc(bufferSize));
    ASSERT_NE(buffer, nullptr);
    ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_JPEG_PATH, buffer, bufferSize);
    ASSERT_EQ(ret, true);
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    uint32_t index = 0;
    DecodeOptions optsPixel;
    errorCode = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(index, optsPixel, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    Media::PixelMap *pixelMapPtr = pixelMap.get();
    ASSERT_NE(pixelMapPtr, nullptr);
    uint8_t *output = static_cast<uint8_t *>(malloc(OUTPUT_SIZE_MAX));
    ASSERT_NE(output, nullptr);
    BufferPackerStream *stream = new (std::nothrow) BufferPackerStream(output, OUTPUT_SIZE_MAX);
    ASSERT_NE(stream, nullptr);
    struct PlEncodeOptions option = { "image/astc/4*4", 100, 1 }; // quality set to 100
    AstcCodec astcEncoder;
    uint32_t setRet = astcEncoder.SetAstcEncode(stream, option, pixelMapPtr);
    ASSERT_EQ(setRet, SUCCESS);
    uint32_t astcRet = astcEncoder.ASTCEncode();
    ASSERT_EQ(astcRet, SUCCESS);
    if (buffer != nullptr) {
        free(buffer);
        buffer = nullptr;
    }
    if (output != nullptr) {
        free(output);
        output = nullptr;
    }
    if (stream != nullptr) {
        delete stream;
        stream = nullptr;
    }
    if (pixelMapPtr != nullptr) {
        pixelMapPtr = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode001 end";
}

/**
 * @tc.name: ASTCEncode002
 * @tc.desc: Test the SetAstcEncode function
 * @tc.desc: Set outputStream to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode002 start";
    size_t bufferSize = 0;
    bool ret = ImageUtils::GetFileSize(IMAGE_INPUT_JPEG_PATH, bufferSize);
    ASSERT_EQ(ret, true);
    uint8_t *buffer = static_cast<uint8_t *>(malloc(bufferSize));
    ASSERT_NE(buffer, nullptr);
    ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_JPEG_PATH, buffer, bufferSize);
    ASSERT_EQ(ret, true);
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    uint32_t index = 0;
    DecodeOptions optsPixel;
    errorCode = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(index, optsPixel, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    Media::PixelMap *pixelMapPtr = pixelMap.get();
    ASSERT_NE(pixelMapPtr, nullptr);

    BufferPackerStream *stream = nullptr;

    struct PlEncodeOptions option = { "image/astc/4*4", 100, 1 }; // quality set to 100
    AstcCodec astcEncoder;
    uint32_t setRet = astcEncoder.SetAstcEncode(stream, option, pixelMapPtr);
    ASSERT_EQ(setRet, ERROR);

    if (buffer != nullptr) {
        free(buffer);
        buffer = nullptr;
    }
    if (pixelMapPtr != nullptr) {
        pixelMapPtr = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode002 end";
}

/**
 * @tc.name: ASTCEncode003
 * @tc.desc: Test the SetAstcEncode function
 * @tc.desc: Set pixelMap to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode003 start";
    Media::PixelMap *pixelMapPtr = nullptr;

    uint8_t *output = static_cast<uint8_t *>(malloc(OUTPUT_SIZE_MAX));
    ASSERT_NE(output, nullptr);
    BufferPackerStream *stream = new (std::nothrow) BufferPackerStream(output, OUTPUT_SIZE_MAX);
    ASSERT_NE(stream, nullptr);

    struct PlEncodeOptions option = { "image/astc/4*4", 100, 1 }; // quality set to 100
    AstcCodec astcEncoder;
    uint32_t setRet = astcEncoder.SetAstcEncode(stream, option, pixelMapPtr);
    ASSERT_EQ(setRet, ERROR);

    if (output != nullptr) {
        free(output);
        output = nullptr;
    }
    if (stream != nullptr) {
        delete stream;
        stream = nullptr;
    }
    if (pixelMapPtr != nullptr) {
        pixelMapPtr = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode003 end";
}

/**
 * @tc.name: ASTCEncode004
 * @tc.desc: AstcSoftwareEncode return error test
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode004 start";
    size_t bufferSize = 0;
    bool ret = ImageUtils::GetFileSize(IMAGE_INPUT_JPEG_PATH, bufferSize);
    ASSERT_EQ(ret, true);
    uint8_t *buffer = static_cast<uint8_t *>(malloc(bufferSize));
    ASSERT_NE(buffer, nullptr);
    ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_JPEG_PATH, buffer, bufferSize);
    ASSERT_EQ(ret, true);
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);
    uint32_t index = 0;
    DecodeOptions optsPixel;
    errorCode = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(index, optsPixel, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    Media::PixelMap *pixelMapPtr = pixelMap.get();
    ASSERT_NE(pixelMapPtr, nullptr);
    uint8_t *output = static_cast<uint8_t *>(malloc(OUTPUT_SIZE_MAX));
    ASSERT_NE(output, nullptr);
    BufferPackerStream *stream = new (std::nothrow) BufferPackerStream(output, OUTPUT_SIZE_MAX);
    ASSERT_NE(stream, nullptr);
    struct PlEncodeOptions option = { "image/astc/7*7", 100, 1 }; // quality set to 100
    AstcCodec astcEncoder;
    uint32_t setRet = astcEncoder.SetAstcEncode(stream, option, pixelMapPtr);
    ASSERT_EQ(setRet, SUCCESS);
    uint32_t astcRet = astcEncoder.ASTCEncode();
    ASSERT_EQ(astcRet, ERROR);
    if (buffer != nullptr) {
        free(buffer);
        buffer = nullptr;
    }
    if (output != nullptr) {
        free(output);
        output = nullptr;
    }
    if (stream != nullptr) {
        delete stream;
        stream = nullptr;
    }
    if (pixelMapPtr != nullptr) {
        pixelMapPtr = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode004 end";
}

/**
 * @tc.name: ASTCEncode005
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set enableQualityCheck to true
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode005 start";

    TextureEncodeOptions *param = SetDefaultOption();
    ASSERT_NE(param, nullptr);

    bool enableQualityCheck = true;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    for (int32_t i = 0; i < inputSize; ++i) {
        *(input++) = static_cast<uint8_t>(i % PIXEL_VALUE_MAX);
    }

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);

    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, SUCCESS);

    if (input != nullptr) {
        free(input);
        input = nullptr;
    }
    if (output != nullptr) {
        free(output);
        output = nullptr;
    }
    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode005 end";
}

/**
 * @tc.name: ASTCEncode006
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set param to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode006 start";

    TextureEncodeOptions *param = SetDefaultOption();
    ASSERT_NE(param, nullptr);

    bool enableQualityCheck = false;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    for (int32_t i = 0; i < inputSize; ++i) {
        *(input++) = static_cast<uint8_t>(i % PIXEL_VALUE_MAX);
    }

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);

    delete param;
    param = nullptr;
    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, ERROR);

    if (input != nullptr) {
        free(input);
        input = nullptr;
    }
    if (output != nullptr) {
        free(output);
        output = nullptr;
    }
    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode006 end";
}

/**
 * @tc.name: ASTCEncode007
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set input to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode007 start";

    TextureEncodeOptions *param = SetDefaultOption();
    ASSERT_NE(param, nullptr);

    bool enableQualityCheck = false;

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);

    uint8_t *input = nullptr;
    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, ERROR);

    if (output != nullptr) {
        free(output);
        output = nullptr;
    }
    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode007 end";
}

/**
 * @tc.name: ASTCEncode008
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set output to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode008 start";

    TextureEncodeOptions *param = SetDefaultOption();
    ASSERT_NE(param, nullptr);

    bool enableQualityCheck = false;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    for (int32_t i = 0; i < inputSize; ++i) {
        *(input++) = static_cast<uint8_t>(i % PIXEL_VALUE_MAX);
    }

    uint8_t *output = nullptr;
    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, ERROR);

    if (input != nullptr) {
        free(input);
        input = nullptr;
    }
    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode008 end";
}
} // namespace Multimedia
} // namespace OHOS