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
#include "common_utils.h"
#include "image.h"
#include "image/image_receiver.h"
#include "image_kits.h"
#include "receiver/include/image_receiver.h"

using namespace testing::ext;
namespace OHOS {
namespace Media {

static constexpr int32_t IMAGE_TEST_WIDTH = 8192;
static constexpr int32_t IMAGE_TEST_HEIGHT = 8;
static constexpr int32_t IMAGE_TEST_FORMAT = 4;
static constexpr int32_t IMAGE_TEST_CAPACITY = 8;

class ImageReceiver2Test : public testing::Test {
public:
    ImageReceiver2Test() {}
    ~ImageReceiver2Test() {}
    static OH_ImageReceiver* CreateReceiver();
};

OH_ImageReceiver* ImageReceiver2Test::CreateReceiver()
{
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    if (nRst != IMAGE_ERRORCODE_SUCCESS || options == nullptr) {
        return nullptr;
    }
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);

    Image_Size size;
    size.width = IMAGE_TEST_WIDTH;
    size.height = IMAGE_TEST_HEIGHT;
    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetSize(options, size);
    if (nRst != IMAGE_ERRORCODE_SUCCESS) {
        return nullptr;
    }

    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetFormat(options, IMAGE_TEST_FORMAT);
    if (nRst != IMAGE_ERRORCODE_SUCCESS) {
        return nullptr;
    }

    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetCapacity(options, IMAGE_TEST_CAPACITY);
    if (nRst != IMAGE_ERRORCODE_SUCCESS) {
        return nullptr;
    }

