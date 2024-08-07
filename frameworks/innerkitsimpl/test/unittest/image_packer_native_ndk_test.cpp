/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "image_packer_native.h"
#include "image_packer_native_impl.h"
#include "file_packer_stream.h"
#include <fcntl.h>

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Multimedia {
class ImagePackerNdk2Test : public testing::Test {
public:
    ImagePackerNdk2Test() {}
    ~ImagePackerNdk2Test() {}
};

static const std::string IMAGE_FD_PATH = "/data/local/tmp/image/test";
static constexpr int32_t BUFFER_LENGTH = 8;
static OH_PictureNative* CreatePictureNative()
{
    uint8_t color[BUFFER_LENGTH] = {0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08};
    size_t dataLength = BUFFER_LENGTH;
    OH_ImageSourceNative *source = nullptr;
    OH_DecodingOptionsForPicture *options = nullptr;
    OH_PictureNative *picture = nullptr;
    Image_ErrorCode ret;
    ret = OH_ImageSourceNative_CreateFromData(color, dataLength, &source);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    ret = OH_DecodingOptionsForPicture_Create(&options);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    ret = OH_ImageSourceNative_CreatePicture(source, options, &picture);
    EXPECT_EQ(ret, IMAGE_SUCCESS);
    return picture;
}

/**
 * @tc.name: OH_ImageSourceInfo_Create
 * @tc.desc: test OH_ImageSourceInfo_Create
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_PackingOptions_Create, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImageSourceInfo_Create start";
    OH_PackingOptions *ops = nullptr;
    Image_ErrorCode ret = OH_PackingOptions_Create(&ops);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    OH_PackingOptions_Release(ops);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImageSourceInfo_Create end";
}

/**
 * @tc.name: OH_PackingOptions_SetGetMimeType
 * @tc.desc: test OH_PackingOptions_SetGetMimeType
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_PackingOptions_SetGetMimeType, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_PackingOptions_SetGetMimeType start";
    OH_PackingOptions *ops = nullptr;
    char str[10] = "";
    char str2[10] = "12";
    Image_MimeType *mimeType = new Image_MimeType();
    mimeType->data = str;
    mimeType->size = 0;
    Image_MimeType *mimeType2 = new Image_MimeType();
    mimeType2->data = str2;
    mimeType2->size = 2;
    Image_ErrorCode ret = OH_PackingOptions_Create(&ops);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    OH_PackingOptions_SetMimeType(ops, mimeType2);
    OH_PackingOptions_GetMimeType(ops, mimeType);
    ASSERT_EQ(mimeType->size, 2);
    string res(mimeType->data, mimeType->size);
    ASSERT_EQ(res, "12");
    OH_PackingOptions_Release(ops);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_PackingOptions_SetGetMimeType end";
}

/**
 * @tc.name: OH_PackingOptions_Release
 * @tc.desc: test OH_PackingOptions_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_PackingOptions_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_PackingOptions_Release start";
    OH_PackingOptions *ops = nullptr;
    Image_ErrorCode ret = OH_PackingOptions_Release(ops);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_PackingOptions_Release end";
}

/**
 * @tc.name: OH_ImagePackerNative_Create
 * @tc.desc: test OH_ImagePackerNative_Create
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_Create, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_Create start";
    OH_ImagePackerNative *imagePacker = nullptr;
    Image_ErrorCode ret = OH_ImagePackerNative_Create(&imagePacker);
    ASSERT_EQ(ret, IMAGE_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_Create end";
}

/**
 * @tc.name: OH_ImagePackerNative_PackToDataFromImageSource
 * @tc.desc: test OH_ImagePackerNative_PackToDataFromImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToDataFromImageSource, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToDataFromImageSource start";
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions* option = nullptr;
    OH_ImageSourceNative* imageSource = nullptr;
    uint8_t* outData = nullptr;
    size_t *size = 0;
    Image_ErrorCode ret = OH_ImagePackerNative_PackToDataFromImageSource(imagePacker, option, imageSource,
        outData, size);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToDataFromImageSource end";
}

/**
 * @tc.name: OH_ImagePackerNative_PackToDataFromPixelmap
 * @tc.desc: test OH_ImagePackerNative_PackToDataFromPixelmap
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToDataFromPixelmap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToDataFromPixelmap start";
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions* option = nullptr;
    OH_PixelmapNative* pixelMap = nullptr;
    uint8_t* outData = nullptr;
    size_t *size = 0;
    Image_ErrorCode ret = OH_ImagePackerNative_PackToDataFromPixelmap(imagePacker, option, pixelMap, outData, size);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToDataFromPixelmap end";
}

/**
 * @tc.name: OH_ImagePacker_PackToDataMultiFrames
 * @tc.desc: test OH_ImagePacker_PackToDataMultiFrames
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker_PackToDataMultiFrames, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker_PackToDataMultiFrames start";
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions* option = nullptr;
    OH_PixelmapNative **pixelMaps = nullptr;
    uint8_t* outData = nullptr;
    size_t *outDataSize = 0;
    Image_ErrorCode ret = OH_ImagePacker_PackToDataMultiFrames(imagePacker, option,
        pixelMaps, 0, outData, outDataSize);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker_PackToDataMultiFrames end";
}

/**
 * @tc.name: OH_ImagePackerNative_PackToFileFromImageSource
 * @tc.desc: test OH_ImagePackerNative_PackToFileFromImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToFileFromImageSource, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToFileFromImageSource start";
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions* option = nullptr;
    OH_ImageSourceNative* imageSource = nullptr;
    int32_t fd = 0;
    Image_ErrorCode ret = OH_ImagePackerNative_PackToFileFromImageSource(imagePacker, option, imageSource, fd);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToFileFromImageSource end";
}

/**
 * @tc.name: OH_ImagePackerNative_PackToFileFromPixelmap
 * @tc.desc: test OH_ImagePackerNative_PackToFileFromPixelmap
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToFileFromPixelmap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToFileFromPixelmap start";
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions* option = nullptr;
    OH_PixelmapNative* pixelMap = nullptr;
    int32_t fd = 0;
    Image_ErrorCode ret = OH_ImagePackerNative_PackToFileFromPixelmap(imagePacker, option, pixelMap, fd);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_PackToFileFromPixelmap end";
}

/**
 * @tc.name: OH_ImagePacker_PackToFileMultiFrames
 * @tc.desc: test OH_ImagePacker_PackToFileMultiFrames
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker_PackToFileMultiFrames, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker_PackToFileMultiFrames start";
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions* option = nullptr;
    OH_PixelmapNative **pixelMaps = nullptr;
    int32_t fd = 0;
    Image_ErrorCode ret = OH_ImagePacker_PackToFileMultiFrames(imagePacker, option, pixelMaps, 0, fd);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker_PackToFileMultiFrames end";
}

/**
 * @tc.name: OH_ImagePackerNative_Release
 * @tc.desc: test OH_ImagePackerNative_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_Release start";
    OH_ImagePackerNative *imagePacker = nullptr;
    Image_ErrorCode ret = OH_ImagePackerNative_Release(imagePacker);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePackerNative_Release end";
}

/**
 * @tc.name: OH_ImagePackerNative_PackToDataFromPictureTest001
 * @tc.desc: Test OH_ImagePackerNative_PackToDataFromPicture with null pointers.
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToDataFromPictureTest001, TestSize.Level3)
{
    Image_ErrorCode res = OH_ImagePackerNative_PackToDataFromPicture(nullptr, nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(res, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_ImagePackerNative_PackToDataFromPictureTest002
 * @tc.desc: Tests packing a picture into data using OH_ImagePackerNative.
 *           The test checks if the packer and options are created, and if the picture is packed successfully.
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToDataFromPictureTest002, TestSize.Level1)
{
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions *options = nullptr;
    OH_PictureNative *picture = nullptr;
    uint8_t outData[BUFFER_LENGTH] = {0};
    size_t size = BUFFER_LENGTH;
    Image_ErrorCode errCode = OH_PackingOptions_Create(&options);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    ASSERT_NE(options, nullptr);

    errCode = OH_ImagePackerNative_Create(&imagePacker);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    ASSERT_NE(imagePacker, nullptr);

    Image_MimeType format;
    char formatStr[] = "image/jpeg";
    format.size = strlen(formatStr) + 1;
    format.data = formatStr;
    errCode = OH_PackingOptions_SetMimeType(options, &format);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);

    picture = CreatePictureNative();
    EXPECT_NE(picture, nullptr);
    errCode = OH_ImagePackerNative_PackToDataFromPicture(imagePacker, options, picture, outData, &size);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);

    errCode = OH_PackingOptions_Release(options);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    errCode = OH_ImagePackerNative_Release(imagePacker);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    errCode = OH_PictureNative_Release(picture);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
}

/**
 * @tc.name: OH_ImagePackerNative_PackToFileFromPictureTest001
 * @tc.desc: test OH_ImagePackerNative_PackToDataFromPicture with null pointers.
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToFileFromPictureTest001, TestSize.Level3)
{
    int32_t fd = 0;
    Image_ErrorCode res = OH_ImagePackerNative_PackToFileFromPicture(nullptr, nullptr, nullptr, fd);
    EXPECT_EQ(res, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_ImagePackerNative_PackToFileFromPictureTest002
 * @tc.desc: Tests packing a picture into file using OH_ImagePackerNative.
 *           The test checks if the packer and options are created, and if the picture is packed successfully.
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePackerNative_PackToFileFromPictureTest002, TestSize.Level1)
{
    OH_ImagePackerNative *imagePacker = nullptr;
    OH_PackingOptions *options = nullptr;
    OH_PictureNative *picture = nullptr;
    int32_t fd = 0;
    fd = open(IMAGE_FD_PATH.c_str(), O_WRONLY|O_CREAT);
    Image_ErrorCode errCode = OH_PackingOptions_Create(&options);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    ASSERT_NE(options, nullptr);

    errCode = OH_ImagePackerNative_Create(&imagePacker);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    ASSERT_NE(imagePacker, nullptr);

    Image_MimeType format;
    char formatStr[] = "image/jpeg";
    format.size = strlen(formatStr) + 1;
    format.data = formatStr;
    errCode = OH_PackingOptions_SetMimeType(options, &format);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);

    picture = CreatePictureNative();
    EXPECT_NE(picture, nullptr);
    errCode = OH_ImagePackerNative_PackToFileFromPicture(imagePacker, options, picture, fd);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);

    errCode = OH_PackingOptions_Release(options);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    errCode = OH_ImagePackerNative_Release(imagePacker);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
    errCode = OH_PictureNative_Release(picture);
    EXPECT_EQ(errCode, IMAGE_SUCCESS);
}
}
}