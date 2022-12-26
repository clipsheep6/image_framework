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

#include <gtest/gtest.h>
#include <fstream>
#include "directory_ex.h"
#include "hilog/log.h"
#include "image_packer.h"
#include "image_source.h"
#include "image_source_util.h"
#include "image_type.h"
#include "image_utils.h"
#include "incremental_pixel_map.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_map.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::HiviewDFX;
using namespace OHOS::ImageSourceUtil;
namespace OHOS {
namespace Multimedia {
namespace {
static constexpr HiLogLabel LABEL_TEST = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageSourceSvgTest" };
static const std::string IMAGE_INPUT_SMALL_SVG_PATH = "/data/local/tmp/image/test_small.svg";
static const std::string IMAGE_OUTPUT_SMALL_SVG_PATH = "/data/local/tmp/image/output_test_small.svg.jpg";
static const std::string IMAGE_INPUT_LARGE_SVG_PATH = "/data/local/tmp/image/test_large.svg";
static const std::string IMAGE_OUTPUT_LARGE_SVG_PATH = "/data/local/tmp/image/output_test_large.svg.jpg";
}

class ImageSourceSvgTest : public testing::Test {
public:
    ImageSourceSvgTest() {}
    ~ImageSourceSvgTest() {}
};

/**
 * @tc.name: SvgImageDecode001
 * @tc.desc: Decode svg image from file source stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageSourceSvgTest, SvgImageDecode001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceSvgTest: SvgImageDecode001 start";

    /**
     * @tc.steps: step1. create image source by correct svg file path and svg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/svg+xml";
    auto imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_SMALL_SVG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options
     * @tc.expected: step2. decode image source to pixel map success.
     */
    DecodeOptions decodeOpts;
    auto pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    /**
     * @tc.steps: step3. compress the pixel map to jpg file.
     * @tc.expected: step3. pack pixel map success.
     */
    int64_t packSize = PackImage(IMAGE_OUTPUT_SMALL_SVG_PATH, std::move(pixelMap));
    ASSERT_NE(packSize, 0);

    GTEST_LOG_(INFO) << "ImageSourceSvgTest: SvgImageDecode001 end";
}

/**
 * @tc.name: SvgImageDecode002
 * @tc.desc: Decode svg image from file source stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageSourceSvgTest, SvgImageDecode002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceSvgTest: SvgImageDecode002 start";

    /**
     * @tc.steps: step1. create image source by correct svg file path and svg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/svg+xml";
    auto imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_LARGE_SVG_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options
     * @tc.expected: step2. decode image source to pixel map success.
     */
    DecodeOptions decodeOpts;
    auto pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);

    /**
     * @tc.steps: step3. compress the pixel map to jpg file.
     * @tc.expected: step3. pack pixel map success.
     */
    int64_t packSize = PackImage(IMAGE_OUTPUT_LARGE_SVG_PATH, std::move(pixelMap));
    ASSERT_NE(packSize, 0);

    GTEST_LOG_(INFO) << "ImageSourceSvgTest: SvgImageDecode002 end";
}
} // namespace Multimedia
} // namespace OHOS