    OH_ImageReceiver* pReceiver = nullptr;
    nRst = OH_ImageReceiver2_Create(options, &pReceiver);
    if (nRst != IMAGE_ERRORCODE_SUCCESS || pReceiver == nullptr) {
        return nullptr;
    }
    return pReceiver;
}

/**
 * @tc.name: OH_ImageReceiver2_CreateImageReceiverOptionsTest
 * @tc.desc: OH_ImageReceiver2_CreateImageReceiverOptions
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_CreateImageReceiverOptionsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_CreateImageReceiverOptionsTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_CreateImageReceiverOptionsTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ReleaseImageReceiverOptionsTest
 * @tc.desc: OH_ImageReceiver2_ReleaseImageReceiverOptions
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ReleaseImageReceiverOptionsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReleaseImageReceiverOptionsTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    nRst = OH_ImageReceiver2_ReleaseImageReceiverOptions(options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReleaseImageReceiverOptionsTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ImageReceiverOptionsSetSizeTest
 * @tc.desc: OH_ImageReceiver2_ImageReceiverOptionsSetSize
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ImageReceiverOptionsSetSizeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsSetSizeTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);

    Image_Size size;
    size.width = IMAGE_TEST_WIDTH;
    size.height = IMAGE_TEST_HEIGHT;
    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetSize(options, size);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsSetSizeTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ImageReceiverOptionsGetSizeTest
 * @tc.desc: OH_ImageReceiver2_ImageReceiverOptionsGetSize
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ImageReceiverOptionsGetSizeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsGetSizeTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);

    Image_Size size;
    size.width = IMAGE_TEST_WIDTH;
    size.height = IMAGE_TEST_HEIGHT;
    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetSize(options, size);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);

    Image_Size size_get;
    nRst = OH_ImageReceiver2_ImageReceiverOptionsGetSize(options, &size_get);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_EQ(size_get.width, IMAGE_TEST_WIDTH);
    ASSERT_EQ(size_get.height, IMAGE_TEST_HEIGHT);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsGetSizeTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ImageReceiverOptionsSetFormatTest
 * @tc.desc: OH_ImageReceiver2_ImageReceiverOptionsSetFormat
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ImageReceiverOptionsSetFormatTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsSetFormatTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);

    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetFormat(options, IMAGE_TEST_FORMAT);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsSetFormatTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ImageReceiverOptionsGetFormatTest
 * @tc.desc: OH_ImageReceiver2_ImageReceiverOptionsGetFormat
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ImageReceiverOptionsGetFormatTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsGetFormatTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);

    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetFormat(options, IMAGE_TEST_FORMAT);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);

    int32_t nFormat = 0;
    nRst = OH_ImageReceiver2_ImageReceiverOptionsGetFormat(options, &nFormat);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_EQ(nFormat, IMAGE_TEST_FORMAT);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsGetFormatTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ImageReceiverOptionsSetCapacityTest
 * @tc.desc: OH_ImageReceiver2_ImageReceiverOptionsSetCapacity
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ImageReceiverOptionsSetCapacityTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsSetCapacityTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);

    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetCapacity(options, IMAGE_TEST_CAPACITY);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsSetCapacityTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ImageReceiverOptionsGetCapacityTest
 * @tc.desc: OH_ImageReceiver2_ImageReceiverOptionsGetCapacity
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ImageReceiverOptionsGetCapacityTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsGetCapacityTest start";
    OH_ImageReceiverOptions* options = nullptr;
    Image_ErrorCode nRst = IMAGE_ERRORCODE_SUCCESS;
    nRst = OH_ImageReceiver2_CreateImageReceiverOptions(&options);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_NE(options, nullptr);
    std::shared_ptr<OH_ImageReceiverOptions> ptrOptions(options, OH_ImageReceiver2_ReleaseImageReceiverOptions);

    nRst = OH_ImageReceiver2_ImageReceiverOptionsSetCapacity(options, IMAGE_TEST_CAPACITY);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);

    int32_t nCapacity = 0;
    nRst = OH_ImageReceiver2_ImageReceiverOptionsGetCapacity(options, &nCapacity);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    ASSERT_EQ(nCapacity, IMAGE_TEST_CAPACITY);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ImageReceiverOptionsGetCapacityTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_CreateTest
 * @tc.desc: OH_ImageReceiver2_Create
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_CreateTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_CreateTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_CreateTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ReleaseTest
 * @tc.desc: OH_ImageReceiver2_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReleaseTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    Image_ErrorCode nRst = OH_ImageReceiver2_Release(pReceiver);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReleaseTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_GetReceivingSurfaceIdTest
 * @tc.desc: OH_ImageReceiver2_GetReceivingSurfaceId
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_GetReceivingSurfaceIdTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetReceivingSurfaceIdTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    uint64_t nSurfaceId = 0;
    Image_ErrorCode nRst = OH_ImageReceiver2_GetReceivingSurfaceId(pReceiver, &nSurfaceId);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetReceivingSurfaceIdTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ReadLatestImageTest
 * @tc.desc: OH_ImageReceiver2_ReadLatestImage
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ReadLatestImageTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReadLatestImageTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    OH_Image* pImg = nullptr;
    Image_ErrorCode nRst = OH_ImageReceiver2_ReadLatestImage(pReceiver, &pImg);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_GET_DATA_ABNORMAL);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReadLatestImageTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_ReadNextImageTest
 * @tc.desc: OH_ImageReceiver2_ReadNextImage
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_ReadNextImageTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReadNextImageTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    OH_Image* pImg = nullptr;
    Image_ErrorCode nRst = OH_ImageReceiver2_ReadNextImage(pReceiver, &pImg);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_GET_DATA_ABNORMAL);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_ReadNextImageTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_OnTest
 * @tc.desc: OH_ImageReceiver2_On
 * @tc.type: FUNC
 */
static void OH_Image_Receiver_On_Callback(OH_ImageReceiver* receiver) {}

HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_OnTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_OnTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    Image_ErrorCode nRst = OH_ImageReceiver2_On(pReceiver, OH_Image_Receiver_On_Callback);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_OnTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_OffTest
 * @tc.desc: OH_ImageReceiver2_Off
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_OffTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_OffTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    Image_ErrorCode nRst = OH_ImageReceiver2_Off(pReceiver);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_OffTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_GetSizeTest
 * @tc.desc: OH_ImageReceiver2_GetSize
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_GetSizeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetSizeTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    Image_Size size;
    Image_ErrorCode nRst = OH_ImageReceiver2_GetSize(pReceiver, &size);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetSizeTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_GetCapacityTest
 * @tc.desc: OH_ImageReceiver2_GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_GetCapacityTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetCapacityTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    int32_t nCapacity = 0;
    Image_ErrorCode nRst = OH_ImageReceiver2_GetCapacity(pReceiver, &nCapacity);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetCapacityTest end";
}

/**
 * @tc.name: OH_ImageReceiver2_GetFormatTest
 * @tc.desc: OH_ImageReceiver2_GetFormat
 * @tc.type: FUNC
 */
HWTEST_F(ImageReceiver2Test, OH_ImageReceiver2_GetFormatTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetFormatTest start";
    OH_ImageReceiver* pReceiver = ImageReceiver2Test::CreateReceiver();
    ASSERT_NE(pReceiver, nullptr);
    std::shared_ptr<OH_ImageReceiver> ptrReceiver(pReceiver, OH_ImageReceiver2_Release);

    int32_t nFormat = 0;
    Image_ErrorCode nRst = OH_ImageReceiver2_GetFormat(pReceiver, &nFormat);
    ASSERT_EQ(nRst, IMAGE_ERRORCODE_SUCCESS);
    GTEST_LOG_(INFO) << "ImageReceiver2Test: OH_ImageReceiver2_GetFormatTest end";
}

} // namespace Media
} // namespace OHOS