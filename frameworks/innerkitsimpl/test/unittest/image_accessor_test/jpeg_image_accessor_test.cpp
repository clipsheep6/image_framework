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
#include <map>

#include "jpeg_image_accessor.h"
#include "file_image_stream.h"
#include "log_tags.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {

static const std::string IMAGE_INPUT1_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readmetadata001.jpg";
static const std::string IMAGE_INPUT2_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readmetadata003.jpg";
static const std::string IMAGE_ERROR1_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readexifblob002.jpg";
static const std::string IMAGE_ERROR2_JPEG_PATH = "/data/local/tmp/image/test_jpeg_readexifblob003.jpg";
static const std::string IMAGE_INPUT3_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writemetadata001.jpg";
static const std::string IMAGE_INPUT4_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writeexifblob001.jpg";
static const std::string IMAGE_OUTPUT_JPEG_PATH = "/data/local/tmp/image/test_jpeg_writeexifblob002.jpg";

class JpegImageAccessorTest : public testing::Test {
public:
    JpegImageAccessorTest() {}
    ~JpegImageAccessorTest() {}
    std::string GetProperty(const std::shared_ptr<ExifMetadata>& metadata, const std::string& prop);
};

/**
 * @tc.name: ReadMetadata001
 * @tc.desc: test the jpegDecoded Exif properties
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, ReadMetadata001, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT1_JPEG_PATH);
    JpegImageAccessor imageAccessor(stream);
    int result = imageAccessor.ReadMetadata();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.GetExifMetadata();
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
 * @tc.name: ReadMetadata003
 * @tc.desc: test the jpegDecoded Exif properties
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, ReadMetadata003, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT2_JPEG_PATH);
    JpegImageAccessor imageAccessor(stream);
    int result = imageAccessor.ReadMetadata();
    ASSERT_EQ(result, 0);
    std::shared_ptr<ExifMetadata> exifMetadata = imageAccessor.GetExifMetadata();
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
 * @tc.name: ReadExifBlob001
 * @tc.desc: test ReadExifBlob from nonexisting image file, return false
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, ReadExifBlob001, TestSize.Level3)
{
    const std::string IMAGE_NONEXISTING_JPEG_PATH = "/data/local/tmp/image/testnonexisting.jpg";
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_NONEXISTING_JPEG_PATH);
    JpegImageAccessor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadExifBlob(exifBuf);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: ReadExifBlob002
 * @tc.desc: test ReadExifBlob from error jpeg image1 which does not have 0xff, return false
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, ReadExifBlob002, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_ERROR1_JPEG_PATH);
    JpegImageAccessor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadExifBlob(exifBuf);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: ReadExifBlob003
 * @tc.desc: test ReadExifBlob from error jpeg image2 which does not have APP1, return false
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, ReadExifBlob003, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_ERROR2_JPEG_PATH);
    JpegImageAccessor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadExifBlob(exifBuf);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: ReadExifBlob004
 * @tc.desc: test ReadExifBlob from right jpeg image, return true and the length of exifBlob
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, ReadExifBlob004, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT1_JPEG_PATH);
    JpegImageAccessor imageAccessor(stream);
    DataBuf exifBuf;
    bool result = imageAccessor.ReadExifBlob(exifBuf);
    ASSERT_TRUE(result);
    ASSERT_EQ(exifBuf.Size(), 0x0932);
}

/**
 * @tc.name: WriteMetadata001
 * @tc.desc: test WriteMetadata from right jpeg image, modify "BitsPerSample" propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, WriteMetadata001, TestSize.Level3)
{
    std::shared_ptr<ImageStream> stream = std::make_shared<FileImageStream>(IMAGE_INPUT3_JPEG_PATH);
    JpegImageAccessor imageAccessor(stream);
    ASSERT_EQ(imageAccessor.ReadMetadata(), 0);

    auto exifMetadata = imageAccessor.GetExifMetadata();
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "9, 7, 8");

    ASSERT_TRUE(exifMetadata->SetValue("BitsPerSample", "8, 8, 8"));
    ASSERT_TRUE(exifMetadata->SetValue("Orientation", "Unknown value 0"));
    ASSERT_TRUE(exifMetadata->SetValue("ImageLength", "4000"));
    ASSERT_TRUE(exifMetadata->SetValue("ImageWidth", "3000"));
    ASSERT_TRUE(exifMetadata->SetValue("DateTimeOriginal", "2024:01:11 09:39:58"));
    ASSERT_TRUE(exifMetadata->SetValue("ExposureTime", "1/590 sec."));
    ASSERT_TRUE(exifMetadata->SetValue("ISOSpeedRatings", "160"));
    ASSERT_TRUE(exifMetadata->SetValue("GPSTimeStamp", "01:39:58.00"));
    ASSERT_TRUE(exifMetadata->SetValue("GPSDateStamp", "2024:01:11"));
    ASSERT_TRUE(exifMetadata->SetValue("ImageDescription", "_cuva"));
    ASSERT_TRUE(exifMetadata->SetValue("Flash", "Flash did not fire"));
    ASSERT_TRUE(exifMetadata->SetValue("PixelXDimension", "4000"));
    ASSERT_TRUE(exifMetadata->SetValue("PixelYDimension", "3000"));
    ASSERT_TRUE(exifMetadata->SetValue("WhiteBalance", "Auto white balance"));
    ASSERT_TRUE(exifMetadata->SetValue("FocalLengthIn35mmFilm", "27"));

    ASSERT_EQ(imageAccessor.WriteMetadata(), 0);

    ASSERT_EQ(imageAccessor.ReadMetadata(), 0);
    ASSERT_EQ(GetProperty(exifMetadata, "BitsPerSample"), "8, 8, 8");
    ASSERT_EQ(GetProperty(exifMetadata, "Orientation"), "Unknown value 0");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageLength"), "4000");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageWidth"), "3000");
    ASSERT_EQ(GetProperty(exifMetadata, "DateTimeOriginal"), "2024:01:11 09:39:58");
    ASSERT_EQ(GetProperty(exifMetadata, "ExposureTime"), "1/590 sec.");
    ASSERT_EQ(GetProperty(exifMetadata, "ISOSpeedRatings"), "160");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSTimeStamp"), "01:39:58.00");
    ASSERT_EQ(GetProperty(exifMetadata, "GPSDateStamp"), "2024:01:11");
    ASSERT_EQ(GetProperty(exifMetadata, "ImageDescription"), "_cuva");
    ASSERT_EQ(GetProperty(exifMetadata, "Flash"), "Flash did not fire");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelXDimension"), "4000");
    ASSERT_EQ(GetProperty(exifMetadata, "PixelYDimension"), "3000");
    ASSERT_EQ(GetProperty(exifMetadata, "WhiteBalance"), "Auto white balance");
    ASSERT_EQ(GetProperty(exifMetadata, "FocalLengthIn35mmFilm"), "27");
}

/**
 * @tc.name: WriteExifBlob001
 * @tc.desc: test WriteExifBlob from right jpeg image, modify propert
 * @tc.type: FUNC
 */
