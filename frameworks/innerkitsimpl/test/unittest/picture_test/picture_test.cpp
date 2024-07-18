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

#define protected public
#include <gtest/gtest.h>
#include "picture.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {

class PictureTest : public testing::Test {
public:
    PictureTest() {}
    ~PictureTest() {}
};

constexpr int32_t sizeWidth = 2;
constexpr int32_t sizeHeight = 3;
constexpr int32_t bufferLength = 8;

static std::shared_ptr<PixelMap> CreatePixelMap()
{
    const uint32_t color[bufferLength] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    uint32_t colorlength = sizeof(color) / sizeof(color[0]);
    EXPECT_TRUE(colorlength == bufferLength);
    InitializationOptions options;
    options.size.width = sizeWidth;
    options.size.height = sizeHeight;
    options.srcPixelFormat = PixelFormat::UNKNOWN;
    options.pixelFormat = PixelFormat::UNKNOWN;
    options.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    std::unique_ptr<PixelMap> tmpPixelMap = PixelMap::Create(color, colorlength, options);
    std::shared_ptr<PixelMap> pixelmap = std::move(tmpPixelMap);
    return pixelmap;
}

/**
 * @tc.name: GetMainPixelTest001
 * @tc.desc: Get the mainPixelmap of the picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetMainPixelTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    std::shared_ptr<PixelMap> picPixelMap = picture->GetMainPixel();
    EXPECT_NE(picPixelMap, nullptr);
}

} // namespace Media
} // namespace OHOS
