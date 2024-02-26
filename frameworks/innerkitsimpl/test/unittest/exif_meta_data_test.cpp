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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "exif_meta_data.h"
#include "media_errors.h"

using namespace testing::ext;
using namespace OHOS::Media;
namespace OHOS {
namespace Media {
static const std::string IMAGE_INPUT_PNG_PATH = "/data/local/tmp/image/test_exif.jpg";
static const std::string GET_EXIV2_INT_KEY = "Exif.Photo.PixelXDimension";
static const std::string GET_EXIV2_STRING_KEY = "Exif.Image.DateTime";
static const std::string MODIFY_EXIV2_STRING_KEY = "Exif.Photo.UserComment";
static const std::string MODIFY_EXIV2_STRING_VALUE_PATH = "modify property with path";
static const std::string MODIFY_EXIV2_STRING_VALUE_BUF = "modify property with buf";
static const std::string MODIFY_EXIV2_STRING_VALUE_FD = "modify property with fd";

class ExifMetaDataTest : public testing::Test {
public:
    ExifMetaDataTest() {}
    ~ExifMetaDataTest() {}
};

/**
 * @tc.name: CreateExiv2Image001
 * @tc.desc: test CreateExiv2Image
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, CreateExiv2Image001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: CreateExiv2Image001 start";
 
    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    uint32_t ret = exifMetaData->CreateExiv2Image(IMAGE_INPUT_PNG_PATH);
    ASSERT_EQ(ret, SUCCESS);
    delete exifMetaData;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: CreateExiv2Image001 end";
}

/**
 * @tc.name: CreateExiv2Image002
 * @tc.desc: test CreateExiv2Image
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, CreateExiv2Image002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: CreateExiv2Image002 start";
 
    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    struct stat fileStat;
    int statRet = fstat(fd, &fileStat);
    ASSERT_NE(statRet, -1);
    ASSERT_GT(fileStat.st_size, 0);
    uint8_t *data = static_cast<uint8_t *>(malloc(fileStat.st_size));
    ASSERT_NE(data, nullptr);
    ssize_t size = read(fd, data, fileStat.st_size);
    ASSERT_EQ(size, fileStat.st_size);
    close(fd);
    uint32_t ret = exifMetaData->CreateExiv2Image(data, size);
    ASSERT_EQ(ret, SUCCESS);
    delete exifMetaData;
    free(data);

    GTEST_LOG_(INFO) << "ExifMetaDataTest: CreateExiv2Image002 end";
}

/**
 * @tc.name: CreateExiv2Image003
 * @tc.desc: test CreateExiv2Image
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, CreateExiv2Image003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: CreateExiv2Image003 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    uint32_t ret = exifMetaData->CreateExiv2Image(fd);
    ASSERT_EQ(ret, SUCCESS);
    delete exifMetaData;
    close(fd);

    GTEST_LOG_(INFO) << "ExifMetaDataTest: CreateExiv2Image003 end";
}

/**
 * @tc.name: GetImagePropertyInt001
 * @tc.desc: test GetImagePropertyInt
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, GetImagePropertyInt001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyInt001 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    uint32_t ret = exifMetaData->CreateExiv2Image(IMAGE_INPUT_PNG_PATH);
    ASSERT_EQ(ret, SUCCESS);
    int32_t value = 0;
    uint32_t getRet = exifMetaData->GetImagePropertyInt(GET_EXIV2_INT_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_GT(value, 0);
    GTEST_LOG_(INFO) << GET_EXIV2_INT_KEY << ": " << value;
    delete exifMetaData;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyInt001 end";
}

/**
 * @tc.name: GetImagePropertyInt002
 * @tc.desc: test GetImagePropertyInt
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, GetImagePropertyInt002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyInt002 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    struct stat fileStat;
    int statRet = fstat(fd, &fileStat);
    ASSERT_NE(statRet, -1);
    ASSERT_GT(fileStat.st_size, 0);
    uint8_t *data = static_cast<uint8_t *>(malloc(fileStat.st_size));
    ASSERT_NE(data, nullptr);
    ssize_t size = read(fd, data, fileStat.st_size);
    ASSERT_EQ(size, fileStat.st_size);
    close(fd);
    uint32_t ret = exifMetaData->CreateExiv2Image(data, size);
    ASSERT_EQ(ret, SUCCESS);
    int32_t value = 0;
    uint32_t getRet = exifMetaData->GetImagePropertyInt(GET_EXIV2_INT_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_GT(value, 0);
    GTEST_LOG_(INFO) << GET_EXIV2_INT_KEY << ": " << value;
    delete exifMetaData;
    free(data);

    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyInt002 end";
}

/**
 * @tc.name: GetImagePropertyInt003
 * @tc.desc: test GetImagePropertyInt
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, GetImagePropertyInt003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyInt003 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    uint32_t ret = exifMetaData->CreateExiv2Image(fd);
    ASSERT_EQ(ret, SUCCESS);
    close(fd);
    int32_t value = 0;
    uint32_t getRet = exifMetaData->GetImagePropertyInt(GET_EXIV2_INT_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_GT(value, 0);
    GTEST_LOG_(INFO) << GET_EXIV2_INT_KEY << ": " << value;
    delete exifMetaData;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyInt003 end";
}

/**
 * @tc.name: GetImagePropertyString001
 * @tc.desc: test GetImagePropertyString
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, GetImagePropertyString001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyString001 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    uint32_t ret = exifMetaData->CreateExiv2Image(IMAGE_INPUT_PNG_PATH);
    ASSERT_EQ(ret, SUCCESS);
    std::string value = "";
    uint32_t getRet = exifMetaData->GetImagePropertyString(GET_EXIV2_STRING_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_NE(value, "");
    GTEST_LOG_(INFO) << GET_EXIV2_STRING_KEY << ": " << value;
    delete exifMetaData;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyString001 end";
}

/**
 * @tc.name: GetImagePropertyString002
 * @tc.desc: test GetImagePropertyString
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, GetImagePropertyString002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyString002 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    struct stat fileStat;
    int statRet = fstat(fd, &fileStat);
    ASSERT_NE(statRet, -1);
    ASSERT_GT(fileStat.st_size, 0);
    uint8_t *data = static_cast<uint8_t *>(malloc(fileStat.st_size));
    ASSERT_NE(data, nullptr);
    ssize_t size = read(fd, data, fileStat.st_size);
    ASSERT_EQ(size, fileStat.st_size);
    close(fd);
    uint32_t ret = exifMetaData->CreateExiv2Image(data, size);
    ASSERT_EQ(ret, SUCCESS);
    std::string value = "";
    uint32_t getRet = exifMetaData->GetImagePropertyString(GET_EXIV2_STRING_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_NE(value, "");
    GTEST_LOG_(INFO) << GET_EXIV2_STRING_KEY << ": " << value;
    delete exifMetaData;
    free(data);

    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyString002 end";
}

/**
 * @tc.name: GetImagePropertyString003
 * @tc.desc: test GetImagePropertyString
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, GetImagePropertyString003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyString003 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    uint32_t ret = exifMetaData->CreateExiv2Image(fd);
    ASSERT_EQ(ret, SUCCESS);
    close(fd);
    std::string value = "";
    uint32_t getRet = exifMetaData->GetImagePropertyString(GET_EXIV2_STRING_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_NE(value, "");
    GTEST_LOG_(INFO) << GET_EXIV2_STRING_KEY << ": " << value;
    delete exifMetaData;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: GetImagePropertyString003 end";
}

/**
 * @tc.name: ModifyImageProperty001
 * @tc.desc: test ModifyImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, ModifyImageProperty001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: ModifyImageProperty001 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    uint32_t ret = exifMetaData->CreateExiv2Image(IMAGE_INPUT_PNG_PATH);
    ASSERT_EQ(ret, SUCCESS);
    uint32_t setRet = exifMetaData->ModifyImageProperty(MODIFY_EXIV2_STRING_KEY, MODIFY_EXIV2_STRING_VALUE_PATH);
    ASSERT_EQ(setRet, SUCCESS);
    exifMetaData->WriteMetadata();
    delete exifMetaData;

    ExifMetaData *exifMetaDataNew = new ExifMetaData();
    ASSERT_NE(exifMetaDataNew, nullptr);
    uint32_t retNew = exifMetaDataNew->CreateExiv2Image(IMAGE_INPUT_PNG_PATH);
    ASSERT_EQ(retNew, SUCCESS);
    std::string value = "";
    uint32_t getRet = exifMetaDataNew->GetImagePropertyString(MODIFY_EXIV2_STRING_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_EQ(value, MODIFY_EXIV2_STRING_VALUE_PATH);
    delete exifMetaDataNew;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: ModifyImageProperty001 end";
}

/**
 * @tc.name: ModifyImageProperty002
 * @tc.desc: test ModifyImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, ModifyImageProperty002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: ModifyImageProperty002 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    struct stat fileStat;
    int statRet = fstat(fd, &fileStat);
    ASSERT_NE(statRet, -1);
    ASSERT_GT(fileStat.st_size, 0);
    uint8_t *data = static_cast<uint8_t *>(malloc(fileStat.st_size));
    ASSERT_NE(data, nullptr);
    ssize_t size = read(fd, data, fileStat.st_size);
    ASSERT_EQ(size, fileStat.st_size);
    close(fd);
    uint32_t ret = exifMetaData->CreateExiv2Image(data, size);
    ASSERT_EQ(ret, SUCCESS);
    uint32_t setRet = exifMetaData->ModifyImageProperty(MODIFY_EXIV2_STRING_KEY, MODIFY_EXIV2_STRING_VALUE_BUF);
    ASSERT_EQ(setRet, SUCCESS);
    exifMetaData->WriteMetadata();
    uint8_t *imageBuf = nullptr;
    uint32_t imageSize = 0;
    uint32_t getBufRet =  exifMetaData->GetExiv2ImageBuf(&imageBuf, imageSize);
    ASSERT_EQ(getBufRet, SUCCESS);
    int fdNew = open(IMAGE_INPUT_PNG_PATH.c_str(), O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fdNew, 0);
    size_t wrRet = write(fdNew, static_cast<void *>(imageBuf), static_cast<size_t>(imageSize));
    ASSERT_EQ(imageSize, wrRet);
    close(fdNew);
    delete exifMetaData;
    free(data);

    ExifMetaData *exifMetaDataNew = new ExifMetaData();
    ASSERT_NE(exifMetaDataNew, nullptr);
    uint32_t retNew = exifMetaDataNew->CreateExiv2Image(IMAGE_INPUT_PNG_PATH);
    ASSERT_EQ(retNew, SUCCESS);
    std::string value = "";
    uint32_t getRet = exifMetaDataNew->GetImagePropertyString(MODIFY_EXIV2_STRING_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_EQ(value, MODIFY_EXIV2_STRING_VALUE_BUF);
    delete exifMetaDataNew;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: ModifyImageProperty002 end";
}

/**
 * @tc.name: ModifyImageProperty003
 * @tc.desc: test ModifyImageProperty
 * @tc.type: FUNC
 */
HWTEST_F(ExifMetaDataTest, ModifyImageProperty003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ExifMetaDataTest: ModifyImageProperty003 start";

    ExifMetaData *exifMetaData = new ExifMetaData();
    ASSERT_NE(exifMetaData, nullptr);
    int fd = open(IMAGE_INPUT_PNG_PATH.c_str(), O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fd, 0);
    uint32_t ret = exifMetaData->CreateExiv2Image(fd);
    ASSERT_EQ(ret, SUCCESS);
    close(fd);
    uint32_t setRet = exifMetaData->ModifyImageProperty(MODIFY_EXIV2_STRING_KEY, MODIFY_EXIV2_STRING_VALUE_FD);
    ASSERT_EQ(setRet, SUCCESS);
    exifMetaData->WriteMetadata();
    uint8_t *imageBuf = nullptr;
    uint32_t imageSize = 0;
    uint32_t getBufRet =  exifMetaData->GetExiv2ImageBuf(&imageBuf, imageSize);
    ASSERT_EQ(getBufRet, SUCCESS);
    int fdNew = open(IMAGE_INPUT_PNG_PATH.c_str(), O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    ASSERT_GT(fdNew, 0);
    size_t wrRet = write(fdNew, static_cast<void *>(imageBuf), static_cast<size_t>(imageSize));
    ASSERT_EQ(imageSize, wrRet);
    close(fdNew);
    delete exifMetaData;

    ExifMetaData *exifMetaDataNew = new ExifMetaData();
    ASSERT_NE(exifMetaDataNew, nullptr);
    uint32_t retNew = exifMetaDataNew->CreateExiv2Image(IMAGE_INPUT_PNG_PATH);
    ASSERT_EQ(retNew, SUCCESS);
    std::string value = "";
    uint32_t getRet = exifMetaDataNew->GetImagePropertyString(MODIFY_EXIV2_STRING_KEY, value);
    ASSERT_EQ(getRet, SUCCESS);
    ASSERT_EQ(value, MODIFY_EXIV2_STRING_VALUE_FD);
    delete exifMetaDataNew;

    GTEST_LOG_(INFO) << "ExifMetaDataTest: ModifyImageProperty003 end";
}
} // namespace Media
} // namespace OHOS
