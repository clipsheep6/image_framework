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


#include <memory>
#include <gtest/gtest.h>

#include "file_image_stream.h"
#include "image_log.h"
#include "media_errors.h"
#include "png_image_accessor.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {
namespace {
    static const std::string IMAGE_INPUT_NOEXIF_PNG_PATH = "/data/local/tmp/image/test_noexif.png";
    static const std::string IMAGE_INPUT_EXIF_PNG_PATH = "/data/local/tmp/image/test_exif.png";
    static const std::string IMAGE_INPUT_TEXTCHUNK_PNG_PATH = "/data/local/tmp/image/test_textchunk.png";
    static const std::string IMAGE_INPUT_CHUNKTYPETEXT_PNG_PATH = "/data/local/tmp/image/test_chunktypetext.png";
    static const std::string IMAGE_INPUT_NOEXIST_PATH = "";
}
class PngImageAccessorTest : public testing::Test {
public:
    PngImageAccessorTest() {}
    ~PngImageAccessorTest() {}
    std::string GetProperty(const std::shared_ptr<ExifMetadata>& metadata, const std::string& prop);
};

std::string PngImageAccessorTest::GetProperty(const std::shared_ptr<ExifMetadata>& metadata, const std::string& prop)
{
    std::string value;
    metadata->GetValue(prop, value);
    return value;
}

/**
 * @tc.name: ReadMetadata001
 * @tc.desc: test ReadMetadata using PNG without exif
 * @tc.type: FUNC
 */
HWTEST_F(PngImageAccessorTest, ReadMetadata001, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT_NOEXIF_PNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    PngImageAccessor imageAccessor(stream);
    int result = imageAccessor.ReadMetadata();
    ASSERT_EQ(result, ERR_IMAGE_SOURCE_DATA);
}

/**
 * @tc.name: ReadMetadata002
 * @tc.desc: test ReadMetadata using PNG with exif
 * @tc.type: FUNC
 */
HWTEST_F(PngImageAccessorTest, ReadMetadata002, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT_EXIF_PNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    PngImageAccessor imageAccessor(stream);
    int result = imageAccessor.ReadMetadata();
    ASSERT_EQ(result, SUCCESS);

    auto exifMetadata = imageAccessor.GetExifMetadata();
    ASSERT_EQ(GetProperty(exifMetadata, "XResolution"), "72");
    ASSERT_EQ(GetProperty(exifMetadata, "ResolutionUnit"), "Inch");
    ASSERT_EQ(GetProperty(exifMetadata, "Software"), "Adobe Photoshop CS Windows");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTime"), "2015:11:05 23:04:30");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrPositioning"), "Centered");
    ASSERT_EQ(GetProperty(exifMetadata, "ColorSpace"), "Uncalibrated");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "1080");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "1920");
}

/**
 * @tc.name: ReadMetadata003
 * @tc.desc: test ReadMetadata
 * @tc.type: FUNC
 */
HWTEST_F(PngImageAccessorTest, ReadMetadata003, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT_TEXTCHUNK_PNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    PngImageAccessor imageAccessor(stream);
    int result = imageAccessor.ReadMetadata();
    ASSERT_EQ(result, SUCCESS);

    auto exifMetadata = imageAccessor.GetExifMetadata();
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "200");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "130");
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "8, 8, 8");
    ASSERT_EQ(GetProperty(exifMetadata, "Compression"), "Uncompressed");
    ASSERT_EQ(GetProperty(exifMetadata, "Make"), "NIKON CORPORATION");
    ASSERT_EQ(GetProperty(exifMetadata, "Model"), "NIKON D1X");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Top-left");
    ASSERT_EQ(GetProperty(exifMetadata, "SamplesPerPixel"), "4");
    ASSERT_EQ(GetProperty(exifMetadata, "XResolution"), "300");
    ASSERT_EQ(GetProperty(exifMetadata, "YResolution"), "300");
    ASSERT_EQ(GetProperty(exifMetadata, "PlanarConfiguration"), "Planar format");
    ASSERT_EQ(GetProperty(exifMetadata, "ResolutionUnit"), "Inch");
    ASSERT_EQ(GetProperty(exifMetadata, "Software"), "GIMP 2.9.5");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTime"), "2016:09:15 06:29:23");
    ASSERT_EQ(GetProperty(exifMetadata, "ExifTag"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/125 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "FNumber"), "f/5.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureProgram"), "Manual");
    ASSERT_EQ(GetProperty(exifMetadata, "ExifVersion"), "Exif Version 2.2");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2004:06:21 23:37:53");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeDigitized"), "2004:06:21 23:37:53");
    ASSERT_EQ(GetProperty(exifMetadata, "ComponentsConfiguration"), "Y Cb Cr -");
    ASSERT_EQ(GetProperty(exifMetadata, "CompressedBitsPerPixel"), " 4");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureBiasValue"), "0.33 EV");
    ASSERT_EQ(GetProperty(exifMetadata, "MaxApertureValue"), "3.00 EV (f/2.8)");
    ASSERT_EQ(GetProperty(exifMetadata, "MeteringMode"), "Center-weighted average");
    ASSERT_EQ(GetProperty(exifMetadata, "LightSource"), "Cloudy weather");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Flash did not fire");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLength"), "42.0 mm");
}

