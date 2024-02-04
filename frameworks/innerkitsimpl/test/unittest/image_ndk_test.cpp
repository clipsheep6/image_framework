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

#include "image_mdk.h"
#include "image_mdk_kits.h"
#include "image_receiver_mdk.h"
#include "image_receiver_mdk_kits.h"
#include "image_source_mdk.h"
#include "image_source_mdk_kits.h"
#include "image_pixel_map_napi.h"
#include "raw_file.h"

using namespace testing::ext;
namespace OHOS {
namespace Media {
static constexpr int32_t UNSUCCESS = -1;
class ImageNdkTest : public testing::Test {
public:
    ImageNdkTest() {}
    ~ImageNdkTest() {}
};

/**
 * @tc.name: OH_Image_ClipRectTest
 * @tc.desc: OH_Image_ClipRect
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_ClipRectTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_ClipRectTest start";
    const ImageNative* native = nullptr;
    struct OhosImageRect* rect = nullptr;
    int32_t result = OH_Image_ClipRect(native, rect);
    ASSERT_EQ(result, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_ClipRectTest end";
}

/**
 * @tc.name: OH_Image_SizeTest
 * @tc.desc: OH_Image_Size
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_SizeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_SizeTest start";
    const ImageNative* native = nullptr;
    struct OhosImageSize* size = nullptr;
    int32_t result = OH_Image_Size(native, size);
    ASSERT_EQ(result, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_SizeTest end";
}

/**
 * @tc.name: OH_Image_FormatTest
 * @tc.desc: OH_Image_Format
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_FormatTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_FormatTest start";
    const ImageNative* native = nullptr;
    int32_t* format = nullptr;
    int32_t result = OH_Image_Format(native, format);
    ASSERT_EQ(result, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_FormatTest end";
}

/**
 * @tc.name: OH_Image_GetComponentTest
 * @tc.desc: OH_Image_GetComponent
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_GetComponentTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_GetComponentTest start";
    const ImageNative* native = nullptr;
    int32_t componentType = 0;
    struct OhosImageComponent* componentNative = nullptr;
    int32_t result = OH_Image_GetComponent(native, componentType, componentNative);
    ASSERT_EQ(result, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_GetComponentTest end";
}

/**
 * @tc.name: OH_Image_ReleaseTest
 * @tc.desc: OH_Image_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_ReleaseTest start";
    ImageNative* native = nullptr;
    int32_t result = OH_Image_Release(native);
    ASSERT_EQ(result, 0);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_ReleaseTest end";
}

/**
 * @tc.name: OH_Image_Receiver_InitImageReceiverNativeTest
 * @tc.desc: OH_Image_Receiver_InitImageReceiverNative
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_InitImageReceiverNativeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_InitImageReceiverNativeTest start";
    napi_env env = nullptr;
    napi_value source = nullptr;
    ImageReceiverNative* res = OH_Image_Receiver_InitImageReceiverNative(env, source);
    ASSERT_EQ(res, nullptr);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_InitImageReceiverNativeTest end";
}

/**
 * @tc.name: OH_Image_Receiver_GetReceivingSurfaceIdTest
 * @tc.desc: OH_Image_Receiver_GetReceivingSurfaceId
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_GetReceivingSurfaceIdTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetReceivingSurfaceIdTest start";
    const ImageReceiverNative *p = nullptr;
    char* id = nullptr;
    size_t len = 100;
    int32_t res = OH_Image_Receiver_GetReceivingSurfaceId(p, id, len);
    ASSERT_EQ(res, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetReceivingSurfaceIdTest end";
}

/**
 * @tc.name: OH_Image_Receiver_ReadLatestImageTest
 * @tc.desc: OH_Image_Receiver_ReadLatestImage
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_ReadLatestImageTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_ReadLatestImageTest start";
    const ImageReceiverNative *p = nullptr;
    napi_value* image = nullptr;
    int32_t res = OH_Image_Receiver_ReadLatestImage(p, image);
    ASSERT_EQ(res, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_ReadLatestImageTest end";
}

/**
 * @tc.name: OH_Image_Receiver_ReadNextImageTest
 * @tc.desc: OH_Image_Receiver_ReadNextImage
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_ReadNextImageTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_ReadNextImageTest start";
    const ImageReceiverNative *p = nullptr;
    napi_value* image = nullptr;
    int32_t res = OH_Image_Receiver_ReadNextImage(p, image);
    ASSERT_EQ(res, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_ReadNextImageTest end";
}

/**
 * @tc.name: OH_Image_Receiver_OnTest
 * @tc.desc: OH_Image_Receiver_On
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_OnTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_OnTest start";
    const ImageReceiverNative *p = nullptr;
    OH_Image_Receiver_On_Callback callback = nullptr;
    int32_t res = OH_Image_Receiver_On(p, callback);
    ASSERT_EQ(res, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_OnTest end";
}

/**
 * @tc.name: OH_Image_Receiver_GetSizeTest
 * @tc.desc: OH_Image_Receiver_GetSize
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_GetSizeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetSizeTest start";
    const ImageReceiverNative *p = nullptr;
    struct OhosImageSize* size = nullptr;
    int32_t res = OH_Image_Receiver_GetSize(p, size);
    ASSERT_EQ(res, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetSizeTest end";
}

/**
 * @tc.name: OH_Image_Receiver_GetCapacityTest
 * @tc.desc: OH_Image_Receiver_GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_GetCapacityTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetCapacityTest start";
    const ImageReceiverNative *p = nullptr;
    int32_t* capacity = nullptr;
    int32_t res = OH_Image_Receiver_GetCapacity(p, capacity);
    ASSERT_EQ(res, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetCapacityTest end";
}

/**
 * @tc.name: OH_Image_Receiver_GetFormatTest
 * @tc.desc: OH_Image_Receiver_GetFormat
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_GetFormatTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetFormatTest start";
    const ImageReceiverNative *p = nullptr;
    int32_t* format = nullptr;
    int32_t res = OH_Image_Receiver_GetFormat(p, format);
    ASSERT_EQ(res, UNSUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_GetFormatTest end";
}

/**
 * @tc.name: OH_Image_Receiver_ReleaseTest
 * @tc.desc: OH_Image_Receiver_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_Receiver_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_ReleaseTest start";
    ImageReceiverNative *p = nullptr;
    int32_t res = OH_Image_Receiver_Release(p);
    ASSERT_EQ(res, IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_Receiver_ReleaseTest end";
}

/**
 * @tc.name: OH_Image_InitImageNativeTest
 * @tc.desc: OH_Image_InitImageNative
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_InitImageNativeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_InitImageNativeTest start";
    napi_env env = nullptr;
    napi_value source = nullptr;
    ImageNative* res = OH_Image_InitImageNative(env, source);
    ASSERT_EQ(res, nullptr);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_InitImageNativeTest end";
}
/**
 * @tc.name: OH_ImageSource_CreateTest
 * @tc.desc: OH_ImageSource_Create
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreateTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateTest start";
    napi_env env = nullptr;
    napi_value* res = nullptr;
    OhosImageSource* src = nullptr;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSource_Create(env, src, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateTest end";
}

/**
 * @tc.name: OH_ImageSource_CreateFromUriTest
 * @tc.desc: OH_ImageSource_CreateFromUri
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreateFromUriTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromUriTest start";
    napi_env env = nullptr;
    napi_value* res = nullptr;
    char* uri = nullptr;
    size_t size = 0;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSource_CreateFromUri(env, uri, size, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromUriTest end";
}

/**
 * @tc.name: OH_ImageSource_CreateFromFdTest
 * @tc.desc: OH_ImageSource_CreateFromFd
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreateFromFdTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromFdTest start";
    napi_env env = nullptr;
    napi_value* res = nullptr;
    int32_t fd = -1;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSource_CreateFromFd(env, fd, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromFdTest end";
}

/**
 * @tc.name: OH_ImageSource_CreateFromDataTest
 * @tc.desc: OH_ImageSource_CreateFromData
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreateFromDataTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromDataTest start";
    napi_env env = nullptr;
    napi_value* res = nullptr;
    uint8_t* data = nullptr;
    size_t dataSize = 0;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSource_CreateFromData(env, data, dataSize, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromDataTest end";
}

/**
 * @tc.name: OH_ImageSource_CreateFromRawFileTest
 * @tc.desc: OH_ImageSource_CreateFromRawFile
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreateFromRawFileTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromRawFileTest start";
    napi_env env = nullptr;
    napi_value* res = nullptr;
    RawFileDescriptor rawFile;
    rawFile.fd = -1;
    rawFile.start = 0;
    rawFile.length = 0;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSource_CreateFromRawFile(env, rawFile, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateFromRawFileTest end";
}

/**
 * @tc.name: OH_ImageSource_CreateIncrementalTest
 * @tc.desc: OH_ImageSource_CreateIncremental
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreateIncrementalTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateIncrementalTest start";
    napi_env env = nullptr;
    napi_value* res = nullptr;
    OhosImageSource* src = nullptr;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSource_CreateIncremental(env, src, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateIncrementalTest end";
}

/**
 * @tc.name: OH_ImageSource_CreateIncrementalFromDataTest
 * @tc.desc: OH_ImageSource_CreateIncrementalFromData
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreateIncrementalFromDataTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateIncrementalFromDataTest start";
    napi_env env = nullptr;
    napi_value* res = nullptr;
    uint8_t* data = nullptr;
    size_t dataSize = 0;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSource_CreateIncrementalFromData(env, data, dataSize, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreateIncrementalFromDataTest end";
}

/**
 * @tc.name: OH_ImageSource_GetSupportedFormatsTest
 * @tc.desc: OH_ImageSource_GetSupportedFormats
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_GetSupportedFormatsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetSupportedFormatsTest start";
    OhosImageSourceSupportedFormatList* res = nullptr;
    int32_t ret = OH_ImageSource_GetSupportedFormats(res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetSupportedFormatsTest end";
}

/**
 * @tc.name: OH_ImageSource_CreatePixelMapTest
 * @tc.desc: OH_ImageSource_CreatePixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreatePixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreatePixelMapTest start";
    ImageSourceNative* native = nullptr;
    OhosImageDecodingOps* ops = nullptr;
    napi_value *res = nullptr;
    int32_t ret = OH_ImageSource_CreatePixelMap(native, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreatePixelMapTest end";
}

/**
 * @tc.name: OH_ImageSource_CreatePixelMapListTest
 * @tc.desc: OH_ImageSource_CreatePixelMapList
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_CreatePixelMapListTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreatePixelMapListTest start";
    ImageSourceNative* native = nullptr;
    OhosImageDecodingOps* ops = nullptr;
    napi_value *res = nullptr;
    int32_t ret = OH_ImageSource_CreatePixelMapList(native, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_CreatePixelMapListTest end";
}

/**
 * @tc.name: OH_ImageSource_GetDelayTimeTest
 * @tc.desc: OH_ImageSource_GetDelayTime
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_GetDelayTimeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetDelayTimeTest start";
    ImageSourceNative* native = nullptr;
    OhosImageSourceDelayTimeList* res = nullptr;
    int32_t ret = OH_ImageSource_GetDelayTime(native, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetDelayTimeTest end";
}

/**
 * @tc.name: OH_ImageSource_GetFrameCountTest
 * @tc.desc: OH_ImageSource_GetFrameCount
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_GetFrameCountTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetFrameCountTest start";
    ImageSourceNative* native = nullptr;
    uint32_t *res = nullptr;
    int32_t ret = OH_ImageSource_GetFrameCount(native, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetFrameCountTest end";
}

/**
 * @tc.name: OH_ImageSource_GetImageInfoTest
 * @tc.desc: OH_ImageSource_GetImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_GetImageInfoTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetImageInfoTest start";
    ImageSourceNative* native = nullptr;
    int32_t index = 0;
    OhosImageSourceInfo* info = nullptr;
    int32_t ret = OH_ImageSource_GetImageInfo(native, index, info);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetImageInfoTest end";
}

/**
 * @tc.name: OH_ImageSource_GetImagePropertyTest
 * @tc.desc: OH_ImageSource_GetImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_GetImagePropertyTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetImagePropertyTest start";
    ImageSourceNative* native = nullptr;
    OhosImageSourceProperty* key = nullptr;
    OhosImageSourceProperty* value = nullptr;
    int32_t ret = OH_ImageSource_GetImageProperty(native, key, value);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_GetImagePropertyTest end";
}

/**
 * @tc.name: OH_ImageSource_ModifyImagePropertyTest
 * @tc.desc: OH_ImageSource_ModifyImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_ModifyImagePropertyTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_ModifyImagePropertyTest start";
    ImageSourceNative* native = nullptr;
    OhosImageSourceProperty* key = nullptr;
    OhosImageSourceProperty* value = nullptr;
    int32_t ret = OH_ImageSource_ModifyImageProperty(native, key, value);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_ModifyImagePropertyTest end";
}

/**
 * @tc.name: OH_ImageSource_UpdateDataTest
 * @tc.desc: OH_ImageSource_UpdateData
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_UpdateDataTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_UpdateDataTest start";
    ImageSourceNative* native = nullptr;
    OhosImageSourceUpdateData* data = nullptr;
    int32_t ret = OH_ImageSource_UpdateData(native, data);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_UpdateDataTest end";
}

/**
 * @tc.name: OH_ImageSource_ReleaseTest
 * @tc.desc: OH_ImageSource_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_ReleaseTest start";
    ImageSourceNative* native = nullptr;
    int32_t ret = OH_ImageSource_Release(native);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_ReleaseTest end";
}

/**
 * @tc.name: OH_ImageSource_InitNativeTest
 * @tc.desc: OH_ImageSource_InitNative
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSource_InitNativeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_InitNativeTest start";
    napi_env env = nullptr;
    napi_value source = nullptr;
    ImageSourceNative*  ret = OH_ImageSource_InitNative(env, source);
    ASSERT_EQ(ret, nullptr);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSource_InitNativeTest end";
}

/* ImageSource Capi Test*/

/**
 * @tc.name: OH_ImageSourceCapi_CreateFromUriTest
 * @tc.desc: OH_ImageSourceCapi_CreateFromUri
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_CreateFromUriTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromUriTest start";
    void* res = nullptr;
    char* uri = nullptr;
    size_t size = 0;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSourceCapi_CreateFromUri(uri, size, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromUriTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_CreateFromFdTest
 * @tc.desc: OH_ImageSourceCapi_CreateFromFd
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_CreateFromFdTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromFdTest start";
    void* res = nullptr;
    int32_t fd = -1;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSourceCapi_CreateFromFd(fd, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromFdTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_CreateFromDataTest
 * @tc.desc: OH_ImageSourceCapi_CreateFromData
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_CreateFromDataTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromDataTest start";
    void* res = nullptr;
    uint8_t* data = nullptr;
    size_t dataSize = 0;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSourceCapi_CreateFromData(data, dataSize, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromDataTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_CreateFromRawFileTest
 * @tc.desc: OH_ImageSourceCapi_CreateFromRawFile
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_CreateFromRawFileTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromRawFileTest start";
    void* res = nullptr;
    RawFileDescriptor rawFile;
    rawFile.fd = -1;
    rawFile.start = 0;
    rawFile.length = 0;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSourceCapi_CreateFromRawFile(rawFile, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateFromRawFileTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_CreateIncrementalTest
 * @tc.desc: OH_ImageSourceCapi_CreateIncremental
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_CreateIncrementalTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateIncrementalTest start";
    void* res = nullptr;
    OhosImageSource* src = nullptr;
    OhosImageSourceOps* ops= nullptr;
    int32_t ret = OH_ImageSourceCapi_CreateIncremental(src, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreateIncrementalTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_GetSupportedFormatsTest
 * @tc.desc: OH_ImageSourceCapi_GetSupportedFormats
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_GetSupportedFormatsTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetSupportedFormatsTest start";
    void* imgSrc = nullptr;
    OhosImageSourceSupportedFormatList* res = nullptr;
    int32_t ret = OH_ImageSourceCapi_GetSupportedFormats(imgSrc,res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetSupportedFormatsTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_CreatePixelMapTest
 * @tc.desc: OH_ImageSourceCapi_CreatePixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_CreatePixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreatePixelMapTest start";
    void* imgSrc = nullptr;
    OhosImageDecodingOps* ops = nullptr;
    void* res = nullptr;
    int32_t ret = OH_ImageSourceCapi_CreatePixelMap(imgSrc, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreatePixelMapTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_CreatePixelMapListTest
 * @tc.desc: OH_ImageSourceCapi_CreatePixelMapList
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_CreatePixelMapListTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreatePixelMapListTest start";
    void* imgSrc = nullptr;
    OhosImageDecodingOps* ops = nullptr;
    void* res[] = nullptr;
    int32_t ret = OH_ImageSourceCapi_CreatePixelMapList(imgSrc, ops, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_CreatePixelMapListTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_GetDelayTimeTest
 * @tc.desc: OH_ImageSourceCapi_GetDelayTime
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_GetDelayTimeTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetDelayTimeTest start";
    void* imgSrc = nullptr;
    OhosImageSourceDelayTimeList* res = nullptr;
    int32_t ret = OH_ImageSourceCapi_GetDelayTime(imgSrc, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetDelayTimeTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_GetFrameCountTest
 * @tc.desc: OH_ImageSourceCapi_GetFrameCount
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_GetFrameCountTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetFrameCountTest start";
    void* imgSrc = nullptr;
    uint32_t *res = nullptr;
    int32_t ret = OH_ImageSourceCapi_GetFrameCount(imgSrc, res);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetFrameCountTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_GetImageInfoTest
 * @tc.desc: OH_ImageSourceCapi_GetImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_GetImageInfoTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetImageInfoTest start";
    void* imgSrc = nullptr;
    int32_t index = 0;
    OhosImageSourceInfo* info = nullptr;
    int32_t ret = OH_ImageSourceCapi_GetImageInfo(imgSrc, index, info);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetImageInfoTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_GetImagePropertyTest
 * @tc.desc: OH_ImageSourceCapi_GetImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_GetImagePropertyTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetImagePropertyTest start";
    void* imgSrc = nullptr;
    OhosImageSourceProperty* key = nullptr;
    OhosImageSourceProperty* value = nullptr;
    int32_t ret = OH_ImageSourceCapi_GetImageProperty(imgSrc, key, value);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_GetImagePropertyTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_ModifyImagePropertyTest
 * @tc.desc: OH_ImageSourceCapi_ModifyImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_ModifyImagePropertyTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_ModifyImagePropertyTest start";
    void* imgSrc = nullptr;
    ImageResource_* imgResource = nullptr;
    OhosImageSourceProperty* key = nullptr;
    OhosImageSourceProperty* value = nullptr;
    int32_t ret = OH_ImageSourceCapi_ModifyImageProperty(imgSrc, imgResource, key, value);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_ModifyImagePropertyTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_UpdateDataTest
 * @tc.desc: OH_ImageSourceCapi_UpdateData
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_UpdateDataTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_UpdateDataTest start";
    void* imgSrc = nullptr;
    OhosImageDecodingOps* decOption = nullptr;
    OhosImageSourceUpdateData* data = nullptr;
    int32_t ret = OH_ImageSourceCapi_UpdateData(imgSrc, decOption, data);
    ASSERT_NE(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_UpdateDataTest end";
}

/**
 * @tc.name: OH_ImageSourceCapi_ReleaseTest
 * @tc.desc: OH_ImageSourceCapi_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_ImageSourceCapi_ReleaseTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_ReleaseTest start";
    void* imgSrc = nullptr;
    int32_t ret = OH_ImageSourceCapi_Release(imgSrc);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_ImageSourceCapi_ReleaseTest end";
}

/* ImageSource Capi Test*/

/* JS and C convert Func Test*/

/**
 * @tc.name: OH_Image_JsToC_Int32Test
 * @tc.desc: OH_Image_JsToC_Int32
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_Int32Test, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Int32Test start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    int32_t out;
    int32_t  ret = OH_Image_JsToC_Int32(env, jsValue, &out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Int32Test end";
}

/**
 * @tc.name: OH_Image_JsToC_Uint32Test
 * @tc.desc: OH_Image_JsToC_Uint32
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_Uint32, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Uint32 start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    int32_t out;
    int32_t  ret = OH_Image_JsToC_Uint32(env, jsValue, &out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Uint32 end";
}

/**
 * @tc.name: OH_Image_JsToC_Int64Test
 * @tc.desc: OH_Image_JsToC_Int64
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_Int64Test, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Int64Test start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    int32_t out;
    int32_t  ret = OH_Image_JsToC_Int64(env, jsValue, &out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Int64Test end";
}

/**
 * @tc.name: OH_Image_JsToC_Uint64Test
 * @tc.desc: OH_Image_JsToC_Uint64
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_Uint64Test, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Uint64Test start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    int32_t out;
    int32_t  ret = OH_Image_JsToC_Uint64(env, jsValue, &out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_Uint64Test end";
}

/**
 * @tc.name: OH_Image_JsToC_BoolTest
 * @tc.desc: OH_Image_JsToC_Bool
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_BoolTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_BoolTest start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    int32_t out;
    int32_t  ret = OH_Image_JsToC_Bool(env, jsValue, &out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_BoolTest end";
}

/**
 * @tc.name: OH_Image_JsToC_CharStr_Utf8Test
 * @tc.desc: OH_Image_JsToC_CharStr_Utf8
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_CharStr_Utf8Test, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_CharStr_Utf8Test start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    char* out = nullptr;
    int32_t  ret = OH_Image_JsToC_CharStr_Utf8(env, jsValue, out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_CharStr_Utf8Test end";
}

/**
 * @tc.name: OH_Image_JsToC_BufferUint8Test
 * @tc.desc: OH_Image_JsToC_BufferUint8
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_BufferUint8Test, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_BufferUint8Test start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    uint8_t* out = nullptr;
    size_t outLen;
    int32_t  ret = OH_Image_JsToC_BufferUint8(env, jsValue, out, &outLen);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_BufferUint8Test end";
}

/**
 * @tc.name: OH_Image_JsToC_ImageSourceTest
 * @tc.desc: OH_Image_JsToC_ImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_ImageSourceTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_ImageSourceTest start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    void* out = nullptr;
    int32_t  ret = OH_Image_JsToC_ImageSource(env, jsValue, out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_ImageSourceTest end";
}

/**
 * @tc.name: OH_Image_JsToC_PixelMapTest
 * @tc.desc: OH_Image_JsToC_PixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_JsToC_PixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_PixelMapTest start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    void* out = nullptr;
    int32_t  ret = OH_Image_JsToC_PixelMap(env, jsValue, out);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_JsToC_PixelMapTest end";
}

/**
 * @tc.name: OH_Image_CToJs_NumberIntTest
 * @tc.desc: OH_Image_CToJs_NumberInt
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_CToJs_NumberIntTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_NumberIntTest start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    int32_t in ;
    int32_t  ret = OH_Image_CToJs_NumberInt(env, in, jsValue);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_NumberIntTest end";
}

/**
 * @tc.name: OH_Image_CToJs_String_Utf8Test
 * @tc.desc: OH_Image_CToJs_String_Utf8
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_CToJs_String_Utf8Test, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_String_Utf8Test start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    std::string in = "";
    int32_t  ret = OH_Image_CToJs_String_Utf8(env, in, jsValue);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_String_Utf8Test end";
}

/**
 * @tc.name: OH_Image_CToJs_ObjImageSourceTest
 * @tc.desc: OH_Image_CToJs_ObjImageSource
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_CToJs_ObjImageSourceTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_ObjImageSourceTest start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    void* in = nullptr;
    int32_t  ret = OH_Image_CToJs_ObjImageSource(env, jsValue, in);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_ObjImageSourceTest end";
}

/**
 * @tc.name: OH_Image_CToJs_ObjPixelMapTest
 * @tc.desc: OH_Image_CToJs_ObjPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_CToJs_ObjPixelMapTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_ObjPixelMapTest start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    void* in = nullptr;
    int32_t  ret = OH_Image_CToJs_ObjPixelMap(env, jsValue, in);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_ObjPixelMapTest end";
}

/**
 * @tc.name: OH_Image_CToJs_ObjPixelMapListTest
 * @tc.desc: OH_Image_CToJs_ObjPixelMapList
 * @tc.type: FUNC
 */
HWTEST_F(ImageNdkTest, OH_Image_CToJs_ObjPixelMapListTest, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_ObjPixelMapListTest start";
    napi_env env = nullptr;
    napi_value jsValue = nullptr;
    void* in[] = nullptr;
    int32_t  ret = OH_Image_CToJs_ObjPixelMapList(env, jsValue, in);
    ASSERT_EQ(ret, OHOS_IMAGE_RESULT_SUCCESS);

    GTEST_LOG_(INFO) << "ImageNdkTest: OH_Image_CToJs_ObjPixelMapListTest end";
}
/* JS and C convert Func Test*/
}
}
