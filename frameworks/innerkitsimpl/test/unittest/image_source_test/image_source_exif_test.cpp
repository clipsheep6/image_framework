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
#define private public
#include <algorithm>
#include <fcntl.h>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>
#include "image/abs_image_decoder.h"
#include "image/abs_image_format_agent.h"
#include "image/image_plugin_type.h"
#include "image_log.h"
#include "image_utils.h"
#include "image_source_util.h"
#include "incremental_source_stream.h"
#include "istream_source_stream.h"
#include "media_errors.h"
#include "pixel_map.h"
#include "plugin_server.h"
#include "post_proc.h"
#include "source_stream.h"
#include "image_source.h"
#include "buffer_source_stream.h"
#include "file_source_stream.h"
#include "memory_manager.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Media {
static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test_exif.jpg";
static const std::string IMAGE_INPUT_HW_JPEG_PATH = "/data/local/tmp/image/mmexport1704942582941.jpg";
static const std::string IMAGE_INPUT_HWMAKE_JPEG_PATH = "/data/local/tmp/image/hwmake.jpg";
static const std::string IMAGE_INPUT_PNG_PATH = "/data/local/tmp/image/test.png";

class ImageSourceExifTest : public testing::Test {
public:
    ImageSourceExifTest() {}
    ~ImageSourceExifTest() {}
};


HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_BitsPerSample_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_BitsPerSample_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "BitsPerSample";
    std::string modifyvalue = "9,9,8";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "9 9 8");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_BitsPerSample_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_BitsPerSample_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_BitsPerSample_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "BitsPerSample";
    std::string modifyvalue = "9 9 8";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string gvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, gvalue);
    GTEST_LOG_(INFO) << "gvalue is "<< gvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(gvalue, "9 9 8");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_BitsPerSample_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_Orientation_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Orientation_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "Orientation";
    std::string modifyvalue = "2";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "top, right");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Orientation_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ImageLength_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ImageLength_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ImageLength";
    std::string modifyvalue = "1000";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1000");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ImageLength_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ImageWidth_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ImageWidth_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ImageWidth";
    std::string modifyvalue = "500";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "500");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ImageWidth_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_CompressedBitsPerPixel_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_CompressedBitsPerPixel_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "CompressedBitsPerPixel";
    std::string modifyvalue = "24/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "24");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_CompressedBitsPerPixel_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_CompressedBitsPerPixel_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_CompressedBitsPerPixel_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "CompressedBitsPerPixel";
    std::string modifyvalue = "24";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "24");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_CompressedBitsPerPixel_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_CompressedBitsPerPixel_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_CompressedBitsPerPixel_003 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "CompressedBitsPerPixel";
    std::string modifyvalue = "24.5";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "24.5");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_CompressedBitsPerPixel_003 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSLatitude_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLatitude";
    std::string modifyvalue = "39/1 54/1 20/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "39 deg 54' 20\"");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSLatitude_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLatitude";
    std::string modifyvalue = "39 54 20";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "39 deg 54' 20\"");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSLatitude_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_003 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLatitude";
    std::string modifyvalue = "39,54,20";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "39 deg 54' 20\"");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_003 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSLatitude_004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_004 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLatitude";
    std::string modifyvalue = "39 54 20";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "39 deg 54' 20\"");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitude_004 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSLongitude_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLongitude_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLongitude";
    std::string modifyvalue = "120,52,26";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "120 deg 52' 26\"");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLongitude_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSLatitudeRef_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitudeRef_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLatitudeRef";
    std::string modifyvalue = "N";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "North");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLatitudeRef_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSLongitudeRef_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLongitudeRef_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLongitudeRef";
    std::string modifyvalue = "E";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "East");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSLongitudeRef_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_WhiteBalance_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_WhiteBalance_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "WhiteBalance";
    std::string modifyvalue = "1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "Manual");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_WhiteBalance_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_FocalLengthIn35mmFilm_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FocalLengthIn35mmFilm_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "FocalLengthIn35mmFilm";
    std::string modifyvalue = "26";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "26.0 mm");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FocalLengthIn35mmFilm_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_Flash_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Flash_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "Flash";
    std::string modifyvalue = "5";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "Fired, return light not detected");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Flash_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_Flash_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Flash_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "Flash";
    std::string modifyvalue = "2";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_NE(modifyret, SUCCESS);

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Flash_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ApertureValue_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ApertureValue_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ApertureValue";
    std::string modifyvalue = "4/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "F4");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ApertureValue_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ApertureValue_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ApertureValue_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ApertureValue";
    std::string modifyvalue = "4";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "F4");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ApertureValue_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ApertureValue_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ApertureValue_003 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ApertureValue";
    std::string modifyvalue = "4.0";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "F4");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ApertureValue_003 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_DateTimeOriginal_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_DateTimeOriginal_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "DateTimeOriginal";
    std::string modifyvalue = "2024:01:25 05:51:34";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "2024:01:25 05:51:34");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_DateTimeOriginal_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_DateTime_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_DateTime_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "DateTimeOriginal";
    std::string modifyvalue = "2024:01:23";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "2024:01:23");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_DateTime_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ExposureBiasValue_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ExposureBiasValue_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ExposureBiasValue";
    std::string modifyvalue = "23/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "+23 EV");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ExposureBiasValue_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ExposureTime_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ExposureTime_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ExposureTime";
    std::string modifyvalue = "1/34";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1/34 s");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ExposureTime_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_FNumber_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FNumber_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "FNumber";
    std::string modifyvalue = "3/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "F3");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FNumber_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_FNumber_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FNumber_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "FNumber";
    std::string modifyvalue = "3";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "F3");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FNumber_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_FNumber_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FNumber_003 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "FNumber";
    std::string modifyvalue = "3.0";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "F3");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FNumber_003 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_FocalLength_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FocalLength_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "FocalLength";
    std::string modifyvalue = "31/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "31.0 mm");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FocalLength_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_FocalLength_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FocalLength_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "FocalLength";
    std::string modifyvalue = "31.0";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "31.0 mm");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_FocalLength_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSTimeStamp_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSTimeStamp_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSTimeStamp";
    std::string modifyvalue = "11/1 37/1 58/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "11:37:58");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSTimeStamp_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSTimeStamp_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSTimeStamp_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSTimeStamp";
    std::string modifyvalue = "11:37:58";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "11:37:58");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSTimeStamp_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_GPSDateStamp_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSDateStamp_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSDateStamp";
    std::string modifyvalue = "2025:01:11";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "2025:01:11");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_GPSDateStamp_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ImageDescription_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ImageDescription_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ImageDescription";
    std::string modifyvalue = "_cuva";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "_cuva");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ImageDescription_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ISOSpeedRatings_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ISOSpeedRatings_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ISOSpeedRatings";
    std::string modifyvalue = "160";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "160");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ISOSpeedRatings_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_ISOSpeed_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ISOSpeed_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "ISOSpeed";
    std::string modifyvalue = "200";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "200");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_ISOSpeed_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_LightSource_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LightSource_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "LightSource";
    std::string modifyvalue = "2";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "Fluorescent");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LightSource_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_MeteringMode_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_MeteringMode_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "MeteringMode";
    std::string modifyvalue = "5";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "Multi-segment");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_MeteringMode_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_Model_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Model_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "Model";
    std::string modifyvalue = "TNY-AL00";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "TNY-AL00");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_Model_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_PixelXDimension_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_PixelXDimension_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "PixelXDimension";
    std::string modifyvalue = "1000";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1000");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_PixelXDimension_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_PixelYDimension_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_PixelYDimension_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "PixelYDimension";
    std::string modifyvalue = "2000";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "2000");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_PixelYDimension_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_RecommendedExposureIndex_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_RecommendedExposureIndex_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "RecommendedExposureIndex";
    std::string modifyvalue = "241";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "241");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_RecommendedExposureIndex_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_SceneType_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_SceneType_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "SceneType";
    std::string modifyvalue = "1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "Directly photographed");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_SceneType_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_SensitivityType_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_SensitivityType_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "SensitivityType";
    std::string modifyvalue = "5";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "5");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_SensitivityType_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_StandardOutputSensitivity_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_StandardOutputSensitivity_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "StandardOutputSensitivity";
    std::string modifyvalue = "5";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "5");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_StandardOutputSensitivity_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_UserComment_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_UserComment_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "UserComment";
    std::string modifyvalue = "comm";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "comm");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_UserComment_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_LensSpecification_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "LensSpecification";
    std::string modifyvalue = "1/1 3/2 1/1 2/1";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1-1.5mm F1-2");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_LensSpecification_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "LensSpecification";
    std::string modifyvalue = "1 3 1 2";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1-3mm F1-2");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_002 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_LensSpecification_003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_003 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "LensSpecification";
    std::string modifyvalue = "1,3,1,2";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1-3mm F1-2");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_003 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_LensSpecification_004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_004 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "LensSpecification";
    std::string modifyvalue = "1.0 3.0 1.0 2.0";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1-3mm F1-2");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_004 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_LensSpecification_005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_005 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "LensSpecification";
    std::string modifyvalue = "1.0,3.0,1.0,2.0";
    int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
    ASSERT_EQ(modifyret, SUCCESS);

    std::string getvalue;
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "1-3mm F1-2");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_LensSpecification_005 end";
}

// HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_HwMnoteCaptureMode_001, TestSize.Level3)
// {
//     GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_HwMnoteCaptureMode_001 start";
//     uint32_t errorCode = 0;
//     SourceOptions opts;
//     std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
//     imageSource->imageStatusMap_.clear();
//     uint32_t index = 0;
//     std::string key = "HwMnoteCaptureMode";
//     std::string modifyvalue = "123";
//     int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
//     uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
//     ASSERT_EQ(modifyret, SUCCESS);

//     std::string getvalue;
//     uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
//     GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
//     ASSERT_EQ(getret, SUCCESS);
//     ASSERT_EQ(getvalue, modifyvalue);

//     GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_HwMnoteCaptureMode_001 end";
// }

// HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_HwMnoteFrontCamera_001, TestSize.Level3)
// {
//     GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_HwMnoteFrontCamera_001 start";
//     uint32_t errorCode = 0;
//     SourceOptions opts;
//     std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
//     imageSource->imageStatusMap_.clear();
//     uint32_t index = 0;
//     std::string key = "HwMnoteFrontCamera";
//     std::string modifyvalue = "11";
//     int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
//     uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
//     ASSERT_NE(modifyret, SUCCESS);
//     GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_HwMnoteFrontCamera_001 end";
// }

std::string MODIFYDATA[][3] = {
    {"BitsPerSample", "9,9,8", "9 9 8"},
    {"Orientation", "2", "top, right"},
    {"ImageLength", "1000", "1000"},
    {"CompressedBitsPerPixel", "24/1", "24"},
    {"GPSLatitude", "39/1 54/1 20/1", "39 deg 54' 20\""},
    {"GPSLongitude", "120,52,26", "120 deg 52' 26\""},
    {"GPSLatitudeRef", "N", "North"},
    {"GPSLongitudeRef", "E", "East"},
    {"WhiteBalance", "1", "Manual"},
    {"FocalLengthIn35mmFilm", "26", "26.0 mm"},
    {"Flash", "5", "Fired, return light not detected"},
    {"ApertureValue", "4/1", "F4"},
    {"DateTimeOriginal", "2024:01:25 05:51:34", "2024:01:25 05:51:34"},
    {"DateTime", "2024:01:25 05:51:34", "2024:01:25 05:51:34"},
    {"DateTime", "2024:01:25", "2024:01:25"},
    {"ExposureBiasValue", "23/1", "+23 EV"},
    {"ExposureTime", "1/34", "1/34 s"},
    {"FNumber", "3/1", "F3"},
    {"FocalLength", "31/1", "31.0 mm"},
    {"GPSTimeStamp", "11/1 37/1 58/1", "11:37:58"},
    {"ImageDescription", "_cuva", "_cuva"},
    {"ISOSpeedRatings", "160", "160"},
    {"ISOSpeed", "200", "200"},
    {"LightSource", "2", "Fluorescent"},
    {"MeteringMode", "5", "Multi-segment"},
    {"Model", "TNY-AL00", "TNY-AL00"},
    {"PixelXDimension", "1000", "1000"},
    {"PixelYDimension", "2000", "2000"},
    {"RecommendedExposureIndex", "241", "241"},
    {"SceneType", "1", "Directly photographed"},
    {"SensitivityType", "5", "5"},
    {"StandardOutputSensitivity", "5", "5"},
    {"UserComment", "comm", "comm"},
    {"LensSpecification", "1/1 3/2 1/1 2/1", "1-1.5mm F1-2"},

    {"JPEGProc", "252", "252"},
    {"Compression", "6", "JPEG (old-style)"},
    {"SamplesPerPixel", "23", "23"},
    {"XResolution", "72/1", "72"},
    {"XResolution", "72", "72"},
    {"XResolution", "72.0", "72"},
    {"YResolution", "72/1", "72"},
    {"YResolution", "72", "72"},
    {"YResolution", "72.0", "72"},
    {"PlanarConfiguration", "1", "Chunky"},
    {"ResolutionUnit", "2", "inch"},
    {"TransferFunction", "2", "2"},
    {"Software", "MNA-AL00 4.0.0.120(C00E116R3P7)", "MNA-AL00 4.0.0.120(C00E116R3P7)"},
    {"Artist", "Joseph.Xu", "Joseph.Xu"},
    {"WhitePoint", "124/1", "124/1"},
    {"WhitePoint", "124", "124/1"},
    {"WhitePoint", "124.0", "124/1"},
    {"WhitePoint", "124/2", "124/2"},
    {"WhitePoint", "124.2", "621/5"},
    {"PrimaryChromaticities", "124/1", "124/1"},
    {"YCbCrCoefficients", "299/1000 587/1000 114/1000", "299/1000 587/1000 114/1000"},
    {"YCbCrSubSampling", "3 2", "3 2"},
    {"YCbCrSubSampling", "3,2", "3 2"},
    {"YCbCrPositioning", "1", "Centered"},
    {"ReferenceBlackWhite", "221/1", "221/1"},
    {"Copyright", "Huawei", "Huawei"},
    {"ExposureProgram", "2", "Auto"},
    {"SpectralSensitivity", "sensitivity", "sensitivity"},
    {"OECF", "45", "45"},
    {"ExifVersion", "0210", "(210)"},
    {"DateTimeDigitized", "2023:01:19 10:39:58", "2023:01:19 10:39:58"},
    {"ComponentsConfiguration", "1 5 6", "YGB"},
    {"ShutterSpeedValue", "13/1", "1/8192 s"},
    {"BrightnessValue", "13/1", "13"},
    {"BrightnessValue", "13", "13"},
    {"BrightnessValue", "13.0", "13"},
    {"BrightnessValue", "5/2", "2.5"},
    {"MaxApertureValue", "1/12", "F1"},
    {"SubjectDistance", "25/1", "25.00 m"},
    {"SubjectDistance", "25", "25.00 m"},
    {"SubjectDistance", "25.0", "25.00 m"},
    {"SubjectArea", "10 20 183 259", "10 20 183 259"},
    {"SubjectArea", "10,20,183,259", "10 20 183 259"},
    {"SubsecTime", "427000", "427000"},
    {"SubSecTimeOriginal", "427000", "427000"},
    {"SubSecTimeDigitized", "427000", "427000"},
    {"FlashpixVersion", "1", "(1)"},
    {"ColorSpace", "2", "Adobe RGB"},
    {"RelatedSoundFile", "/usr/home/sound/sea.wav", "/usr/home/sound/sea.wav"},
    {"FlashEnergy", "832/1", "832/1"},
    {"SpatialFrequencyResponse", "13", "13"},
    {"FocalPlaneXResolution", "1080/1", "1080"},
    {"FocalPlaneXResolution", "1080.0", "1080"},
    {"FocalPlaneYResolution", "880/1", "880"},
    {"FocalPlaneYResolution", "880.0", "880"},
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
    {"ImageUniqueID", "FXIC012", "FXIC012"},
    {"GPSVersionID", "2 2 0 0", "2.2.0.0"},
    {"GPSAltitudeRef", "1", "Below sea level"},
    {"GPSAltitude", "0/100", "0.0 m"},
    {"GPSSatellites", "xxx", "xxx"},
    {"GPSStatus", "V", "Measurement interrupted"},
    {"GPSMeasureMode", "2", "2-dimensional measurement"},
    {"GPSDOP", "182/1", "182"},
    {"GPSSpeedRef", "K", "km/h"},
    {"GPSSpeed", "150/1", "150"},
    {"GPSTrackRef", "T", "True direction"},
    {"GPSTrack", "114/3", "38"},
    {"GPSTrack", "56.0", "56"},
    {"GPSImgDirectionRef", "M", "Magnetic direction"},
    {"GPSImgDirection", "125/56", "2.23214"},
    {"GPSImgDirection", "2.23214", "2.23214"},
    {"GPSMapDatum", "xxxx", "xxxx"},
    {"GPSDestLatitudeRef", "N", "North"},
    {"GPSDestLatitude", "33/1 22/1 11/1", "33 deg 22' 11\""},
    {"GPSDestLongitudeRef", "E", "East"},
    {"GPSDestLongitude", "33/1 22/1 11/1", "33 deg 22' 11\""},
    {"GPSDestBearingRef", "T", "True direction"},
    {"GPSDestBearing", "22/11", "2"},
    {"GPSDestBearing", "2.6", "2.6"},
    {"GPSDestDistanceRef", "N", "nautical miles"},
    {"GPSDestDistance", "10/1", "10"},
    {"GPSProcessingMethod", "CELLID", "CELLID"},
    {"GPSAreaInformation", "23、15、57", "23、15、57"},
    {"GPSDifferential", "0", "Without correction"},
    {"GPSDifferential", "1", "Correction applied"},

    {"BodySerialNumber", "xx", "xx"},
    {"CameraOwnerName", "xx", "xx"},
    {"CompositeImage", "1", "NonComposite"},
    {"CompressedBitsPerPixel", "3/2", "1.5"},
    {"CompressedBitsPerPixel", "1/2", "0.5"},
    {"CompressedBitsPerPixel", "0.5", "0.5"},
    {"DNGVersion", "1 113", "1 113"},
    {"DNGVersion", "1,1,2,3", "1 1 2 3"},
    {"DefaultCropSize", "12 1", "12 1"},
    {"Gamma", "3/2", "1.5"},
    {"ISOSpeedLatitudeyyy", "3", "3"},
    {"ISOSpeedLatitudezzz", "3", "3"},
    {"LensMake", "xxx", "xxx"},
    {"LensModel", "xxx", "xxx"},
    {"LensSerialNumber", "xxx", "xxx"},
    {"LensSpecification", "1/1 3/2 1/1 2/1", "1-1.5mm F1-2"},
    {"NewSubfileType", "1", "Thumbnail/Preview image"},
    {"NewSubfileType", "2", "Primary image, Multi page file"},
    {"OffsetTime", "xx", "xx"},
    {"OffsetTimeDigitized", "xx", "xx"},
    {"OffsetTimeOriginal", "xx", "xx"},
    {"SourceExposureTimesOfCompositeImage", "11", "11"},
    {"SourceImageNumberOfCompositeImage", "1234", "1234"},
    {"GPSHPositioningError", "3/1", "3"},
};

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_data_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_data_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    // int cols = sizeof(MODIFYDATA[0]) / sizeof(MODIFYDATA[0][0]);
    for (int i = 0; i < rows; ++i) {
        uint32_t index = 0;
        std::string key = MODIFYDATA[i][0];
        std::string modifyvalue = MODIFYDATA[i][1];
        int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
        GTEST_LOG_(INFO) <<"ModifyImageProperty_fd_data_001 key: "<<key<<" modifyvalue: "<< modifyvalue;
        ASSERT_EQ(modifyret, SUCCESS);

        std::string getvalue;
        uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
        GTEST_LOG_(INFO) <<"key: "<<key<<" getvalue is "<< getvalue;
        ASSERT_EQ(getret, SUCCESS);
        ASSERT_EQ(getvalue, MODIFYDATA[i][2]);
    }
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_data_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_fd_data_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_data_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    
    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    // int cols = sizeof(MODIFYDATA[0]) / sizeof(MODIFYDATA[0][0]);
    for (int i = 0; i < rows; ++i) {
        std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
        // imageSource->imageStatusMap_.clear();

        uint32_t index = 0;
        std::string key = MODIFYDATA[i][0];
        std::string modifyvalue = MODIFYDATA[i][1];
        int fd = open(IMAGE_INPUT_JPEG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
        GTEST_LOG_(INFO) <<"ModifyImageProperty_fd_data_002 key: "<<key<<" modifyvalue: "<< modifyvalue;
        ASSERT_EQ(modifyret, SUCCESS);
        imageSource->Reset();

        std::unique_ptr<ImageSource> imageSourceGet = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
        // imageSourceGet->imageStatusMap_.clear();

        std::string getvalue;
        uint32_t getret = imageSourceGet->GetImagePropertyString(index, key, getvalue);
        GTEST_LOG_(INFO) <<"key: "<<key<<" getvalue is "<< getvalue;
        ASSERT_EQ(getret, SUCCESS);
        ASSERT_EQ(getvalue, MODIFYDATA[i][2]);
        imageSourceGet->Reset();
    }
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_fd_data_002 end";
}

