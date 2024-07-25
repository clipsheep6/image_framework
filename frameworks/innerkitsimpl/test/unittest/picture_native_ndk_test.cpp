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

using namespace testing::ext;
// using namespace OHOS::Media;
namespace OHOS {
namespace Media {
class PictureNdkTest : public testing::Test {
public:
    PictureNdkTest() {}
    ~PictureNdkTest() {}
};

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest001, TestSize.Level1)
{
    const uint32_t color[8] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    size_t dataLength = 8;
    Image_Size *size;
    size->height = 5;
    size->width = 6;
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(coreinterpret_cast<uint8_t*>(color), dataLength, size,
        AuxiliaryPictureType::GAINMAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(coreinterpret_cast<uint8_t*>(color), dataLength, size,
        AuxiliaryPictureType::DEPTH_MAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(coreinterpret_cast<uint8_t*>(color), dataLength, size,
        AuxiliaryPictureType::FRAGMENT_MAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(coreinterpret_cast<uint8_t*>(color), dataLength, size,
        AuxiliaryPictureType::LINEAR_MAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(coreinterpret_cast<uint8_t*>(color), dataLength, size,
        AuxiliaryPictureType::NONE, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(coreinterpret_cast<uint8_t*>(color), dataLength, size,
        AuxiliaryPictureType::UNREFOCUS_MAP, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_CreateTest002
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_CreateTest002, TestSize.Level2)
{
    const uint8_t color = nullptr;
    size_t dataLength = 0;
    Image_Size *size;
    size->height = 5;
    size->width = 6;
    OH_AuxiliaryPictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_Create(color, dataLength, size,
        AuxiliaryPictureType::GAINMAP, &picture);
    EXPECT_EQ(ret, IMAGE_ALLOC_FAILED);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_WritePixelsTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_WritePixelsTest001, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    uint8_t *source = nullptr;
    size_t bufferSize = 0;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_WritePixels(picture, source, bufferSize);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_ReadPixelsTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_ReadPixelsTest001, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    uint8_t *destination = nullptr;
    size_t bufferSize = 0;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_WritePixels(picture, source, bufferSize);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetTypeTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetTypeTest001, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    AuxiliaryPictureType type;
    AuxiliaryPictureType* typeptr = &type;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetType(picture, typeptr);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_GetInfoTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_GetInfoTest001, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    std::shared_ptr<OH_AuxiliaryPictureInfo> info = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_GetInfo(picture, &(info.get()));
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetInfoTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetInfoTest001, TestSize.Level3)
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
    OH_AuxiliaryPictureNative *picture = nullptr;
    std::shared_ptr<OH_PictureMetadata> metadata;
    Image_ErrorCode ret = (picture, MetadataType::EXIF, &(metadata.get()));
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_AuxiliaryPictureNative_SetMetadataTest001
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_AuxiliaryPictureNative_SetMetadataTest001, TestSize.Level3)
{
    OH_AuxiliaryPictureNative *picture = nullptr;
    OH_PictureMetadata* metadata = nullptr;
    Image_ErrorCode ret = OH_AuxiliaryPictureNative_SetMetadata(picture, MetadataType::FRAGMENT, metadata);
    EXPECT_EQ(ret, IMAGE_BAD_PARAMETER);
}

} // namespace Media
} // namespace OHOS