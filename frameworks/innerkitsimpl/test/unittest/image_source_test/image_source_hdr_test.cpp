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
#include <fstream>
#include <fcntl.h>
#include "hilog/log.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_map.h"
#include "image_source_util.h"
#include "file_source_stream.h"
#include "buffer_source_stream.h"
#include "ext_stream.h"
#include "hdr_utils.h"
#include "SkCodec.h"
#include "src/codec/SkJpegCodec.h"
#include "src/codec/SkJpegDecoderMgr.h"
#include "src/codec/SkHeifCodec.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::HiviewDFX;
using namespace OHOS::ImageSourceUtil;

namespace OHOS {
namespace Multimedia {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL_TEST = {
    LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageSourceJpegTest"
};

static const std::string IMAGE_INPUT_ULTRA_HDR_PATH = "/data/local/tmp/image/ultrahdr.jpg";

class ImageSourceHdrTest : public testing::Test {
public:
    ImageSourceHdrTest() {}
    ~ImageSourceHdrTest() {}
};

/**
 * @tc.name: HdrDecode001
 * @tc.desc: Test UltraHdr decode
 * @tc.type: FUNC
 */
HWTEST_F(ImageSourceHdrTest, HdrDecode001, TestSize.Level3)
{
    uint32_t errorCode = 0;
    SourceOptions opts;
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_ULTRA_HDR_PATH,
                                                                              opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    uint32_t index = 0;
    DecodeOptions optsPixel;
    optsPixel.dynamicRange = Media::DecodeDynamicRange::DEFAULT;
    errorCode = 0;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(index, optsPixel, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
#ifdef IMAGE_HDR_CONVERTER_FLAG
    ASSERT_EQ(pixelMap->GetPixelFormat(), Media::PixelFormat::RGBA_1010102);
#endif
}

/**
 * @tc.name: HdrUtils001
 * @tc.desc: Test CheckHdrType ultrahdr
 * @tc.type: FUNC
 */
HWTEST_F(ImageSourceHdrTest, HdrUtils001, TestSize.Level3)
{
    std::unique_ptr<Media::SourceStream> stream =
        Media::FileSourceStream::CreateSourceStream(IMAGE_INPUT_ULTRA_HDR_PATH);
    ImagePlugin::InputDataStream* inputStream = stream.get();
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromStream(std::make_unique<ImagePlugin::ExtStream>(inputStream));
    uint32_t offset;
    HdrType type = HdrUtils::CheckHdrType(codec.get(), offset);
    HiLog::Info(LABEL_TEST, "HdrUtils001 CheckHdrType type=%{public}d, offset=%{public}d", type, offset);
    ASSERT_EQ(type, HdrType::HDR_ISO);
    ASSERT_NE(offset, 0);
}

/**
 * @tc.name: HdrUtils002
 * @tc.desc: Test GetMetadata ultrahdr
 * @tc.type: FUNC
 */
HWTEST_F(ImageSourceHdrTest, HdrUtils002, TestSize.Level3)
{
    std::unique_ptr<Media::SourceStream> stream =
            Media::FileSourceStream::CreateSourceStream(IMAGE_INPUT_ULTRA_HDR_PATH);
    ASSERT_NE(stream, nullptr);

    ImagePlugin::InputDataStream* inputStream = stream.get();
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromStream(std::make_unique<ImagePlugin::ExtStream>(inputStream));
    ASSERT_NE(codec, nullptr);

    uint32_t offset;
    HdrType type = HdrUtils::CheckHdrType(codec.get(), offset);
    HiLog::Info(LABEL_TEST, "HdrUtils001 CheckHdrType type=%{public}d, offset=%{public}d", type, offset);
    ASSERT_EQ(type, HdrType::HDR_ISO);
    ASSERT_NE(offset, 0);

    uint8_t* data = stream->GetDataPtr() + offset;
    uint32_t size = stream->GetStreamSize() - offset;
    std::unique_ptr<Media::SourceStream> gainMapStream = Media::BufferSourceStream::CreateSourceStream(data, size);
    std::unique_ptr<SkCodec> gainMapCodec = SkCodec::MakeFromStream(
                                            std::make_unique<ImagePlugin::ExtStream>(gainMapStream.get()));
    ASSERT_NE(gainMapCodec, nullptr);
    HdrMetadata metadata = {};
    bool result = HdrUtils::GetMetadata(gainMapCodec.get(), type, metadata);
    ASSERT_EQ(result, true);
}
}
}