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
#include "picture_native.h"
#include "pixelmap_native_impl.h"
#include "image_type.h"
#include "pixel_map.h"

using namespace testing::ext;
// using namespace OHOS::Media;
namespace OHOS {
namespace Media {
class PictureNdkTest : public testing::Test {
public:
    PictureNdkTest() {}
    ~PictureNdkTest() {}
};

constexpr int32_t sizeWidthTrue = 820;
constexpr int32_t sizeHeightTrue = 312;
constexpr int32_t bufferLengthTrue = 255900;
constexpr int32_t sizeWidthFalse = 20;
constexpr int32_t sizeHeightFalse = 50;
constexpr int32_t bufferLengthFalse = 8;
constexpr int8_t NUM_0 = 0;
constexpr int8_t NUM_2 = 2;

Image_ErrorCode CreateAuxiliaryPictureNative(OH_AuxiliaryPictureNative **picture)
{
    uint32_t color[bufferLengthTrue] = {0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08};
    size_t dataLength = bufferLengthTrue;
    Image_Size size;
    size.width = sizeWidthTrue;
    size.height = sizeHeightTrue;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, picture);
    return ret;
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest001, TestSize.Level1)
{
    uint32_t color[bufferLengthTrue] = {0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08};
    size_t dataLength = bufferLengthTrue;
    Image_Size size;
    size.width = sizeWidthTrue;
    size.height = sizeHeightTrue;
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::DEPTH_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::FRAGMENT_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::LINEAR_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
    ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::UNREFOCUS_MAP, &picture);
    EXPECT_EQ(ret, ::IMAGE_SUCCESS);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest002
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest002, TestSize.Level2)
{
    uint8_t *color = nullptr;
    size_t dataLength = bufferLengthTrue;
    Image_Size size;
    size.width = sizeWidthTrue;
    size.height = sizeHeightTrue;
    OH_AuxiliaryPictureNative *picture = nullptr;
    ::AuxiliaryPictureType type = ::AuxiliaryPictureType::GAINMAP;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(color, dataLength, &size, type, &picture);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest003
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest003, TestSize.Level2)
{
    uint32_t color[bufferLengthFalse] = {0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08};
    size_t dataLength = bufferLengthFalse;
    Image_Size size;
    size.width = sizeWidthFalse;
    size.height = sizeHeightFalse;
    OH_AuxiliaryPictureNative *picture = nullptr;
    ::AuxiliaryPictureType type = ::AuxiliaryPictureType::GAINMAP;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size, type, &picture);
    EXPECT_EQ(ret, IMAGE_ALLOC_FAILED);
}

// /**
//  * @tc.name: OH_AuxiliaryPictureNative_CreateTest004
//  * @tc.desc: 
//  * @tc.type: FUNC
//  */
// HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest004, TestSize.Level2)
// {
//     uint32_t color[bufferLengthTrue] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
//     size_t dataLength = bufferLengthTrue;
//     Image_Size size;
//     size.width = sizeWidthTrue;
//     size.height = sizeHeightTrue;
//     OH_AuxiliaryPictureNative *picture = nullptr;
//     // ::AuxiliaryPictureType type = static_cast<::AuxiliaryPictureType>(8);
//     Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size, 8, &picture);
//     EXPECT_EQ(ret, IMAGE_ALLOC_FAILED);
// }

