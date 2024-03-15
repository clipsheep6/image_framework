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
#include "image_packer2.h"
#include "image_packer2_impl.h"
#include "file_packer_stream.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Multimedia {
class ImagePackerNdk2Test : public testing::Test {
public:
    ImagePackerNdk2Test() {}
    ~ImagePackerNdk2Test() {}
};

/**
 * @tc.name: OH_ImageSource2_CreateImageInfo
 * @tc.desc: test OH_ImageSource2_CreateImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_CreatePackingOptions, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImageSource2_CreateImageInfo start";
    OH_ImagePacker_PackingOptions *ops = nullptr;
    Image_ErrorCode ret = OH_ImagePacker2_CreatePackingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImageSource2_CreateImageInfo end";
}

/**
 * @tc.name: OH_ImagePacker2_PackingOptionsSetGetMimeType
 * @tc.desc: test OH_ImagePacker2_PackingOptionsSetGetMimeType
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_PackingOptionsSetGetMimeType, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackingOptionsSetGetMimeType start";
    OH_ImagePacker_PackingOptions *ops = nullptr;
    char str[] = "1";
    char str2[] = "12";
    Image_MimeType mimeType = {str, 1}; 
    Image_ErrorCode ret = OH_ImagePacker2_CreatePackingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    OH_ImagePacker2_PackingOptionsSetMimeType(ops, {str2, 2});
    OH_ImagePacker2_PackingOptionsGetMimeType(ops, &mimeType);
    ASSERT_EQ(mimeType.size, 2);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackingOptionsSetGetMimeType end";
}

/**
 * @tc.name: OH_ImagePacker2_ReleasePackingOptions
 * @tc.desc: test OH_ImagePacker2_ReleasePackingOptions
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_ReleasePackingOptions, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_ReleasePackingOptions start";
    OH_ImagePacker_PackingOptions *ops = nullptr;
    Image_ErrorCode ret = OH_ImagePacker2_ReleasePackingOptions(ops);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_ReleasePackingOptions end";
}

/**
 * @tc.name: OH_ImagePacker2_Create
 * @tc.desc: test OH_ImagePacker2_Create
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_Create, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_Create start";
    OH_ImagePacker *imagePacker = nullptr;
    Image_ErrorCode ret = OH_ImagePacker2_Create(&imagePacker);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_Create end";
}

/**
 * @tc.name: OH_ImagePacker2_PackToDataFromImageSource
 * @tc.desc: test OH_ImagePacker2_PackToDataFromImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_PackToDataFromImageSource, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToDataFromImageSource start";
    OH_ImagePacker *imagePacker = nullptr;
    OH_ImagePacker_PackingOptions* option = nullptr;
    OH_ImageSource* imageSource = nullptr;
    uint8_t* outData = nullptr;
    size_t *size = 0;
    Image_ErrorCode ret = OH_ImagePacker2_PackToDataFromImageSource(imagePacker, option, imageSource, outData, size);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToDataFromImageSource end";
}

/**
 * @tc.name: OH_ImagePacker2_PackToDataFromPixelMap
 * @tc.desc: test OH_ImagePacker2_PackToDataFromPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_PackToDataFromPixelMap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToDataFromPixelMap start";
    OH_ImagePacker *imagePacker = nullptr;
    OH_ImagePacker_PackingOptions* option = nullptr;
    OH_Pixelmap* pixelMap = nullptr;
    uint8_t* outData = nullptr;
    size_t *size = 0;
    Image_ErrorCode ret = OH_ImagePacker2_PackToDataFromPixelMap(imagePacker, option, pixelMap, outData, size);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToDataFromPixelMap end";
}

/**
 * @tc.name: OH_ImagePacker2_PackToFileFromImageSource
 * @tc.desc: test OH_ImagePacker2_PackToFileFromImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_PackToFileFromImageSource, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToFileFromImageSource start";
    OH_ImagePacker *imagePacker = nullptr;
    OH_ImagePacker_PackingOptions* option = nullptr;
    OH_ImageSource* imageSource = nullptr;
    int32_t fd = 0;
    Image_ErrorCode ret = OH_ImagePacker2_PackToFileFromImageSource(imagePacker, option, imageSource, fd);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToFileFromImageSource end";
}

/**
 * @tc.name: OH_ImagePacker2_PackToFileFromPixelMap
 * @tc.desc: test OH_ImagePacker2_PackToFileFromPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_PackToFileFromPixelMap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToFileFromPixelMap start";
    OH_ImagePacker *imagePacker = nullptr;
    OH_ImagePacker_PackingOptions* option = nullptr;
    OH_Pixelmap* pixelMap = nullptr;
    int32_t fd = 0;
    Image_ErrorCode ret = OH_ImagePacker2_PackToFileFromPixelMap(imagePacker, option, pixelMap, fd);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_PackToFileFromPixelMap end";
}

/**
 * @tc.name: OH_ImagePacker2_Release
 * @tc.desc: test OH_ImagePacker2_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerNdk2Test, OH_ImagePacker2_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_Release start";
    OH_ImagePacker *imagePacker = nullptr;
    Image_ErrorCode ret = OH_ImagePacker2_Release(imagePacker);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerNdk2Test: OH_ImagePacker2_Release end";
}

}
}