HWTEST_F(ImageSourceExifTest, GetImageProperty_data_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImageProperty_data_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    // int cols = sizeof(MODIFYDATA[0]) / sizeof(MODIFYDATA[0][0]);
    for (int i = 0; i < rows; ++i) {
        uint32_t index = 0;
        std::string key = MODIFYDATA[i][0];
        std::string getvalue;
        uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
        GTEST_LOG_(INFO) <<"GetImageProperty_data_001 key: "<<key<<" getvalue: "<< getvalue;
        ASSERT_EQ(getret, SUCCESS);
    }
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImageProperty_data_001 end";
}


HWTEST_F(ImageSourceExifTest, ModifyImageProperty_png_fd_data_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_png_fd_data_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_PNG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    // int cols = sizeof(MODIFYDATA[0]) / sizeof(MODIFYDATA[0][0]);
    for (int i = 0; i < rows; ++i) {
        uint32_t index = 0;
        std::string key = MODIFYDATA[i][0];
        std::string modifyvalue = MODIFYDATA[i][1];
        int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
        GTEST_LOG_(INFO) <<"ModifyImageProperty_png_fd_data_001 key: "<<key<<" modifyvalue: "<< modifyvalue;
        ASSERT_EQ(modifyret, SUCCESS);

        std::string getvalue;
        uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
        GTEST_LOG_(INFO) <<"key: "<<key<<" getvalue is "<< getvalue;
        ASSERT_EQ(getret, SUCCESS);
        ASSERT_EQ(getvalue, MODIFYDATA[i][2]);
    }
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_png_fd_data_001 end";
}

