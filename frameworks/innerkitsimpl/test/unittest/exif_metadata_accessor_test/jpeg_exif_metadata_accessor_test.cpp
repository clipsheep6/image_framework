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

#include "file_metadata_stream.h"
#include "jpeg_exif_metadata_accessor.h"
#include "log_tags.h"
#include "media_errors.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {

namespace {
    static const std::string IMAGE_INPUT1_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readmetadata001.jpg";
    static const std::string IMAGE_INPUT2_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readmetadata003.jpg";
    static const std::string IMAGE_ERROR1_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readexifblob002.jpg";
    static const std::string IMAGE_ERROR2_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readexifblob003.jpg";
    static const std::string IMAGE_INPUT_WRITE1_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata001.jpg";
    static const std::string IMAGE_INPUT_WRITE3_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata003.jpg";
    static const std::string IMAGE_INPUT_WRITE5_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata005.jpg";
    static const std::string IMAGE_INPUT_WRITE7_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata007.jpg";
    static const std::string IMAGE_INPUT_WRITE9_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata009.jpg";
    static const std::string IMAGE_INPUT_WRITE11_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata011.jpg";
    static const std::string IMAGE_INPUT_WRITE13_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata013.jpg";
    static const std::string IMAGE_INPUT_WRITE15_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata015.jpg";
    static const std::string IMAGE_INPUT_WRITE17_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata017.jpg";
    static const std::string IMAGE_INPUT_WRITE19_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata019.jpg";
    static const std::string IMAGE_INPUT_WRITE21_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata021.jpg";
    static const std::string IMAGE_INPUT_WRITE23_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata013.jpg";
    static const std::string IMAGE_INPUT_WRITE25_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata015.jpg";
    static const std::string IMAGE_INPUT_WRITE27_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata017.jpg";
    static const std::string IMAGE_INPUT_WRITE29_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata019.jpg";
    static const std::string IMAGE_INPUT_WRITE31_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata021.jpg";
    static const std::string IMAGE_INPUT_WRITE2_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writeexifblob001.jpg";
    static const std::string IMAGE_INPUT_WRITE4_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writeexifblob003.jpg";
    static const std::string IMAGE_OUTPUT_WRITE1_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata002.jpg";
    static const std::string IMAGE_OUTPUT_WRITE2_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writeexifblob002.jpg";
    static const std::string IMAGE_OUTPUT_WRITE4_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writeexifblob004.jpg";
    static const std::string IMAGE_OUTPUT_WRITE6_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writeexifblob006.jpg";
    constexpr auto EXIF_ID = "Exif\0\0";
    constexpr auto EXIF_ID_SIZE = 6;
}

class JpegExifMetadataAccssorTest : public testing::Test {
public:
    JpegExifMetadataAccssorTest() {}
    ~JpegExifMetadataAccssorTest() {}
    std::string GetProperty(const std::shared_ptr<ExifMetadata>& metadata, const std::string& prop);
};

