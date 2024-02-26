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
 * @tc.desc: test OH_ImagePacker_Create
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
 * @tc.desc: test OH_ImagePacker_InitNative
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_InitNative, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_InitNative start";
    napi_env env = nullptr;
    napi_value packer = nullptr;
    ImagePacker_Native*  ret = OH_ImagePacker_InitNative(env, packer);
    ASSERT_EQ(ret, nullptr);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_InitNative end";
}

/**
 * @tc.name: OH_ImagePacker_PackToData
 * @tc.desc: test OH_ImagePacker_PackToData
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_PackToData, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackToData start";
    ImagePacker_Native* native = nullptr;
    napi_value source = nullptr;
    ImagePacker_Opts opts;
    uint8_t* outData = nullptr;
    size_t size = TEST_SIZE;
    int32_t ret = OH_ImagePacker_PackToData(native, source, &opts, outData, &size);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackToData end";
}

/**
 * @tc.name: OH_ImagePacker_PackToFile
 * @tc.desc: test OH_ImagePacker_PackToFile
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_PackToFile, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackToFile start";
    ImagePacker_Native* native = nullptr;
    napi_value source = nullptr;
    ImagePacker_Opts opts;
    int fd = TEST_FD;
    int32_t ret = OH_ImagePacker_PackToFile(native, source, &opts, fd);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_PackToFile end";
}

/**
 * @tc.name: OH_ImagePacker_Release
 * @tc.desc: test OH_ImagePacker_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePacker_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_Release start";
    ImagePacker_Native* native = nullptr;
    int32_t ret = OH_ImagePacker_Release(native);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePacker_Release end";
}

/**
 * @tc.name: OH_ImagePackerCapi_Create
 * @tc.desc: test OH_ImagePackerCapi_Create
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePackerCapi_Create, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_Create start";
    ImagePackerCapi* imagePackerCapi = nullptr;
    int32_t ret = OH_ImagePackerCapi_Create(&imagePackerCapi);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_Create end";
}

/**
 * @tc.name: OH_ImagePackerCapi_PackingFromImageSource
 * @tc.desc: test OH_ImagePackerCapi_PackingFromImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePackerCapi_PackingFromImageSource, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackingFromImageSource start";
    ImagePackerCapi* imagePackerCapi = nullptr;
    ImagePacker_Opts* option = nullptr;
    ImageSourceCapi* imageSourceCapi = nullptr;
    uint8_t* outData;
    int64_t* size = nullptr;
    int32_t ret = OH_ImagePackerCapi_PackingFromImageSource(imagePackerCapi, option, imageSourceCapi, &outData, size);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackingFromImageSource end";
}

/**
 * @tc.name: OH_ImagePackerCapi_PackingFromPixelMap
 * @tc.desc: test OH_ImagePackerCapi_PackingFromPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePackerCapi_PackingFromPixelMap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackingFromPixelMap start";
    ImagePackerCapi* imagePackerCapi = nullptr;
    ImagePacker_Opts* option = nullptr;
    PixelMapCapi* pixelMapCapi = nullptr;
    uint8_t* outData;
    int64_t* size;
    int32_t ret = OH_ImagePackerCapi_PackingFromPixelMap(imagePackerCapi, option, pixelMapCapi, &outData, size);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackingFromPixelMap end";
}

/**
 * @tc.name: OH_ImagePackerCapi_PackToFileFromImageSource
 * @tc.desc: test OH_ImagePackerCapi_PackToFileFromImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePackerCapi_PackToFileFromImageSource, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackToFileFromImageSource start";
    ImagePackerCapi* imagePackerCapi = nullptr;
    ImagePacker_Opts* option = nullptr;
    ImageSourceCapi* imageSourceCapi = nullptr;
    int fd = TEST_FD;
    int32_t ret = OH_ImagePackerCapi_PackToFileFromImageSource(imagePackerCapi, option, imageSourceCapi, fd);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackToFileFromImageSource end";
}

/**
 * @tc.name: OH_ImagePackerCapi_PackToFileFromPixelMap
 * @tc.desc: test OH_ImagePackerCapi_PackToFileFromPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePackerCapi_PackToFileFromPixelMap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackToFileFromPixelMap start";
    ImagePackerCapi* imagePackerCapi = nullptr;
    ImagePacker_Opts* option = nullptr;
    PixelMapCapi* pixelMapCapi = nullptr;
    int fd = TEST_FD;
    int32_t ret = OH_ImagePackerCapi_PackToFileFromPixelMap(imagePackerCapi, option, pixelMapCapi, fd);
    ASSERT_NE(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_PackToFileFromPixelMap end";
}

/**
 * @tc.name: OH_ImagePackerCapi_Release
 * @tc.desc: test OH_ImagePackerCapi_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImagePackerMdkTest, OH_ImagePackerCapi_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_Release start";
    ImagePackerCapi* imagePackerCapi = nullptr;
    int32_t ret = OH_ImagePackerCapi_Release(imagePackerCapi);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagePackerMdkTest: OH_ImagePackerCapi_Release end";
}

}
}