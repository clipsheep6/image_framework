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
#include "istream_source_stream.h"
#include "image_source_util.h"
#include "media_errors.h"
#include "pixel_map.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImageSourceUtil;

namespace OHOS {
namespace Multimedia {
using namespace std;
static const std::string IMAGE_INPUT_JPG_PATH = "/data/local/tmp/image/test.jpg";
class IstreamSourceStreamTest : public testing::Test {
public:
    IstreamSourceStreamTest() {}
    ~IstreamSourceStreamTest() {}
};

/**
 * @tc.name: Peek001
 * @tc.desc: Write buffer is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(IstreamSourceStreamTest, Peek001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "IstreamSourceStreamTest: Peek001 start";
    std::filebuf in;
    in.open(IMAGE_INPUT_JPG_PATH, std::ios::in);
    std::unique_ptr<std::istream> inputStream = std::make_unique<std::istream>(&in);
    unique_ptr<IstreamSourceStream> streamPtr = IstreamSourceStream::CreateSourceStream(std::move(inputStream));

    uint8_t buffer[32] = {0};
    uint32_t size = 32;
    uint32_t desireSize = static_cast<uint32_t>(size);
    uint32_t bufferSize = desireSize;
    uint32_t readSize = desireSize;
    streamPtr->Peek(desireSize, static_cast<uint8_t *>(buffer), bufferSize, readSize);
    ImagePlugin::DataStreamBuffer outData;
    uint32_t desiredSize = 32;
    streamPtr->Peek(desiredSize, outData);
    streamPtr->GetDataPtr();
    GTEST_LOG_(INFO) << "IstreamSourceStreamTest: Peek001 end";
}
}
}