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
#include <fstream>
#include <fcntl.h>
#include "file_packer_stream.h"
#include "image_type.h"
#include "image_utils.h"
#include "image_source.h"
#include "image_source_util.h"
#include "media_errors.h"
#include "pixel_map.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImageSourceUtil;

namespace OHOS {
namespace Multimedia {
static const std::string IMAGE_INPUT_JPG_PATH = "/data/local/tmp/image/test.jpg";
class FilePackerStreamTest : public testing::Test {
public:
    FilePackerStreamTest() {}
    ~FilePackerStreamTest() {}
};

/**
 * @tc.name: FilePackerStreamTest002
 * @tc.desc: Write buffer is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(FilePackerStreamTest, FilePackerStreamTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "FilePackerStreamTest: FilePackerStreamTest002 start";
    FilePackerStream filePackerStream(IMAGE_INPUT_JPG_PATH);
    uint8_t buffer[32] = {0};
    uint32_t size = 32;
    filePackerStream.Write(buffer, size);
    FilePackerStream filePackerStream2(IMAGE_INPUT_JPG_PATH);
    filePackerStream2.BytesWritten();
    GTEST_LOG_(INFO) << "FilePackerStreamTest: FilePackerStreamTest002 end";
}

}
}