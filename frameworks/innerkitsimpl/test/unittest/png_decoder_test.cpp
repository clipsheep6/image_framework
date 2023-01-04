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
#include "buffer_source_stream.h"
#include "media_errors.h"
#include "memory.h"
#include "png_decoder.h"
#include "securec.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::ImagePlugin;
namespace OHOS {
namespace Multimedia {
static constexpr size_t NUMBER_ONE = 1;
static constexpr size_t NUMBER_TWO = 2;
class PngDecoderTest : public testing::Test {
public:
    PngDecoderTest() {}
    ~PngDecoderTest() {}
};

class MockInputDataStream : public SourceStream {
public:
    MockInputDataStream() = default;

    uint32_t UpdateData(const uint8_t *data, uint32_t size, bool isCompleted) override
    {
        return ERR_IMAGE_DATA_UNSUPPORT;
    }
    bool Read(uint32_t desiredSize, DataStreamBuffer &outData) override
    {
        if (streamSize == NUMBER_ONE) {
            streamBuffer = std::make_shared<uint8_t>(streamSize);
            outData.inputStreamBuffer = streamBuffer.get();
        } else if (streamSize == NUMBER_TWO) {
            outData.dataSize = streamSize;
        }
        return returnValue_;
    }

    bool Read(uint32_t desiredSize, uint8_t *outBuffer, uint32_t bufferSize, uint32_t &readSize) override
    {
        return returnValue_;
    }

    bool Peek(uint32_t desiredSize, DataStreamBuffer &outData) override
    {
        return returnValue_;
    }

    bool Peek(uint32_t desiredSize, uint8_t *outBuffer, uint32_t bufferSize, uint32_t &readSize) override
    {
        return returnValue_;
    }

    uint32_t Tell() override
    {
        return 0;
    }

    bool Seek(uint32_t position) override
    {
        return returnValue_;
    }

    uint32_t GetStreamType()
    {
        return -1;
    }

    uint8_t *GetDataPtr()
    {
        return nullptr;
    }

    bool IsStreamCompleted()
    {
        return returnValue_;
    }

    size_t GetStreamSize()
    {
        return streamSize;
    }

    void SetReturn(bool returnValue)
    {
        returnValue_ = returnValue;
    }

    void SetStreamSize(size_t size)
    {
        streamSize = size;
    }

