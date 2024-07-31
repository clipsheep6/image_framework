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
#include <string>
#include "pixel_yuv_ext_utils.h"

using namespace OHOS::Media;
using namespace testing::ext;
namespace OHOS {
namespace Media {
static const std::string ERROR_MESSAGE = "PixelYuvExtUtilsTest SetDecodeErrorMsg";
class PixelYuvExtUtilsTest : public testing::Test {
    public:
        PixelYuvExtUtilsTest() {}
        ~PixelYuvExtUtilsTest() {}
};

#ifdef EXT_PIXEL
static constexpr int32_t LENGTH = 8;

/**
 * @tc.name: Yuv420ToARGB_NV21
 * @tc.desc: Yuv420ToARGB NV21
 * @tc.type: FUNC
 */
HWTEST_F(PixelYuvExtUtilsTest, Yuv420ToARGB_NV21, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV21 start";
    uint8_t src[LENGTH] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t dst[LENGTH] = {0};
    Size size;
    size.width = 1;
    size.height = 1;
    PixelFormat pixelFormat = PixelFormat::NV21;
    YUVDataInfo info;
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV21 before para 1";
    bool res = PixelYuvExtUtils::Yuv420ToARGB(src, dst, size, pixelFormat, info);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV21 after para 1";
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV21 end";
}

/**
 * @tc.name: Yuv420ToARGB_NV12
 * @tc.desc: Yuv420ToARGB NV12
 * @tc.type: FUNC
 */
HWTEST_F(PixelYuvExtUtilsTest, Yuv420ToARGB_NV12, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV12 start";
    uint8_t src[LENGTH] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t dst[LENGTH] = {0};
    Size size;
    size.width = 1;
    size.height = 1;
    PixelFormat pixelFormat = PixelFormat::NV12;
    YUVDataInfo info;
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV12 before para 1";
    bool res = PixelYuvExtUtils::Yuv420ToARGB(src, dst, size, pixelFormat, info);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV12 after para 1";
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_NV12 end";
}

/**
 * @tc.name: Yuv420ToARGB_RGB888
 * @tc.desc: Yuv420ToARGB RGB_888
 * @tc.type: FUNC
 */
HWTEST_F(PixelYuvExtUtilsTest, Yuv420ToARGB_RGB888, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_RGB888 start";
    uint8_t src[LENGTH] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t dst[LENGTH] = {0};
    Size size;
    size.width = 1;
    size.height = 1;
    PixelFormat pixelFormat = PixelFormat::RGB_888;
    YUVDataInfo info;
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_RGB888 before para 1";
    bool res = PixelYuvExtUtils::Yuv420ToARGB(src, dst, size, pixelFormat, info);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_RGB888 after para 1";
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_RGB888 end";
}

/**
 * @tc.name: Yuv420ToARGB_Null
 * @tc.desc: Yuv420ToARGB null pointer
 * @tc.type: FUNC
 */
HWTEST_F(PixelYuvExtUtilsTest, Yuv420ToARGB_Null, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_Null start";
    uint8_t src[LENGTH] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    Size size;
    size.width = 1;
    size.height = 1;
    PixelFormat pixelFormat = PixelFormat::NV21;
    YUVDataInfo info;
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_Null before para 1";
    bool res = PixelYuvExtUtils::Yuv420ToARGB(src, nullptr, size, pixelFormat, info);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_Null after para 1";
    ASSERT_EQ(res, false);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: Yuv420ToARGB_Null end";
}

/**
 * @tc.name: YuvRotate_degree350
 * @tc.desc: YuvRotate degree 350
 * @tc.type: FUNC
 */
HWTEST_F(PixelYuvExtUtilsTest, YuvRotate_degree350, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: YuvRotate_degree350 start";
    uint8_t src[LENGTH] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    Size size;
    size.width = 1;
    size.height = 1;
    int32_t degrees = 350;
    PixelFormat pixelFormat = PixelFormat::RGB_888;
    YUVDataInfo info;
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: YuvRotate_degree350 before para 1";
    bool res = PixelYuvExtUtils::YuvRotate(src, size, degrees, pixelFormat, info);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: YuvRotate_degree350 after para 1";
    ASSERT_EQ(res, true);
    GTEST_LOG_(INFO) << "PixelYuvExtUtilsTest: YuvRotate_degree350 end";
}
#endif
} // namespace Multimedia
} // namespace OHOS