/**
 * @tc.name: ReadMetadata004
 * @tc.desc: test ReadMetadata
 * @tc.type: FUNC
 */
HWTEST_F(PngImageAccessorTest, ReadMetadata004, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT_TEXTCHUNK_PNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    PngImageAccessor imageAccessor(stream);
    int result = imageAccessor.ReadMetadata();
    ASSERT_EQ(result, SUCCESS);

    auto exifMetadata = imageAccessor.GetExifMetadata();
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTime"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeOriginal"), "06");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeDigitized"), "06");
    ASSERT_EQ(GetProperty(exifMetadata, "FlashpixVersion"), "FlashPix Version 1.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ColorSpace"), "sRGB");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "200");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "130");
    ASSERT_EQ(GetProperty(exifMetadata, "SensingMethod"), "One-chip color area sensor");
    ASSERT_EQ(GetProperty(exifMetadata, "FileSource"), "DSC");
    ASSERT_EQ(GetProperty(exifMetadata, "SceneType"), "Directly photographed");
    ASSERT_EQ(GetProperty(exifMetadata, "CustomRendered"), "Normal process");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureMode"), "Manual exposure");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Manual white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "DigitalZoomRatio"), " 1");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "63");
    ASSERT_EQ(GetProperty(exifMetadata, "SceneCaptureType"), "Standard");
    ASSERT_EQ(GetProperty(exifMetadata, "GainControl"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Contrast"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Saturation"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Sharpness"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistanceRange"), "Unknown");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageUniqueID"), "127c1377b054a3f65bf2754ebb24e7f2");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTag"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSVersionID"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "PhotometricInterpretation"), "YCbCr");
}

/**
 * @tc.name: ReadMetadata005
 * @tc.desc: test ReadMetadata
 * @tc.type: FUNC
 */
HWTEST_F(PngImageAccessorTest, ReadMetadata005, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT_CHUNKTYPETEXT_PNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    PngImageAccessor imageAccessor(stream);
    int result = imageAccessor.ReadMetadata();
    ASSERT_EQ(result, SUCCESS);

    auto exifMetadata = imageAccessor.GetExifMetadata();
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "320");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "211");
    ASSERT_EQ(GetProperty(exifMetadata, "Make"), "NIKON CORPORATION");
    ASSERT_EQ(GetProperty(exifMetadata, "Model"), "NIKON D70");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Top-left");
    ASSERT_EQ(GetProperty(exifMetadata, "XResolution"), "300");
    ASSERT_EQ(GetProperty(exifMetadata, "YResolution"), "300");
    ASSERT_EQ(GetProperty(exifMetadata, "ResolutionUnit"), "Inch");
    ASSERT_EQ(GetProperty(exifMetadata, "Software"), "digiKam-0.9.0-svn");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTime"), "2006:02:04 16:09:30");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/4 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "FNumber"), "f/22.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureProgram"), "Shutter priority");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedRatings"), "200");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2006:02:04 16:09:30");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeDigitized"), "2006:02:04 16:09:30");
    ASSERT_EQ(GetProperty(exifMetadata, "ShutterSpeedValue"), "2.00 EV (1/4 sec.)");
    ASSERT_EQ(GetProperty(exifMetadata, "ApertureValue"), "8.92 EV (f/22.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureBiasValue"), "0.33 EV");
    ASSERT_EQ(GetProperty(exifMetadata, "MaxApertureValue"), "2.97 EV (f/2.8)");
    ASSERT_EQ(GetProperty(exifMetadata, "LightSource"), "Unknown");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Flash did not fire");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLength"), "50.0 mm");
    ASSERT_EQ(GetProperty(exifMetadata, "MakerNote"), "6989 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Auto white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "LightSource"), "Unknown");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "320");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "211");
    ASSERT_EQ(GetProperty(exifMetadata, "SensingMethod"), "One-chip color area sensor");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureMode"), "Auto exposure");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Auto white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "75");
    ASSERT_EQ(GetProperty(exifMetadata, "SceneCaptureType"), "Standard");
    ASSERT_EQ(GetProperty(exifMetadata, "Contrast"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Saturation"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Sharpness"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistanceRange"), "Unknown");
    ASSERT_EQ(GetProperty(exifMetadata, "Compression"), "JPEG compression");
}

/**
 * @tc.name: ReadExifBlob001
 * @tc.desc: test ReadExifBlob using PNG with exif
 * @tc.type: FUNC
 */
HWTEST_F(PngImageAccessorTest, ReadExifBlob001, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT_EXIF_PNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    PngImageAccessor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadExifBlob(exifBuf);
    ASSERT_EQ(result, true);
    ASSERT_EQ(exifBuf.Size(), 4244);
}

/**
 * @tc.name: ReadExifBlob002
 * @tc.desc: test ReadExifBlob using PNG without exif
 * @tc.type: FUNC
 */
HWTEST_F(PngImageAccessorTest, ReadExifBlob002, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT_NOEXIF_PNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    PngImageAccessor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadExifBlob(exifBuf);
    ASSERT_EQ(result, false);
}

} // namespace Multimedia
} // namespace OHOS