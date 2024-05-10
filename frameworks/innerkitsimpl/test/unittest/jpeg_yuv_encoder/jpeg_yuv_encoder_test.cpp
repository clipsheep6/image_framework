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
#include "media_errors.h"
#include "pixel_map.h"
#include "image_source.h"
#include "jpeg_encoder_yuv.h"

using namespace testing::ext;
using namespace OHOS::ImagePlugin;
using namespace OHOS::Media;

namespace OHOS {
namespace Multimedia {

static const char* IMAGE_FORMAT = "image/jpeg";
static const char* IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test_packing.jpg";

class JpgYuvEncoderTest : public testing::Test {
public:
    JpgYuvEncoderTest() {}
    ~JpgYuvEncoderTest() {}
    std::unique_ptr<PixelMap> Read(PixelFormat fmt);
    JpegYuvEncodeError Encode(PixelMap& pixelMap);
};

std::unique_ptr<PixelMap> JpgYuvEncoderTest::Read(PixelFormat fmt)
{
    uint32_t errorCode = 0;
    std::unique_ptr<PixelMap> pixelMap;
    SourceOptions opts = { .formatHint = IMAGE_FORMAT, };
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    if (errorCode != OHOS::Media::SUCCESS || !imageSource) {
        return pixelMap;
    }

    DecodeOptions decOpts = { .desiredPixelFormat = fmt, };
    pixelMap = imageSource->CreatePixelMap(decOpts, errorCode);
    return pixelMap;
}

JpegYuvEncodeError JpgYuvEncoderTest::Encode(PixelMap& pixelMap)
{
    uint8_t* jpegBuffer = nullptr;
    unsigned long jpegBufSize = 0;
    JpegYuvEncodeError errEnc = JpegYuvEncodeError_Success;
    JpegEncoderYuv encYUV(
        pixelMap.GetWidth(),
        pixelMap.GetHeight(),
        pixelMap.GetPixelFormat(),
        pixelMap.GetPixels(),
        pixelMap.GetByteCount(),
        &jpegBuffer, &jpegBufSize, errEnc);
    return errEnc;
}

/**
 * @tc.name: JpgYuvEncoderTest001
 * @tc.desc: test NV12 compress to jpeg
 * @tc.type: FUNC
 */
HWTEST_F(JpgYuvEncoderTest, JpgYuvEncoderTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest001 start";

    std::unique_ptr<PixelMap> pixelMap = Read(PixelFormat::NV12);
    ASSERT_NE(pixelMap.get(), nullptr);

    JpegYuvEncodeError err = Encode(*pixelMap);
    ASSERT_EQ(err, JpegYuvEncodeError_Success);

    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest001 end";
}

/**
 * @tc.name: JpgYuvEncoderTest002
 * @tc.desc: test NV21 compress to jpeg
 * @tc.type: FUNC
 */
HWTEST_F(JpgYuvEncoderTest, JpgYuvEncoderTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest002 start";

    std::unique_ptr<PixelMap> pixelMap = Read(PixelFormat::NV21);
    ASSERT_NE(pixelMap.get(), nullptr);

    JpegYuvEncodeError err = Encode(*pixelMap);
    ASSERT_EQ(err, JpegYuvEncodeError_Success);

    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest002 end";
}

/**
 * @tc.name: JpgYuvEncoderTest003
 * @tc.desc: test all formats excapt NV12/NV21 compress to jpeg
 * @tc.type: FUNC
 */
HWTEST_F(JpgYuvEncoderTest, JpgYuvEncoderTest003, TestSize.Level3)
{
    PixelFormat fmtList[] = {
        PixelFormat::ARGB_8888,
        PixelFormat::RGB_565,
        PixelFormat::RGBA_8888,
        PixelFormat::BGRA_8888,
        PixelFormat::RGB_888,
        PixelFormat::ALPHA_8,
        PixelFormat::RGBA_F16,
        PixelFormat::CMYK,
        PixelFormat::ASTC_4x4,
        PixelFormat::ASTC_6x6,
        PixelFormat::ASTC_8x8,
        PixelFormat::RGBA_1010102
    };

    const char* fmtString[] = {
        "ARGB_8888",
        "RGB_565",
        "RGBA_8888",
        "BGRA_8888",
        "RGB_888",
        "ALPHA_8",
        "RGBA_F16",
        "CMYK",
        "ASTC_4x4",
        "ASTC_6x6",
        "ASTC_8x8",
        "RGBA_1010102"
    };

    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest003 start";

    for (int32_t i = 0; i < sizeof(fmtList) / sizeof(PixelFormat); i++)  {
        std::unique_ptr<PixelMap> pixelMap = Read(fmtList[i]);
        GTEST_LOG_(INFO) << "format = " << fmtString[i] << " pixelmap : " << pixelMap.get();
        if (pixelMap) {
            JpegYuvEncodeError err = Encode(*pixelMap);
            GTEST_LOG_(INFO) << "encode result = " << (int32_t)err;
            ASSERT_EQ(err, JpegYuvEncodeError_UnsupportPixelFormat);
        }
    }

    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest003 end";
}

/**
 * @tc.name: JpgYuvEncoderTest004
 * @tc.desc: test bad parameter compress to jpeg
 * @tc.type: FUNC
 */
HWTEST_F(JpgYuvEncoderTest, JpgYuvEncoderTest004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest004 start";

    JpegYuvEncodeError err = JpegYuvEncodeError_Success;
    JpegEncoderYuv encYUV(0, 0, PixelFormat::UNKNOWN, 0, 0, 0, 0, err);
    ASSERT_EQ(err, JpegYuvEncodeError_BadParameter);

    GTEST_LOG_(INFO) << "JpgYuvEncoderTest: JpgYuvEncoderTest004 end";
}

} // namespace Multimedia
} // namespace OHOS