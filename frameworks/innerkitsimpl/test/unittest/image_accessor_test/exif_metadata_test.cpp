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

HWTEST_F(ExifMetadataTest, SetExifValue001, TestSize.Level3)
{
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_PATH.c_str());
    ExifMetadata metadata(exifData);
    ASSERT_EQ(metadata.SetExifValue("BitsPerSample", "9,9,8"), true);
    ASSERT_EQ(metadata.SetExifValue("Orientation", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("ImageLength", "1000"), true);
    ASSERT_EQ(metadata.SetExifValue("ImageWidth", "1001"), true);
    ASSERT_EQ(metadata.SetExifValue("GPSLatitude", "39,54,20"), true);
    ASSERT_EQ(metadata.SetExifValue("GPSLongitude", "120,52,26"), true);
    ASSERT_EQ(metadata.SetExifValue("GPSLatitudeRef", "N"), true);
    ASSERT_EQ(metadata.SetExifValue("GPSLongitudeRef", "E"), true);
    ASSERT_EQ(metadata.SetExifValue("WhiteBalance", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("FocalLengthIn35mmFilm", "2"), true);
    ASSERT_EQ(metadata.SetExifValue("Flash", "5"), true);
    ASSERT_EQ(metadata.SetExifValue("ApertureValue", "4/1"), true);
    ASSERT_EQ(metadata.SetExifValue("DateTimeOriginal", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetExifValue("DateTime", "2024:01:25 05:51:34"), true);
    ASSERT_EQ(metadata.SetExifValue("ExposureBiasValue", "23/1"), true);
    ASSERT_EQ(metadata.SetExifValue("ExposureTime", "1/34"), true);
    ASSERT_EQ(metadata.SetExifValue("FNumber", "3/1"), true);
    ASSERT_EQ(metadata.SetExifValue("FocalLength", "31/1"), true);
    ASSERT_EQ(metadata.SetExifValue("GPSTimeStamp", "11:37:56"), true);
    ASSERT_EQ(metadata.SetExifValue("GPSDateStamp", "2024:01:25"), true);
    ASSERT_EQ(metadata.SetExifValue("ImageDescription", "_cuva"), true);
    ASSERT_EQ(metadata.SetExifValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetExifValue("ISOSpeedRatings", "160"), true);
    ASSERT_EQ(metadata.SetExifValue("LightSource", "2"), true);
    ASSERT_EQ(metadata.SetExifValue("Make", "5"), true);
    ASSERT_EQ(metadata.SetExifValue("MeteringMode", "5"), true);
    ASSERT_EQ(metadata.SetExifValue("Model", "TNY-AL00"), true);
    ASSERT_EQ(metadata.SetExifValue("PixelXDimension", "1000"), true);
    ASSERT_EQ(metadata.SetExifValue("PixelYDimension", "2000"), true);
    ASSERT_EQ(metadata.SetExifValue("RecommendedExposureIndex", "241"), true);
    ASSERT_EQ(metadata.SetExifValue("SceneType", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("SensitivityType", "5"), true);
    ASSERT_EQ(metadata.SetExifValue("StandardOutputSensitivity", "5"), true);
    ASSERT_EQ(metadata.SetExifValue("UserComment", "comm"), true);

    ASSERT_EQ(metadata.SetExifValue("JPEGProc", "252"), true);
    ASSERT_EQ(metadata.SetExifValue("Compression", "6"), true);
    ASSERT_EQ(metadata.SetExifValue("PhotometricInterpretation", "0"), true);
    ASSERT_EQ(metadata.SetExifValue("StripOffsets", "11"), true);
    ASSERT_EQ(metadata.SetExifValue("SamplesPerPixel", "23"), true);
    ASSERT_EQ(metadata.SetExifValue("RowsPerStrip", "252"), true);
    ASSERT_EQ(metadata.SetExifValue("StripByteCounts", "252"), true);
    ASSERT_EQ(metadata.SetExifValue("XResolution", "72/1"), true);
    ASSERT_EQ(metadata.SetExifValue("YResolution", "252/1"), true);
    ASSERT_EQ(metadata.SetExifValue("PlanarConfiguration", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("ResolutionUnit", "2"), true);
    ASSERT_EQ(metadata.SetExifValue("TransferFunction", "2"), true);
    ASSERT_EQ(metadata.SetExifValue("Software", "MNA-AL00 4.0.0.120(C00E116R3P7)"), true);
    ASSERT_EQ(metadata.SetExifValue("Artist", "Joseph.Xu"), true);
    ASSERT_EQ(metadata.SetExifValue("WhitePoint", "252/1"), true);
    ASSERT_EQ(metadata.SetExifValue("PrimaryChromaticities", "124/1"), true);
    ASSERT_EQ(metadata.SetExifValue("YCbCrCoefficients", "299/1000 587/1000 114/1000"), true);
    ASSERT_EQ(metadata.SetExifValue("YCbCrSubSampling", "3 2"), true);
    ASSERT_EQ(metadata.SetExifValue("YCbCrPositioning", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("ReferenceBlackWhite", "221/1"), true);
    ASSERT_EQ(metadata.SetExifValue("Copyright", "Huawei"), true);

    ASSERT_EQ(metadata.SetExifValue("JPEGInterchangeFormat", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("JPEGInterchangeFormatLength", "111"), true);
    ASSERT_EQ(metadata.SetExifValue("ExposureProgram", "2"), true);
    ASSERT_EQ(metadata.SetExifValue("SpectralSensitivity", "sensitivity"), true);
    ASSERT_EQ(metadata.SetExifValue("OECF", "45"), true);
    ASSERT_EQ(metadata.SetExifValue("ExifVersion", "0210"), true);
    ASSERT_EQ(metadata.SetExifValue("DateTimeDigitized", "2023:01:19 10:39:58"), true);
    ASSERT_EQ(metadata.SetExifValue("ComponentsConfiguration", "1 5 6"), true);
    ASSERT_EQ(metadata.SetExifValue("ShutterSpeedValue", "13/1"), true);
    ASSERT_EQ(metadata.SetExifValue("BrightnessValue", "13/1"), true);
    ASSERT_EQ(metadata.SetExifValue("MaxApertureValue", "1/12"), true);
    ASSERT_EQ(metadata.SetExifValue("SubjectDistance", "25/1"), true);
    ASSERT_EQ(metadata.SetExifValue("SubjectArea", "10 20 183 259"), true);
    ASSERT_EQ(metadata.SetExifValue("SubsecTime", "427000"), true);
    ASSERT_EQ(metadata.SetExifValue("SubSecTimeOriginal", "427000"), true);
    ASSERT_EQ(metadata.SetExifValue("SubSecTimeDigitized", "427000"), true);
    ASSERT_EQ(metadata.SetExifValue("FlashpixVersion", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("ColorSpace", "1"), true);
    ASSERT_EQ(metadata.SetExifValue("RelatedSoundFile", "/usr/home/sound/sea.wav"), true);



}

std::string MODIFYDATA[][3] = {
    {"BitsPerSample", "9 9 8", "9, 9, 8"},
    {"BitsPerSample", "9,9,8", "9, 9, 8"},
    {"Orientation", "1", "Top-left"},
    {"ImageLength", "1000", "1000"},
    {"ImageWidth", "1001", "1001"},
    {"CompressedBitsPerPixel", "24/1", "24"},
    // GPSLatitude显示数值有问题 39, 54, 20, 0.1649189067, 0.7691442537, 1755226327/0, 0.0000000000, 553648128/0
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

HWTEST_F(ExifMetadataTest, SetExifValue002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetadataTest: SetExifValue002 start";
    auto exifData = exif_data_new_from_file(IMAGE_INPUT_JPEG_BLANKEXIF_PATH.c_str());
    if (exifData == nullptr)
    {
        GTEST_LOG_(INFO) << "ExifMetadataTest SetExifValue002 exifData is nullptr ";
    }
    
    std::string value;
    ExifMetadata metadata(exifData);

    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    // int cols = sizeof(MODIFYDATA[0]) / sizeof(MODIFYDATA[0][0]);
    for (int i = 0; i < rows; ++i) {
        std::string key = MODIFYDATA[i][0];
        std::string modifyvalue = MODIFYDATA[i][1];
        GTEST_LOG_(INFO) << "ExifMetadataTest: modifyvalue: "<< modifyvalue;
        ASSERT_EQ(metadata.SetExifValue(key, modifyvalue), true);

        std::string retvalue;
        metadata.GetValue(key, retvalue);
        GTEST_LOG_(INFO) << "ExifMetadataTest: SetExifValue002" << "key: " << key << " modifyvalue: " << modifyvalue 
            << " retvalue: " << retvalue;
        ASSERT_EQ(retvalue, MODIFYDATA[i][2]);
    }
    GTEST_LOG_(INFO) << "ExifMetadataTest: SetExifValue002 end";
}

} // namespace Multimedia
} // namespace OHOS
