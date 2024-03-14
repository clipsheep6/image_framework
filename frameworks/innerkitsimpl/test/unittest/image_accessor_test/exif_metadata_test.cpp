/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "tiff_parser.h"
#include "exif_metadata.h"
#include "image_log.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {

static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test.jpg";

class ExifMetadataTest : public testing::Test {
public:
    ExifMetadataTest() {}
    ~ExifMetadataTest() {}
};

HWTEST_F(ExifMetadataTest, SetValue001, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("BitsPerSample", "9,9,8"), true);
    ASSERT_EQ(metadata.SetValue("Orientation", "1"), true);
    ASSERT_EQ(metadata.SetValue("ImageLength", "1000"), true);
    ASSERT_EQ(metadata.SetValue("ImageWidth", "1001"), true);
    ASSERT_EQ(metadata.SetValue("GPSLatitude", "39,54,20"), true);
    ASSERT_EQ(metadata.SetValue("GPSLongitude", "120,52,26"), true);
    ASSERT_EQ(metadata.SetValue("GPSLatitudeRef", "N"), true);
    ASSERT_EQ(metadata.SetValue("GPSLongitudeRef", "E"), true);
    ASSERT_EQ(metadata.SetValue("DateTimeOriginal", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetValue("ExposureTime", "1/34"), true);
    ASSERT_EQ(metadata.SetValue("SceneType", "1"), true);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetValue("FNumber", "3/1"), true);
    ASSERT_EQ(metadata.SetValue("DateTime", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetValue("GPSTimeStamp", "11:37:56.00"), true);
    ASSERT_EQ(metadata.SetValue("ImageDescription", "_cuva"), true);
    ASSERT_EQ(metadata.SetValue("Model", "TNY-AL00"), true);
    ASSERT_EQ(metadata.SetValue("SensitivityType", "5"), true);
    ASSERT_EQ(metadata.SetValue("StandardOutputSensitivity", "5"), true);
    ASSERT_EQ(metadata.SetValue("RecommendedExposureIndex", "241"), true);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetValue("ApertureValue", "4/1"), true);
    ASSERT_EQ(metadata.SetValue("ExposureBiasValue", "23/1"), true);
    ASSERT_EQ(metadata.SetValue("MeteringMode", "5"), true);
    ASSERT_EQ(metadata.SetValue("LightSource", "2"), true);
    ASSERT_EQ(metadata.SetValue("Flash", "5"), true);
    ASSERT_EQ(metadata.SetValue("FocalLength", "31/1"), true);
    ASSERT_EQ(metadata.SetValue("UserComment", "comm"), true);
    ASSERT_EQ(metadata.SetValue("PixelXDimension", "1000"), true);
    ASSERT_EQ(metadata.SetValue("PixelYDimension", "2000"), true);
    ASSERT_EQ(metadata.SetValue("WhiteBalance", "1"), true);
    ASSERT_EQ(metadata.SetValue("FocalLengthIn35mmFilm", "2"), true);
}

