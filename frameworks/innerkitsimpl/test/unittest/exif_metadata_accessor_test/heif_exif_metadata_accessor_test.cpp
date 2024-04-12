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

#include "buffer_metadata_stream.h"
#include "file_metadata_stream.h"
#include "heif_exif_metadata_accessor.h"
#include "log_tags.h"
#include "media_errors.h"

#include <iostream>

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {
namespace {
static const std::string IMAGE_INPUT_HEIF_EXIF_PATH = "/data/local/tmp/image/test_exif.heic";
static const std::string IMAGE_INPUT_HEIF_HW_EXIF_PATH = "/data/local/tmp/image/test_hw.heic";
static const std::string IMAGE_INPUT_HEIF_NO_EXIF_PATH = "/data/local/tmp/image/test.heic";
}

class HeifExifMetadataAccessorTest : public testing::Test {
public:
    HeifExifMetadataAccessorTest() {}
    ~HeifExifMetadataAccessorTest() {}
    std::string GetProperty(const std::shared_ptr<ExifMetadata> &metadata, const std::string &key);
};

std::string HeifExifMetadataAccessorTest::GetProperty(const std::shared_ptr<ExifMetadata> &metadata,
    const std::string &key)
{
    std::string value;
    metadata->GetValue(key, value);
    std::cerr << "key: " << key << ", value: " << value << std::endl;
    return value;
}

/**
 * @tc.name: Read001
 * @tc.desc: test the Heif format get exif properties
 * @tc.type: FUNC
 */
HWTEST_F(HeifExifMetadataAccessorTest, Read001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read001 start";
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_HEIF_EXIF_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    HeifExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);
    GetProperty(exifMetadata, "CameraOwnerName");
    GetProperty(exifMetadata, "GPSLatitudeRef");
    GetProperty(exifMetadata, "GPSLongitudeRef");
    GetProperty(exifMetadata, "GPSLatitude");
    GetProperty(exifMetadata, "GPSLongitude");

    GetProperty(exifMetadata, "DateTimeOriginal");
    GetProperty(exifMetadata, "Make");
    GetProperty(exifMetadata, "Model");
    GetProperty(exifMetadata, "DateTimeOriginal");
    GetProperty(exifMetadata, "SceneType");
    GetProperty(exifMetadata, "DateTime");
    GetProperty(exifMetadata, "WhiteBalance");

    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read001 end";
}

/**
 * @tc.name: Read002
 * @tc.desc: test the Heif format get no exif
 * @tc.type: FUNC
 */
HWTEST_F(HeifExifMetadataAccessorTest, Read002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read002 start";
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_HEIF_NO_EXIF_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    HeifExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, ERR_MEDIA_VALUE_INVALID);

    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read002 end";
}


/**
 * @tc.name: Read003
 * @tc.desc: test the Heif format get exif HW properties
 * @tc.type: FUNC
 */
