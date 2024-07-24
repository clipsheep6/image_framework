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
#include "picture.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"
#include "metadata.h"
#include "exif_metadata.h"
#include "fragment_metadata.h"
#include "media_errors.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Multimedia {

class PictureTest : public testing::Test {
public:
    PictureTest() {}
    ~PictureTest() {}
};

static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test_metadata.jpg";
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

/**
 * @tc.name: SurfaceBuffer2PixelMapTest001
 * @tc.desc: Obtain pixelmap through an empty surfaceBuffer.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SurfaceBuffer2PixelMapTest001, TestSize.Level3)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    std::shared_ptr<PixelMap> pixelmap = Picture::SurfaceBuffer2PixelMap(buffer);
    EXPECT_EQ(nullptr, pixelmap);
}

/**
 * @tc.name: SurfaceBuffer2PixelMapTest002
 * @tc.desc: Obtain pixelmap through surfaceBuffer.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SurfaceBuffer2PixelMapTest002, TestSize.Level1)
{
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = SurfaceBuffer::Create();
    std::shared_ptr<PixelMap> pixelmap = Picture::SurfaceBuffer2PixelMap(buffer);
    EXPECT_NE(nullptr, pixelmap);
}

/**
 * @tc.name: SetAuxiliaryPictureTest001
 * @tc.desc: Set gainmap to picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SetAuxiliaryPictureTest001, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpGainmap =
        AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpGainmap, nullptr);
    std::shared_ptr<AuxiliaryPicture> gainmap = std::move(tmpGainmap);
    picture->SetAuxiliaryPicture(gainmap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: SetAuxiliaryPictureTest002
 * @tc.desc: Set depth map to picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SetAuxiliaryPictureTest002, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::DEPTH_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpDepthMap =
        AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpDepthMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> depthMap = std::move(tmpDepthMap);
    picture->SetAuxiliaryPicture(depthMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: SetAuxiliaryPictureTest003
 * @tc.desc: Set unrefocus map to picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SetAuxiliaryPictureTest003, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::UNREFOCUS_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpUnrefocusMap =
        AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpUnrefocusMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> unrefocusMap = std::move(tmpUnrefocusMap);
    picture->SetAuxiliaryPicture(unrefocusMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: SetAuxiliaryPictureTest004
 * @tc.desc: Set linear map to picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SetAuxiliaryPictureTest004, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::LINEAR_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpLinearMap =
        AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpLinearMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> linearMap = std::move(tmpLinearMap);
    picture->SetAuxiliaryPicture(linearMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: SetAuxiliaryPictureTest005
 * @tc.desc: Set fragment map to picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SetAuxiliaryPictureTest005, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::FRAGMENT_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpFragmentMap =
        AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpFragmentMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> fragmentMap = std::move(tmpFragmentMap);
    picture->SetAuxiliaryPicture(fragmentMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: SetAuxiliaryPictureTest006
 * @tc.desc: Set a auxiliary picture which type is AuxiliaryPictureType::NONE to the picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, SetAuxiliaryPictureTest006, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpAuxiliaryPicture =
        AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpAuxiliaryPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> srcAuxiliaryPicture = std::move(tmpAuxiliaryPicture);
    picture->SetAuxiliaryPicture(srcAuxiliaryPicture);
    std::shared_ptr<AuxiliaryPicture> dstAuxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(dstAuxiliaryPicture, nullptr);
    EXPECT_EQ(dstAuxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: GetAuxiliaryPictureTest001
 * @tc.desc: Get gainmap from picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetAuxiliaryPictureTest001, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpGainmap = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpGainmap, nullptr);
    std::shared_ptr<AuxiliaryPicture> gainmap = std::move(tmpGainmap);
    picture->SetAuxiliaryPicture(gainmap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: GetAuxiliaryPictureTest002
 * @tc.desc: Get depth map from picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetAuxiliaryPictureTest002, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::DEPTH_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpDepthMap = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpDepthMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> depthMap = std::move(tmpDepthMap);
    picture->SetAuxiliaryPicture(depthMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: GetAuxiliaryPictureTest003
 * @tc.desc: Get unrefocus map from picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetAuxiliaryPictureTest003, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::UNREFOCUS_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpUnrefocusMap = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpUnrefocusMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> unrefocusMap = std::move(tmpUnrefocusMap);
    picture->SetAuxiliaryPicture(unrefocusMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: GetAuxiliaryPictureTest004
 * @tc.desc: Get linear map from picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetAuxiliaryPictureTest004, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::LINEAR_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpLinearMap = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpLinearMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> linearMap = std::move(tmpLinearMap);
    picture->SetAuxiliaryPicture(linearMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: GetAuxiliaryPictureTest005
 * @tc.desc: Get fragment cut map from picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetAuxiliaryPictureTest005, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::FRAGMENT_MAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpFragmentMap = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpFragmentMap, nullptr);
    std::shared_ptr<AuxiliaryPicture> fragmentMap = std::move(tmpFragmentMap);
    picture->SetAuxiliaryPicture(fragmentMap);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(auxiliaryPicture, nullptr);
    EXPECT_EQ(auxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: GetAuxiliaryPictureTest006
 * @tc.desc: Get auxiliary picture with AuxiliaryPictureType::NONE from picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetAuxiliaryPictureTest006, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::NONE;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpAuxiliaryPicture = AuxiliaryPicture::Create(pixelmap, type, size);
    ASSERT_NE(tmpAuxiliaryPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> srcAuxiliaryPicture = std::move(tmpAuxiliaryPicture);
    picture->SetAuxiliaryPicture(srcAuxiliaryPicture);
    std::shared_ptr<AuxiliaryPicture> dstAuxiliaryPicture = picture->GetAuxiliaryPicture(type);
    ASSERT_NE(dstAuxiliaryPicture, nullptr);
    EXPECT_EQ(srcAuxiliaryPicture, dstAuxiliaryPicture);
    EXPECT_EQ(dstAuxiliaryPicture->GetType(), type);
}

/**
 * @tc.name: GetAuxiliaryPictureTest007
 * @tc.desc: Get gainmap from a picture without gainamap.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, GetAuxiliaryPictureTest007, TestSize.Level1)
{
    const AuxiliaryPictureType type = AuxiliaryPictureType::GAINMAP;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    EXPECT_FALSE(picture->HasAuxiliaryPicture(type));
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture = picture->GetAuxiliaryPicture(type);
    EXPECT_EQ(auxiliaryPicture, nullptr);
}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc: Marshalling picture without auxiliary picture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, MarshallingTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> srcPicture = Picture::Create(pixelmap);
    ASSERT_NE(srcPicture, nullptr);
    Parcel data;
    bool ret = srcPicture->Marshalling(data);
    ASSERT_TRUE(ret);
    auto dstPicture = Picture::Unmarshalling(data);
    ASSERT_NE(dstPicture, nullptr);
    std::shared_ptr<PixelMap> dstMainPixelMap = dstPicture->GetMainPixel();
    ASSERT_NE(dstMainPixelMap, nullptr);
    EXPECT_EQ(dstMainPixelMap->GetWidth(), sizeWidth);
    EXPECT_EQ(dstMainPixelMap->GetHeight(), sizeHeight);
}

/**
 * @tc.name: MarshallingTest002
 * @tc.desc: Marshalling picture which main pixelmap is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, MarshallingTest002, TestSize.Level2)
{
    std::shared_ptr<PixelMap> pixelmap1 = CreatePixelMap();
    ASSERT_NE(pixelmap1, nullptr);
    std::unique_ptr<Picture> srcPicture = Picture::Create(pixelmap1);
    ASSERT_NE(srcPicture, nullptr);
    std::shared_ptr<PixelMap> pixelmap2 = nullptr;
    srcPicture->SetMainPixel(pixelmap2);
    Parcel data;
    bool ret = srcPicture->Marshalling(data);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MarshallingTest003
 * @tc.desc: Marshalling the picture with a auxiliary picture without metadata.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, MarshallingTest003, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> srcPicture = Picture::Create(pixelmap);
    ASSERT_NE(srcPicture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpAuxiliaryPicture =
        AuxiliaryPicture::Create(pixelmap, AuxiliaryPictureType::GAINMAP, size);
    ASSERT_NE(tmpAuxiliaryPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> gainmap = std::move(tmpAuxiliaryPicture);
    srcPicture->SetAuxiliaryPicture(gainmap);
    Parcel data;
    bool ret = srcPicture->Marshalling(data);
    ASSERT_TRUE(ret);
    auto dstPicture = Picture::Unmarshalling(data);
    ASSERT_NE(dstPicture, nullptr);
    std::shared_ptr<PixelMap> dstMainPixelMap = dstPicture->GetMainPixel();
    ASSERT_NE(dstMainPixelMap, nullptr);
    EXPECT_EQ(dstMainPixelMap->GetWidth(), sizeWidth);
    EXPECT_EQ(dstMainPixelMap->GetHeight(), sizeHeight);
    std::shared_ptr<PixelMap> dstGainmapPixelMap = dstPicture->GetGainmapPixelMap();
    ASSERT_NE(dstGainmapPixelMap, nullptr);
    EXPECT_EQ(dstGainmapPixelMap->GetWidth(), sizeWidth);
    EXPECT_EQ(dstGainmapPixelMap->GetHeight(), sizeHeight);
}

/**
 * @tc.name: MarshallingTest004
 * @tc.desc: Marshalling the picture with a auxiliary picture with exif metadata but empty exifdata.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, MarshallingTest004, TestSize.Level2)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> srcPicture = Picture::Create(pixelmap);
    ASSERT_NE(srcPicture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpAuxiliaryPicture =
        AuxiliaryPicture::Create(pixelmap, AuxiliaryPictureType::GAINMAP, size);
    ASSERT_NE(tmpAuxiliaryPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> gainmap = std::move(tmpAuxiliaryPicture);
    std::shared_ptr<ExifMetadata> metadata = std::make_shared<ExifMetadata>();
    gainmap->SetMetadata(MetadataType::EXIF, metadata);
    srcPicture->SetAuxiliaryPicture(gainmap);
    Parcel data;
    bool ret = srcPicture->Marshalling(data);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MarshallingTest005
 * @tc.desc: Marshalling the picture with a auxiliary picture with exif metadata.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, MarshallingTest005, TestSize.Level1)
{
    const std::string srcValue = "9, 9, 8";
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    ASSERT_NE(exifData, nullptr);
    std::shared_ptr<ExifMetadata> srcExifMetadata = std::make_shared<ExifMetadata>(exifData);
    ASSERT_NE(srcExifMetadata, nullptr);
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> srcPicture = Picture::Create(pixelmap);
    ASSERT_NE(srcPicture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpAuxiliaryPicture =
        AuxiliaryPicture::Create(pixelmap, AuxiliaryPictureType::GAINMAP, size);
    ASSERT_NE(tmpAuxiliaryPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> gainmap = std::move(tmpAuxiliaryPicture);
    ASSERT_TRUE(srcExifMetadata->SetValue("BitsPerSample", srcValue));
    gainmap->SetMetadata(MetadataType::EXIF, srcExifMetadata);
    srcPicture->SetAuxiliaryPicture(gainmap);
    Parcel data;
    bool ret = srcPicture->Marshalling(data);
    ASSERT_TRUE(ret);
    Picture *dstPicture = Picture::Unmarshalling(data);
    ASSERT_NE(dstPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture =
        dstPicture->GetAuxiliaryPicture(AuxiliaryPictureType::GAINMAP);
    ASSERT_NE(auxiliaryPicture, nullptr);
    std::shared_ptr<ImageMetadata> dstExifMetadata = auxiliaryPicture->GetMetadata(MetadataType::EXIF);
    ASSERT_NE(dstExifMetadata, nullptr);
    std::string dstValue;
    EXPECT_EQ(dstExifMetadata->GetValue("BitsPerSample", dstValue), SUCCESS);
    EXPECT_EQ(dstValue, srcValue);
}

/**
 * @tc.name: MarshallingTest006
 * @tc.desc: Marshalling the picture with a auxiliary picture with fragment metadata.
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, MarshallingTest006, TestSize.Level1)
{
    const std::string srcValue = "2";
    std::shared_ptr<FragmentMetadata> srcFragmentMetadata = std::make_shared<FragmentMetadata>();
    ASSERT_NE(srcFragmentMetadata, nullptr);
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> srcPicture = Picture::Create(pixelmap);
    ASSERT_NE(srcPicture, nullptr);
    Size size = {sizeWidth, sizeHeight};
    std::unique_ptr<AuxiliaryPicture> tmpAuxiliaryPicture =
        AuxiliaryPicture::Create(pixelmap, AuxiliaryPictureType::GAINMAP, size);
    ASSERT_NE(tmpAuxiliaryPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> gainmap = std::move(tmpAuxiliaryPicture);
    ASSERT_TRUE(srcFragmentMetadata->SetValue("WIDTH", srcValue));
    gainmap->SetMetadata(MetadataType::FRAGMENT, srcFragmentMetadata);
    srcPicture->SetAuxiliaryPicture(gainmap);
    Parcel data;
    bool ret = srcPicture->Marshalling(data);
    ASSERT_TRUE(ret);
    Picture *dstPicture = Picture::Unmarshalling(data);
    ASSERT_NE(dstPicture, nullptr);
    std::shared_ptr<AuxiliaryPicture> auxiliaryPicture =
        dstPicture->GetAuxiliaryPicture(AuxiliaryPictureType::GAINMAP);
    ASSERT_NE(auxiliaryPicture, nullptr);
    std::shared_ptr<ImageMetadata> dstFragmentMetadata = auxiliaryPicture->GetMetadata(MetadataType::FRAGMENT);
    ASSERT_NE(dstFragmentMetadata, nullptr);
    std::string dstValue;
    EXPECT_EQ(dstFragmentMetadata->GetValue("WIDTH", dstValue), SUCCESS);
    EXPECT_EQ(dstValue, srcValue);
}

/**
* @tc.name: CreateTest001
* @tc.desc: Create a Picture using the correct PixelMap.
* @tc.type: FUNC
*/
HWTEST_F(PictureTest, CreateTest001, TestSize.Level1)
{
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    ASSERT_NE(pixelmap, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    EXPECT_NE(picture, nullptr);
}

/**
* @tc.name: CreateTest002
* @tc.desc: Create a Picture using PixelMap with null ptr.
* @tc.type: FUNC
*/
HWTEST_F(PictureTest, CreateTest002, TestSize.Level2)
{
    std::shared_ptr<PixelMap> pixelmap = nullptr;
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    EXPECT_EQ(picture, nullptr);
}

/**
* @tc.name: CreateTest003
* @tc.desc: Create a Picture using the correct SurfaceBuffer.
* @tc.type: FUNC
*/
HWTEST_F(PictureTest, CreateTest003, TestSize.Level1)
{
    OHOS::sptr<OHOS::SurfaceBuffer> surfaceBuffer = OHOS::SurfaceBuffer::Create();
    ASSERT_NE(surfaceBuffer, nullptr);
    std::unique_ptr<Picture> picture = Picture::Create(surfaceBuffer);
    EXPECT_NE(picture, nullptr);
}

/**
* @tc.name: CreateTest004
* @tc.desc: Create a Picture using the SurfaceBuffer with null ptr.
* @tc.type: FUNC
*/
HWTEST_F(PictureTest, CreateTest004, TestSize.Level2)
{
    OHOS::sptr<OHOS::SurfaceBuffer> surfaceBuffer = nullptr;
    std::unique_ptr<Picture> picture = Picture::Create(surfaceBuffer);
    EXPECT_EQ(picture, nullptr);
}

/**
* @tc.name: createPictureFromParcelTest001
* @tc.desc: Create a Picture using the correct Parcel.
* @tc.type: FUNC
*/
HWTEST_F(PictureTest, createPictureFromParcelTest001, TestSize.Level1)
{
    Parcel data;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    ASSERT_NE(picture, nullptr);
    auto ret = picture->Marshalling(data);
    EXPECT_TRUE(ret);
    Picture *newPicture = Picture::Unmarshalling(data);
    EXPECT_EQ(newPicture->GetMainPixel()->GetHeight(), picture->GetMainPixel()->GetHeight());
}

/**
* @tc.name: GetGainmapPixelmapTest001
* @tc.desc: There is an auxiliary map of type GainMap, and the corresponding Pixelmap is obtained.
* @tc.type: FUNC
*/
HWTEST_F(PictureTest, GetGainmapPixelmapTest001, TestSize.Level1)
{
    Size size;
    size.width = sizeWidth;
    size.height = sizeHeight;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    std::unique_ptr<AuxiliaryPicture> auxiliaryPic = AuxiliaryPicture::Create(pixelmap,
        AuxiliaryPictureType::GAINMAP, size);
    std::shared_ptr<AuxiliaryPicture> gainmapAuxiliaryPic = std::move(auxiliaryPic);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    picture->SetAuxiliaryPicture(gainmapAuxiliaryPic);
    auto tempPixelMap = picture->GetGainmapPixelMap();

    EXPECT_NE(tempPixelMap, nullptr);
    EXPECT_EQ(tempPixelMap->GetHeight(), pixelmap->GetHeight());
}

/**
* @tc.name: GetGainmapPixelmapTest002
* @tc.desc: There is no auxiliary map of type gain map, obtain null ptr.
* @tc.type: FUNC
*/
HWTEST_F(PictureTest, GetGainmapPixelmapTest002, TestSize.Level2)
{
    Size size;
    size.width = sizeWidth;
    size.height = sizeHeight;
    std::shared_ptr<PixelMap> pixelmap = CreatePixelMap();
    std::unique_ptr<AuxiliaryPicture> auxiliaryPic = AuxiliaryPicture::Create(pixelmap,
        AuxiliaryPictureType::FRAGMENT_MAP, size);
    std::shared_ptr<AuxiliaryPicture> gainmapAuxiliaryPic = std::move(auxiliaryPic);
    std::unique_ptr<Picture> picture = Picture::Create(pixelmap);
    picture->SetAuxiliaryPicture(gainmapAuxiliaryPic);
    std::shared_ptr<PixelMap> desPixelMap = picture->GetGainmapPixelMap();
    EXPECT_EQ(desPixelMap, nullptr);
}
} // namespace Media
} // namespace OHOS