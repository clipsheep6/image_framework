/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "hardware/jpeg_hw_decoder.h"
#include "mock_jpeg_hw_decode_flow.h"

namespace OHOS::Media {
using namespace testing::ext;
using namespace OHOS::ImagePlugin;
using namespace OHOS::HDI::Codec::Image::V1_0;

class JpegHwDecoderUnitTest : public testing::Test {
public:
    static constexpr char JPEG_FORMAT[] = "image/jpeg";
    static constexpr char HEIF_FORMAT[] = "image/heif";
    static constexpr char TEST_JPEG_IMG[] = "/data/local/tmp/image/test_hw.jpg";
};

HWTEST_F(JpegHwDecoderUnitTest, supported_img_inner_size, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 2736,
        .height = 3648
    };
    bool ret = testObj.IsHardwareDecodeSupported(JPEG_FORMAT, srcImgSize);
    ASSERT_TRUE(ret);
}

HWTEST_F(JpegHwDecoderUnitTest, supported_img_lower_bound_size, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 512,
        .height = 512
    };
    bool ret = testObj.IsHardwareDecodeSupported(JPEG_FORMAT, srcImgSize);
    ASSERT_TRUE(ret);
}

HWTEST_F(JpegHwDecoderUnitTest, supported_img_upper_bound_size, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 8192,
        .height = 8192
    };
    bool ret = testObj.IsHardwareDecodeSupported(JPEG_FORMAT, srcImgSize);
    ASSERT_TRUE(ret);
}

HWTEST_F(JpegHwDecoderUnitTest, unsupported_img_empty_format, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 8192,
        .height = 8192
    };
    bool ret = testObj.IsHardwareDecodeSupported("", srcImgSize);
    ASSERT_FALSE(ret);
}

HWTEST_F(JpegHwDecoderUnitTest, unsupported_img_unknown_format, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 512,
        .height = 512
    };
    bool ret = testObj.IsHardwareDecodeSupported(HEIF_FORMAT, srcImgSize);
    ASSERT_FALSE(ret);
}

HWTEST_F(JpegHwDecoderUnitTest, unsupported_img_size_too_small, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 511,
        .height = 512
    };
    bool ret = testObj.IsHardwareDecodeSupported(JPEG_FORMAT, srcImgSize);
    ASSERT_FALSE(ret);
}

HWTEST_F(JpegHwDecoderUnitTest, unsupported_img_size_too_big, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 8192,
        .height = 8193
    };
    bool ret = testObj.IsHardwareDecodeSupported(JPEG_FORMAT, srcImgSize);
    ASSERT_FALSE(ret);
}

} // namespace OHOS::Media