/**
 * @tc.name: OH_AuxiliaryPictureNative_WritePixelsTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_WritePixelsTest001, TestSize.Level1)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = CreateAuxiliaryPictureNative(&picture);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    uint8_t source[NUM_2] = {};
    size_t bufferSize = NUM_2;
    ret = OH_AuxiliaryPictureNative_WritePixels(picture, source, bufferSize);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_WritePixelsTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_WritePixelsTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    uint8_t *source = nullptr;
    size_t bufferSize = NUM_0;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_WritePixels(picture, source, bufferSize);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_ReadPixelsTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
// HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_ReadPixelsTest001, TestSize.Level1)
// {
//     uint32_t color[bufferLengthTrue] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
//     size_t dataLength = bufferLengthTrue;
//     Image_Size size;
//     size.width = sizeWidthTrue;
//     size.height = sizeHeightTrue;
//     // OHOS::Media::InitializationOptions options;
//     // options.size.height = sizeHeightTrue;
//     // options.size.width = sizeWidthTrue;
//     // auto dataLengthTmp = static_cast<uint32_t>(dataLength);
//     // auto pixelmap = OHOS::Media::PixelMap::Create(color, dataLengthTmp, options);
//     OHOS::Media::InitializationOptions initializationOptions;
//     initializationOptions.size.width = sizeWidthTrue;
//     initializationOptions.size.height = sizeHeightTrue;
//     auto dataTmp = reinterpret_cast<uint32_t*>(color);
//     auto dataLengthTmp = static_cast<uint32_t>(dataLength);

//     auto pixelMap = OHOS::Media::PixelMap::Create(dataTmp, dataLengthTmp, initializationOptions);
//     OH_AuxiliaryPictureNative *picture = nullptr;
//     Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
//         ::AuxiliaryPictureType::GAINMAP, &picture);
//     ASSERT_EQ(ret, IMAGE_SUCCESS);
//     size_t buff = pixelMap->GetCapacity();
//     uint8_t *destination = new uint8_t[buff];
//     size_t *bufferSize = &buff;
//     ret = OH_AuxiliaryPictureNative_ReadPixels(picture, destination, bufferSize);
//     EXPECT_EQ(ret, IMAGE_SUCCESS);
//     delete []destination;
// }

/**
 * @tc.name: OH_AuxiliaryPictureNative_ReadPixelsTest002
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_ReadPixelsTest002, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    uint8_t *destination = nullptr;
    size_t *bufferSize = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_ReadPixels(picture, destination, bufferSize);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetTypeTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetTypeTest001, TestSize.Level1)
{
    uint32_t color[bufferLengthTrue] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    size_t dataLength = bufferLengthTrue;
    Image_Size size;
    size.width = sizeWidthTrue;
    size.height = sizeHeightTrue;
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, &picture);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    ::AuxiliaryPictureType type = ::AuxiliaryPictureType::GAINMAP;
    ::AuxiliaryPictureType* typeptr = &type;
    ret = OH_AuxiliaryPictureNative_GetType(picture, typeptr);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetTypeTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetTypeTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    ::AuxiliaryPictureType* typeptr = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetType(picture, typeptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetInfoTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetInfoTest001, TestSize.Level1)
{
    uint32_t color[bufferLengthTrue] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    size_t dataLength = bufferLengthTrue;
    Image_Size size;
    size.width = sizeWidthTrue;
    size.height = sizeHeightTrue;
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, &picture);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    OH_AuxiliaryPictureInfo* infoptr = nullptr;
    ret = OH_AuxiliaryPictureNative_GetInfo(picture, &infoptr);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    //delete infoptr;
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetInfoTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetInfoTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_AuxiliaryPictureInfo* infoptr = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetInfo(picture, &infoptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
    //delete infoptr;
}

// /**
//  * @tc.name: OH_AuxiliaryPictureNative_SetInfoTest001
//  * @tc.desc: 
//  * @tc.type: FUNC
//  */
// HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetInfoTest001, TestSize.Level1)
// {
//     uint32_t color[bufferLengthTrue] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
//     size_t dataLength = bufferLengthTrue;
//     Image_Size size;
//     size.width = sizeWidthTrue;
//     size.height = sizeHeightTrue;
//     OH_AuxiliaryPictureNative *picture = nullptr;
//     Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
//         ::AuxiliaryPictureType::GAINMAP, &picture);
//     ASSERT_EQ(ret, IMAGE_SUCCESS);
//     auto p = picture->GetInnerAuxiliaryPicture()->GetAuxiliaryPictureInfo();
//     OH_AuxiliaryPictureInfo info(p);
//     OH_AuxiliaryPictureInfo* infoptr = &info;
//     ret = OH_AuxiliaryPictureNative_SetInfo(picture, info);
//     EXPECT_EQ(ret, IMAGE_SUCCESS);
// }

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetInfoTest002
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetInfoTest002, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_AuxiliaryPictureInfo *info = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_SetInfo(picture, info);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetMetadataTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetMetadataTest001, TestSize.Level3)
{
    uint32_t color[bufferLengthTrue] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    size_t dataLength = bufferLengthTrue;
    Image_Size size;
    size.width = sizeWidthTrue;
    size.height = sizeHeightTrue;
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, &picture);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    OH_PictureMetadata *metadataptr = nullptr;
    ::MetadataType type = ::MetadataType::EXIF_METADATA;
    ret = OH_AuxiliaryPictureNative_GetMetadata(picture, type, &metadataptr);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    //delete metadataptr;
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetMetadataTest002
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetMetadataTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_PictureMetadata *metadataptr = nullptr;
    ::MetadataType type = ::MetadataType::EXIF_METADATA;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetMetadata(picture, type, &metadataptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
    //delete metadataptr;
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetMetadataTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetMetadataTest001, TestSize.Level1)
{
    uint32_t color[bufferLengthTrue] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    size_t dataLength = bufferLengthTrue;
    Image_Size size;
    size.width = sizeWidthTrue;
    size.height = sizeHeightTrue;
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(reinterpret_cast<uint8_t*>(color), dataLength, &size,
        ::AuxiliaryPictureType::GAINMAP, &picture);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    OH_PictureMetadata *metadataptr = nullptr;
    ::MetadataType type = ::MetadataType::EXIF_METADATA;
    auto metadataPtr = picture->GetInnerAuxiliaryPicture()->GetMetadata(type);
    // ret = OH_AuxiliaryPictureNative_GetMetadata(picture, type, &metadataptr);
    // EXPECT_EQ(ret, IMAGE_SUCCESS);
    ret = OH_AuxiliaryPictureNative_SetMetadata(picture, ::MetadataType::EXIF_METADATA, metadataptr);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
}


/**
 * @tc.name: OH_AuxiliaryPictureNative_SetMetadataTest002
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetMetadataTest002, TestSize.Level2)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_PictureMetadata* metadata = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_SetMetadata(picture, ::MetadataType::FRAGMENT_METADATA, metadata);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

} // namespace Media
} // namespace OHOS