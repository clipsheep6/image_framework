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
#include <memory>
#include "jpeg_image_accessor.h"
#include "file_source_stream.h"

using namespace OHOS::Media;
using namespace testing::ext;

namespace OHOS {
namespace Multimedia {

static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test.jpg";

class JpegImageAccessorTest : public testing::Test {
public:
    JpegImageAccessorTest() {}
    ~JpegImageAccessorTest() {}
};

HWTEST_F(JpegImageAccessorTest, TC001, TestSize.Level3)
{
    JpegImageAccessor imageAccessor;
    std::unique_ptr<FileSourceStream> sourceStream = FileSourceStream::CreateSourceStream(IMAGE_INPUT_JPEG_PATH);
    ExifMetadata metadata = imageAccessor.ReadMetadata(*sourceStream);
    ASSERT_EQ(metadata.GetValue("FileType"), "JPEG");
}
} // namespace Multimedia
} // namespace OHOS