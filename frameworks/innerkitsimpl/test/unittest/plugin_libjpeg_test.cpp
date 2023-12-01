/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <fcntl.h>
#include "buffer_source_stream.h"
#include "exif_info.h"
#include "jpeg_decoder.h"
#include "media_errors.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImagePlugin;
namespace OHOS {
namespace Media {
static const std::string IMAGE_INPUT_NULL_JPEG_PATH = "/data/local/tmp/image/test_null.jpg";
static const std::string IMAGE_INPUT_TXT_PATH = "/data/local/tmp/image/test.txt";
class PluginLibJpegTest : public testing::Test {
public:
    PluginLibJpegTest() {}
    ~PluginLibJpegTest() {}
};

/**
 * @tc.name: exif_info001
 * @tc.desc: ParseExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001 start";
    EXIFInfo exinfo;
    unsigned char *buf = nullptr;
    unsigned len = 1000;
    exinfo.ParseExifData(buf, len);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001 end";
}

/**
 * @tc.name: exif_info001_1
 * @tc.desc: ParseExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info001_1, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_1 start";
    EXIFInfo exinfo;
    unsigned char buf = 'n';
    unsigned len = 1000;
    exinfo.ParseExifData(&buf, len);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_1 end";
}

/**
 * @tc.name: exif_info001_2
 * @tc.desc: ParseExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info001_2, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_2 start";
    EXIFInfo exinfo;
    unsigned char *buf = nullptr;
    unsigned len = 0;
    exinfo.ParseExifData(buf, len);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_2 end";
}

/**
 * @tc.name: exif_info001_3
 * @tc.desc: ParseExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info001_3, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_3 start";
    EXIFInfo exinfo;
    string data = "";
    exinfo.ParseExifData(data);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_3 end";
}

/**
 * @tc.name: exif_info001_4
 * @tc.desc: ParseExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info001_4, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_4 start";
    EXIFInfo exinfo;
    string data = "aaaaaaa";
    exinfo.ParseExifData(data);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info001_4 end";
}

/**
 * @tc.name: exif_info002
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info002 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    string value = "111";
    uint32_t ret = exinfo.ModifyExifData(tag, value, "");
    ASSERT_EQ(ret, ERR_MEDIA_IO_ABNORMAL);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info002 end";
}

/**
 * @tc.name: exif_info003
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info003 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    string value = "111";
    uint32_t ret = exinfo.ModifyExifData(tag, value, IMAGE_INPUT_NULL_JPEG_PATH);
    ASSERT_EQ(ret, ERR_MEDIA_BUFFER_TOO_SMALL);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info003 end";
}

/**
 * @tc.name: exif_info004
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info004 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    string value = "111";
    uint32_t ret = exinfo.ModifyExifData(tag, value, IMAGE_INPUT_TXT_PATH);
    ASSERT_NE(ret, SUCCESS);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info004 end";
}

/**
 * @tc.name: exif_info005
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info005 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    string value = "111";
    int fd = open("/data/local/tmp/image/test_noexit.jpg", O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t ret = exinfo.ModifyExifData(tag, value, fd);
    ASSERT_NE(ret, SUCCESS);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info005 end";
}

/**
 * @tc.name: exif_info006
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info006 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    string value = "111";
    const int fd = open("/data/local/tmp/image/test_null.jpg", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    uint32_t ret = exinfo.ModifyExifData(tag, value, fd);
    ASSERT_EQ(ret, ERR_MEDIA_BUFFER_TOO_SMALL);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info006 end";
}

/**
 * @tc.name: exif_info007
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info007, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info007 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    string value = "111";
    const int fd = open("/data/local/tmp/image/test.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    uint32_t ret = exinfo.ModifyExifData(tag, value, fd);
    ASSERT_NE(ret, SUCCESS);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info007 end";
}

/**
 * @tc.name: exif_info008
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info008, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info008 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    unsigned char *data = nullptr;
    string value = "111";
    uint32_t size = 0;
    uint32_t ret = exinfo.ModifyExifData(tag, value, data, size);
    ASSERT_EQ(ret, ERR_IMAGE_SOURCE_DATA);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info008 end";
}

/**
 * @tc.name: exif_info009
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info009, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info009 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    unsigned char data = 'n';
    string value = "111";
    uint32_t size = 0;
    uint32_t ret = exinfo.ModifyExifData(tag, value, &data, size);
    ASSERT_EQ(ret, ERR_MEDIA_BUFFER_TOO_SMALL);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info009 end";
}

/**
 * @tc.name: exif_info010
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info010, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info010 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    unsigned char data = 'n';
    std::string value = "111";
    uint32_t size = 1;
    uint32_t ret = exinfo.ModifyExifData(tag, value, &data, size);
    ASSERT_EQ(ret, ERR_IMAGE_MISMATCHED_FORMAT);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info010 end";
}

/**
 * @tc.name: exif_info011
 * @tc.desc: ModifyExifData
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info011, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info011 start";
    EXIFInfo exinfo;
    ExifTag tag = EXIF_TAG_GPS_LATITUDE;
    unsigned char data = 0xFF;
    std::string value = "111";
    uint32_t size = 1;
    uint32_t ret = exinfo.ModifyExifData(tag, value, &data, size);
    ASSERT_EQ(ret, ERR_IMAGE_MISMATCHED_FORMAT);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info011 end";
}

/**
 * @tc.name: exif_info012
 * @tc.desc: GetFilterArea
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info012, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info012 start";
    EXIFInfo exinfo;
    const uint32_t bufSize = 5;
    uint8_t buf[bufSize] = "exif";
    int filterType = 0;
    std::vector<std::pair<uint32_t, uint32_t>> ranges;
    exinfo.GetFilterArea(buf, bufSize, filterType, ranges);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info012 end";
}

/**
 * @tc.name: exif_info013
 * @tc.desc: GetFilterArea
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info013, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info013 start";
    EXIFInfo exinfo;
    const uint32_t bufSize = 5;
    uint8_t buf[bufSize] = "exif";
    int filterType = 0;
    std::vector<std::pair<uint32_t, uint32_t>> ranges;
    uint32_t ret = exinfo.GetFilterArea(buf, bufSize, filterType, ranges);
    ASSERT_EQ(ret, 1);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: exif_info013 end";
}

/**
 * @tc.name: exif_info014
 * @tc.desc: IsIFDPointerTag
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info014, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: IsIFDPointerTag start";
    EXIFInfo exinfo;
    bool ret;
    ret = exinfo.IsIFDPointerTag(0x014a);
    ASSERT_EQ(ret, true);
    ret = exinfo.IsIFDPointerTag(0x8769);
    ASSERT_EQ(ret, true);
    ret = exinfo.IsIFDPointerTag(0x8825);
    ASSERT_EQ(ret, true);
    ret = exinfo.IsIFDPointerTag(0xa005);
    ASSERT_EQ(ret, true);
    ret = exinfo.IsIFDPointerTag(0xa301);
    ASSERT_EQ(ret, false);
    ret = exinfo.IsIFDPointerTag(-1);
    ASSERT_EQ(ret, false);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: IsIFDPointerTag end";
}

/**
 * @tc.name: exif_info015
 * @tc.desc: GetIFDOfIFDPointerTag
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info015, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: GetIFDOfIFDPointerTag start";
    EXIFInfo exinfo;
    ExifIfd ret;
    ret = exinfo.GetIFDOfIFDPointerTag(0x8769);
    ASSERT_EQ(ret, EXIF_IFD_EXIF);
    ret = exinfo.GetIFDOfIFDPointerTag(0x8825);
    ASSERT_EQ(ret, EXIF_IFD_GPS);
    ret = exinfo.GetIFDOfIFDPointerTag(0xa005);
    ASSERT_EQ(ret, EXIF_IFD_INTEROPERABILITY);
    ret = exinfo.GetIFDOfIFDPointerTag(-1);
    ASSERT_EQ(ret, EXIF_IFD_COUNT);
    // bool ret = exinfo.GetIFDOfIFDPointerTag(0xa301);
    // ASSERT_EQ(ret, EXIF_IFD_COUNT);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: GetIFDOfIFDPointerTag end";
}

/**
 * @tc.name: exif_info016
 * @tc.desc: CheckExifEntryValid
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info016, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: CheckExifEntryValid start";
    EXIFInfo exinfo;
    bool ret;
    ret = exinfo.CheckExifEntryValid(EXIF_IFD_0, EXIF_TAG_ORIENTATION);
    ASSERT_EQ(ret, true);
    ret = exinfo.CheckExifEntryValid(EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);
    ASSERT_EQ(ret, true);
    ret = exinfo.CheckExifEntryValid(EXIF_IFD_GPS, EXIF_TAG_GPS_LATITUDE);
    ASSERT_EQ(ret, true);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: CheckExifEntryValid end";
}

/**
 * @tc.name: exif_info017
 * @tc.desc: CheckExifEntryValidEx
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info017, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: CheckExifEntryValidEx start";
    EXIFInfo exinfo;
    bool ret;
    ret = exinfo.CheckExifEntryValid(EXIF_IFD_0, EXIF_TAG_DATE_TIME);
    ASSERT_EQ(ret, true);
    ret = exinfo.CheckExifEntryValid(EXIF_IFD_EXIF, TAG_SENSITIVITY_TYPE);
    ASSERT_EQ(ret, true);
    ret = exinfo.CheckExifEntryValid(EXIF_IFD_GPS, EXIF_TAG_GPS_TIME_STAMP);
    ASSERT_EQ(ret, true);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: CheckExifEntryValidEx end";
}

/**
 * @tc.name: exif_info018
 * @tc.desc: GetEncodeFormat
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info018, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: GetEncodeFormat start";
    auto jpegEncoder = std::make_shared<JpegEncoder>();
    int32_t componentsNum;
    jpegEncoder->GetEncodeFormat(PixelFormat::RGBA_F16, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_RGBA);
    
    jpegEncoder->GetEncodeFormat(PixelFormat::RGBA_8888, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_RGBA);
    
    jpegEncoder->GetEncodeFormat(PixelFormat::BGRA_8888, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_BGRA);
    
    jpegEncoder->GetEncodeFormat(PixelFormat::ARGB_8888, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_ARGB);

    jpegEncoder->GetEncodeFormat(PixelFormat::ALPHA_8, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_GRAY);

    jpegEncoder->GetEncodeFormat(PixelFormat::RGB_565, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_RGB);

    jpegEncoder->GetEncodeFormat(PixelFormat::RGB_888, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_RGB);

    jpegEncoder->GetEncodeFormat(PixelFormat::NV12, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_YUV420SP);

    jpegEncoder->GetEncodeFormat(PixelFormat::NV21, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_YUV420SP);

    jpegEncoder->GetEncodeFormat(PixelFormat::CMYK, componentsNum);
    ASSERT_EQ(componentsNum, COMPONENT_NUM_RGBA);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: GetEncodeFormat end";
}

/**
 * @tc.name: exif_info019
 * @tc.desc: GenerateDEArray
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info019, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: GenerateDEArray start";
    EXIFInfo exinfo;
    unsigned char *buf = new unsigned char;
    exinfo.GetOrginExifDataLength(false, buf);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: GenerateDEArray end";
}

/**
 * @tc.name: exif_info020
 * @tc.desc: EXIFInfoBufferCheck
 * @tc.type: FUNC
 */
HWTEST_F(PluginLibJpegTest, exif_info020, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PluginLibJpegTest: EXIFInfoBufferCheck002 start";
    EXIFInfo exinfo;
    auto exifEntry = std::make_shared<ExifEntry>();
    exifEntry->size = 2;
    exinfo.EXIFInfoBufferCheck(exifEntry, 5);
    GTEST_LOG_(INFO) << "PluginLibJpegTest: EXIFInfoBufferCheck002 end";
}
} // namespace Multimedia
} // namespace OHOS