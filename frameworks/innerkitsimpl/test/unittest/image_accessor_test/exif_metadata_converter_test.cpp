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
#include "exif_metadata_converter.h"
#include "image_log.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {
static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test_exif.jpg";

class MetadataConverterTest : public testing::Test {
public:
    MetadataConverterTest() {}
    ~MetadataConverterTest() {}
};

std::string MODIFYDATA[][3] = {
    {"BitsPerSample", "9 9 8", "9, 9, 8"},
    {"BitsPerSample", "9,9,8", "9, 9, 8"},
    {"Orientation", "1", "Top-left"},
    {"ImageLength", "1000", "1000"},
    {"ImageWidth", "1001", "1001"},
    {"CompressedBitsPerPixel", "24/1", "24"},
    {"GPSLatitude", "39,54,20", "39, 54, 20"},
    {"GPSLatitude", "39/1 54/1 20/1", "39, 54, 20"},
    {"GPSLongitude", "120,52,26", "120, 52, 26"},
    {"GPSLatitudeRef", "N", "N"},
    {"GPSLongitudeRef", "E", "E"},
    {"WhiteBalance", "1", "Manual white balance"},
    {"FocalLengthIn35mmFilm", "2", "2"},
    {"Flash", "5", "Strobe return light not detected"},
    {"ApertureValue", "4/1", "4.00 EV (f/4.0)"},
    {"DateTimeOriginal", "2024:01:25 05:51:34", "2024:01:25 05:51:34"},
    {"DateTime", "2024:01:25 05:51:34", "2024:01:25 05:51:34"},
    {"DateTime", "2024:01:25", "2024:01:25"},
    {"ExposureBiasValue", "23/1", "23.00 EV"},
    {"ExposureTime", "1/34", "1/34 sec."},
    {"FNumber", "3/1", "f/3.0"},
    {"FocalLength", "31/1", "31.0 mm"},
    {"GPSTimeStamp", "11/1 37/1 56/1", "11:37:56.00"},
    {"ImageDescription", "_cuva", "_cuva"},
    {"ISOSpeedRatings", "160", "160"},
    {"LightSource", "2", "Fluorescent"},
    {"MeteringMode", "5", "Pattern"},
    {"Model", "TNY-AL00", "TNY-AL00"},
    {"PixelXDimension", "1000", "1000"},
    {"PixelYDimension", "2000", "2000"},
    {"RecommendedExposureIndex", "241", "241"},
    {"SceneType", "1", "Internal error (unknown value 49)"},
    {"SensitivityType", "5", "Standard output sensitivity (SOS) and ISO speed"},
    {"StandardOutputSensitivity", "5", "5"},
    {"UserComment", "comm", "comm"},
};

HWTEST_F(MetadataConverterTest, Validate001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MetadataConverterTest: Validate001 start";
    ASSERT_EQ(ExifMetadataConverter::Validate("BitsPerSample", "9,9,8"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("Orientation", "1"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("ImageLength", "100"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("ImageWidth", "100"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("GPSLatitude", "39,54"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("GPSLatitude", "39,54,20"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("GPSLongitude", "39,54"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("GPSLongitude", "39,54,20"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("GPSLatitudeRef", "N"), 0);
    ASSERT_EQ(ExifMetadataConverter::Validate("GPSLongitudeRef", "E"), 0);
    GTEST_LOG_(INFO) << "MetadataConverterTest: Validate001 end";
}

HWTEST_F(MetadataConverterTest, Validate002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "MetadataConverterTest: Validate002 start";
    ASSERT_NE(ExifMetadataConverter::Validate("BitsPerSample", "xx"), 0);
    ASSERT_NE(ExifMetadataConverter::Validate("ImageLength", "XXX"), 0);
    ASSERT_NE(ExifMetadataConverter::Validate("GPSLatitudeRef", "C"), 0);
    ASSERT_NE(ExifMetadataConverter::Validate("GPSLongitudeRef", "C"), 0);
    GTEST_LOG_(INFO) << "MetadataConverterTest: Validate002 end";
}
} // namespace Multimedia
} // namespace OHOS
