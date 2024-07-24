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
 * @tc.name: OH_PictureNative_CreatePicture
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_PictureNative_CreatePicture, TestSize.Level3)
{
    OH_PixelmapNative *mainPixelmap = nullptr;
    OH_PictureNative *picture = nullptr;
    Image_ErrorCode ret = OH_PictureNative_CreatePicture(mainPixelmap, &picture);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_PictureNative_GetMainPixelmap
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_PictureNative_GetMainPixelmap, TestSize.Level3)
{
    OH_PictureNative *picture = nullptr;
    OH_PixelmapNative *mainPixelmap = nullptr;
    Image_ErrorCode ret = OH_PictureNative_GetMainPixelmap(picture, &mainPixelmap);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_PictureNative_GetGainmapPixelmap
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_PictureNative_GetGainmapPixelmap, TestSize.Level3)
{
    OH_PictureNative *picture = nullptr;
    OH_PixelmapNative *mainPixelmap = nullptr;
    Image_ErrorCode ret = OH_PictureNative_GetGainmapPixelmap(picture, &mainPixelmap);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_PictureNative_SetAuxiliaryPicture
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_PictureNative_SetAuxiliaryPicture, TestSize.Level3)
{
    OH_PictureNative *picture = nullptr;
    OH_PixelmapNative *gainmapPixelmap = nullptr;
    Image_ErrorCode ret = OH_PictureNative_GetGainmapPixelmap(picture, &gainmapPixelmap);
    ASSERT_EQ(ret, IMAGE_BAD_PARAMETER);
}

/**
 * @tc.name: OH_PictureNative_GetAuxiliaryPicture
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_PictureNative_GetAuxiliaryPicture, TestSize.Level3)
{

}

/**
 * @tc.name: OH_PictureNative_Release
 * @tc.desc: 
 * @tc.type: FUNC
 */
HWTEST_F(PictureNdkTest, OH_PictureNative_Release, TestSize.Level3)
{

}
} // namespace Media
} // namespace OHOS