/**
 * @tc.name: Read001
 * @tc.desc: test the jpegDecoded Exif properties
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Read001, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    int result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "9, 7, 8");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Top-right");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "1000");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "500");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitude"), "39, 54, 20");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitude"), "120, 52, 26");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitudeRef"), "N");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitudeRef"), "E");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2024:01:25 05:51:34");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/34 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "SceneType"), "Directly photographed");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedRatings"), "160");
    ASSERT_EQ(GetProperty(exifMetadata, "FNumber"), "f/3.0");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTime"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTimeStamp"), "11:37:58.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDateStamp"), "2025:01:11");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageDescription"), "_cuva");
    ASSERT_EQ(GetProperty(exifMetadata, "Make"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "Model"), "TNY-AL00");
    ASSERT_EQ(GetProperty(exifMetadata, "SensitivityType"), "Standard output sensitivity (SOS) and ISO speed");
    ASSERT_EQ(GetProperty(exifMetadata, "StandardOutputSensitivity"), "5");
    ASSERT_EQ(GetProperty(exifMetadata, "RecommendedExposureIndex"), "241");
    ASSERT_EQ(GetProperty(exifMetadata, "ApertureValue"), "4.00 EV (f/4.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureBiasValue"), "23.00 EV");
    ASSERT_EQ(GetProperty(exifMetadata, "MeteringMode"), "Pattern");
    ASSERT_EQ(GetProperty(exifMetadata, "LightSource"), "Fluorescent");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Strobe return light not detected");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLength"), "31.0 mm");
    ASSERT_EQ(GetProperty(exifMetadata, "UserComment"), "comm");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "1000");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "2000");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Manual white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "26");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGProc"), "252");
}

/**
 * @tc.name: Read002
 * @tc.desc: test the jpegDecoded Exif Image properties
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Read002, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    int result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "MaxApertureValue"), "0.08 EV (f/1.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "Artist"), "Joseph.Xu");
    ASSERT_EQ(GetProperty(exifMetadata, "NewSubfileType"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "OECF"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "PlanarConfiguration"), "Planar format");
    ASSERT_EQ(GetProperty(exifMetadata, "PrimaryChromaticities"), "124");
    ASSERT_EQ(GetProperty(exifMetadata, "ReferenceBlackWhite"), "221");
    ASSERT_EQ(GetProperty(exifMetadata, "ResolutionUnit"), "Inch");
    ASSERT_EQ(GetProperty(exifMetadata, "SamplesPerPixel"), "23");
    ASSERT_EQ(GetProperty(exifMetadata, "Compression"), "JPEG compression");
    ASSERT_EQ(GetProperty(exifMetadata, "Software"), "MNA-AL00 4.0.0.120(C00E116R3P7)");
    ASSERT_EQ(GetProperty(exifMetadata, "SpectralSensitivity"), "sensitivity");
    ASSERT_EQ(GetProperty(exifMetadata, "DNGVersion"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistance"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "DefaultCropSize"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectLocation"), "3");
    ASSERT_EQ(GetProperty(exifMetadata, "TransferFunction"), "2");
    ASSERT_EQ(GetProperty(exifMetadata, "WhitePoint"), "124.2");
    ASSERT_EQ(GetProperty(exifMetadata, "XResolution"), "72");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrCoefficients"), "0.299, 0.587, 0.114");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrPositioning"), "Centered");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrSubSampling"), "3, 2");
    ASSERT_EQ(GetProperty(exifMetadata, "YResolution"), "72");
    ASSERT_EQ(GetProperty(exifMetadata, "Gamma"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeed"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedLatitudeyyy"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedLatitudezzz"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageUniqueID"), "FXIC012");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGInterchangeFormat"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGInterchangeFormatLength"), "");
}

/**
 * @tc.name: Read003
 * @tc.desc: test the jpegDecoded Exif GPSInfo properties
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Read003, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    int result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAltitude"), "0.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAltitudeRef"), "Sea level reference");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAreaInformation"), "20 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDOP"), "182");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestBearing"), "2.6");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestBearingRef"), "T");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestDistance"), "10");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestDistanceRef"), "N");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLatitude"), "33, 22, 11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLatitudeRef"), "N");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLongitude"), "33, 22, 11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLongitudeRef"), "E");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDifferential"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSImgDirection"), "2.23214");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSImgDirectionRef"), "M");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSMapDatum"), "xxxx");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSMeasureMode"), "2");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSProcessingMethod"), "14 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSatellites"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSpeed"), "150");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSpeedRef"), "K");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSStatus"), "V");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTrack"), "56");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTrackRef"), "T");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSVersionID"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSHPositioningError"), " 3");
    ASSERT_EQ(GetProperty(exifMetadata, "LensMake"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "LensModel"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "LensSerialNumber"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "LensSpecification"), " 1, 1.5,  1,  2");
    ASSERT_EQ(GetProperty(exifMetadata, "MakerNote"), "34 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "GainControl"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTime"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTimeDigitized"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTimeOriginal"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "PhotometricInterpretation"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "RelatedSoundFile"), "/usr/home/sound/sea.wav");
    ASSERT_EQ(GetProperty(exifMetadata, "RowsPerStrip"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "Saturation"), "Normal");
}

/**
 * @tc.name: Read004
 * @tc.desc: test the jpegDecoded Exif photo properties
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Read004, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    int result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "SceneCaptureType"), "Standard");
    ASSERT_EQ(GetProperty(exifMetadata, "SensingMethod"), "Two-chip color area sensor");
    ASSERT_EQ(GetProperty(exifMetadata, "Sharpness"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "ShutterSpeedValue"), "13.00 EV (1/8192 sec.)");
    ASSERT_EQ(GetProperty(exifMetadata, "SourceExposureTimesOfCompositeImage"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "SourceImageNumberOfCompositeImage"), "1234");
    ASSERT_EQ(GetProperty(exifMetadata, "SpatialFrequencyResponse"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "StripByteCounts"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "StripOffsets"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTime"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeDigitized"), "427000");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeOriginal"), "427000");
    ASSERT_EQ(GetProperty(exifMetadata, "SubfileType"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectArea"),
              "Within rectangle (width 183, height 259) around (x,y) = (10,20)");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistanceRange"), "Unknown");
    ASSERT_EQ(GetProperty(exifMetadata, "BodySerialNumber"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "BrightnessValue"), "2.50 EV (19.38 cd/m^2)");
    ASSERT_EQ(GetProperty(exifMetadata, "CFAPattern"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "CameraOwnerName"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "ColorSpace"), "Adobe RGB");
    ASSERT_EQ(GetProperty(exifMetadata, "ComponentsConfiguration"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "CompositeImage"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "CompressedBitsPerPixel"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "Contrast"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "CustomRendered"), "Custom process");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeDigitized"), "2023:01:19 10:39:58");
    ASSERT_EQ(GetProperty(exifMetadata, "DeviceSettingDescription"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "DigitalZoomRatio"), "321");
    ASSERT_EQ(GetProperty(exifMetadata, "ExifVersion"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureIndex"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureMode"), "Auto exposure");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureProgram"), "Normal program");
    ASSERT_EQ(GetProperty(exifMetadata, "FileSource"), "DSC");
    ASSERT_EQ(GetProperty(exifMetadata, "FlashEnergy"), "832");
    ASSERT_EQ(GetProperty(exifMetadata, "FlashpixVersion"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneResolutionUnit"), "Centimeter");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneXResolution"), "1080");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneYResolution"), "880");
}

/**
 * @tc.name: Read005
 * @tc.desc: test the jpegDecoded Exif properties
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Read005, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT2_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    int result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    std::shared_ptr<ExifMetadata> exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "8, 8, 8");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Unknown value 0");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "4000");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "3000");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitude"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitude"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitudeRef"), "R98");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitudeRef"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2024:01:11 09:39:58");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/590 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "SceneType"), "Directly photographed");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedRatings"), "160");
    ASSERT_EQ(GetProperty(exifMetadata, "FNumber"), "f/2.0");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTime"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTimeStamp"), "01:39:58.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDateStamp"), "2024:01:11");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageDescription"), "_cuva");
    ASSERT_EQ(GetProperty(exifMetadata, "Make"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "Model"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "SensitivityType"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "StandardOutputSensitivity"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "RecommendedExposureIndex"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "ApertureValue"), "2.00 EV (f/2.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureBiasValue"), "0.00 EV");
    ASSERT_EQ(GetProperty(exifMetadata, "MeteringMode"), "Pattern");
    ASSERT_EQ(GetProperty(exifMetadata, "LightSource"), "Daylight");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Flash did not fire");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLength"), "6.3 mm");
    ASSERT_EQ(GetProperty(exifMetadata, "UserComment"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "4000");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "3000");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Auto white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "27");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGProc"), "");
}

/**
 * @tc.name: ReadBlob002
 * @tc.desc: test ReadBlob from error jpeg image1 which does not have 0xff, return false
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, ReadBlob001, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_ERROR1_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadBlob(exifBuf);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: ReadBlob003
 * @tc.desc: test ReadBlob from error jpeg image2 which does not have APP1, return false
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, ReadBlob002, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_ERROR2_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadBlob(exifBuf);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: ReadBlob004
 * @tc.desc: test ReadBlob from right jpeg image, return true and the length of exifBlob
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, ReadBlob003, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadBlob(exifBuf);
    ASSERT_TRUE(result);
    ASSERT_EQ(exifBuf.Size(), 0x0932);
}

/**
 * @tc.name: Write001
 * @tc.desc: test Write from right jpeg image, modify "BitsPerSample" propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write001, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE1_JPEG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(stream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);
    ASSERT_TRUE(exifMetadata->SetValue("BitsPerSample", "8,8,8"));
    ASSERT_TRUE(exifMetadata->SetValue("Orientation", "8"));
    ASSERT_TRUE(exifMetadata->SetValue("ImageLength", "4000"));
    ASSERT_TRUE(exifMetadata->SetValue("ImageWidth", "3000"));
    ASSERT_TRUE(exifMetadata->SetValue("DateTimeOriginal", "2024:01:11 09:39:58"));
    ASSERT_TRUE(exifMetadata->SetValue("ExposureTime", "1/590"));
    ASSERT_TRUE(exifMetadata->SetValue("ISOSpeedRatings", "160"));
    ASSERT_TRUE(exifMetadata->SetValue("GPSDateStamp", "2024:01:11"));
    ASSERT_TRUE(exifMetadata->SetValue("ImageDescription", "_cuva"));
    ASSERT_TRUE(exifMetadata->SetValue("Flash", "7"));
    ASSERT_TRUE(exifMetadata->SetValue("PixelXDimension", "4000"));
    ASSERT_TRUE(exifMetadata->SetValue("PixelYDimension", "3000"));
    ASSERT_TRUE(exifMetadata->SetValue("WhiteBalance", "1"));
    ASSERT_TRUE(exifMetadata->SetValue("FocalLengthIn35mmFilm", "28"));

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "8, 8, 8");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Left-bottom");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "4000");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "3000");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2024:01:11 09:39:58");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/590 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedRatings"), "160");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDateStamp"), "2024:01:11");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageDescription"), "_cuva");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Strobe return light detected");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "4000");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "3000");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Manual white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "28");
}

/**
 * @tc.name: Write002
 * @tc.desc: test Write from nonexisting image file, return error number
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write002, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE3_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "9, 7, 8");

    std::shared_ptr<MetadataStream> writeStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT_WRITE1_JPEG_PATH);
    ASSERT_TRUE(writeStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageWriteAccessor(writeStream);
    ASSERT_EQ(imageWriteAccessor.Write(), ERR_MEDIA_VALUE_INVALID);
}

/**
 * @tc.name: Write003
 * @tc.desc: test Write from right jpeg image, modify "GPSLongitudeRef" propert set value "W"
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write003, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE5_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_TRUE(exifMetadata->SetValue("GPSLongitudeRef", "W"));
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitudeRef"), "W");
    ASSERT_EQ(imageAccessor.Write(), 0);
    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitudeRef"), "W");
}

/**
 * @tc.name: Write004
 * @tc.desc: test Write from right jpeg image,read and write
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write004, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE7_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(imageAccessor.Write(), 0);
    ASSERT_EQ(imageAccessor.Read(), 0);
}

/**
 * @tc.name: Write005
 * @tc.desc: test Write from right jpeg image, modify Image propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write005, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE9_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "NewSubfileType"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "500");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "1000");
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "9, 7, 8");
    ASSERT_EQ(GetProperty(exifMetadata, "Compression"), "JPEG compression");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageDescription"), "_cuva");
    ASSERT_EQ(GetProperty(exifMetadata, "Model"), "TNY-AL00");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Top-right");
    ASSERT_EQ(GetProperty(exifMetadata, "SamplesPerPixel"), "23");
    ASSERT_EQ(GetProperty(exifMetadata, "XResolution"), "72");
    ASSERT_EQ(GetProperty(exifMetadata, "YResolution"), "72");
    ASSERT_EQ(GetProperty(exifMetadata, "PlanarConfiguration "), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ResolutionUnit"), "Inch");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "NewSubfileType"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "500");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "1000");
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "9, 7, 8");
    ASSERT_EQ(GetProperty(exifMetadata, "Compression"), "JPEG compression");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageDescription"), "_cuva");
    ASSERT_EQ(GetProperty(exifMetadata, "Model"), "TNY-AL00");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Top-right");
    ASSERT_EQ(GetProperty(exifMetadata, "SamplesPerPixel"), "23");
    ASSERT_EQ(GetProperty(exifMetadata, "XResolution"), "72");
    ASSERT_EQ(GetProperty(exifMetadata, "YResolution"), "72");
    ASSERT_EQ(GetProperty(exifMetadata, "PlanarConfiguration "), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ResolutionUnit"), "Inch");
}

/**
 * @tc.name: Write006
 * @tc.desc: test Write from right jpeg image, modify Image propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write006, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE11_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "TransferFunction"), "2");
    ASSERT_EQ(GetProperty(exifMetadata, "Software"), "MNA-AL00 4.0.0.120(C00E116R3P7)");
    ASSERT_EQ(GetProperty(exifMetadata, "Artist"), "Joseph.Xu");
    ASSERT_EQ(GetProperty(exifMetadata, "WhitePoint"), "124.2");
    ASSERT_EQ(GetProperty(exifMetadata, "PrimaryChromaticities"), "124");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGProc"), "252");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrCoefficients"), "0.299, 0.587, 0.114");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrSubSampling"), "3, 2");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrPositioning"), "Centered");
    ASSERT_EQ(GetProperty(exifMetadata, "ReferenceBlackWhite"), "221");
    ASSERT_EQ(GetProperty(exifMetadata, "ExifTag"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SpectralSensitivity"), "sensitivity");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "TransferFunction"), "2");
    ASSERT_EQ(GetProperty(exifMetadata, "Software"), "MNA-AL00 4.0.0.120(C00E116R3P7)");
    ASSERT_EQ(GetProperty(exifMetadata, "Artist"), "Joseph.Xu");
    ASSERT_EQ(GetProperty(exifMetadata, "WhitePoint"), "124.2");
    ASSERT_EQ(GetProperty(exifMetadata, "PrimaryChromaticities"), "124");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGProc"), "252");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrCoefficients"), "0.299, 0.587, 0.114");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrSubSampling"), "3, 2");
    ASSERT_EQ(GetProperty(exifMetadata, "YCbCrPositioning"), "Centered");
    ASSERT_EQ(GetProperty(exifMetadata, "ReferenceBlackWhite"), "221");
    ASSERT_EQ(GetProperty(exifMetadata, "ExifTag"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SpectralSensitivity"), "sensitivity");
}

/**
 * @tc.name: Write007
 * @tc.desc: test Write from right jpeg image, modify Image propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write007, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE13_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTag"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "OECF"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "MaxApertureValue"), "0.08 EV (f/1.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistance"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectLocation"), "3");
    ASSERT_EQ(GetProperty(exifMetadata, "DNGVersion"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "DefaultCropSize"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubfileType"), "2.2.0.0");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTag"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "OECF"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "MaxApertureValue"), "0.08 EV (f/1.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistance"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectLocation"), "3");
    ASSERT_EQ(GetProperty(exifMetadata, "DNGVersion"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "DefaultCropSize"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubfileType"), "2.2.0.0");
}

/**
 * @tc.name: Write008
 * @tc.desc: test Write from right jpeg image, modify GPSInfo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write008, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE15_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "GPSVersionID"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitudeRef"), "N");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitude"), "39, 54, 20");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitudeRef"), "E");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitude"), "120, 52, 26");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAltitudeRef"), "Sea level reference");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAltitude"), "0.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTimeStamp"), "11:37:58.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSatellites"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSStatus"), "V");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSMeasureMode"), "2");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDOP"), "182");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSpeedRef"), "K");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "GPSVersionID"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitudeRef"), "N");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLatitude"), "39, 54, 20");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitudeRef"), "E");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSLongitude"), "120, 52, 26");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAltitudeRef"), "Sea level reference");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAltitude"), "0.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTimeStamp"), "11:37:58.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSatellites"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSStatus"), "V");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSMeasureMode"), "2");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDOP"), "182");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSpeedRef"), "K");
}

/**
 * @tc.name: Write009
 * @tc.desc: test Write from right jpeg image, modify GPSInfo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write009, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE17_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSpeed"), "150");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTrackRef"), "T");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTrack"), "56");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSImgDirectionRef"), "M");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSImgDirection"), "2.23214");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSMapDatum"), "xxxx");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLatitudeRef"), "N");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLatitude"), "33, 22, 11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLongitudeRef"), "E");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLongitude"), "33, 22, 11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestBearingRef"), "T");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestBearing"), "2.6");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestDistanceRef"), "N");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "GPSSpeed"), "150");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTrackRef"), "T");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTrack"), "56");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSImgDirectionRef"), "M");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSImgDirection"), "2.23214");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSMapDatum"), "xxxx");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLatitudeRef"), "N");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLatitude"), "33, 22, 11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLongitudeRef"), "E");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestLongitude"), "33, 22, 11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestBearingRef"), "T");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestBearing"), "2.6");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestDistanceRef"), "N");
}

/**
 * @tc.name: Write010
 * @tc.desc: test Write from right jpeg image, modify GPSInfo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write010, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE19_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestDistance"), "10");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSProcessingMethod"), "14 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAreaInformation"), "20 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDateStamp"), "2025:01:11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDifferential"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSHPositioningError"), " 3");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDestDistance"), "10");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSProcessingMethod"), "14 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSAreaInformation"), "20 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDateStamp"), "2025:01:11");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDifferential"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSHPositioningError"), " 3");
}

/**
 * @tc.name: Write011
 * @tc.desc: test Write from right jpeg image, modify Photo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write011, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE21_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/34 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "FNumber"), "f/3.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureProgram"), "Normal program");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedRatings"), "160");
    ASSERT_EQ(GetProperty(exifMetadata, "SensitivityType"), "Standard output sensitivity (SOS) and ISO speed");
    ASSERT_EQ(GetProperty(exifMetadata, "StandardOutputSensitivity"), "5");
    ASSERT_EQ(GetProperty(exifMetadata, "RecommendedExposureIndex"), "241");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeed"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedLatitudeyyy"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedLatitudezzz"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ExifVersion"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2024:01:25 05:51:34");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeDigitized"), "2023:01:19 10:39:58");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/34 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "FNumber"), "f/3.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureProgram"), "Normal program");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedRatings"), "160");
    ASSERT_EQ(GetProperty(exifMetadata, "SensitivityType"), "Standard output sensitivity (SOS) and ISO speed");
    ASSERT_EQ(GetProperty(exifMetadata, "StandardOutputSensitivity"), "5");
    ASSERT_EQ(GetProperty(exifMetadata, "RecommendedExposureIndex"), "241");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeed"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedLatitudeyyy"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedLatitudezzz"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "ExifVersion"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2024:01:25 05:51:34");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeDigitized"), "2023:01:19 10:39:58");
}

/**
 * @tc.name: Write012
 * @tc.desc: test Write from right jpeg image, modify Photo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write012, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE23_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTime"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTimeOriginal"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTimeDigitized"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "ComponentsConfiguration"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "CompressedBitsPerPixel"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "ShutterSpeedValue"), "13.00 EV (1/8192 sec.)");
    ASSERT_EQ(GetProperty(exifMetadata, "ApertureValue"), "4.00 EV (f/4.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "BrightnessValue"), "2.50 EV (19.38 cd/m^2)");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureBiasValue"), "23.00 EV");
    ASSERT_EQ(GetProperty(exifMetadata, "MeteringMode"), "Pattern");
    ASSERT_EQ(GetProperty(exifMetadata, "LightSource"), "Fluorescent");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Strobe return light not detected");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLength"), "31.0 mm");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTime"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTimeOriginal"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "OffsetTimeDigitized"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "ComponentsConfiguration"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "CompressedBitsPerPixel"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "ShutterSpeedValue"), "13.00 EV (1/8192 sec.)");
    ASSERT_EQ(GetProperty(exifMetadata, "ApertureValue"), "4.00 EV (f/4.0)");
    ASSERT_EQ(GetProperty(exifMetadata, "BrightnessValue"), "2.50 EV (19.38 cd/m^2)");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureBiasValue"), "23.00 EV");
    ASSERT_EQ(GetProperty(exifMetadata, "MeteringMode"), "Pattern");
    ASSERT_EQ(GetProperty(exifMetadata, "LightSource"), "Fluorescent");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Strobe return light not detected");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLength"), "31.0 mm");
}

/**
 * @tc.name: Write013
 * @tc.desc: test Write from right jpeg image, modify Photo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write013, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE25_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "MakerNote"), "34 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "UserComment"), "comm");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTime"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeOriginal"), "427000");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeDigitized"), "427000");
    ASSERT_EQ(GetProperty(exifMetadata, "FlashpixVersion"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "ColorSpace"), "Adobe RGB");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "1000");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "2000");
    ASSERT_EQ(GetProperty(exifMetadata, "RelatedSoundFile"), "/usr/home/sound/sea.wav");
    ASSERT_EQ(GetProperty(exifMetadata, "FlashEnergy"), "832");
    ASSERT_EQ(GetProperty(exifMetadata, "SpatialFrequencyResponse"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneXResolution"), "1080");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "MakerNote"), "34 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "UserComment"), "comm");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTime"), "2.2.0.0");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeOriginal"), "427000");
    ASSERT_EQ(GetProperty(exifMetadata, "SubSecTimeDigitized"), "427000");
    ASSERT_EQ(GetProperty(exifMetadata, "FlashpixVersion"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "ColorSpace"), "Adobe RGB");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "1000");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "2000");
    ASSERT_EQ(GetProperty(exifMetadata, "RelatedSoundFile"), "/usr/home/sound/sea.wav");
    ASSERT_EQ(GetProperty(exifMetadata, "FlashEnergy"), "832");
    ASSERT_EQ(GetProperty(exifMetadata, "SpatialFrequencyResponse"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneXResolution"), "1080");
}

/**
 * @tc.name: Write014
 * @tc.desc: test Write from right jpeg image, modify Photo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write014, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE27_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneYResolution"), "880");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneResolutionUnit"), "Centimeter");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectLocation"), "3");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureIndex"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "SensingMethod"), "Two-chip color area sensor");
    ASSERT_EQ(GetProperty(exifMetadata, "FileSource"), "DSC");
    ASSERT_EQ(GetProperty(exifMetadata, "SceneType"), "Directly photographed");
    ASSERT_EQ(GetProperty(exifMetadata, "CFAPattern"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "CustomRendered"), "Custom process");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureMode"), "Auto exposure");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Manual white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "DigitalZoomRatio"), "321");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "26");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneYResolution"), "880");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalPlaneResolutionUnit"), "Centimeter");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectLocation"), "3");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureIndex"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "SensingMethod"), "Two-chip color area sensor");
    ASSERT_EQ(GetProperty(exifMetadata, "FileSource"), "DSC");
    ASSERT_EQ(GetProperty(exifMetadata, "SceneType"), "Directly photographed");
    ASSERT_EQ(GetProperty(exifMetadata, "CFAPattern"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "CustomRendered"), "Custom process");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureMode"), "Auto exposure");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Manual white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "DigitalZoomRatio"), "321");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "26");
}

/**
 * @tc.name: Write015
 * @tc.desc: test Write from right jpeg image, modify Photo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write015, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE29_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "SceneCaptureType"), "Standard");
    ASSERT_EQ(GetProperty(exifMetadata, "GainControl"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Contrast"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Saturation"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Sharpness"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "DeviceSettingDescription"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistanceRange"), "Unknown");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageUniqueID"), "FXIC012");
    ASSERT_EQ(GetProperty(exifMetadata, "CameraOwnerName"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "BodySerialNumber"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "LensSpecification"), " 1, 1.5,  1,  2");
    ASSERT_EQ(GetProperty(exifMetadata, "LensMake"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "LensModel"), "xxx");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "SceneCaptureType"), "Standard");
    ASSERT_EQ(GetProperty(exifMetadata, "GainControl"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Contrast"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Saturation"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "Sharpness"), "Normal");
    ASSERT_EQ(GetProperty(exifMetadata, "DeviceSettingDescription"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectDistanceRange"), "Unknown");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageUniqueID"), "FXIC012");
    ASSERT_EQ(GetProperty(exifMetadata, "CameraOwnerName"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "BodySerialNumber"), "xx");
    ASSERT_EQ(GetProperty(exifMetadata, "LensSpecification"), " 1, 1.5,  1,  2");
    ASSERT_EQ(GetProperty(exifMetadata, "LensMake"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "LensModel"), "xxx");
}

/**
 * @tc.name: Write016
 * @tc.desc: test Write from right jpeg image, modify Photo propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, Write016, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE31_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageAccessor(readStream);
    ASSERT_EQ(imageAccessor.Read(), 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_EQ(GetProperty(exifMetadata, "LensSerialNumber"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "CompositeImage"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "SourceImageNumberOfCompositeImage"), "1234");
    ASSERT_EQ(GetProperty(exifMetadata, "SourceExposureTimesOfCompositeImage"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "Gamma"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGInterchangeFormat"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGInterchangeFormatLength"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "PhotometricInterpretation"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "RowsPerStrip"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "StripByteCounts"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "StripOffsets"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectArea"), "Within rectangle (width 183, height 259) around (x,y) = (10,20)");

    ASSERT_EQ(imageAccessor.Write(), 0);

    ASSERT_EQ(imageAccessor.Read(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "LensSerialNumber"), "xxx");
    ASSERT_EQ(GetProperty(exifMetadata, "CompositeImage"), "1");
    ASSERT_EQ(GetProperty(exifMetadata, "SourceImageNumberOfCompositeImage"), "1234");
    ASSERT_EQ(GetProperty(exifMetadata, "SourceExposureTimesOfCompositeImage"), "1 bytes undefined data");
    ASSERT_EQ(GetProperty(exifMetadata, "Gamma"), "1.5");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGInterchangeFormat"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "JPEGInterchangeFormatLength"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "PhotometricInterpretation"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "RowsPerStrip"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "StripByteCounts"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "StripOffsets"), "");
    ASSERT_EQ(GetProperty(exifMetadata, "SubjectArea"),
              "Within rectangle (width 183, height 259) around (x,y) = (10,20)");
}

/**
 * @tc.name: WriteBlob001
 * @tc.desc: test WriteBlob from right jpeg image, modify propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, WriteBlob001, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE2_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageReadAccessor(readStream);
    DataBuf inputBuf;
    ASSERT_TRUE(imageReadAccessor.ReadBlob(inputBuf));

    std::shared_ptr<MetadataStream> writeStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT_WRITE2_JPEG_PATH);
    ASSERT_TRUE(writeStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageWriteAccessor(writeStream);
    ASSERT_EQ(imageWriteAccessor.WriteBlob(inputBuf), 0);

    DataBuf outputBuf;
    ASSERT_TRUE(imageWriteAccessor.ReadBlob(outputBuf));
    ASSERT_EQ(outputBuf.Size(), inputBuf.Size());
}

/**
 * @tc.name: WriteBlob002
 * @tc.desc: test WriteBlob from empty data buffer, return error number
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, WriteBlob002, TestSize.Level3)
{
    DataBuf inputBuf;
    std::shared_ptr<MetadataStream> writeStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT_WRITE4_JPEG_PATH);
    ASSERT_TRUE(writeStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageWriteAccessor(writeStream);
    ASSERT_EQ(imageWriteAccessor.WriteBlob(inputBuf), ERR_MEDIA_VALUE_INVALID);
}

/**
 * @tc.name: WriteBlob003
 * @tc.desc: test WriteBlob from right jpeg image, Data buffer not container "EXIF\0\0"
 * @tc.type: FUNC
 */
