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
#include "auxiliary_picture.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"
#include "picture.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {

class AuxiliaryPictureTest : public testing::Test {
public:
    AuxiliaryPictureTest() {}
    ~AuxiliaryPictureTest() {}
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
 * @tc.name: AuxiliaryPictureCreateTest001
 * @tc.desc: Create an auxiliary picture using Pixelmap.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, AuxiliaryPictureCreateTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = { sizeWidth, sizeHeight } ;
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_NE(auxPicture, nullptr);
}

/**
 * @tc.name: GetTypeTest001
 * @tc.desc: When the auxiliaryPicture type is GAINMAP, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = { sizeWidth, sizeHeight } ;
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::GAINMAP);
}

/**
 * @tc.name: GetTypeTest002
 * @tc.desc: When the auxiliaryPicture type is LINEAR_MAP, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest002, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::LINEAR_MAP;
    Size size = { sizeWidth, sizeHeight };
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::LINEAR_MAP);
}

/**
 * @tc.name: GetTypeTest003
 * @tc.desc: When the auxiliaryPicture type is FRAGMENT_MAP, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest003, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::FRAGMENT_MAP;
    Size size = { sizeWidth, sizeHeight };
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::FRAGMENT_MAP);
}

/**
 * @tc.name: GetTypeTest004
 * @tc.desc: When the auxiliaryPicture type is UNREFOCUS_MAP, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest004, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::UNREFOCUS_MAP;
    Size size = { sizeWidth, sizeHeight };
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::UNREFOCUS_MAP);
}

/**
 * @tc.name: GetTypeTest005
 * @tc.desc: When the auxiliaryPicture type is DEPTH_MAP, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest005, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::DEPTH_MAP;
    Size size = { sizeWidth, sizeHeight };
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::DEPTH_MAP);
}

} // namespace Media
} // namespace OHOS