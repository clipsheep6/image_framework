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
#include <surface.h>
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

constexpr int32_t infoRowstride = 1;
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
 * @tc.name: CreateTest001
 * @tc.desc: Create an auxiliaryPicture using pixelmap, gain map type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {sizeWidth, sizeWidth};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_NE(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest002
 * @tc.desc: Create an auxiliaryPicture using an empty pixelmap, none type, and zero size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest002, TestSize.Level3)
{
    std::shared_ptr<PixelMap> pixelmap = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    Size size = {0, 0};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_EQ(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest003
 * @tc.desc: Create an auxiliaryPicture using an empty pixelmap, gain map type, and zero size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest003, TestSize.Level3)
{
    std::shared_ptr<PixelMap> pixelmap = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {0, 0};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_EQ(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest004
 * @tc.desc: Create an auxiliaryPicture using an empty pixelmap, none type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest004, TestSize.Level3)
{
    std::shared_ptr<PixelMap> pixelmap = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_EQ(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest005
 * @tc.desc: Create an auxiliaryPicture using an empty pixelmap, gain map type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest005, TestSize.Level3)
{
    std::shared_ptr<PixelMap> pixelmap = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_EQ(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest006
 * @tc.desc: Create an auxiliaryPicture using pixelmap, none type, and zero size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest006, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    Size size = {0, 0};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_NE(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest007
 * @tc.desc: Create an auxiliaryPicture using pixelmap, gain map type, and zero size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest007, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {0, 0};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_NE(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest008
 * @tc.desc: Create an auxiliaryPicture using pixelmap, none type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest008, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    EXPECT_NE(auxPicture, nullptr);
}

/**
 * @tc.name: CreateTest009
 * @tc.desc: Create an auxiliaryPicture using surfaceBuffer, gain map type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest009, TestSize.Level1)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = SurfaceBuffer::Create();
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {sizeWidth, sizeHeight};
    auto auxPicture = AuxiliaryPicture::Create(buffer, type, size);
    EXPECT_NE(nullptr, auxPicture);
}

/**
 * @tc.name: CreateTest010
 * @tc.desc: Create an auxiliaryPicture using an empty surfaceBuffer, none type, and zero size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest010, TestSize.Level3)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    Size size = {0, 0};
    auto auxPicture = AuxiliaryPicture::Create(buffer, type, size);
    EXPECT_EQ(nullptr, auxPicture);
}

/**
 * @tc.name: CreateTest011
 * @tc.desc: Create an auxiliaryPicture using an empty surfaceBuffer, gain map type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest011, TestSize.Level3)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {sizeWidth, sizeHeight};
    auto auxPicture = AuxiliaryPicture::Create(buffer, type, size);
    EXPECT_EQ(nullptr, auxPicture);
}

/**
 * @tc.name: CreateTest012
 * @tc.desc: Create an auxiliaryPicture using an empty surfaceBuffer, gain map type, and zero size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest012, TestSize.Level3)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {0, 0};
    auto auxPicture = AuxiliaryPicture::Create(buffer, type, size);
    EXPECT_EQ(nullptr, auxPicture);
}

/**
 * @tc.name: CreateTest013
 * @tc.desc: Create an auxiliaryPicture using a surfaceBuffer, none type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest013, TestSize.Level3)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    Size size = {sizeWidth, sizeHeight};
    auto auxPicture = AuxiliaryPicture::Create(buffer, type, size);
    EXPECT_EQ(nullptr, auxPicture);
}

/**
 * @tc.name: CreateTest014
 * @tc.desc: Create an auxiliaryPicture using surfaceBuffer, none type, and normal size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest014, TestSize.Level1)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = SurfaceBuffer::Create();
    AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    Size size = {sizeWidth, sizeHeight};
    auto auxPicture = AuxiliaryPicture::Create(buffer, type, size);
    EXPECT_NE(nullptr, auxPicture);
}

/**
 * @tc.name: CreateTest015
 * @tc.desc: Create an auxiliaryPicture using surfaceBuffer, gain map type, and zero size.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, CreateTest015, TestSize.Level1)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = SurfaceBuffer::Create();
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {0, 0};
    auto auxPicture = AuxiliaryPicture::Create(buffer, type, size);
    EXPECT_NE(nullptr, auxPicture);
}

/**
 * @tc.name: GetAuxiliaryPictureInfoTest001
 * @tc.desc: Get auxiliaryPictureInfo of the auxiliaryPicture.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetAuxiliaryPictureInfoTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::DEPTH_MAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    EXPECT_EQ(AuxiliaryPictureType::DEPTH_MAP, auxPicture->GetAuxiliaryPictureInfo().auxiliaryPictureType);
    EXPECT_EQ(sizeWidth, auxPicture->GetAuxiliaryPictureInfo().size.width);
    EXPECT_EQ(sizeHeight, auxPicture->GetAuxiliaryPictureInfo().size.height);
}

/**
 * @tc.name: SetAuxiliaryPictureInfoTest001
 * @tc.desc: Set auxiliaryPictureInfo to the auxiliaryPicture.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, SetAuxiliaryPictureInfoTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::DEPTH_MAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureInfo auxiliaryPictureInfo;
    auxiliaryPictureInfo.auxiliaryPictureType = AuxiliaryPictureType::GAINMAP;
    auxiliaryPictureInfo.colorSpace = ColorSpace::SRGB;
    auxiliaryPictureInfo.pixelFormat = PixelFormat::RGBA_8888;
    auxiliaryPictureInfo.rowStride = infoRowstride;
    auxiliaryPictureInfo.size = {sizeWidth, sizeHeight};
    auxPicture->SetAuxiliaryPictureInfo(auxiliaryPictureInfo);
    AuxiliaryPictureInfo info = auxPicture->GetAuxiliaryPictureInfo();
    EXPECT_EQ(AuxiliaryPictureType::GAINMAP, info.auxiliaryPictureType);
    EXPECT_EQ(ColorSpace::SRGB, info.colorSpace);
    EXPECT_EQ(PixelFormat::RGBA_8888, info.pixelFormat);
    EXPECT_EQ(infoRowstride, info.rowStride);
    EXPECT_EQ(sizeWidth, info.size.width);
    EXPECT_EQ(sizeHeight, info.size.height);
}

/**
 * @tc.name: GetTypeTest001
 * @tc.desc: When the auxiliaryPicture type is gain map, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::GAINMAP);
}

/**
 * @tc.name: GetTypeTest002
 * @tc.desc: When the auxiliaryPicture type is linear map, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest002, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::LINEAR_MAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::LINEAR_MAP);
}

/**
 * @tc.name: GetTypeTest003
 * @tc.desc: When the auxiliaryPicture type is fragment map, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest003, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::FRAGMENT_MAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::FRAGMENT_MAP);
}

/**
 * @tc.name: GetTypeTest004
 * @tc.desc: When the auxiliaryPicture type is unrefocus map, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest004, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::UNREFOCUS_MAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::UNREFOCUS_MAP);
}

/**
 * @tc.name: GetTypeTest005
 * @tc.desc: When the auxiliaryPicture type is depth map, obtain the type.
 * @tc.type: FUNC
 */
HWTEST_F(AuxiliaryPictureTest, GetTypeTest005, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    AuxiliaryPictureType type = AuxiliaryPictureType::DEPTH_MAP;
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> auxPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(auxPicture, nullptr);
    AuxiliaryPictureType auxType = auxPicture->GetType();
    EXPECT_EQ(auxType, AuxiliaryPictureType::DEPTH_MAP);
}
} // namespace Media
} // namespace OHOS