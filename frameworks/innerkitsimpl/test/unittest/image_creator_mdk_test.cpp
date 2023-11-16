/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "image_creator_mdk.h"

using namespace testing::ext;
namespace OHOS {
namespace Media {
static constexpr int32_t TEST_WIDTH = 8192;
static constexpr int32_t TEST_HEIGHT = 8;
static constexpr int32_t TEST_CAPICITY = 8;
static constexpr int32_t TEST_FORMAT = 4;
class ImageCreatorMdkTest : public testing::Test {
public:
    ImageCreatorMdkTest() {}
    ~ImageCreatorMdkTest() {}
};

/**
 * @tc.name: OH_ImageCreator_CreateTest
 * @tc.desc: OH_ImageCreator_Create api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_CreateTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_CreateTest start";

    napi_env env = nullptr;
    napi_value res = nullptr;
    struct ImageCreator_Opts_ opts;
    opts.width = TEST_WIDTH;
    opts.height = TEST_HEIGHT;
    opts.capicity = TEST_CAPICITY;
    opts.format = TEST_FORMAT;
    int32_t result = OH_ImageCreator_Create(env, opts, &res);
    ASSERT_EQ(result, IMAGE_RESULT_INVALID_PARAMETER);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_CreateTest end";
}

/**
 * @tc.name: OH_ImageCreator_InitNativeTest
 * @tc.desc: OH_ImageCreator_InitNative api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_InitNativeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_InitNativeTest start";

    napi_env env = nullptr;
    napi_value source = nullptr;
    ImageCreator_Native* result = OH_ImageCreator_InitNative(env, source);
    ASSERT_EQ(result, nullptr);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_InitNativeTest end";
}

/**
 * @tc.name: OH_ImageCreator_DequeueTest
 * @tc.desc: OH_ImageCreator_Dequeue api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_DequeueTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_DequeueTest start";

    ImageCreator_Native* native = nullptr;
    napi_value image = nullptr;
    int32_t result = OH_ImageCreator_Dequeue(native, &image);
    ASSERT_NE(result, IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_DequeueTest end";
}

/**
 * @tc.name: OH_ImageCreator_QueueTest
 * @tc.desc: OH_ImageCreator_Queue api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_QueueTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_QueueTest start";

    ImageCreator_Native* native = nullptr;
    napi_value image = nullptr;
    int32_t result = OH_ImageCreator_Queue(native, image);
    ASSERT_NE(result, IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_QueueTest end";
}

static void onTestEvent()
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_OnTest onTestEvent";
}

/**
 * @tc.name: OH_ImageCreator_OnTest
 * @tc.desc: OH_ImageCreator_On api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_OnTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_OnTest start";

    ImageCreator_Native* native = nullptr;
    int32_t result = OH_ImageCreator_On(native, onTestEvent);
    ASSERT_NE(result, IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_OnTest end";
}

/**
 * @tc.name: OH_ImageCreator_GetCapacityTest
 * @tc.desc: OH_ImageCreator_GetCapacity api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_GetCapacityTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_GetCapacityTest start";

    ImageCreator_Native* native = nullptr;
    int32_t capacity;
    int32_t result = OH_ImageCreator_GetCapacity(native, &capacity);
    ASSERT_NE(result, IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_GetCapacityTest end";
}

/**
 * @tc.name: OH_ImageCreator_GetFormatTest
 * @tc.desc: OH_ImageCreator_GetFormat api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_GetFormatTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_GetFormatTest start";

    ImageCreator_Native* native = nullptr;
    int32_t format;
    int32_t result = OH_ImageCreator_GetFormat(native, &format);
    ASSERT_NE(result, IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_GetFormatTest end";
}

/**
 * @tc.name: OH_ImageCreator_ReleaseTest
 * @tc.desc: OH_ImageCreator_Release api test 
 * @tc.type: FUNC
 */
HWTEST_F(ImageCreatorMdkTest, OH_ImageCreator_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_ReleaseTest start";

    ImageCreator_Native* native = nullptr;
    int32_t result = OH_ImageCreator_Release(native);
    ASSERT_EQ(result, IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageCreatorMdkTest: OH_ImageCreator_ReleaseTest end";
}
}
}
