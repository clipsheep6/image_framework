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
#include "exif_metadata.h"
#include "image_common_impl.h"
#include "metadata.h"
#include "picture_native.h"
#include "pixelmap_native_impl.h"
#include "pixel_map.h"

using namespace testing::ext;
namespace OHOS {
namespace Media {
class PictureNdkTest : public testing::Test {
public:
    PictureNdkTest() {}
    ~PictureNdkTest() {}
};

constexpr int32_t sizeWidth = 820;
constexpr int32_t sizeHeight = 312;
constexpr int32_t bufferLength = 255900;
constexpr int32_t sizeWidthExceed = 20;
constexpr int32_t sizeHeightExceed = 50;
constexpr int32_t bufferLengthExceed = 8;
constexpr int32_t sizeBuffer = 2017220;
constexpr int8_t NUM_0 = 0;

OH_AuxiliaryPictureNative *CreateAuxiliaryPictureNative()
{
    uint32_t color[bufferLength] = {0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08};
    size_t dataLength = bufferLength;
    Image_Size size;
    size.width = sizeWidth;
    size.height = sizeHeight;
    OH_AuxiliaryPictureNative *picture = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);

    return picture;
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest001
 * @tc.desc: Creating OH_AuxiliaPictureNative with all normal parameters.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest001, TestSize.Level1)
{
    uint32_t color[bufferLength] = {0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08};
    size_t dataLength = bufferLength;
    Image_Size size;
    size.width = sizeWidth;
    size.height = sizeHeight;
    OH_AuxiliaryPictureNative *picture = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);

    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::DEPTH_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);

    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::FRAGMENT_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);

    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::LINEAR_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);

    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::UNREFOCUS_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest002
 * @tc.desc: Create OH_AuxiliaryPictureNative, pass null parameter, return IMAGE_BAD_PARAMETER.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest002, TestSize.Level2)
{
    uint8_t *color = nullptr;
    size_t dataLength = bufferLength;
    Image_Size size;
    size.width = sizeWidth;
    size.height = sizeHeight;
    OH_AuxiliaryPictureNative *picture = nullptr;
    ::AuxiliaryPictureType type = ::AuxiliaryPictureType::GAINMAP;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(color, dataLength, &size, type, &picture);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest003
 * @tc.desc: Create OH_AuxiliaryPictureNative, pass in error parameters, return IMAGE_ALLOC_FAILED.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest003, TestSize.Level2)
{
    uint32_t color[bufferLengthExceed] = {0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08};
    size_t dataLength = bufferLengthExceed;
    Image_Size size;
    size.width = sizeWidthExceed;
    size.height = sizeHeightExceed;
    OH_AuxiliaryPictureNative *picture = nullptr;
    ::AuxiliaryPictureType type = ::AuxiliaryPictureType::GAINMAP;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color),
        dataLength, &size, type, &picture);
    EXPECT_EQ(ret, IMAGE_UNKNOWN_ERROR);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest004
 * @tc.desc: Pass in a non-existent AuxiliaryPictureType and return an exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest004, TestSize.Level2)
{
    uint32_t color[bufferLength] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    size_t dataLength = bufferLength;
    Image_Size size;
    size.width = sizeWidth;
    size.height = sizeHeight;
    OH_AuxiliaryPictureNative *picture = nullptr;
    ::AuxiliaryPictureType type = static_cast<::AuxiliaryPictureType>(-1);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color),
        dataLength, &size, type, &picture);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_WritePixelsTest001
 * @tc.desc: Pass in the correct parameters to WritePixels and return success.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_WritePixelsTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    size_t size = sizeBuffer;
    std::unique_ptr<uint8_t[]> source = std::make_unique<uint8_t[]>(size);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_WritePixels(picture, source.get(), size);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_WritePixelsTest002
 * @tc.desc: Passing an exception buff to WritePixels returns an exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_WritePixelsTest002, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    size_t size = bufferLengthExceed;
    std::unique_ptr<uint8_t[]> source = std::make_unique<uint8_t[]>(size);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_WritePixels(picture, source.get(), size);
    EXPECT_EQ(ret, IMAGE_UNKNOWN_ERROR);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_WritePixelsTest003
 * @tc.desc: Passing an empty parameter to WritePixels and returning an exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_WritePixelsTest003, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    uint8_t *source = nullptr;
    size_t bufferSize = NUM_0;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_WritePixels(picture, source, bufferSize);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_ReadPixelsTest001
 * @tc.desc: Passing a normal buff to ReadPixels returns success.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_ReadPixelsTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    size_t size = sizeBuffer;
    std::unique_ptr<uint8_t[]> destination = std::make_unique<uint8_t[]>(size);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_ReadPixels(picture, destination.get(), &size);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_ReadPixelsTest002
 * @tc.desc: Passing an exception buff to ReadPixels returns an exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_ReadPixelsTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    size_t size = bufferLengthExceed;
    std::unique_ptr<uint8_t[]> destination = std::make_unique<uint8_t[]>(size);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_ReadPixels(picture, destination.get(), &size);
    EXPECT_EQ(ret, IMAGE_UNKNOWN_ERROR);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_ReadPixelsTest003
 * @tc.desc: Pass an empty parameter to ReadPixels and return an exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_ReadPixelsTest003, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    uint8_t *destination = nullptr;
    size_t *bufferSize = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_ReadPixels(picture, destination, bufferSize);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetTypeTest001
 * @tc.desc: The input auxiliary image is GAINMAP, and the returned type is GAINMAP.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetTypeTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    ::AuxiliaryPictureType type;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetType(picture, &type);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    EXPECT_EQ(type, ::AuxiliaryPictureType::GAINMAP);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetTypeTest002
 * @tc.desc: Pass in an empty parameter and return an empty type.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetTypeTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    ::AuxiliaryPictureType *typeptr = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetType(picture, typeptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
    EXPECT_EQ(typeptr, nullptr);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetInfoTest001
 * @tc.desc: Pass in the correct parameters and compare the type of info with the original type of AuxiliaryPicture.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetInfoTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    OH_AuxiliaryPictureInfo *infoptr = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetInfo(picture, &infoptr);
    EXPECT_EQ(ret, IMAGE_SUCCESS);

    ::AuxiliaryPictureType type;
    ret = OH_AuxiliaryPictureInfo_GetType(infoptr, &type);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    EXPECT_EQ(type, ::AuxiliaryPictureType::GAINMAP);

    OH_AuxiliaryPictureInfo_Release(infoptr);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetInfoTest002
 * @tc.desc: Pass in incorrect parameter, get empty.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetInfoTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_AuxiliaryPictureInfo *infoptr = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetInfo(picture, &infoptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
    EXPECT_EQ(infoptr, nullptr);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetInfoTest001
 * @tc.desc: Passing in the correct parameter settings info returned a success message.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetInfoTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    OH_AuxiliaryPictureInfo *infoptr = nullptr;
    OH_AuxiliaryPictureInfo_Create(&infoptr);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_SetInfo(picture, infoptr);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetInfoTest002
 * @tc.desc: Passing empty parameter setting info returns an exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetInfoTest002, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_AuxiliaryPictureInfo *infoptr = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_SetInfo(picture, infoptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetMetadataTest001
 * @tc.desc: Pass in normal parameters to obtain Metadata.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetMetadataTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    OH_PictureMetadata *metadataptr = nullptr;
    ::MetadataType type = ::MetadataType::EXIF_METADATA;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetMetadata(picture, type, &metadataptr);
    EXPECT_EQ(ret, IMAGE_SUCCESS);

    OH_PictureMetadata_Release(metadataptr);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetMetadataTest002
 * @tc.desc: Pass in a non-existent Metadata Type and return an exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetMetadataTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();
    OH_PictureMetadata *metadataptr = nullptr;
    ::MetadataType type = static_cast<::MetadataType>(9);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetMetadata(picture, type, &metadataptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
    EXPECT_EQ(metadataptr, nullptr);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetMetadataTest003
 * @tc.desc: Pass in an empty parameter and return a null pointer.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetMetadataTest003, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_PictureMetadata *metadataptr = nullptr;
    ::MetadataType type = ::MetadataType::EXIF_METADATA;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetMetadata(picture, type, &metadataptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
    EXPECT_EQ(metadataptr, nullptr);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetMetadataTest001
 * @tc.desc: Pass in the normal parameter SetMetadata and return success.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetMetadataTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = CreateAuxiliaryPictureNative();

    std::shared_ptr<OHOS::Media::ImageMetadata> metadata = std::make_shared<OHOS::Media::ExifMetadata>();
    auto exifMetadata = static_cast<OHOS::Media::ExifMetadata *>(metadata.get());
    exifMetadata->CreateExifdata();
    std::unique_ptr<OH_PictureMetadata> metadataptr = std::make_unique<OH_PictureMetadata>(metadata);

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_SetMetadata(picture,
        ::MetadataType::EXIF_METADATA, metadataptr.get());
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    OH_AuxiliaryPictureNative_Release(picture);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetMetadataTest002
 * @tc.desc: Pass in empty parameter SetMetadata, return exception.
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetMetadataTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_PictureMetadata *metadata = nullptr;

    Image_ErrorCode ret = OH_AuxiliaryPictureNative_SetMetadata(picture, ::MetadataType::FRAGMENT_METADATA, metadata);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

} // namespace Media
} // namespace OHOS