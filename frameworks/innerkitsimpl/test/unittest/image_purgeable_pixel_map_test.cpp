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

#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include "media_errors.h"
#include "purgeable_pixel_map.h"
#include "image_type.h"

using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Multimedia {

class ImagePurgeablePixelMapTest : public testing::Test {
public:
    ImagePurgeablePixelMapTest(){}
    ~ImagePurgeablePixelMapTest(){}
};

/**
 * @tc.name: ImagePurgeablePixelMap012
 * @tc.desc: create pixelmap with initialization options
 * @tc.type: FUNC
 */
HWTEST_F(ImagePurgeablePixelMapTest, ImagePurgeablePixelMap001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "ImagePurgeablePixelMapTest: ImagePurgeablePixelMap001 start";
    /**
     * @tc.steps: step1. set initialization options
     * @tc.expected: step1. The new pixelmap is not null.
     */
    // const uint32_t color[8] = { 0x80, 0x02, 0x04, 0x08, 0x40, 0x02, 0x04, 0x08 };
    // uint32_t colorlength = sizeof(color) / sizeof(color[0]);
    // const int32_t offset = 1;
    InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 300;
    opts.pixelFormat = PixelFormat::ARGB_8888;
    opts.alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    // int32_t width = opts.size.width;
    struct Position pos;
    pos.x = 0;
    pos.y = 0;
    uint32_t out_color1 = 0;

    std::unique_ptr<PurgeablePixelMap> newPurgeablePixelMap = PurgeablePixelMap::Create(opts);

    newPurgeablePixelMap->BeginRead();
    newPurgeablePixelMap->ReadPixel(pos, out_color1);
    newPurgeablePixelMap->EndRead();
    std::cout << "Before reclaim 1: " << out_color1 << std::endl;

    newPurgeablePixelMap->BeginRead();
    newPurgeablePixelMap->ReadPixel(pos, out_color1);
    newPurgeablePixelMap->EndRead();
    std::cout << "Before reclaim 2: " << out_color1 << std::endl;
 
    out_color1 = 1234;
    std::cout << "set out_color1: " << out_color1 << std::endl;

    FILE *f = fopen("/proc/sys/kernel/purgeable", "w");
    if (!f) {
        std::cout << "open file failed" << std::endl;
        return;
    }
    bool succ = true;
    if (fputs("1", f) == EOF) {
        succ = false;
    }
    if (fclose(f) == EOF) {
        std::cout << "close file failed" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2)); /* wait reclaim finish */
    std::cout << "Reclaim result=" << (succ ? "succ" : "fail") << std::endl;

    newPurgeablePixelMap->BeginRead();
    newPurgeablePixelMap->ReadPixel(pos, out_color1);
    newPurgeablePixelMap->EndRead();
    std::cout << "After reclaim: " << out_color1 << std::endl;

    // EXPECT_EQ(newPurgeablePixelMap, nullptr);
    GTEST_LOG_(INFO) << "ImagePurgeablePixelMapTest: ImagePurgeablePixelMap001 end";
}

} // namespace Multimedia
} // namespace OHOS
