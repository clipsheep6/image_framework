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

static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test_metadata.jpg";
static const std::string IMAGE_INPUT_JPEG_BLANKEXIF_PATH = "/data/local/tmp/image/test_exif_blank.jpg";
static const std::string IMAGE_INPUT_JPEG_HW_PATH = "/data/local/tmp/image/test_hwkey.jpg";

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
    ASSERT_EQ(metadata.SetValue("GPSTimeStamp", "11:37:56"), true);
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

HWTEST_F(ExifMetadataTest, GetValue033, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_HW_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    metadata.GetValue("HwScenePointer", value);
    ASSERT_EQ(value, "256");
    metadata.GetValue("HwSceneVersion", value);
    ASSERT_EQ(value, "1");
    metadata.GetValue("HwSceneFoodConf", value);
    ASSERT_EQ(value, "2");
    metadata.GetValue("HwSceneStageConf", value);
    ASSERT_EQ(value, "3");
    metadata.GetValue("HwSceneBlueSkyConf", value);
    ASSERT_EQ(value, "4");
    metadata.GetValue("HwSceneGreenPlantConf", value);
    ASSERT_EQ(value, "5");
    metadata.GetValue("HwSceneBeachConf", value);
    ASSERT_EQ(value, "6");
    metadata.GetValue("HwSceneSnowConf", value);
    ASSERT_EQ(value, "7");
    metadata.GetValue("HwSceneSunsetConf", value);
    ASSERT_EQ(value, "8");
    metadata.GetValue("HwSceneFlowersConf", value);
    ASSERT_EQ(value, "9");
    metadata.GetValue("HwSceneNightConf", value);
    ASSERT_EQ(value, "10");
    metadata.GetValue("HwSceneTextConf", value);
    ASSERT_EQ(value, "11");
}

HWTEST_F(ExifMetadataTest, GetValue034, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_HW_PATH.c_str());
    std::string value;
    ExifMetadata metadata(exifData);
    metadata.GetValue("HwFacePointer", value);
    ASSERT_EQ(value, "122");
    metadata.GetValue("HwFaceVersion", value);
    ASSERT_EQ(value, "1");
    metadata.GetValue("HwFaceCount", value);
    ASSERT_EQ(value, "2");
    metadata.GetValue("HwFaceConf", value);
    ASSERT_EQ(value, "3");
    metadata.GetValue("HwFaceSmileScore", value);
    ASSERT_EQ(value, "1 2 3 4 5 6 7 8");
    metadata.GetValue("HwFaceRect", value);
    ASSERT_EQ(value, "1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8");
    metadata.GetValue("HwFaceLeyeCenter", value);
    ASSERT_EQ(value, "1 2 3 4");
    metadata.GetValue("HwFaceReyeCenter", value);
    ASSERT_EQ(value, "5 6 7 8");
    metadata.GetValue("HwFaceMouthCenter", value);
    ASSERT_EQ(value, "1 2 3 4 5 6 7 8");
    metadata.GetValue("HwCaptureMode", value);
    ASSERT_EQ(value, "1");
    metadata.GetValue("HwBurstNumber", value);
    ASSERT_EQ(value, "2");
    metadata.GetValue("HwFrontCamera", value);
    ASSERT_EQ(value, "3");
    metadata.GetValue("HwRollAngle", value);
    ASSERT_EQ(value, "4");
    metadata.GetValue("HwPitchAngle", value);
    ASSERT_EQ(value, "5");
    metadata.GetValue("HwPhysicalAperture", value);
    ASSERT_EQ(value, "6");
    metadata.GetValue("HwFocusMode", value);
    ASSERT_EQ(value, "7");
}

