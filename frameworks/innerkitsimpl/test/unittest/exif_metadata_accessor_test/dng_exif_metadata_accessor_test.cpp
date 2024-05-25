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
#include <iostream>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <memory>

#include "dng_exif_metadata_accessor.h"
#include "file_metadata_stream.h"
#include "log_tags.h"
#include "media_errors.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {
namespace {
    static const std::string IMAGE_INPUT1_DNG_PATH = "/data/local/tmp/image/test_dng_readmetadata001.dng";
    static const std::string IMAGE_INPUT2_DNG_PATH = "/data/local/tmp/image/test_dng_readmetadata002.dng";
    static const std::string IMAGE_INPUT3_DNG_PATH = "/data/local/tmp/image/test_dng_readmetadata003.dng";
    static const std::string IMAGE_INPUT4_DNG_PATH = "/data/local/tmp/image/test_notiff.dng";
    static const std::string IMAGE_OUTPUT2_DNG_PATH = IMAGE_INPUT1_DNG_PATH;
}

class DngExifMetadataAccessorTest : public testing::Test {
public:
    DngExifMetadataAccessorTest() {}
    ~DngExifMetadataAccessorTest() {}
};

/**
 * @tc.name: Read001
 * @tc.desc: test the dngDecoded Exif properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read001, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_DNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadata->GetValue("BitsPerSample", value), SUCCESS);
    ASSERT_EQ(value, "8, 8, 8");
    ASSERT_EQ(exifMetadata->GetValue("Orientation", value), SUCCESS);
    ASSERT_EQ(value, "Top-left");
    ASSERT_EQ(exifMetadata->GetValue("ImageLength", value), SUCCESS);
    ASSERT_EQ(value, "192");
    ASSERT_EQ(exifMetadata->GetValue("ImageWidth", value), SUCCESS);
    ASSERT_EQ(value, "256");
    ASSERT_EQ(exifMetadata->GetValue("DateTimeOriginal", value), SUCCESS);
    ASSERT_EQ(value, "2024:04:18 18:18:43");
    ASSERT_EQ(exifMetadata->GetValue("ExposureTime", value), SUCCESS);
    ASSERT_EQ(value, "1/100 sec.");
    ASSERT_EQ(exifMetadata->GetValue("ISOSpeedRatings", value), SUCCESS);
    ASSERT_EQ(value, "205");
    ASSERT_EQ(exifMetadata->GetValue("FNumber", value), SUCCESS);
    ASSERT_EQ(value, "f/2.0");
    ASSERT_EQ(exifMetadata->GetValue("DateTime", value), SUCCESS);
    ASSERT_EQ(value, "2024:04:18 18:18:43");
    ASSERT_EQ(exifMetadata->GetValue("Make", value), SUCCESS);
    ASSERT_EQ(value, "HUAWEI");
    ASSERT_EQ(exifMetadata->GetValue("Model", value), SUCCESS);
    ASSERT_EQ(value, "MNA-AL00");
    ASSERT_EQ(exifMetadata->GetValue("FocalLength", value), SUCCESS);
    ASSERT_EQ(value, "6.3 mm");
    ASSERT_EQ(exifMetadata->GetValue("NewSubfileType", value), SUCCESS);
    ASSERT_EQ(value, "1");
    ASSERT_EQ(exifMetadata->GetValue("PlanarConfiguration", value), SUCCESS);
    ASSERT_EQ(value, "Chunky format");
    ASSERT_EQ(exifMetadata->GetValue("SamplesPerPixel", value), SUCCESS);
    ASSERT_EQ(value, "3");
    ASSERT_EQ(exifMetadata->GetValue("Compression", value), SUCCESS);
    ASSERT_EQ(value, "Uncompressed");
    ASSERT_EQ(exifMetadata->GetValue("Software", value), SUCCESS);
    ASSERT_EQ(value, "HUAWEI/MNA-AL00/HWMNA:12/HUAWEIMNA-AL00/104.0.0.56C00:user/release-keys");
}

/**
 * @tc.name: Read002
 * @tc.desc: test the dngDecoded Exif properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read002, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_DNG_PATH);
    ASSERT_NE(stream, nullptr);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, SUCCESS);

    std::shared_ptr<ExifMetadata> exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadata->GetValue("DNGVersion", value), SUCCESS);
    ASSERT_EQ(value, "0x01, 0x03, 0x00, 0x00");
    ASSERT_EQ(exifMetadata->GetValue("PhotometricInterpretation", value), SUCCESS);
    ASSERT_EQ(value, "RGB");
    ASSERT_EQ(exifMetadata->GetValue("RowsPerStrip", value), SUCCESS);
    ASSERT_EQ(value, "192");
    ASSERT_EQ(exifMetadata->GetValue("StripByteCounts", value), SUCCESS);
    ASSERT_EQ(value, "147456");
    ASSERT_EQ(exifMetadata->GetValue("StripOffsets", value), SUCCESS);
    ASSERT_EQ(value, "8864");
    ASSERT_EQ(exifMetadata->GetValue("ShutterSpeedValue", value), SUCCESS);
    ASSERT_EQ(value, "6.64 EV (1/100 sec.)");
    ASSERT_EQ(exifMetadata->GetValue("ApertureValue", value), SUCCESS);
    ASSERT_EQ(value, "2.00 EV (f/2.0)");
    ASSERT_EQ(exifMetadata->GetValue("GPSVersionID", value), SUCCESS);
    ASSERT_EQ(value, "2.2.0.0");
    ASSERT_EQ(exifMetadata->GetValue("GPSLatitudeRef", value), SUCCESS);
    ASSERT_EQ(value, "N");
    ASSERT_EQ(exifMetadata->GetValue("GPSLatitude", value), SUCCESS);
    ASSERT_EQ(value, "39, 54, 20");
    ASSERT_EQ(exifMetadata->GetValue("GPSAltitudeRef", value), SUCCESS);
    ASSERT_EQ(value, "Sea level reference");
    ASSERT_EQ(exifMetadata->GetValue("GPSAltitude", value), SUCCESS);
    ASSERT_EQ(value, "0.00");
    ASSERT_EQ(exifMetadata->GetValue("GPSSatellites", value), SUCCESS);
    ASSERT_EQ(value, "xxx");
    ASSERT_EQ(exifMetadata->GetValue("GPSStatus", value), SUCCESS);
    ASSERT_EQ(value, "A");
    ASSERT_EQ(exifMetadata->GetValue("GPSMeasureMode", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadata->GetValue("GPSDOP", value), SUCCESS);
    ASSERT_EQ(value, "182");
}

/**
 * @tc.name: Read003
 * @tc.desc: test dng file not open, read fail
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read003, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT2_DNG_PATH);
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, ERR_IMAGE_SOURCE_DATA);
}

/**
 * @tc.name: Read004
 * @tc.desc: test read empty file
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read004, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT3_DNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, ERR_IMAGE_SOURCE_DATA);
}

/**
 * @tc.name: Read005
 * @tc.desc: test read error file which does not have tiff signature
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read005, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT4_DNG_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, ERR_IMAGE_SOURCE_DATA);
}

/**
 * @tc.name: Read006
 * @tc.desc: test the read, checking if it can correctly initialize
 * a stream from an existing file descriptor
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read006, TestSize.Level3)
{
    int fileDescription = open(IMAGE_INPUT1_DNG_PATH.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_NE(fileDescription, -1);

    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(fileDescription);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, 0);

    auto exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadata->GetValue("DateTimeOriginal", value), SUCCESS);
    ASSERT_EQ(value, "2024:04:18 18:18:43");
    ASSERT_EQ(exifMetadata->GetValue("JPEGInterchangeFormat", value), SUCCESS);
    ASSERT_EQ(value, "8860");
    ASSERT_EQ(exifMetadata->GetValue("JPEGInterchangeFormatLength", value), SUCCESS);
    ASSERT_EQ(value, "4");
}

/**
 * @tc.name: Read007
 * @tc.desc: test the dngDecoded hw properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read007, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_DNG_PATH);
    ASSERT_NE(stream, nullptr);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, SUCCESS);

    std::shared_ptr<ExifMetadata> exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteBurstNumber", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteCaptureMode", value), SUCCESS);
    ASSERT_EQ(value, "1");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceConf", value), SUCCESS);
    ASSERT_EQ(value, "3");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceCount", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceLeyeCenter", value), SUCCESS);
    ASSERT_EQ(value, "1 2 3 4");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceMouthCenter", value), SUCCESS);
    ASSERT_EQ(value, "1 2 3 4 5 6 7 8");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFacePointer", value), SUCCESS);
    ASSERT_EQ(value, "166");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceRect", value), SUCCESS);
    ASSERT_EQ(value, "1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceReyeCenter", value), SUCCESS);
    ASSERT_EQ(value, "5 6 7 8");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceSmileScore", value), SUCCESS);
    ASSERT_EQ(value, "1 2 3 4 5 6 7 8");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceVersion", value), SUCCESS);
    ASSERT_EQ(value, "1");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFocusMode", value), SUCCESS);
    ASSERT_EQ(value, "7");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFrontCamera", value), SUCCESS);
    ASSERT_EQ(value, "3");
    ASSERT_EQ(exifMetadata->GetValue("HwMnotePhysicalAperture", value), SUCCESS);
    ASSERT_EQ(value, "6");
    ASSERT_EQ(exifMetadata->GetValue("HwMnotePitchAngle", value), SUCCESS);
    ASSERT_EQ(value, "5");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteRollAngle", value), SUCCESS);
    ASSERT_EQ(value, "4");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneBeachConf", value), SUCCESS);
    ASSERT_EQ(value, "6");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneBlueSkyConf", value), SUCCESS);
    ASSERT_EQ(value, "4");
}

/**
 * @tc.name: Read008
 * @tc.desc: test the dngDecoded hw properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Read008, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_DNG_PATH);
    ASSERT_NE(stream, nullptr);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, SUCCESS);

    std::shared_ptr<ExifMetadata> exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneFlowersConf", value), SUCCESS);
    ASSERT_EQ(value, "9");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneFoodConf", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneGreenPlantConf", value), SUCCESS);
    ASSERT_EQ(value, "5");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneNightConf", value), SUCCESS);
    ASSERT_EQ(value, "10");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteScenePointer", value), SUCCESS);
    ASSERT_EQ(value, "300");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneSnowConf", value), SUCCESS);
    ASSERT_EQ(value, "7");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneStageConf", value), SUCCESS);
    ASSERT_EQ(value, "3");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneSunsetConf", value), SUCCESS);
    ASSERT_EQ(value, "8");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneTextConf", value), SUCCESS);
    ASSERT_EQ(value, "11");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteSceneVersion", value), SUCCESS);
    ASSERT_EQ(value, "1");
    ASSERT_EQ(exifMetadata->GetValue("MakerNote", value), SUCCESS);
    ASSERT_EQ(value, "HwMnoteCaptureMode:1,HwMnoteBurstNumber:2,HwMnoteFrontCamera:3,HwMnoteRollAngle:4,"
        "HwMnotePitchAngle:5,HwMnotePhysicalAperture:6,HwMnoteFocusMode:7,MovingPhotoId:8,MovingPhotoVersion:9,"
        "MicroVideoPresentationTimestampUS:1713866834879000,HwMnoteFacePointer:166,HwMnoteFaceVersion:1,"
        "HwMnoteFaceCount:2,HwMnoteFaceConf:3,HwMnoteFaceSmileScore:1 2 3 4 5 6 7 8,"
        "HwMnoteFaceRect:1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8,HwMnoteFaceLeyeCenter:1 2 3 4,HwMnoteFaceReyeCenter:5 6 7 8,"
        "HwMnoteFaceMouthCenter:1 2 3 4 5 6 7 8,HwMnoteScenePointer:300,HwMnoteSceneVersion:1,HwMnoteSceneFoodConf:2,"
        "HwMnoteSceneStageConf:3,HwMnoteSceneBlueSkyConf:4,HwMnoteSceneGreenPlantConf:5,HwMnoteSceneBeachConf:6,"
        "HwMnoteSceneSnowConf:7,HwMnoteSceneSunsetConf:8,HwMnoteSceneFlowersConf:9,HwMnoteSceneNightConf:10,"
        "HwMnoteSceneTextConf:11");
}

/**
 * @tc.name: Write001
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write001, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT1_DNG_PATH);
    ASSERT_NE(stream, nullptr);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, SUCCESS);

    std::shared_ptr<ExifMetadata> exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteBurstNumber", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteCaptureMode", value), SUCCESS);
    ASSERT_EQ(value, "1");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceConf", value), SUCCESS);
    ASSERT_EQ(value, "3");

    ASSERT_TRUE(exifMetadata->SetValue("HwMnoteCaptureMode", "0"));
    ASSERT_EQ(imageAccessor.Write(), 0);

    result = imageAccessor.Read();
    ASSERT_EQ(result, SUCCESS);
    exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);

    ASSERT_EQ(exifMetadata->GetValue("HwMnoteBurstNumber", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteCaptureMode", value), SUCCESS);
    ASSERT_EQ(value, "0");
    ASSERT_EQ(exifMetadata->GetValue("HwMnoteFaceConf", value), SUCCESS);
    ASSERT_EQ(value, "3");
}

/**
 * @tc.name: Write002
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write002, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("Orientation", "1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSLatitude", "39/1 54/1 20/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSLatitudeRef", "N"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSLongitudeRef", "E"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSVersionID", "2.2.0.0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSAltitudeRef", "1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSAltitude", "0/100"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSSatellites", "xxx"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSStatus", "A"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSMeasureMode", "2"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("Orientation", value), SUCCESS);
    ASSERT_EQ(value, "Top-left");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSLatitude", value), SUCCESS);
    ASSERT_EQ(value, "39, 54, 20");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSLongitudeRef", value), SUCCESS);
    ASSERT_EQ(value, "E");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSVersionID", value), SUCCESS);
    ASSERT_EQ(value, "2.2.0.0");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSAltitudeRef", value), SUCCESS);
    ASSERT_EQ(value, "Sea level reference");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSAltitude", value), SUCCESS);
    ASSERT_EQ(value, "0.00");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSSatellites", value), SUCCESS);
    ASSERT_EQ(value, "xxx");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSStatus", value), SUCCESS);
    ASSERT_EQ(value, "A");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSMeasureMode", value), SUCCESS);
    ASSERT_EQ(value, "2");
}

/**
 * @tc.name: Write003
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write003, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSImgDirection", "125/56"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSMapDatum", "xxxx"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestLatitudeRef", "N"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestLatitude", "33/1 22/1 11/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestLongitudeRef", "E"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestLongitude", "33/1 22/1 11/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestBearingRef", "T"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestBearing", "22/11"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestDistanceRef", "N"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDestDistance", "10/1"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("GPSImgDirection", value), SUCCESS);
    ASSERT_EQ(value, "2.23");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSMapDatum", value), SUCCESS);
    ASSERT_EQ(value, "xxxx");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestLatitudeRef", value), SUCCESS);
    ASSERT_EQ(value, "N");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestLatitude", value), SUCCESS);
    ASSERT_EQ(value, "33, 22, 11");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestLongitudeRef", value), SUCCESS);
    ASSERT_EQ(value, "E");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestLongitude", value), SUCCESS);
    ASSERT_EQ(value, "33, 22, 11");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestBearingRef", value), SUCCESS);
    ASSERT_EQ(value, "T");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestBearing", value), SUCCESS);
    ASSERT_EQ(value, "2.0");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestDistanceRef", value), SUCCESS);
    ASSERT_EQ(value, "N");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDestDistance", value), SUCCESS);
    ASSERT_EQ(value, "10");
}

/**
 * @tc.name: Write004
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write004, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("FlashpixVersion", "0100"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ColorSpace", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("RelatedSoundFile", "/usr/home/sound/sea.wav"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("FlashEnergy", "832/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SpatialFrequencyResponse", "13"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("FocalPlaneXResolution", "1080/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("FocalPlaneYResolution", "880/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("FocalPlaneResolutionUnit", "3"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubjectLocation", "3 12"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ExposureIndex", "3/2"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("FlashpixVersion", value), SUCCESS);
    ASSERT_EQ(value, "FlashPix Version 1.0");
    ASSERT_EQ(exifMetadataReader->GetValue("ColorSpace", value), SUCCESS);
    ASSERT_EQ(value, "Adobe RGB");
    ASSERT_EQ(exifMetadataReader->GetValue("RelatedSoundFile", value), SUCCESS);
    ASSERT_EQ(value, "/usr/home/sound/sea.wav");
    ASSERT_EQ(exifMetadataReader->GetValue("FlashEnergy", value), SUCCESS);
    ASSERT_EQ(value, "832");
    ASSERT_EQ(exifMetadataReader->GetValue("SpatialFrequencyResponse", value), SUCCESS);
    ASSERT_EQ(value, "13");
    ASSERT_EQ(exifMetadataReader->GetValue("FocalPlaneXResolution", value), SUCCESS);
    ASSERT_EQ(value, "1080");
    ASSERT_EQ(exifMetadataReader->GetValue("FocalPlaneYResolution", value), SUCCESS);
    ASSERT_EQ(value, "880");
    ASSERT_EQ(exifMetadataReader->GetValue("FocalPlaneResolutionUnit", value), SUCCESS);
    ASSERT_EQ(value, "Centimeter");
    ASSERT_EQ(exifMetadataReader->GetValue("SubjectLocation", value), SUCCESS);
    ASSERT_EQ(value, "3, 12");
    ASSERT_EQ(exifMetadataReader->GetValue("ExposureIndex", value), SUCCESS);
    ASSERT_EQ(value, "1.5");
}

/**
 * @tc.name: Write005
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write005, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("FileSource", "3"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("CFAPattern", "3"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("CustomRendered", "1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ExposureMode", "0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("DigitalZoomRatio", "321/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SceneCaptureType", "0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GainControl", "0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Contrast", "0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Saturation", "0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Sharpness", "0"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("FileSource", value), SUCCESS);
    ASSERT_EQ(value, "DSC");
    ASSERT_EQ(exifMetadataReader->GetValue("CFAPattern", value), SUCCESS);
    ASSERT_EQ(value, "1 bytes undefined data");
    ASSERT_EQ(exifMetadataReader->GetValue("CustomRendered", value), SUCCESS);
    ASSERT_EQ(value, "Custom process");
    ASSERT_EQ(exifMetadataReader->GetValue("ExposureMode", value), SUCCESS);
    ASSERT_EQ(value, "Auto exposure");
    ASSERT_EQ(exifMetadataReader->GetValue("DigitalZoomRatio", value), SUCCESS);
    ASSERT_EQ(value, "321");
    ASSERT_EQ(exifMetadataReader->GetValue("SceneCaptureType", value), SUCCESS);
    ASSERT_EQ(value, "Standard");
    ASSERT_EQ(exifMetadataReader->GetValue("GainControl", value), SUCCESS);
    ASSERT_EQ(value, "Normal");
    ASSERT_EQ(exifMetadataReader->GetValue("Contrast", value), SUCCESS);
    ASSERT_EQ(value, "Normal");
    ASSERT_EQ(exifMetadataReader->GetValue("Saturation", value), SUCCESS);
    ASSERT_EQ(value, "Normal");
    ASSERT_EQ(exifMetadataReader->GetValue("Sharpness", value), SUCCESS);
    ASSERT_EQ(value, "Normal");
}

/**
 * @tc.name: Write006
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write006, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("OffsetTimeDigitized", "cfh"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("OffsetTimeOriginal", "chex"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SourceExposureTimesOfCompositeImage", "xxxw"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SourceImageNumberOfCompositeImage", "23 34"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSHPositioningError", "5/2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Software", "MNA-AL00 4.0.0.120(C00E116R3P7)"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Artist", "Joseph.Xu"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("WhitePoint", "252/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("PrimaryChromaticities", "124/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ISOSpeedLatitudeyyy", "1456"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("OffsetTimeDigitized", value), SUCCESS);
    ASSERT_EQ(value, "cfh");
    ASSERT_EQ(exifMetadataReader->GetValue("OffsetTimeOriginal", value), SUCCESS);
    ASSERT_EQ(value, "chex");
    ASSERT_EQ(exifMetadataReader->GetValue("SourceExposureTimesOfCompositeImage", value), SUCCESS);
    ASSERT_EQ(value, "xxxw");
    ASSERT_EQ(exifMetadataReader->GetValue("SourceImageNumberOfCompositeImage", value), SUCCESS);
    ASSERT_EQ(value, "23, 34");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSHPositioningError", value), SUCCESS);
    ASSERT_EQ(value, "2.5");
    ASSERT_EQ(exifMetadataReader->GetValue("Software", value), SUCCESS);
    ASSERT_EQ(value, "MNA-AL00 4.0.0.120(C00E116R3P7)");
    ASSERT_EQ(exifMetadataReader->GetValue("Artist", value), SUCCESS);
    ASSERT_EQ(value, "Joseph.Xu");
    ASSERT_EQ(exifMetadataReader->GetValue("WhitePoint", value), SUCCESS);
    ASSERT_EQ(value, "252, 0/0");
    ASSERT_EQ(exifMetadataReader->GetValue("PrimaryChromaticities", value), SUCCESS);
    ASSERT_EQ(value, "124");
    ASSERT_EQ(exifMetadataReader->GetValue("ISOSpeedLatitudeyyy", value), SUCCESS);
    ASSERT_EQ(value, "1456");
}

/**
 * @tc.name: Write007
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write007, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("XResolution", "72/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("LensSpecification", "3/4 5/2 3/2 1/2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubjectArea", "12 13"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubfileType", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("LensMake", "xxwx"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ImageLength", "1000"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ImageWidth", "1001"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("DateTimeOriginal", "2024:01:25 05:51:34"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ExposureTime", "1/34"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SceneType", "1"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("XResolution", value), SUCCESS);
    ASSERT_EQ(value, "72");
    ASSERT_EQ(exifMetadataReader->GetValue("LensSpecification", value), SUCCESS);
    ASSERT_EQ(value, "0.8, 2.5, 1.5, 0.5");
    ASSERT_EQ(exifMetadataReader->GetValue("SubjectArea", value), SUCCESS);
    ASSERT_EQ(value, "(x,y) = (12,13)");
    ASSERT_EQ(exifMetadataReader->GetValue("SubfileType", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadataReader->GetValue("LensMake", value), SUCCESS);
    ASSERT_EQ(value, "xxwx");
    ASSERT_EQ(exifMetadataReader->GetValue("ImageLength", value), SUCCESS);
    ASSERT_EQ(value, "1000");
    ASSERT_EQ(exifMetadataReader->GetValue("ImageWidth", value), SUCCESS);
    ASSERT_EQ(value, "1001");
    ASSERT_EQ(exifMetadataReader->GetValue("DateTimeOriginal", value), SUCCESS);
    ASSERT_EQ(value, "2024:01:25 05:51:34");
    ASSERT_EQ(exifMetadataReader->GetValue("ExposureTime", value), SUCCESS);
    ASSERT_EQ(value, "1/34 sec.");
    ASSERT_EQ(exifMetadataReader->GetValue("SceneType", value), SUCCESS);
    ASSERT_EQ(value, "Directly photographed");
}

/**
 * @tc.name: Write008
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write008, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("Make", "XiaoMI"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Model", "TNY-AL00"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("PhotoMode", "252"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SensitivityType", "5"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("StandardOutputSensitivity", "5"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("RecommendedExposureIndex", "241"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ISOSpeed", "1456"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ApertureValue", "4/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ExposureBiasValue", "23/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("MeteringMode", "5"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("Make", value), SUCCESS);
    ASSERT_EQ(value, "XiaoMI");
    ASSERT_EQ(exifMetadataReader->GetValue("Model", value), SUCCESS);
    ASSERT_EQ(value, "TNY-AL00");
    ASSERT_EQ(exifMetadataReader->GetValue("PhotoMode", value), SUCCESS);
    ASSERT_EQ(value, "252");
    ASSERT_EQ(exifMetadataReader->GetValue("SensitivityType", value), SUCCESS);
    ASSERT_EQ(value, "Standard output sensitivity (SOS) and ISO speed");
    ASSERT_EQ(exifMetadataReader->GetValue("StandardOutputSensitivity", value), SUCCESS);
    ASSERT_EQ(value, "5");
    ASSERT_EQ(exifMetadataReader->GetValue("RecommendedExposureIndex", value), SUCCESS);
    ASSERT_EQ(value, "241");
    ASSERT_EQ(exifMetadataReader->GetValue("ISOSpeed", value), SUCCESS);
    ASSERT_EQ(value, "1456");
    ASSERT_EQ(exifMetadataReader->GetValue("ApertureValue", value), SUCCESS);
    ASSERT_EQ(value, "4.00 EV (f/4.0)");
    ASSERT_EQ(exifMetadataReader->GetValue("ExposureBiasValue", value), SUCCESS);
    ASSERT_EQ(value, "23.00 EV");
    ASSERT_EQ(exifMetadataReader->GetValue("MeteringMode", value), SUCCESS);
    ASSERT_EQ(value, "Pattern");
}

/**
 * @tc.name: Write009
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write009, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("WhiteBalance", "1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("FocalLengthIn35mmFilm", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("CompressedBitsPerPixel", "24/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("YResolution", "252/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("JPEGProc", "252"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("RowsPerStrip", "252"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSLongitude", "120,52,26"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("MaxApertureValue", "4/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("OECF", "45"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SpectralSensitivity", "sensitivity"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("WhiteBalance", value), SUCCESS);
    ASSERT_EQ(value, "Manual white balance");
    ASSERT_EQ(exifMetadataReader->GetValue("FocalLengthIn35mmFilm", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadataReader->GetValue("CompressedBitsPerPixel", value), SUCCESS);
    ASSERT_EQ(value, "24");
    ASSERT_EQ(exifMetadataReader->GetValue("YResolution", value), SUCCESS);
    ASSERT_EQ(value, "252");
    ASSERT_EQ(exifMetadataReader->GetValue("JPEGProc", value), SUCCESS);
    ASSERT_EQ(value, "252");
    ASSERT_EQ(exifMetadataReader->GetValue("RowsPerStrip", value), SUCCESS);
    ASSERT_EQ(value, "252");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSLongitude", value), SUCCESS);
    ASSERT_EQ(value, "120, 52, 26");
    ASSERT_EQ(exifMetadataReader->GetValue("MaxApertureValue", value), SUCCESS);
    ASSERT_EQ(value, "4.00 EV (f/4.0)");
    ASSERT_EQ(exifMetadataReader->GetValue("OECF", value), SUCCESS);
    ASSERT_EQ(value, "2 bytes undefined data");
    ASSERT_EQ(exifMetadataReader->GetValue("SpectralSensitivity", value), SUCCESS);
    ASSERT_EQ(value, "sensitivity");
}

/**
 * @tc.name: Write010
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write010, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("CompositeImage", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("DateTimeDigitized", "2023:01:19 10:39:58"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ExifVersion", "0210"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ExposureProgram", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("DNGVersion", "1 1 0 0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("StripByteCounts", "3"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("StripOffsets", "6"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("BitsPerSample", "8,6,7"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("CompositeImage", value), SUCCESS);
    ASSERT_EQ(value, "2");
    ASSERT_EQ(exifMetadataReader->GetValue("DateTimeDigitized", value), SUCCESS);
    ASSERT_EQ(value, "2023:01:19 10:39:58");
    ASSERT_EQ(exifMetadataReader->GetValue("ExifVersion", value), SUCCESS);
    ASSERT_EQ(value, "Exif Version 2.1");
    ASSERT_EQ(exifMetadataReader->GetValue("ExposureProgram", value), SUCCESS);
    ASSERT_EQ(value, "Normal program");
    ASSERT_EQ(exifMetadataReader->GetValue("DNGVersion", value), SUCCESS);
    ASSERT_EQ(value, "0x01, 0x01, 0x00, 0x00");
    ASSERT_EQ(exifMetadataReader->GetValue("StripByteCounts", value), SUCCESS);
    ASSERT_EQ(value, "3");
    ASSERT_EQ(exifMetadataReader->GetValue("StripOffsets", value), SUCCESS);
    ASSERT_EQ(value, "6");
    ASSERT_EQ(exifMetadataReader->GetValue("BitsPerSample", value), SUCCESS);
    ASSERT_EQ(value, "8, 6, 7");
}

/**
 * @tc.name: Write011
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write011, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("YCbCrCoefficients", "299/1000 587/1000 114/1000"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("YCbCrSubSampling", "3 2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("YCbCrPositioning", "1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ReferenceBlackWhite", "222 0 1.5 0 25.2 25.2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Copyright", "Hw"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SensingMethod", "3"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("LightSource", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Flash", "5"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("FocalLength", "31/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("UserComment", "comm"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("YCbCrCoefficients", value), SUCCESS);
    ASSERT_EQ(value, "0.299, 0.587, 0.114");
    ASSERT_EQ(exifMetadataReader->GetValue("YCbCrSubSampling", value), SUCCESS);
    ASSERT_EQ(value, "3, 2");
    ASSERT_EQ(exifMetadataReader->GetValue("YCbCrPositioning", value), SUCCESS);
    ASSERT_EQ(value, "Centered");
    ASSERT_EQ(exifMetadataReader->GetValue("ReferenceBlackWhite", value), SUCCESS);
    ASSERT_EQ(value, "222,  0, 1.5,  0, 25.2, 25.2");
    ASSERT_EQ(exifMetadataReader->GetValue("Copyright", value), SUCCESS);
    ASSERT_EQ(value, "Hw (Photographer) - [None] (Editor)");
    ASSERT_EQ(exifMetadataReader->GetValue("SensingMethod", value), SUCCESS);
    ASSERT_EQ(value, "Two-chip color area sensor");
    ASSERT_EQ(exifMetadataReader->GetValue("LightSource", value), SUCCESS);
    ASSERT_EQ(value, "Fluorescent");
    ASSERT_EQ(exifMetadataReader->GetValue("Flash", value), SUCCESS);
    ASSERT_EQ(value, "Strobe return light not detected");
    ASSERT_EQ(exifMetadataReader->GetValue("FocalLength", value), SUCCESS);
    ASSERT_EQ(value, "31.0 mm");
    ASSERT_EQ(exifMetadataReader->GetValue("UserComment", value), SUCCESS);
    ASSERT_EQ(value, "comm");
}

/**
 * @tc.name: Write012
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write012, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("PixelXDimension", "1000"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("PixelYDimension", "2000"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ISOSpeedLatitudezzz", "1456"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubjectDistance", "5/2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("DefaultCropSize", "153 841"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("PlanarConfiguration", "1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ResolutionUnit", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("TransferFunction", "2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ISOSpeedRatings", "160"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("FNumber", "3/1"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("PixelXDimension", value), SUCCESS);
    ASSERT_EQ(value, "1000");
    ASSERT_EQ(exifMetadataReader->GetValue("PixelYDimension", value), SUCCESS);
    ASSERT_EQ(value, "2000");
    ASSERT_EQ(exifMetadataReader->GetValue("ISOSpeedLatitudezzz", value), SUCCESS);
    ASSERT_EQ(value, "1456");
    ASSERT_EQ(exifMetadataReader->GetValue("SubjectDistance", value), SUCCESS);
    ASSERT_EQ(value, "2.5 m");
    ASSERT_EQ(exifMetadataReader->GetValue("DefaultCropSize", value), SUCCESS);
    ASSERT_EQ(value, "153, 841");
    ASSERT_EQ(exifMetadataReader->GetValue("PlanarConfiguration", value), SUCCESS);
    ASSERT_EQ(value, "Chunky format");
    ASSERT_EQ(exifMetadataReader->GetValue("ResolutionUnit", value), SUCCESS);
    ASSERT_EQ(value, "Inch");
    ASSERT_EQ(exifMetadataReader->GetValue("TransferFunction", value), SUCCESS);
    ASSERT_EQ(value, "1 bytes undefined data");
    ASSERT_EQ(exifMetadataReader->GetValue("ISOSpeedRatings", value), SUCCESS);
    ASSERT_EQ(value, "160");
    ASSERT_EQ(exifMetadataReader->GetValue("FNumber", value), SUCCESS);
    ASSERT_EQ(value, "f/3.0");
}

/**
 * @tc.name: Write013
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write013, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("DateTime", "2024:01:25 05:51:34"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSTimeStamp", "11/1 37/1 56/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDateStamp", "2023:10:19"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ImageDescription", "_cuva"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSProcessingMethod", "CELLID"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSAreaInformation", "arexxx"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDifferential", "0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubsecTime", "427000"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubsecTimeOriginal", "427000"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubsecTimeDigitized", "427000"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("DateTime", value), SUCCESS);
    ASSERT_EQ(value, "2024:01:25 05:51:34");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSTimeStamp", value), SUCCESS);
    ASSERT_EQ(value, "11:37:56.00");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDateStamp", value), SUCCESS);
    ASSERT_EQ(value, "2023:10:19");
    ASSERT_EQ(exifMetadataReader->GetValue("ImageDescription", value), SUCCESS);
    ASSERT_EQ(value, "_cuva");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSProcessingMethod", value), SUCCESS);
    ASSERT_EQ(value, "CELLID");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSAreaInformation", value), SUCCESS);
    ASSERT_EQ(value, "arexxx");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDifferential", value), SUCCESS);
    ASSERT_EQ(value, "0");
    ASSERT_EQ(exifMetadataReader->GetValue("SubsecTime", value), SUCCESS);
    ASSERT_EQ(value, "427000");
    ASSERT_EQ(exifMetadataReader->GetValue("SubsecTimeOriginal", value), SUCCESS);
    ASSERT_EQ(value, "427000");
    ASSERT_EQ(exifMetadataReader->GetValue("SubsecTimeDigitized", value), SUCCESS);
    ASSERT_EQ(value, "427000");
}

/**
 * @tc.name: Write014
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write014, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSDOP", "182/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSSpeedRef", "K"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSSpeed", "150/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSTrackRef", "T"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSTrack", "114/3"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("GPSImgDirectionRef", "M"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("DeviceSettingDescription", "2xxx"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("SubjectDistanceRange", "0"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ImageUniqueID", "FXIC012"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ComponentsConfiguration", "1456"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("GPSDOP", value), SUCCESS);
    ASSERT_EQ(value, "182");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSSpeedRef", value), SUCCESS);
    ASSERT_EQ(value, "K");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSSpeed", value), SUCCESS);
    ASSERT_EQ(value, "150");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSTrackRef", value), SUCCESS);
    ASSERT_EQ(value, "T");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSTrack", value), SUCCESS);
    ASSERT_EQ(value, "38.0");
    ASSERT_EQ(exifMetadataReader->GetValue("GPSImgDirectionRef", value), SUCCESS);
    ASSERT_EQ(value, "M");
    ASSERT_EQ(exifMetadataReader->GetValue("DeviceSettingDescription", value), SUCCESS);
    ASSERT_EQ(value, "2xxx");
    ASSERT_EQ(exifMetadataReader->GetValue("SubjectDistanceRange", value), SUCCESS);
    ASSERT_EQ(value, "Unknown");
    ASSERT_EQ(exifMetadataReader->GetValue("ImageUniqueID", value), SUCCESS);
    ASSERT_EQ(value, "FXIC012");
    ASSERT_EQ(exifMetadataReader->GetValue("ComponentsConfiguration", value), SUCCESS);
    ASSERT_EQ(value, "Y R G B");
}

/**
 * @tc.name: Write015
 * @tc.desc: test write properties
 * @tc.type: FUNC
 */
