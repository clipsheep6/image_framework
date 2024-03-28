/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include <cmath>
#include "securec.h"
#include "memory.h"
#include "nine_patch_listener.h"
#include "plugin_export.h"

using namespace testing::ext;
namespace OHOS {
namespace Multimedia {
const std::string CHUNK_NAME = "npTc";
constexpr float FLOAT_NEAR_ZERO = (1.0f / (1 << 12));
constexpr float NO_SCALE = 1.0f;
class NinePathListenerTest : public testing::Test {
public:
    NinePathListenerTest() {}
    ~NinePathListenerTest() {}
};

/**
 * @tc.name: ReadChunk001
 * @tc.desc: test ReadChunk
 * @tc.type: FUNC
 */
HWTEST_F(NinePathListenerTest, ReadChunk001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NinePathListenerTest: ReadChunk001 start";
    ImagePlugin::NinePatchListener ninepath;
    const std::string tag = "npTc";
    void *data = nullptr;
    ASSERT_EQ(data, nullptr);
    size_t length = 88;
    bool readck = ninepath.ReadChunk(tag, data, length);
    ASSERT_EQ(readck, false);
    GTEST_LOG_(INFO) << "NinePathListenerTest: ReadChunk001 end";
}

/**
 * @tc.name: Scale001
 * @tc.desc: test Scale
 * @tc.type: FUNC
 */
HWTEST_F(NinePathListenerTest, Scale001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NinePathListenerTest: Scale001 start";
    ImagePlugin::NinePatchListener ninepath;
    float scaleX = 1.0;
    float scaleY = 2.0;
    int32_t scaledWidth = 3;
    int32_t scaledHeight = 4;
    ninepath.Scale(scaleX, scaleY, scaledWidth, scaledHeight);
    ASSERT_NE(&ninepath, nullptr);
    GTEST_LOG_(INFO) << "NinePathListenerTest: Scale001 end";
}

/**
 * @tc.name: ReadChunk002
 * @tc.desc: test ReadChunk
 * @tc.type: FUNC
 */
HWTEST_F(NinePathListenerTest, ReadChunk002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NinePathListenerTest: ReadChunk002 start";
    ImagePlugin::NinePatchListener ninepath;
    const std::string tag = "npTc";
    int *p = new int;
    int32_t length = 33;
    bool ret = ninepath.ReadChunk(tag, static_cast<void *>(p), length);
    ASSERT_EQ(ret, false);
    delete p;
    p = nullptr;
    GTEST_LOG_(INFO) << "NinePathListenerTest: ReadChunk002 end";
}

/**
 * @tc.name: ReadChunk003
 * @tc.desc: test ReadChunk
 * @tc.type: FUNC
 */
HWTEST_F(NinePathListenerTest, ReadChunk003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NinePathListenerTest: ReadChunk003 start";
    ImagePlugin::NinePatchListener ninepath;
    std::string tag = CHUNK_NAME;
    void *data = malloc(32);
    ImagePlugin::PngNinePatchRes *patch = static_cast<ImagePlugin::PngNinePatchRes *>(data);
    patch->numXDivs = 0;
    patch->numYDivs = 0;
    patch->numColors = 0;
    size_t length = patch->SerializedSize();
    size_t size = 32 + patch->numXDivs * sizeof(int32_t) + patch->numYDivs * sizeof(int32_t)
        + patch->numColors * sizeof(uint32_t);
    bool readck = ninepath.ReadChunk(tag, data, length);
    ASSERT_EQ(length, size);
    ASSERT_EQ(ninepath.patchSize_, length);
    ASSERT_EQ(readck, true);
    GTEST_LOG_(INFO) << "NinePathListenerTest: ReadChunk003 end";
}

/**
 * @tc.name: Scale002
 * @tc.desc: test Scale
 * @tc.type: FUNC
 */
HWTEST_F(NinePathListenerTest, Scale002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "NinePathListenerTest: Scale002 start";
    ImagePlugin::NinePatchListener ninepath;
    float scaleX = 1.0f;
    float scaleY = 2.0f;
    int32_t scaledWidth = 3;
    int32_t scaledHeight = 4;
    ninepath.Scale(scaleX, scaleY, scaledWidth, scaledHeight);
    ASSERT_EQ(ninepath.patch_, nullptr);

    uint8_t *ptr = new uint8_t;
    void *data = static_cast<void *>(ptr);
    ninepath.patch_ = static_cast<ImagePlugin::PngNinePatchRes *>(data);
    scaleX = NO_SCALE + FLOAT_NEAR_ZERO + 1.0f;
    ninepath.patch_->paddingLeft = 1;
    ninepath.patch_->paddingRight = 1;
    ninepath.patch_->xDivsOffset = 0;
    ninepath.patch_->numXDivs = 3;

    scaleY = NO_SCALE + FLOAT_NEAR_ZERO + 1.0f;
    ninepath.patch_->paddingTop = 1;
    ninepath.patch_->paddingBottom = 1;
    ninepath.patch_->yDivsOffset = 0;
    ninepath.patch_->numYDivs = 3;
    ninepath.Scale(scaleX, scaleY, scaledWidth, scaledHeight);
    ASSERT_NE(ninepath.patch_, nullptr);
    ASSERT_NE(ninepath.patch_->GetYDivs(), nullptr);
    GTEST_LOG_(INFO) << "NinePathListenerTest: Scale002 end";
}

/**
 * @tc.name: PluginExternalCreate001
 * @tc.desc: test PluginExternalCreate
 * @tc.type: FUNC
 */
HWTEST_F(NinePathListenerTest, PluginExternalCreate001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginExportTest: PluginExternalCreate001 start";
    std::string className = "#ImplClassType";
    auto result = PluginExternalCreate(className);
    ASSERT_EQ(result, nullptr);
    className = "#OHOS::PluginExample::LabelDetector";
    result = PluginExternalCreate(className);
    ASSERT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "PluginExportTest: PluginExternalCreate001 end";
}
} // namespace Multimedia
} // namespace OHOS