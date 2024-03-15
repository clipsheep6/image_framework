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
    metadata.GetValue("BitsPerSample", value);
    ASSERT_EQ(value, "9, 9, 8");
}

HWTEST_F(ExifMetadataTest, GetValue002, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("Orientation", "1"), true);
    metadata.GetValue("Orientation", value);
    ASSERT_EQ(value, "Top-left");
}

HWTEST_F(ExifMetadataTest, GetValue003, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ImageLength", "1000"), true);
    metadata.GetValue("ImageLength", value);
    ASSERT_EQ(value, "1000");
}

HWTEST_F(ExifMetadataTest, GetValue004, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ImageWidth", "1001"), true);
    metadata.GetValue("ImageWidth", value);
    ASSERT_EQ(value, "1001");
}

HWTEST_F(ExifMetadataTest, GetValue005, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("GPSLatitude", "39,54,20"), true);
    metadata.GetValue("GPSLatitude", value);
    ASSERT_EQ(value, "39, 54, 20.000000");
}

HWTEST_F(ExifMetadataTest, GetValue006, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("GPSLongitude", "120,52,26"), true);
    metadata.GetValue("GPSLongitude", value);
    ASSERT_EQ(value, "120, 52, 26.000000");
}

HWTEST_F(ExifMetadataTest, GetValue007, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("GPSLatitudeRef", "N"), true);
    metadata.GetValue("GPSLatitudeRef", value);
    ASSERT_EQ(value, "N");
}

HWTEST_F(ExifMetadataTest, GetValue008, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("GPSLongitudeRef", "E"), true);
    metadata.GetValue("GPSLongitudeRef", value);
    ASSERT_EQ(value, "E");
}

HWTEST_F(ExifMetadataTest, GetValue009, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("DateTimeOriginal", "2024:01:25 05:51:34"), true);
    metadata.GetValue("DateTimeOriginal", value);
    ASSERT_EQ(value, "2024:01:25 05:51:34");
}

HWTEST_F(ExifMetadataTest, GetValue010, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ExposureTime", "1/34"), true);
    metadata.GetValue("ExposureTime", value);
    ASSERT_EQ(value, "1/34 sec.");
}

HWTEST_F(ExifMetadataTest, GetValue011, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("SceneType", "1"), true);
    metadata.GetValue("SceneType", value);
    ASSERT_EQ(value, "Internal error (unknown value 49)");
}

HWTEST_F(ExifMetadataTest, GetValue012, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    metadata.GetValue("ISOSpeedRatings", value);
    ASSERT_EQ(value, "160");
}

HWTEST_F(ExifMetadataTest, GetValue013, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("FNumber", "3/1"), true);
    metadata.GetValue("FNumber", value);
    ASSERT_EQ(value, "f/3.0");
}

HWTEST_F(ExifMetadataTest, GetValue014, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("DateTime", "2024:01:25 05:51:34"), true);
    metadata.GetValue("DateTime", value);
    ASSERT_EQ(value, "2024:01:25 05:51:34");
}

HWTEST_F(ExifMetadataTest, GetValue015, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("GPSTimeStamp", "11:37:56.00"), true);
    metadata.GetValue("GPSTimeStamp", value);
    ASSERT_EQ(value, "11:37:56.00");
}

HWTEST_F(ExifMetadataTest, GetValue016, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ImageDescription", "_cuva"), true);
    metadata.GetValue("ImageDescription", value);
    ASSERT_EQ(value, "_cuva");
}

HWTEST_F(ExifMetadataTest, GetValue017, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("Model", "TNY-AL00"), true);
    metadata.GetValue("Model", value);
    ASSERT_EQ(value, "TNY-AL00");
}

HWTEST_F(ExifMetadataTest, GetValue018, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("SensitivityType", "5"), true);
    metadata.GetValue("SensitivityType", value);
    ASSERT_EQ(value, "Standard output sensitivity (SOS) and ISO speed");
}

HWTEST_F(ExifMetadataTest, GetValue019, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("StandardOutputSensitivity", "5"), true);
    metadata.GetValue("StandardOutputSensitivity", value);
    ASSERT_EQ(value, "5");
}

HWTEST_F(ExifMetadataTest, GetValue020, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("RecommendedExposureIndex", "241"), true);
    metadata.GetValue("RecommendedExposureIndex", value);
    ASSERT_EQ(value, "241");
}

HWTEST_F(ExifMetadataTest, GetValue021, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    metadata.GetValue("ISOSpeedRatings", value);
    ASSERT_EQ(value, "160");
}

HWTEST_F(ExifMetadataTest, GetValue022, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ApertureValue", "4/1"), true);
    metadata.GetValue("ApertureValue", value);
    ASSERT_EQ(value, "4.00 EV (f/4.0)");
}

HWTEST_F(ExifMetadataTest, GetValue023, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("ExposureBiasValue", "23/1"), true);
    metadata.GetValue("ExposureBiasValue", value);
    ASSERT_EQ(value, "23.00 EV");
}

HWTEST_F(ExifMetadataTest, GetValue024, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("MeteringMode", "5"), true);
    metadata.GetValue("MeteringMode", value);
    ASSERT_EQ(value, "Pattern");
}

HWTEST_F(ExifMetadataTest, GetValue025, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("LightSource", "2"), true);
    metadata.GetValue("LightSource", value);
    ASSERT_EQ(value, "Fluorescent");
}

HWTEST_F(ExifMetadataTest, GetValue026, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("Flash", "5"), true);
    metadata.GetValue("Flash", value);
    ASSERT_EQ(value, "Strobe return light not detected");
}

HWTEST_F(ExifMetadataTest, GetValue027, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("FocalLength", "31/1"), true);
    metadata.GetValue("FocalLength", value);
    ASSERT_EQ(value, "31.0 mm");
}

HWTEST_F(ExifMetadataTest, GetValue028, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("UserComment", "comm2"), true);
    metadata.GetValue("UserComment", value);
    ASSERT_EQ(value, "comm2");
}

HWTEST_F(ExifMetadataTest, GetValue029, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("PixelXDimension", "1000"), true);
    metadata.GetValue("PixelXDimension", value);
    ASSERT_EQ(value, "1000");
}

HWTEST_F(ExifMetadataTest, GetValue030, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("PixelYDimension", "2000"), true);
    metadata.GetValue("PixelYDimension", value);
    ASSERT_EQ(value, "2000");
}

HWTEST_F(ExifMetadataTest, GetValue031, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("WhiteBalance", "1"), true);
    metadata.GetValue("WhiteBalance", value);
    ASSERT_EQ(value, "Manual white balance");
}

HWTEST_F(ExifMetadataTest, GetValue032, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("FocalLengthIn35mmFilm", "2"), true);
    metadata.GetValue("FocalLengthIn35mmFilm", value);
    ASSERT_EQ(value, "2");
}

} // namespace Multimedia
} // namespace OHOS