HWTEST_F(DngExifMetadataAccessorTest, Write015, TestSize.Level3)
{
    std::shared_ptr<MetadataStream> writerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(writerStream, nullptr);
    ASSERT_TRUE(writerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageWriterAccessor(writerStream);
    ASSERT_EQ(imageWriterAccessor.Read(), 0);
    auto exifMetadataWriter = imageWriterAccessor.Get();
    ASSERT_NE(exifMetadataWriter, nullptr);

    ASSERT_TRUE(exifMetadataWriter->SetValue("LensModel", "txaw"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("LensSerialNumber", "qxhc"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("Gamma", "5/2"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("OffsetTime", "2024:01:25"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("ShutterSpeedValue", "13/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("BodySerialNumber", "exoch"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("BrightnessValue", "13/1"));
    ASSERT_TRUE(exifMetadataWriter->SetValue("CameraOwnerName", "c.uec"));

    ASSERT_EQ(imageWriterAccessor.Write(), 0);

    std::shared_ptr<MetadataStream> readerStream = std::make_shared<FileMetadataStream>(IMAGE_OUTPUT2_DNG_PATH);
    ASSERT_NE(readerStream, nullptr);
    ASSERT_TRUE(readerStream->Open(OpenMode::ReadWrite));
    DngExifMetadataAccessor imageReaderAccessor(readerStream);
    ASSERT_EQ(imageReaderAccessor.Read(), 0);
    auto exifMetadataReader = imageReaderAccessor.Get();
    ASSERT_NE(exifMetadataReader, nullptr);

    std::string value;
    ASSERT_EQ(exifMetadataReader->GetValue("LensModel", value), SUCCESS);
    ASSERT_EQ(value, "txaw");
    ASSERT_EQ(exifMetadataReader->GetValue("LensSerialNumber", value), SUCCESS);
    ASSERT_EQ(value, "qxhc");
    ASSERT_EQ(exifMetadataReader->GetValue("Gamma", value), SUCCESS);
    ASSERT_EQ(value, "2.5");
    ASSERT_EQ(exifMetadataReader->GetValue("OffsetTime", value), SUCCESS);
    ASSERT_EQ(value, "2024:01:25");
    ASSERT_EQ(exifMetadataReader->GetValue("ShutterSpeedValue", value), SUCCESS);
    ASSERT_EQ(value, "13.00 EV (1/8192 sec.)");
    ASSERT_EQ(exifMetadataReader->GetValue("BodySerialNumber", value), SUCCESS);
    ASSERT_EQ(value, "exoch");
    ASSERT_EQ(exifMetadataReader->GetValue("BrightnessValue", value), SUCCESS);
    ASSERT_EQ(value, "13.00 EV (28067.91 cd/m^2)");
    ASSERT_EQ(exifMetadataReader->GetValue("CameraOwnerName", value), SUCCESS);
    ASSERT_EQ(value, "c.uec");
}
} // namespace Multimedia
} // namespace OHOS