HWTEST_F(ExifMetadataTest, GetValue001, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("BitsPerSample", "9,9,8"), true);
    ASSERT_EQ(metadata.SetValue("Orientation", "1"), true);
    ASSERT_EQ(metadata.SetValue("ImageLength", "1000"), true);
    ASSERT_EQ(metadata.SetValue("ImageWidth", "1001"), true);
    ASSERT_EQ(metadata.SetValue("GPSLatitude", "39,54,20"), true);
    ASSERT_EQ(metadata.SetValue("GPSLongitude", "120,52,26"), true);
    ASSERT_EQ(metadata.SetValue("GPSLatitudeRef", "N"), true);
    ASSERT_EQ(metadata.SetValue("GPSLongitudeRef", "E"), true);
    ASSERT_EQ(metadata.SetValue("DateTimeOriginal", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetValue("ExposureTime", "1/34"), true);
    ASSERT_EQ(metadata.SetValue("SceneType", "1"), true);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetValue("FNumber", "3/1"), true);
    ASSERT_EQ(metadata.SetValue("DateTime", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetValue("GPSTimeStamp", "11:37:56.00"), true);
    ASSERT_EQ(metadata.SetValue("ImageDescription", "_cuva"), true);
    ASSERT_EQ(metadata.SetValue("Model", "TNY-AL00"), true);
    ASSERT_EQ(metadata.SetValue("SensitivityType", "5"), true);
    ASSERT_EQ(metadata.SetValue("StandardOutputSensitivity", "5"), true);
    ASSERT_EQ(metadata.SetValue("RecommendedExposureIndex", "241"), true);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetValue("ApertureValue", "4/1"), true);
    ASSERT_EQ(metadata.SetValue("ExposureBiasValue", "23/1"), true);
    ASSERT_EQ(metadata.SetValue("MeteringMode", "5"), true);
    ASSERT_EQ(metadata.SetValue("LightSource", "2"), true);
    ASSERT_EQ(metadata.SetValue("Flash", "5"), true);
    ASSERT_EQ(metadata.SetValue("FocalLength", "31/1"), true);
    ASSERT_EQ(metadata.SetValue("UserComment", "comm2"), true);
    ASSERT_EQ(metadata.SetValue("PixelXDimension", "1000"), true);
    ASSERT_EQ(metadata.SetValue("PixelYDimension", "2000"), true);
    ASSERT_EQ(metadata.SetValue("WhiteBalance", "1"), true);
    ASSERT_EQ(metadata.SetValue("FocalLengthIn35mmFilm", "2"), true);
    metadata.GetValue("BitsPerSample",value);
    ASSERT_EQ(value, "9, 9, 8");
    metadata.GetValue("Orientation", value);
    ASSERT_EQ(value, "Top-left");
    metadata.GetValue("ImageLength", value);
    ASSERT_EQ(value, "1000");
    metadata.GetValue("ImageWidth", value);
    ASSERT_EQ(value, "1001");
    metadata.GetValue("GPSLatitude", value);
    ASSERT_EQ(value, "39, 54, 20.000000");
    metadata.GetValue("GPSLongitude", value);
    ASSERT_EQ(value, "120, 52, 26.000000");
    metadata.GetValue("GPSLatitudeRef", value);
    ASSERT_EQ(value, "N");
    metadata.GetValue("GPSLongitudeRef", value);
    ASSERT_EQ(value, "E");
    metadata.GetValue("DateTimeOriginal", value);
    ASSERT_EQ(value, "2024:01:25 05:51:34"); 
    metadata.GetValue("ExposureTime", value);
    ASSERT_EQ(value, "1/34 sec."); 
    metadata.GetValue("SceneType", value);
    ASSERT_EQ(value, "Internal error (unknown value 49)");
    metadata.GetValue("ISOSpeedRatings", value);
    ASSERT_EQ(value, "160");
    metadata.GetValue("FNumber", value);
    ASSERT_EQ(value, "f/3.0");
    metadata.GetValue("DateTime", value);
    ASSERT_EQ(value, "2024:01:25 05:51:34");
    metadata.GetValue("GPSTimeStamp", value);
    ASSERT_EQ(value, "11:37:56.00");
    metadata.GetValue("ImageDescription", value);
    ASSERT_EQ(value, "_cuva");
    metadata.GetValue("Model", value);
    ASSERT_EQ(value, "TNY-AL00");
    metadata.GetValue("SensitivityType", value);
    ASSERT_EQ(value, "Standard output sensitivity (SOS) and ISO speed");
    metadata.GetValue("StandardOutputSensitivity", value);
    ASSERT_EQ(value, "5");
    metadata.GetValue("RecommendedExposureIndex", value);
    ASSERT_EQ(value, "241");
    metadata.GetValue("ISOSpeedRatings", value);
    ASSERT_EQ(value, "160");
    metadata.GetValue("ApertureValue", value);
    ASSERT_EQ(value, "4.00 EV (f/4.0)");
    metadata.GetValue("ExposureBiasValue", value);
    ASSERT_EQ(value, "23.00 EV");
    metadata.GetValue("MeteringMode", value);
    ASSERT_EQ(value, "Pattern");
    metadata.GetValue("LightSource", value);
    ASSERT_EQ(value, "Fluorescent");
    metadata.GetValue("Flash", value);
    ASSERT_EQ(value, "Strobe return light not detected");
    metadata.GetValue("FocalLength", value);
    ASSERT_EQ(value, "31.0 mm"); 
    metadata.GetValue("UserComment", value);
    ASSERT_EQ(value, "comm2");
    metadata.GetValue("PixelXDimension", value);
    ASSERT_EQ(value, "1000");
    metadata.GetValue("PixelYDimension", value);
    ASSERT_EQ(value, "2000"); 
    metadata.GetValue("WhiteBalance", value);
    ASSERT_EQ(value, "Manual white balance");
    metadata.GetValue("FocalLengthIn35mmFilm", value);
    ASSERT_EQ(value, "2"); 
}

} // namespace Multimedia
} // namespace OHOS