HWTEST_F(JpegImageAccessorTest, WriteExifBlob001, TestSize.Level3)
{
    std::shared_ptr<ImageStream> readStream = std::make_shared<FileImageStream>(IMAGE_INPUT4_JPEG_PATH);
    JpegImageAccessor imageReadAccessor(readStream);
    DataBuf exifBuf;
    ASSERT_TRUE(imageReadAccessor.ReadExifBlob(exifBuf));
    ASSERT_EQ(exifBuf.Size(), 0x0932);

    std::shared_ptr<ImageStream> writeStream = std::make_shared<FileImageStream>(IMAGE_OUTPUT_JPEG_PATH);
    JpegImageAccessor imageWriteAccessor(writeStream);
    ASSERT_EQ(imageWriteAccessor.WriteExifBlob(exifBuf), 0);

    DataBuf outputBuf;
    ASSERT_TRUE(imageWriteAccessor.ReadExifBlob(outputBuf));
    ASSERT_EQ(outputBuf.Size(), 0x0932);
}

std::string JpegImageAccessorTest::GetProperty(const std::shared_ptr<ExifMetadata>& metadata, const std::string& prop)
{
    std::string value;
    metadata->GetValue(prop, value);
    return value;
}

} // namespace Multimedia
} // namespace OHOS