HWTEST_F(ExifMetadataTest, SetValueBatch001, TestSize.Level3)
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
    ASSERT_EQ(metadata.SetValue("WhiteBalance", "1"), true);
    ASSERT_EQ(metadata.SetValue("FocalLengthIn35mmFilm", "2"), true);
    ASSERT_EQ(metadata.SetValue("Flash", "5"), true);
    ASSERT_EQ(metadata.SetValue("ApertureValue", "4/1"), true);
    ASSERT_EQ(metadata.SetValue("DateTimeOriginal", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetValue("DateTime", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetValue("ExposureBiasValue", "23/1"), true);
    ASSERT_EQ(metadata.SetValue("ExposureTime", "1/34"), true);
    ASSERT_EQ(metadata.SetValue("FNumber", "3/1"), true);
    ASSERT_EQ(metadata.SetValue("FocalLength", "31/1"), true);
    ASSERT_EQ(metadata.SetValue("GPSTimeStamp", "11:37:56"), true);
    ASSERT_EQ(metadata.SetValue("GPSDateStamp", "2024:01:25"), true);
    ASSERT_EQ(metadata.SetValue("ImageDescription", "_cuva"), true);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetValue("LightSource", "2"), true);
    ASSERT_EQ(metadata.SetValue("Make", "5"), true);
    ASSERT_EQ(metadata.SetValue("MeteringMode", "5"), true);
    ASSERT_EQ(metadata.SetValue("Model", "TNY-AL00"), true);
    ASSERT_EQ(metadata.SetValue("PixelXDimension", "1000"), true);
    ASSERT_EQ(metadata.SetValue("PixelYDimension", "2000"), true);
    ASSERT_EQ(metadata.SetValue("RecommendedExposureIndex", "241"), true);
    ASSERT_EQ(metadata.SetValue("SceneType", "1"), true);
    ASSERT_EQ(metadata.SetValue("SensitivityType", "5"), true);
    ASSERT_EQ(metadata.SetValue("StandardOutputSensitivity", "5"), true);
    ASSERT_EQ(metadata.SetValue("UserComment", "comm"), true);
    ASSERT_EQ(metadata.SetValue("JPEGProc", "252"), true);
    ASSERT_EQ(metadata.SetValue("Compression", "6"), true);
    ASSERT_EQ(metadata.SetValue("PhotometricInterpretation", "0"), true);
    ASSERT_EQ(metadata.SetValue("StripOffsets", "11"), true);
    ASSERT_EQ(metadata.SetValue("SamplesPerPixel", "23"), true);
    ASSERT_EQ(metadata.SetValue("RowsPerStrip", "252"), true);
    ASSERT_EQ(metadata.SetValue("StripByteCounts", "252"), true);
}