HWTEST_F(ImageSourceExifTest, ModifyImageProperty_png_fd_data_002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_png_fd_data_002 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    
    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    // int cols = sizeof(MODIFYDATA[0]) / sizeof(MODIFYDATA[0][0]);
    for (int i = 0; i < rows; ++i) {
        std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_PNG_PATH, opts, errorCode);
        // imageSource->imageStatusMap_.clear();

        uint32_t index = 0;
        std::string key = MODIFYDATA[i][0];
        std::string modifyvalue = MODIFYDATA[i][1];
        int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, fd);
        GTEST_LOG_(INFO) <<"ModifyImageProperty_png_fd_data_002 key: "<<key<<" modifyvalue: "<< modifyvalue;
        ASSERT_EQ(modifyret, SUCCESS);
        imageSource->Reset();

        std::unique_ptr<ImageSource> imageSourceGet = ImageSource::CreateImageSource(IMAGE_INPUT_PNG_PATH, opts, errorCode);
        // imageSourceGet->imageStatusMap_.clear();

        std::string getvalue;
        uint32_t getret = imageSourceGet->GetImagePropertyString(index, key, getvalue);
        GTEST_LOG_(INFO) <<"key: "<<key<<" getvalue is "<< getvalue;
        ASSERT_EQ(getret, SUCCESS);
        ASSERT_EQ(getvalue, MODIFYDATA[i][2]);
        imageSourceGet->Reset();
    }
    GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_png_fd_data_002 end";
}

