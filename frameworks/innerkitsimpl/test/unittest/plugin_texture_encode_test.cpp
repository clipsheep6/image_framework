/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#define private public
#include <stdlib.h>
#include "astc_codec.h"
#include "image_compressor.h"
#include "image_system_properties.h"
#include "securec.h"
#include "media_errors.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "image_source_util.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImagePlugin;
namespace OHOS {
namespace Media {
static const std::string IMAGE_INPUT_RGBA_PATH = "/data/local/tmp/image/3840x2160.rgb";
static constexpr int32_t RGBA_TEST0001_WIDTH = 3840;
static constexpr int32_t RGBA_TEST0001_HEIGHT = 2160;
static constexpr uint8_t RGBA_BYTES_PIXEL_LOG2 = 2;
static constexpr int32_t TEXTURE_BLOCK_BYTES = 16;
static constexpr int32_t ASTC_BLOCK_WIDTH = 4;
static constexpr int32_t ASTC_BLOCK_HEIGHT = 4;
static constexpr int32_t ASTC_BLOCK_WIDTH_7 = 7;
static constexpr int32_t ASTC_BLOCK_HEIGHT_7 = 7;
class PluginTextureEncodeTest : public testing::Test {
public:
    PluginTextureEncodeTest() {}
    ~PluginTextureEncodeTest() {}
};

/**
 * @tc.name: ASTCEncode001
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode001 start";
    
    TextureEncodeOptions *param = new TextureEncodeOptions;
    param->width_ = RGBA_TEST0001_WIDTH;
    param->height_ = RGBA_TEST0001_HEIGHT;
    param->stride_ = RGBA_TEST0001_WIDTH;
    param->blockX_ = ASTC_BLOCK_WIDTH;
    param->blockY_ = ASTC_BLOCK_HEIGHT;
    bool enableQualityCheck = false;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    bool ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_RGBA_PATH, input, inputSize);
    ASSERT_EQ(ret, true);

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);

    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, SUCCESS);

    free(input);
    input = nullptr;
    free(output);
    output = nullptr;
    delete param;
    param = nullptr;
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode001 end";
}

/**
 * @tc.name: ASTCEncode002
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set enableQualityCheck to true
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode002 start";
    
    TextureEncodeOptions *param = new TextureEncodeOptions;
    param->width_ = RGBA_TEST0001_WIDTH;
    param->height_ = RGBA_TEST0001_HEIGHT;
    param->stride_ = RGBA_TEST0001_WIDTH;
    param->blockX_ = ASTC_BLOCK_WIDTH;
    param->blockY_ = ASTC_BLOCK_HEIGHT;
    bool enableQualityCheck = true;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    bool ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_RGBA_PATH, input, inputSize);
    ASSERT_EQ(ret, true);

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);

    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, SUCCESS);

    free(input);
    input = nullptr;
    free(output);
    output = nullptr;
    delete param;
    param = nullptr;
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode002 end";
}

/**
 * @tc.name: ASTCEncode003
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Invalid block size
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode003 start";
    
    TextureEncodeOptions *param = new TextureEncodeOptions;
    param->width_ = RGBA_TEST0001_WIDTH;
    param->height_ = RGBA_TEST0001_HEIGHT;
    param->stride_ = RGBA_TEST0001_WIDTH;
    param->blockX_ = ASTC_BLOCK_WIDTH_7;
    param->blockY_ = ASTC_BLOCK_HEIGHT_7;
    bool enableQualityCheck = true;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    bool ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_RGBA_PATH, input, inputSize);
    ASSERT_EQ(ret, true);

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);
    
    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, ERROR);

    free(input);
    input = nullptr;
    free(output);
    output = nullptr;
    delete param;
    param = nullptr;
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode003 end";
}

/**
 * @tc.name: ASTCEncode004
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set param to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode004 start";
    
    TextureEncodeOptions *param = new TextureEncodeOptions;
    param->width_ = RGBA_TEST0001_WIDTH;
    param->height_ = RGBA_TEST0001_HEIGHT;
    param->stride_ = RGBA_TEST0001_WIDTH;
    param->blockX_ = ASTC_BLOCK_WIDTH;
    param->blockY_ = ASTC_BLOCK_HEIGHT;
    bool enableQualityCheck = false;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    bool ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_RGBA_PATH, input, inputSize);
    ASSERT_EQ(ret, true);

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);

    delete param;
    param = nullptr;
    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, ERROR);

    free(input);
    input = nullptr;
    free(output);
    output = nullptr;
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode004 end";
}

/**
 * @tc.name: ASTCEncode005
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set input to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode005 start";
    
    TextureEncodeOptions *param = new TextureEncodeOptions;
    param->width_ = RGBA_TEST0001_WIDTH;
    param->height_ = RGBA_TEST0001_HEIGHT;
    param->stride_ = RGBA_TEST0001_WIDTH;
    param->blockX_ = ASTC_BLOCK_WIDTH;
    param->blockY_ = ASTC_BLOCK_HEIGHT;
    bool enableQualityCheck = false;

    int32_t blocksNum = ((param->width_ + param->blockX_ - 1) / param->blockX_) *
        ((param->height_ + param->blockY_ - 1) / param->blockY_);
    int32_t outputSize = TEXTURE_HEAD_BYTES + blocksNum * TEXTURE_BLOCK_BYTES;
    uint8_t *output = static_cast<uint8_t *>(malloc(outputSize));
    ASSERT_NE(output, nullptr);

    uint8_t *input = nullptr;
    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, ERROR);

    free(output);
    output = nullptr;
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode005 end";
}

/**
 * @tc.name: ASTCEncode006
 * @tc.desc: Test the AstcSoftwareEncode function
 * @tc.desc: Set output to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PluginTextureEncodeTest, ASTCEncode006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode006 start";
    
    TextureEncodeOptions *param = new TextureEncodeOptions;
    param->width_ = RGBA_TEST0001_WIDTH;
    param->height_ = RGBA_TEST0001_HEIGHT;
    param->stride_ = RGBA_TEST0001_WIDTH;
    param->blockX_ = ASTC_BLOCK_WIDTH;
    param->blockY_ = ASTC_BLOCK_HEIGHT;
    bool enableQualityCheck = false;

    int32_t inputSize = (param->width_ * param->height_) << RGBA_BYTES_PIXEL_LOG2;
    uint8_t *input = static_cast<uint8_t *>(malloc(inputSize));
    ASSERT_NE(input, nullptr);
    bool ret = OHOS::ImageSourceUtil::ReadFileToBuffer(IMAGE_INPUT_RGBA_PATH, input, inputSize);
    ASSERT_EQ(ret, true);

    uint8_t *output = nullptr;
    uint32_t softwareRet = AstcSoftwareEncode(input, param, enableQualityCheck, output);
    ASSERT_EQ(softwareRet, ERROR);

    free(input);
    input = nullptr;
    GTEST_LOG_(INFO) << "PluginTextureEncodeTest: ASTCEncode006 end";
}

} // namespace Multimedia
} // namespace OHOS