    ~MockInputDataStream() {}
private:
    bool returnValue_ = false;
    size_t streamSize = 0;
    std::shared_ptr<uint8_t> streamBuffer = nullptr;
};

/**
 * @tc.name: GetImageSizeTest001
 * @tc.desc: Test of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, GetImageSizeTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest001 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    ImagePlugin::PlSize plSize;
    pngDecoder->SetSource(*streamPtr.release());
    pngDecoder->GetImageSize(2, plSize);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest001 end";
}

/**
 * @tc.name: GetImageSizeTest002
 * @tc.desc: Test of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, GetImageSizeTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest002 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    ImagePlugin::PlSize plSize;
    pngDecoder->GetImageSize(0, plSize);
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest002 end";
}

/**
 * @tc.name: GetImageSizeTest003
 * @tc.desc: Test of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, GetImageSizeTest003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest003 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    auto mock = std::make_shared<MockInputDataStream>();
    pngDecoder->SetSource(*mock.get());
    ImagePlugin::PlSize plSize;
    pngDecoder->GetImageSize(0, plSize);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest003 end";
}

/**
 * @tc.name: GetImageSizeTest004
 * @tc.desc: Test of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, GetImageSizeTest004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest004 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    auto mock = std::make_shared<MockInputDataStream>();
    mock->SetReturn(true);
    pngDecoder->SetSource(*mock.get());
    ImagePlugin::PlSize plSize;
    pngDecoder->GetImageSize(0, plSize);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest004 end";
}

/**
 * @tc.name: GetImageSizeTest005
 * @tc.desc: Test of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, GetImageSizeTest005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest005 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    auto mock = std::make_shared<MockInputDataStream>();
    mock->SetStreamSize(1);
    mock->SetReturn(true);
    pngDecoder->SetSource(*mock.get());
    ImagePlugin::PlSize plSize;
    pngDecoder->GetImageSize(0, plSize);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest005 end";
}

/**
 * @tc.name: GetImageSizeTest006
 * @tc.desc: Test of GetImageSize
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, GetImageSizeTest006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest006 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    auto mock = std::make_shared<MockInputDataStream>();
    mock->SetStreamSize(2);
    mock->SetReturn(true);
    pngDecoder->SetSource(*mock.get());
    ImagePlugin::PlSize plSize;
    pngDecoder->GetImageSize(0, plSize);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: GetImageSizeTest006 end";
}

/**
 * @tc.name: SetDecodeOptionsTest001
 * @tc.desc: Test of SetDecodeOptions
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, SetDecodeOptionsTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest001 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    PixelDecodeOptions opts;
    PlImageInfo info;
    pngDecoder->SetDecodeOptions(2, opts, info);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest001 end";
}

/**
 * @tc.name: SetDecodeOptionsTest002
 * @tc.desc: Test of SetDecodeOptions
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, SetDecodeOptionsTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest002 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    PixelDecodeOptions opts;
    PlImageInfo info;
    pngDecoder->SetDecodeOptions(0, opts, info);
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest002 end";
}

/**
 * @tc.name: SetDecodeOptionsTest003
 * @tc.desc: Test of SetDecodeOptions
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, SetDecodeOptionsTest003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest003 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    PixelDecodeOptions opts;
    opts.desiredPixelFormat = PlPixelFormat::RGB_565;
    PlImageInfo info;
    pngDecoder->SetDecodeOptions(0, opts, info);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest003 end";
}

/**
 * @tc.name: SetDecodeOptionsTest004
 * @tc.desc: Test of SetDecodeOptions
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, SetDecodeOptionsTest004, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest004 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    auto mock = std::make_shared<MockInputDataStream>();
    mock->SetReturn(false);
    pngDecoder->SetSource(*mock.get());
    PixelDecodeOptions opts;
    PlImageInfo info;
    pngDecoder->SetDecodeOptions(0, opts, info);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: SetDecodeOptionsTest004 end";
}

/**
 * @tc.name: DecodeTest001
 * @tc.desc: Test of Decode
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, DecodeTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: DecodeTest001 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    DecodeContext context;
    pngDecoder->Decode(2, context);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: DecodeTest001 end";
}

/**
 * @tc.name: DecodeTest002
 * @tc.desc: Test of Decode
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, DecodeTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: DecodeTest002 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    DecodeContext context;
    pngDecoder->Decode(0, context);
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: DecodeTest002 end";
}

/**
 * @tc.name: HasProperty001
 * @tc.desc: test HasProperty
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, HasProperty001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: HasProperty001 start";
    ImagePlugin::PngDecoder pngdcod;
    std::string key = "";
    bool haspro = pngdcod.HasProperty(key);
    ASSERT_EQ(haspro, false);
    GTEST_LOG_(INFO) << "PngDecoderTest: HasProperty001 end";
}

/**
 * @tc.name: PromoteIncrementalDecodeTest001
 * @tc.desc: Test of Decode
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, PromoteIncrementalDecodeTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: PromoteIncrementalDecodeTest001 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    ProgDecodeContext context;
    pngDecoder->PromoteIncrementalDecode(2, context);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: PromoteIncrementalDecodeTest001 end";
}

/**
 * @tc.name: PromoteIncrementalDecodeTest002
 * @tc.desc: Test of Decode
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, PromoteIncrementalDecodeTest002, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: PromoteIncrementalDecodeTest002 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    ProgDecodeContext context;
    pngDecoder->PromoteIncrementalDecode(0, context);
    int size = 1000;
    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(size);
    auto streamPtr = BufferSourceStream::CreateSourceStream(data.get(), size);
    pngDecoder->SetSource(*streamPtr.release());
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: PromoteIncrementalDecodeTest002 end";
}

/**
 * @tc.name: PromoteIncrementalDecodeTest003
 * @tc.desc: Test of Decode
 * @tc.type: FUNC
 */
HWTEST_F(PngDecoderTest, PromoteIncrementalDecodeTest003, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PngDecoderTest: PromoteIncrementalDecodeTest003 start";
    auto pngDecoder = std::make_shared<PngDecoder>();
    auto mock = std::make_shared<MockInputDataStream>();
    mock->SetReturn(false);
    pngDecoder->SetSource(*mock.get());
    ProgDecodeContext context;
    pngDecoder->PromoteIncrementalDecode(0, context);
    bool result = (pngDecoder != nullptr);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "PngDecoderTest: PromoteIncrementalDecodeTest003 end";
}
} // namespace Multimedia
} // namespace OHOS