HWTEST_F(HeifExifMetadataAccessorTest, Read003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read003 start";
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_HEIF_HW_EXIF_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    HeifExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, 0);

    auto exifMetadata = imageAccessor.Get();
    GetProperty(exifMetadata, "HwMnoteBurstNumber");
    GetProperty(exifMetadata, "HwMnoteCaptureMode");
    GetProperty(exifMetadata, "HwMnoteFaceConf");
    GetProperty(exifMetadata, "HwMnoteFaceCount");
    GetProperty(exifMetadata, "HwMnoteFaceLeyeCenter");
    GetProperty(exifMetadata, "HwMnoteFaceMouthCenter");
    GetProperty(exifMetadata, "HwMnoteFacePointer");
    GetProperty(exifMetadata, "HwMnoteFaceRect");
    GetProperty(exifMetadata, "HwMnoteFaceReyeCenter");
    GetProperty(exifMetadata, "HwMnoteFaceSmileScore");
    GetProperty(exifMetadata, "HwMnoteFaceVersion");
    GetProperty(exifMetadata, "HwMnoteFocusMode");
    GetProperty(exifMetadata, "HwMnoteFrontCamera");
    GetProperty(exifMetadata, "HwMnotePhysicalAperture");
    GetProperty(exifMetadata, "HwMnotePitchAngle");
    GetProperty(exifMetadata, "HwMnoteRollAngle");
    GetProperty(exifMetadata, "HwMnoteSceneBeachConf");
    GetProperty(exifMetadata, "HwMnoteSceneBlueSkyConf");
    GetProperty(exifMetadata, "HwMnoteSceneFlowersConf");
    GetProperty(exifMetadata, "HwMnoteSceneFoodConf");
    GetProperty(exifMetadata, "HwMnoteSceneGreenPlantConf");
    GetProperty(exifMetadata, "HwMnoteSceneNightConf");
    GetProperty(exifMetadata, "HwMnoteScenePointer");
    GetProperty(exifMetadata, "HwMnoteSceneSnowConf");
    GetProperty(exifMetadata, "HwMnoteSceneStageConf");
    GetProperty(exifMetadata, "HwMnoteSceneSunsetConf");
    GetProperty(exifMetadata, "HwMnoteSceneTextConf");
    GetProperty(exifMetadata, "HwMnoteSceneVersion");

    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read003 end";
}

/**
 * @tc.name: Read001
 * @tc.desc: test the Heif format get exif properties
 * @tc.type: FUNC
 */
HWTEST_F(HeifExifMetadataAccessorTest, Write001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Write001 start";
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_HEIF_EXIF_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    HeifExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.Get();

    GetProperty(exifMetadata, "Model");
    uint32_t errcode = imageAccessor.Write();
    ASSERT_EQ(errcode, SUCCESS);

    exifMetadata = imageAccessor.Get();
    GetProperty(exifMetadata, "Model");

    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Write001 end";
}


/**
 * @tc.name: Read001
 * @tc.desc: test the Heif format get exif properties
 * @tc.type: FUNC
 */
HWTEST_F(HeifExifMetadataAccessorTest, Append001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Append001 start";
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_HEIF_NO_EXIF_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    HeifExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, ERR_MEDIA_VALUE_INVALID);
    auto exifMetadata = imageAccessor.Get();

    if (exifMetadata == nullptr) {
        bool ret = imageAccessor.Create();
        ASSERT_EQ(ret, true);
        GTEST_LOG_(INFO) << "Create exif matadata success...";
        exifMetadata = imageAccessor.Get();
    }

    bool retSet = exifMetadata->SetValue("Model", "test");
    ASSERT_EQ(retSet, true);
    retSet = exifMetadata->SetValue("GPSLatitudeRef", "N");
    ASSERT_EQ(retSet, true);
    retSet = exifMetadata->SetValue("GPSLongitudeRef", "E");
    ASSERT_EQ(retSet, true);

    uint32_t errcode = imageAccessor.Write();
    ASSERT_EQ(errcode, SUCCESS);

    exifMetadata = imageAccessor.Get();
    GetProperty(exifMetadata, "Model");

    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Append001 end";
}


/**
 * @tc.name: Read100
 * @tc.desc: test the Heif format get exif properties
 * @tc.type: FUNC
 */
HWTEST_F(HeifExifMetadataAccessorTest, Read100, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read100 start";
    std::shared_ptr<MetadataStream> stream = std::make_shared<FileMetadataStream>(IMAGE_INPUT_HEIF_NO_EXIF_PATH);
    ASSERT_TRUE(stream->Open(OpenMode::ReadWrite));
    HeifExifMetadataAccessor imageAccessor(stream);
    uint32_t result = imageAccessor.Read();
    ASSERT_EQ(result, 0);
    auto exifMetadata = imageAccessor.Get();
    ASSERT_NE(exifMetadata, nullptr);
    GetProperty(exifMetadata, "Model");
    GetProperty(exifMetadata, "GPSLatitudeRef");
    GetProperty(exifMetadata, "GPSLongitudeRef");

    GTEST_LOG_(INFO) << "HeifExifMetadataAccessorTest: Read100 end";
}
} // namespace Multimedia
} // namespace OHOS