HWTEST_F(JpegExifMetadataAccssorTest, WriteBlob003, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> readStream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_WRITE2_JPEG_PATH);
    ASSERT_TRUE(readStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageReadAccessor(readStream);
    DataBuf inputBuf;
    ASSERT_TRUE(imageReadAccessor.ReadBlob(inputBuf));

    auto length = 0;
    if (inputBuf.CmpBytes(0, EXIF_ID, EXIF_ID_SIZE) == 0) {
        length = EXIF_ID_SIZE;
    }

    std::shared_ptr<MetadataStream> writeStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT_WRITE6_JPEG_PATH);
    ASSERT_TRUE(writeStream->Open(OpenMode::ReadWrite));
    JpegExifMetadataAccssor imageWriteAccessor(writeStream);
    DataBuf dataBlob(inputBuf.CData(length), (inputBuf.Size() - length));
    ASSERT_EQ(imageWriteAccessor.WriteBlob(dataBlob), 0);

    DataBuf outputBuf;
    ASSERT_TRUE(imageWriteAccessor.ReadBlob(outputBuf));
    ASSERT_EQ(outputBuf.Size(), inputBuf.Size());
}

std::string JpegExifMetadataAccssorTest::GetProperty(const std::shared_ptr<ExifMetadata>& metadata,
    const std::string& prop)
{
    std::string value;
    metadata->GetValue(prop, value);
    return value;
}

} // namespace Multimedia
} // namespace OHOS