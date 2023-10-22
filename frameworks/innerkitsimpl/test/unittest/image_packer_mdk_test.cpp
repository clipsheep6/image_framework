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
#include "image_packer_mdk.h"
#include "file_packer_stream.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Multimedia {
static constexpr int TEST_FD = -1;
static constexpr size_t TEST_SIZE = 0;
class ImagePackerMdkTest : public testing::Test {
public:
    ImagePackerMdkTest() {}
    ~ImagePackerMdkTest() {}
};

/**
 * @tc.name: OH_ImagePacker_Create
 * @tc.desc: test SetData and ClearData data type is bool
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_Create, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_Create start";
    napi_env env = nullptr;
    napi_value* packer = nullptr;
    int32_t ret = OH_ImagePacker_Create(env, packer);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_Create end";
}

/**
 * @tc.name: OH_ImagePacker_InitNative
 * @tc.desc: test 
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_InitNative, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_InitNative start";
    napi_env env = nullptr;
    napi_value packer = nullptr;
    ImagePackerNative*  ret = OH_ImagePacker_InitNative(env, packer);
    ASSERT_EQ(ret, nullptr);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_InitNative end";
}

/**
 * @tc.name: OH_ImagePacker_PackingToBuffer
 * @tc.desc: test 
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_PackingToBuffer, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackingToBuffer start";
    ImagePackerNative* native = nullptr;
    napi_value source = nullptr;
    struct OhosImagePackerOpts opts;
    uint8_t* outBuffer = nullptr;
    size_t size = TEST_SIZE;
    int32_t ret = OH_ImagePacker_PackingToBuffer(native, source, &opts, outBuffer, &size);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackingToBuffer end";
}

/**
 * @tc.name: OH_ImagePacker_PackingToFile
 * @tc.desc: test 
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_PackingToFile, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackingToFile start";
    ImagePackerNative* native = nullptr;
    napi_value source = nullptr;
    struct OhosImagePackerOpts opts;
    int fd = TEST_FD;
    int32_t ret = OH_ImagePacker_PackingToFile(native, source, &opts, fd);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackingToFile end";
}

/**
 * @tc.name: OH_ImagePacker_Release
 * @tc.desc: test 
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_Release start";
    ImagePackerNative* native = nullptr;
    int32_t ret = OH_ImagePacker_Release(native);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_Release end";
}
}
}