HWTEST_F(ExifMetadataTest, SetValueBatch002, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetValue("XResolution", "72/1"), true);
    ASSERT_EQ(metadata.SetValue("YResolution", "252/1"), true);
    ASSERT_EQ(metadata.SetValue("PlanarConfiguration", "1"), true);
    ASSERT_EQ(metadata.SetValue("ResolutionUnit", "2"), true);
    ASSERT_EQ(metadata.SetValue("TransferFunction", "2"), true);
    ASSERT_EQ(metadata.SetValue("Software", "MNA-AL00 4.0.0.120(C00E116R3P7)"), true);
    ASSERT_EQ(metadata.SetValue("Artist", "Joseph.Xu"), true);
    ASSERT_EQ(metadata.SetValue("WhitePoint", "252/1"), true);
    ASSERT_EQ(metadata.SetValue("PrimaryChromaticities", "124/1"), true);
    ASSERT_EQ(metadata.SetValue("YCbCrCoefficients", "299/1000 587/1000 114/1000"), true);
    ASSERT_EQ(metadata.SetValue("YCbCrSubSampling", "3 2"), true);
    ASSERT_EQ(metadata.SetValue("YCbCrPositioning", "1"), true);
    ASSERT_EQ(metadata.SetValue("ReferenceBlackWhite", "221/1"), true);
    ASSERT_EQ(metadata.SetValue("Copyright", "Huawei"), true);
    ASSERT_EQ(metadata.SetValue("JPEGInterchangeFormat", "1"), true);
    ASSERT_EQ(metadata.SetValue("JPEGInterchangeFormatLength", "111"), true);
    ASSERT_EQ(metadata.SetValue("ExposureProgram", "2"), true);
    ASSERT_EQ(metadata.SetValue("SpectralSensitivity", "sensitivity"), true);
    ASSERT_EQ(metadata.SetValue("OECF", "45"), true);
    ASSERT_EQ(metadata.SetValue("ExifVersion", "0210"), true);
    ASSERT_EQ(metadata.SetValue("DateTimeDigitized", "2023:01:19 10:39:58"), true);
    ASSERT_EQ(metadata.SetValue("ComponentsConfiguration", "1 5 6"), true);
    ASSERT_EQ(metadata.SetValue("ShutterSpeedValue", "13/1"), true);
    ASSERT_EQ(metadata.SetValue("BrightnessValue", "13/1"), true);
    ASSERT_EQ(metadata.SetValue("MaxApertureValue", "1/12"), true);
    ASSERT_EQ(metadata.SetValue("SubjectDistance", "25/1"), true);
    ASSERT_EQ(metadata.SetValue("SubjectArea", "10 20 183 259"), true);
    ASSERT_EQ(metadata.SetValue("SubsecTime", "427000"), true);
    ASSERT_EQ(metadata.SetValue("SubSecTimeOriginal", "427000"), true);
    ASSERT_EQ(metadata.SetValue("SubSecTimeDigitized", "427000"), true);
    ASSERT_EQ(metadata.SetValue("FlashpixVersion", "1"), true);
    ASSERT_EQ(metadata.SetValue("ColorSpace", "1"), true);
    ASSERT_EQ(metadata.SetValue("RelatedSoundFile", "/usr/home/sound/sea.wav"), true);
}

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
    {"JPEGProc", "252", "3 bytes undefined data"},
    {"Compression", "6", "JPEG compression"},
    {"PhotometricInterpretation", "0", "Reversed mono"},
    {"StripOffsets", "11", "2 bytes undefined data"},
    {"SamplesPerPixel", "23", "23"},
    {"RowsPerStrip", "252", "3 bytes undefined data"},
    {"StripByteCounts", "252", "3 bytes undefined data"},
    {"XResolution", "72/1", "72"},
    {"YResolution", "252/1", "252"},
    {"PlanarConfiguration", "1", "Planar format"},
    {"ResolutionUnit", "2", "Inch"},
    {"TransferFunction", "2", "2"},
    {"Software", "MNA-AL00 4.0.0.120(C00E116R3P7)", "MNA-AL00 4.0.0.120(C00E116R3P7)"},
    {"Artist", "Joseph.Xu", "Joseph.Xu"},
    {"WhitePoint", "252/1", "252, 0/0"},
    {"PrimaryChromaticities", "124/1", "124"},
    {"YCbCrCoefficients", "299/1000 587/1000 114/1000", "26 bytes undefined data"},
    {"YCbCrSubSampling", "3 2", "3, 2"},
    {"YCbCrPositioning", "1", "Centered"},
    {"ReferenceBlackWhite", "221/1", "221, 255,  0, 255,  0, 255"},
    {"Copyright", "Huawei", "Huawei (Photographer) - [None] (Editor)"},
    {"SubsecTime", "427000", "427000"},
    {"SubSecTimeOriginal", "427000", "427000"},
    {"SubSecTimeDigitized", "427000", "427000"},
    {"FlashpixVersion", "1", "1"},
    {"ColorSpace", "2", "Adobe RGB"},
    {"RelatedSoundFile", "/usr/home/sound/sea.wav", "/usr/home/sound/sea.wav"},
    {"FlashEnergy", "832/1", "832/1"},
    {"SpatialFrequencyResponse", "13", "13"},
    {"FocalPlaneXResolution", "1080/1", "1080"},
    {"FocalPlaneYResolution", "880/1", "880"},
    {"FocalPlaneResolutionUnit", "3", "cm"},
    {"SubjectLocation", "3", "3"},
    {"ExposureIndex", "3/2", "3/2"},
    {"SensingMethod", "3", "Two-chip color area"},
    {"FileSource", "3", "Digital still camera"},
    {"CFAPattern", "3", "3"},
    {"CustomRendered", "1", "Custom process"},
    {"ExposureMode", "0", "Auto"},
    {"DigitalZoomRatio", "321/1", "321.0"},
    {"SceneCaptureType", "0", "Standard"},
    {"GainControl", "0", "None"},
    {"Contrast", "0", "Normal"},
    {"Saturation", "0", "Normal"},
    {"Sharpness", "0", "Normal"},
    {"DeviceSettingDescription", "2", "2"},
    {"SubjectDistanceRange", "0", "Unknown"},
    {"ImageUniqueID", "FXIC012", "7 bytes undefined data"},
    {"GPSVersionID", "2 2 0 0", "2 2 0 0"}, //TODO fix
    {"GPSAltitudeRef", "1", "1"}, //TODO fix
    {"GPSAltitude", "0/100", "0.00"},
    {"GPSSatellites", "xxx", "xxx"},
    {"GPSStatus", "A", "A"},
    {"GPSMeasureMode", "2", "2"},
    {"GPSDOP", "182/1", "182"},
    {"GPSSpeedRef", "K", "k"},
    {"GPSSpeed", "150/1", "150"},
    {"GPSTrackRef", "T", "T"},
    {"GPSTrack", "114/3", "38.0"},
    {"GPSImgDirectionRef", "M", "M"},
    {"GPSImgDirection", "125/56", "2.23"},
    {"GPSMapDatum", "xxxx", "xxxx"},
    {"GPSDestLatitudeRef", "N", "N"},
    {"GPSDestLatitude", "33/1 22/1 11/1", "33, 22, 11"},
    {"GPSDestLongitudeRef", "E", "E"},
    {"GPSDestLongitude", "33/1 22/1 11/1", "33, 22, 11"},
    {"GPSDestBearingRef", "T", "T"},
    {"GPSDestBearing", "22/11", "2.0"},
    {"GPSDestDistanceRef", "N", "N"},
    {"GPSDestDistance", "10/1", "10"},
    {"GPSProcessingMethod", "CELLID", "6 bytes undefined data"},
    {"GPSAreaInformation", "23、15、57", "12 bytes undefined data"},
    {"GPSDifferential", "0", "0"},
};

HWTEST_F(ExifMetadataTest, SetValueBatch003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetadataTest: SetValueBatch003 start";
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_BLANKEXIF_PATH.c_str());
    if (exifData == nullptr)
    {
        GTEST_LOG_(INFO) << "ExifMetadataTest SetValueBatch003 exifData is nullptr ";
    }
    
    std::string value;
    ExifMetadata metadata(exifData);

    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    for (int i = 0; i < rows; ++i) {
        std::string key = MODIFYDATA[i][0];
        std::string modifyvalue = MODIFYDATA[i][1];
        GTEST_LOG_(INFO) << "ExifMetadataTest: modifyvalue: "<< modifyvalue;
        ASSERT_EQ(metadata.SetValue(key, modifyvalue), true);

        std::string retvalue;
        metadata.GetValue(key, retvalue);
        GTEST_LOG_(INFO) << "[key]: " << key << " [modifyvalue]: " << modifyvalue 
            << " [retvalue]: " << retvalue;
        // ASSERT_EQ(retvalue, MODIFYDATA[i][2]);
    }
    GTEST_LOG_(INFO) << "ExifMetadataTest: SetValueBatch003 end";
}

} // namespace Multimedia
} // namespace OHOS