HWTEST_F(ImageSourceExifTest, GetImageProperty_png_data_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImageProperty_png_data_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_PNG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    int rows = sizeof(MODIFYDATA) / sizeof(MODIFYDATA[0]);
    // int cols = sizeof(MODIFYDATA[0]) / sizeof(MODIFYDATA[0][0]);
    for (int i = 0; i < rows; ++i) {
        uint32_t index = 0;
        std::string key = MODIFYDATA[i][0];
        std::string getvalue;
        uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
        GTEST_LOG_(INFO) <<"GetImageProperty_png_data_001 key: "<<key<<" getvalue: "<< getvalue;
        ASSERT_EQ(getret, SUCCESS);
    }
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImageProperty_png_data_001 end";
}


// HWTEST_F(ImageSourceExifTest, ModifyImageProperty_path_HwMnoteFrontCamera_001, TestSize.Level3)
// {
//     GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_path_HwMnoteFrontCamera_001 start";
//     uint32_t errorCode = 0;
//     SourceOptions opts;
//     std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
//     imageSource->imageStatusMap_.clear();
//     uint32_t index = 0;
//     std::string key = "HwMnoteFrontCamera";
//     std::string modifyvalue = "11";
//     uint32_t modifyret = imageSource->ModifyImageProperty(index, key, modifyvalue, IMAGE_INPUT_JPEG_PATH);
//     ASSERT_EQ(modifyret, Media::ERR_FORBID_EXIV2_MODIFY);

