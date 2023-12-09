/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "image_source_util.h"
#include "log_tags.h"
#include "media_errors.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImageSourceUtil;

namespace OHOS {
namespace Multimedia {
namespace {
static const std::string NINE_PNG_FORMAT_TYPE = "image/png";
static const std::string INPUT_PATH = "/data/local/tmp/image/";
static const std::string TEST_FILE_SVG = "test.9.png";
static const int TEST_NINE_PNG_PADDINGTOP = 41;
static const int TEST_NINE_PNG_PADDINGBOTTOM = 22;
static const int TEST_NINE_PNG_PADDINGLEFT = 38;
static const int TEST_NINE_PNG_PADDINGRIGHT = 31;
}

class ImageSourceNineTest : public testing::Test {};

/**
 * @tc.name: NineImageDecode
 * @tc.desc: Decode svg image from file source stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageSourceNineTest, NineImageDecode, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImageSourceNineTest: NineImageDecode start";

    const std::string testName = TEST_FILE_SVG;

    /**
     * @tc.steps: step1. create image source by correct nine file path and nine png format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = NINE_PNG_FORMAT_TYPE;
    const std::string inName = INPUT_PATH + testName;
    auto imageSource = ImageSource::CreateImageSource(inName, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options
     * @tc.expected: step2. decode image source to pixel map success.
     */
    DecodeOptions decodeOpts;
    auto pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
    ASSERT_EQ(pixelMap->InnerGetNinePngResPtr()->paddingTop, TEST_NINE_PNG_PADDINGTOP);
    ASSERT_EQ(pixelMap->InnerGetNinePngResPtr()->paddingBottom, TEST_NINE_PNG_PADDINGBOTTOM);
    ASSERT_EQ(pixelMap->InnerGetNinePngResPtr()->paddingLeft, TEST_NINE_PNG_PADDINGLEFT);
    ASSERT_EQ(pixelMap->InnerGetNinePngResPtr()->paddingRight, TEST_NINE_PNG_PADDINGRIGHT); 
    GTEST_LOG_(INFO) << "ImageSourceNineTest: NineImageDecode end";
}
} // namespace Multimedia
} // namespace OHOS