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

#include "image_source2.h"
#include "image_source2_impl.h"
#include "raw_file.h"

using namespace testing::ext;
namespace OHOS {
namespace Media {
class ImagSourceNdk2Test : public testing::Test {
public:
    ImagSourceNdk2Test() {}
    ~ImagSourceNdk2Test() {}
};

/**
 * @tc.name: OH_ImageSource2_CreateImageInfo
 * @tc.desc: test OH_ImageSource2_CreateImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreateImageInfo, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateImageInfo start";
    OH_ImageSource_ImageInfo *ops = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_CreateImageInfo(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateImageInfo end";
}

/**
 * @tc.name: OH_ImageSource2_ImageInfoGetWidth
 * @tc.desc: test OH_ImageSource2_ImageInfoGetWidth
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_ImageInfoGetWidth, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ImageInfoGetWidth start";
    OH_ImageSource_ImageInfo *ops = nullptr;
    uint32_t *width = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_ImageInfoGetWidth(ops, width);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ImageInfoGetWidth end";
}

/**
 * @tc.name: OH_ImageSource2_ImageInfoGetHeight
 * @tc.desc: test OH_ImageSource2_ImageInfoGetHeight
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_ImageInfoGetHeight, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ImageInfoGetHeight start";
    OH_ImageSource_ImageInfo *ops = nullptr;
    uint32_t *width = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_ImageInfoGetHeight(ops, width);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ImageInfoGetHeight end";
}

/**
 * @tc.name: OH_ImageSource2_ReleaseImageInfo
 * @tc.desc: test OH_ImageSource2_ReleaseImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_ReleaseImageInfo, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ReleaseImageInfo start";
    OH_ImageSource_ImageInfo *ops = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_ReleaseImageInfo(ops);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ReleaseImageInfo end";
}

/**
 * @tc.name: OH_ImageSource2_CreateDecodingOptions
 * @tc.desc: test OH_ImageSource2_CreateDecodingOptions
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreateDecodingOptions, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateDecodingOptions start";
    OH_ImageSource_DecodingOptions *ops = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_CreateDecodingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateDecodingOptions end";
}

/**
 * @tc.name: OH_ImageSource_DecodingOptionsSetGetPixelFormat
 * @tc.desc: test OH_ImageSource_DecodingOptionsSetGetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource_DecodingOptionsSetGetPixelFormat, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetPixelFormat start";
    OH_ImageSource_DecodingOptions *ops = nullptr;
    Image_ErrorCode ret = IMAGE_RESULT_MEDIA_UNKNOWN;
    int32_t pixelFormat = 0;
    ret = OH_ImageSource2_CreateDecodingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    OH_ImageSource2_DecodingOptionsSetPixelFormat(ops, 1);
    OH_ImageSource2_DecodingOptionsGetPixelFormat(ops, &pixelFormat);
    ASSERT_EQ(pixelFormat, 1);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetPixelFormat end";
}

/**
 * @tc.name: OH_ImageSource_DecodingOptionsSetGetIndex
 * @tc.desc: test OH_ImageSource_DecodingOptionsSetGetIndex
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource_DecodingOptionsSetGetIndex, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetIndex start";
    OH_ImageSource_DecodingOptions *ops = nullptr;
    Image_ErrorCode ret = IMAGE_RESULT_MEDIA_UNKNOWN;
    uint32_t index = 0;
    ret = OH_ImageSource2_CreateDecodingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    OH_ImageSource2_DecodingOptionsSetIndex(ops, 1);
    OH_ImageSource2_DecodingOptionsGetIndex(ops, &index);
    ASSERT_EQ(index, 1);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetIndex end";
}

/**
 * @tc.name: OH_ImageSource_DecodingOptionsSetGetRotate
 * @tc.desc: test OH_ImageSource_DecodingOptionsSetGetRotate
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource_DecodingOptionsSetGetRotate, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetRotate start";
    OH_ImageSource_DecodingOptions *ops = nullptr;
    Image_ErrorCode ret = IMAGE_RESULT_MEDIA_UNKNOWN;
    float rotate = 0;
    ret = OH_ImageSource2_CreateDecodingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    OH_ImageSource2_DecodingOptionsSetRotate(ops, 1);
    OH_ImageSource2_DecodingOptionsGetRotate(ops, &rotate);
    ASSERT_EQ(rotate, 1);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetRotate end";
}

/**
 * @tc.name: OH_ImageSource_DecodingOptionsSetGetDesiredSize
 * @tc.desc: test OH_ImageSource_DecodingOptionsSetGetDesiredSize
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource_DecodingOptionsSetGetDesiredSize, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetDesiredSize start";
    OH_ImageSource_DecodingOptions *ops = nullptr;
    Image_ErrorCode ret = IMAGE_RESULT_MEDIA_UNKNOWN;
    Image_Size desiredSize = {0, 0};
    Image_Size desiredSize2 = {1, 2};
    ret = OH_ImageSource2_CreateDecodingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    OH_ImageSource2_DecodingOptionsSetDesiredSize(ops, &desiredSize2);
    OH_ImageSource2_DecodingOptionsGetDesiredSize(ops, &desiredSize);
    ASSERT_EQ(desiredSize.width, desiredSize2.width);
    ASSERT_EQ(desiredSize.height, desiredSize2.height);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetDesiredSize end";
}

/**
 * @tc.name: OH_ImageSource_DecodingOptionsSetGetDesiredRegion
 * @tc.desc: test OH_ImageSource_DecodingOptionsSetGetDesiredRegion
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource_DecodingOptionsSetGetDesiredRegion, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetDesiredRegion start";
    OH_ImageSource_DecodingOptions *ops = nullptr;
    Image_ErrorCode ret = IMAGE_RESULT_MEDIA_UNKNOWN;
    Image_Region desiredRegion = {0, 0, 0, 0};
    Image_Region desiredRegion2 = {1, 2, 3, 4};
    ret = OH_ImageSource2_CreateDecodingOptions(&ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    OH_ImageSource2_DecodingOptionsSetDesiredRegion(ops, &desiredRegion2);
    OH_ImageSource2_DecodingOptionsGetDesiredRegion(ops, &desiredRegion);
    ASSERT_EQ(desiredRegion.x, 1);
    ASSERT_EQ(desiredRegion.y, 2);
    ASSERT_EQ(desiredRegion.width, 3);
    ASSERT_EQ(desiredRegion.height, 4);
    ret = OH_ImageSource2_ReleaseDecodingOptions(ops);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource_DecodingOptionsSetGetDesiredRegion end";
}

/**
 * @tc.name: OH_ImageSource2_CreateFromUri
 * @tc.desc: test OH_ImageSource2_CreateFromUri
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreateFromUri, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromUri start";
    OH_ImageSource *imageSource = nullptr;
    char *uri = nullptr;
    size_t uriSize = 0;
    Image_ErrorCode ret = OH_ImageSource2_CreateFromUri(uri, uriSize, &imageSource);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromUri end";
}

/**
 * @tc.name: OH_ImageSource2_CreateFromFd
 * @tc.desc: test OH_ImageSource2_CreateFromFd
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreateFromFd, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromFd start";
    OH_ImageSource *imageSource = nullptr;
    int32_t fd = 0;
    Image_ErrorCode ret = OH_ImageSource2_CreateFromFd(fd, &imageSource);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromFd end";
}

/**
 * @tc.name: OH_ImageSource2_CreateFromData
 * @tc.desc: test OH_ImageSource2_CreateFromData
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreateFromData, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromData start";
    OH_ImageSource *imageSource = nullptr;
    uint8_t* data = nullptr;
    size_t dataSize = 0;
    Image_ErrorCode ret = OH_ImageSource2_CreateFromData(data, dataSize, &imageSource);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromData end";
}

/**
 * @tc.name: OH_ImageSource2_CreateFromRawFile
 * @tc.desc: test OH_ImageSource2_CreateFromRawFile
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreateFromRawFile, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromRawFile start";
    OH_ImageSource *imageSource = nullptr;
    RawFileDescriptor rawFile;
    Image_ErrorCode ret = OH_ImageSource2_CreateFromRawFile(rawFile, &imageSource);
    ASSERT_EQ(ret, IMAGE_RESULT_SUCCESS);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreateFromRawFile end";
}

/**
 * @tc.name: OH_ImageSource2_CreatePixelMap
 * @tc.desc: test OH_ImageSource2_CreatePixelMap
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreatePixelMap, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreatePixelMap start";
    OH_ImageSource *imageSource = nullptr;
    OH_ImageSource_DecodingOptions* ops = nullptr;
    OH_Pixelmap* resPixMap = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_CreatePixelMap(imageSource, ops, &resPixMap);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreatePixelMap end";
}

/**
 * @tc.name: OH_ImageSource2_CreatePixelMapList
 * @tc.desc: test OH_ImageSource2_CreatePixelMapList
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_CreatePixelMapList, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreatePixelMapList start";
    OH_ImageSource_DecodingOptions *ops = nullptr;
    OH_ImageSource *imageSource = nullptr;
    // OH_Pixelmap* resVecPixMap[1];
    OH_Pixelmap** resVecPixMap;
    size_t* outSize = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_CreatePixelMapList(imageSource, ops, &resVecPixMap, outSize);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_CreatePixelMapList end";
}

/**
 * @tc.name: OH_ImageSource2_GetDelayTime
 * @tc.desc: test OH_ImageSource2_GetDelayTime
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_GetDelayTime, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetDelayTime start";
    OH_ImageSource *imageSource = nullptr;
    int32_t* delayTimeList = nullptr;
    size_t* size = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_GetDelayTime(imageSource, delayTimeList, size);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetDelayTime end";
}

/**
 * @tc.name: OH_ImageSource2_GetImageInfo
 * @tc.desc: test OH_ImageSource2_GetImageInfo
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_GetImageInfo, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetImageInfo start";
    OH_ImageSource *imageSource = nullptr;
    int32_t index = 0;
    OH_ImageSource_ImageInfo* info = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_GetImageInfo(imageSource, index, info);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetImageInfo end";
}

/**
 * @tc.name: OH_ImageSource2_GetImageProperty
 * @tc.desc: test OH_ImageSource2_GetImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_GetImageProperty, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetImageProperty start";
    OH_ImageSource *imageSource = nullptr;
    Image_String* key = nullptr;
    Image_String* value = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_GetImageProperty(imageSource, key, value);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetImageProperty end";
}

/**
 * @tc.name: OH_ImageSource2_ModifyImageProperty
 * @tc.desc: test OH_ImageSource2_ModifyImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_ModifyImageProperty, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ModifyImageProperty start";
    OH_ImageSource *imageSource = nullptr;
    Image_String* key = nullptr;
    Image_String* value = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_ModifyImageProperty(imageSource, key, value);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_ModifyImageProperty end";
}

/**
 * @tc.name: OH_ImageSource2_GetFrameCount
 * @tc.desc: test OH_ImageSource2_GetFrameCount
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_GetFrameCount, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetFrameCount start";
    OH_ImageSource *imageSource = nullptr;
    uint32_t* res = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_GetFrameCount(imageSource, res);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_GetFrameCount end";
}

/**
 * @tc.name: OH_ImageSource2_Release
 * @tc.desc: test OH_ImageSource2_Release
 * @tc.type: FUNC
 */
HWTEST_F(ImagSourceNdk2Test, OH_ImageSource2_Release, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_Release start";
    OH_ImageSource *imageSource = nullptr;
    Image_ErrorCode ret = OH_ImageSource2_Release(imageSource);
    ASSERT_EQ(ret, IMAGE_RESULT_BAD_PARAMETER);
    GTEST_LOG_(INFO) << "ImagSourceNdk2Test: OH_ImageSource2_Release end";
}

}
}