//     GTEST_LOG_(INFO) << "ImageSourceExifTest: ModifyImageProperty_path_HwMnoteFrontCamera_001 end";
// }

HWTEST_F(ImageSourceExifTest, GetImagePropertyString_HwMnoteFrontCamera_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_HwMnoteFrontCamera_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "HwMnoteFrontCamera";
    std::string getvalue = "";
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    ASSERT_NE(getret, SUCCESS);
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_HwMnoteFrontCamera_001 end";
}

HWTEST_F(ImageSourceExifTest, GetImagePropertyString_png_HwMnoteFrontCamera_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_png_HwMnoteFrontCamera_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_PNG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "HwMnoteFrontCamera";
    std::string getvalue = "";
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    ASSERT_NE(getret, SUCCESS);
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_png_HwMnoteFrontCamera_001 end";
}

HWTEST_F(ImageSourceExifTest, GetImagePropertyString_BitsPerSample_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_BitsPerSample_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "BitsPerSample";
    std::string getvalue = "";
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "9 9 8");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_BitsPerSample_001 end";
}

HWTEST_F(ImageSourceExifTest, GetImagePropertyString_SceneCaptureType_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_SceneCaptureType_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "SceneCaptureType";
    std::string getvalue = "";
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "Standard");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_SceneCaptureType_001 end";
}

HWTEST_F(ImageSourceExifTest, GetImagePropertyString_GPSLatitudeRef_001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_GPSLatitudeRef_001 start";
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH, opts, errorCode);
    imageSource->imageStatusMap_.clear();
    uint32_t index = 0;
    std::string key = "GPSLatitudeRef";
    std::string getvalue = "";
    uint32_t getret = imageSource->GetImagePropertyString(index, key, getvalue);
    GTEST_LOG_(INFO) << "getvalue is "<< getvalue;
    ASSERT_EQ(getret, SUCCESS);
    ASSERT_EQ(getvalue, "North");

    GTEST_LOG_(INFO) << "ImageSourceExifTest: GetImagePropertyString_GPSLatitudeRef_001 end";
}

} // namespace Multimedia
} // namespace OHOS