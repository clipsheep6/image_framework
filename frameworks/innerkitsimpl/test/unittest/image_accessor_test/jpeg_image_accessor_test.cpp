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
#include <memory>

#include "jpeg_image_accessor.h"
#include "file_image_stream.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {

static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test.jpg";

class JpegImageAccessorTest : public testing::Test {
public:
    JpegImageAccessorTest() {}
    ~JpegImageAccessorTest() {}
};

HWTEST_F(JpegImageAccessorTest, TC001, TestSize.Level3)
{
    JpegImageAccessor imageAccessor;
    OHOS::Media::FileImageStream imageStream(IMAGE_INPUT_JPEG_PATH);
    ExifMetadata metadata = imageAccessor.ReadMetadata(imageStream);
    ASSERT_EQ(metadata.GetValue("BitsPerSample"), "9, 7, 8");
    ASSERT_EQ(metadata.GetValue("Orientation"), "Top-right");
    ASSERT_EQ(metadata.GetValue("ImageLength"), "1000");
    ASSERT_EQ(metadata.GetValue("ImageWidth"), "500");
    ASSERT_EQ(metadata.GetValue("GPSLatitude"), "39, 54, 20");
    ASSERT_EQ(metadata.GetValue("GPSLongitude"), "120, 52, 26");
    ASSERT_EQ(metadata.GetValue("GPSLatitudeRef"), "N");
    ASSERT_EQ(metadata.GetValue("GPSLongitudeRef"), "E");
    ASSERT_EQ(metadata.GetValue("DateTimeOriginal"), "2024:01:25 05:51:34");
    ASSERT_EQ(metadata.GetValue("ExposureTime"), "1/34 sec.");
    ASSERT_EQ(metadata.GetValue("SceneType"), "Directly photographed");
    ASSERT_EQ(metadata.GetValue("ISOSpeedRatings"), "160");
    ASSERT_EQ(metadata.GetValue("FNumber"), "f/3.0");
    ASSERT_EQ(metadata.GetValue("DateTime"), "");
    ASSERT_EQ(metadata.GetValue("GPSTimeStamp"), "11:37:58.00");
    ASSERT_EQ(metadata.GetValue("GPSDateStamp"), "2025:01:11");
    ASSERT_EQ(metadata.GetValue("ImageDescription"), "_cuva");
    ASSERT_EQ(metadata.GetValue("Make"), "");
    ASSERT_EQ(metadata.GetValue("Model"), "TNY-AL00");
    ASSERT_EQ(metadata.GetValue("SensitivityType"), "Standard output sensitivity (SOS) and ISO speed");
    ASSERT_EQ(metadata.GetValue("StandardOutputSensitivity"), "5");
    ASSERT_EQ(metadata.GetValue("RecommendedExposureIndex"), "241");
    ASSERT_EQ(metadata.GetValue("ISOSpeedRatings"), "160");
    ASSERT_EQ(metadata.GetValue("ApertureValue"), "4.00 EV (f/4.0)");
    ASSERT_EQ(metadata.GetValue("ExposureBiasValue"), "23.00 EV");
    ASSERT_EQ(metadata.GetValue("MeteringMode"), "Pattern");
    ASSERT_EQ(metadata.GetValue("LightSource"), "Fluorescent");
    ASSERT_EQ(metadata.GetValue("Flash"), "Strobe return light not detected");
    ASSERT_EQ(metadata.GetValue("FocalLength"), "31.0 mm");
    ASSERT_EQ(metadata.GetValue("UserComment"), "comm");
    ASSERT_EQ(metadata.GetValue("PixelXDimension"), "1000");
    ASSERT_EQ(metadata.GetValue("PixelYDimension"), "2000");
    ASSERT_EQ(metadata.GetValue("WhiteBalance"), "Manual white balance");
    ASSERT_EQ(metadata.GetValue("FocalLengthIn35mmFilm"), "26");
    ASSERT_EQ(metadata.GetValue("JPEGProc"), "252");
}

HWTEST_F(JpegImageAccessorTest, TC002, TestSize.Level3)
{
    JpegImageAccessor imageAccessor;
    OHOS::Media::FileImageStream imageStream(IMAGE_INPUT_JPEG_PATH);
    ExifMetadata metadata = imageAccessor.ReadMetadata(imageStream);
    ASSERT_EQ(metadata.GetValue("HwMnoteCaptureMode"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnotePhysicalAperture"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteRollAngle"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnotePitchAngle"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneFoodConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneStageConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneBlueSkyConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneGreenPlantConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneBeachConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneSnowConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneSunsetConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneFlowersConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneNightConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteSceneTextConf"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteFaceCount"), "1");
    ASSERT_EQ(metadata.GetValue("HwMnoteFocusMode"), "1");
}

} // namespace Multimedia
} // namespace OHOS