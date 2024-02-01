/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#define private public
#include <gtest/gtest.h>
#include "media_errors.h"
#include "hardware/jpeg_hw_decoder.h"
#include "mock_jpeg_hw_decode_flow.h"
#include "image_system_properties.h"

namespace OHOS::Media {
using namespace testing::ext;
using namespace OHOS::ImagePlugin;
using namespace OHOS::HDI::Codec::Image::V1_0;

class JpegHwDecoderTest : public testing::Test {
public:
    static constexpr char JPEG_FORMAT[] = "image/jpeg";
    static constexpr char HEIF_FORMAT[] = "image/heif";
    static constexpr char TEST_JPEG_IMG[] = "/data/local/tmp/image/test_hw1.jpg";
};

class MockJpegHwDecode : public ImagePlugin::InputDataStream {
public:
    MockJpegHwDecode() = default;
    virtual ~MockJpegHwDecode() {}

    bool Read(uint32_t desiredSize, DataStreamBuffer &outData)
    {
        return returnBoll_;
    }
    bool Read(uint32_t desiredSize, uint8_t *outBuffer, uint32_t bufferSize, uint32_t &readSize)
    {
        return returnBoll_;
    }
    bool Peek(uint32_t desiredSize, DataStreamBuffer &outData)
    {
        return returnBoll_;
    }
    bool Peek(uint32_t desiredSize, uint8_t *outBuffer, uint32_t bufferSize, uint32_t &readSize)
    {
        return returnBoll_;
    }
    uint32_t Tell()
    {
        return returnValue_;
    }
    bool Seek(uint32_t position)
    {
        return returnBoll_;
    }
private:
    bool returnBoll_;
    uint32_t returnValue_;
};

HWTEST_F(JpegHwDecoderTest, unsupported_img_empty_format, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 8192,
        .height = 8192
    };
    bool ret = testObj.IsHardwareDecodeSupported("", srcImgSize);
    ASSERT_FALSE(ret);
}

HWTEST_F(JpegHwDecoderTest, unsupported_img_unknown_format, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 512,
        .height = 512
    };
    bool ret = testObj.IsHardwareDecodeSupported(HEIF_FORMAT, srcImgSize);
    ASSERT_FALSE(ret);
}

HWTEST_F(JpegHwDecoderTest, unsupported_img_size_too_small, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 140,
        .height = 512
    };
    bool ret = testObj.IsHardwareDecodeSupported(JPEG_FORMAT, srcImgSize);
    ASSERT_FALSE(ret);
}

HWTEST_F(JpegHwDecoderTest, unsupported_img_size_too_big, TestSize.Level1)
{
    JpegHardwareDecoder testObj;
    PlSize srcImgSize = {
        .width = 8192,
        .height = 8193
    };
    bool ret = testObj.IsHardwareDecodeSupported(JPEG_FORMAT, srcImgSize);
    ASSERT_FALSE(ret);
}

HWTEST_F(JpegHwDecoderTest, decode_ok, TestSize.Level1)
{
    CommandOpt opt;
    opt.width = 1280;
    opt.height = 768;
    opt.sampleSize = 1;
    opt.inputFile = TEST_JPEG_IMG;
    JpegHwDecoderFlow demo;
    bool ret = true;
    if (ImageSystemProperties::GetHardWareDecodeEnabled()) {
        ret = demo.Run(opt, false);
    }
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: GetCurrentTimeInUsTest001
 * @tc.desc: Test of GetCurrentTimeInUs
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, GetCurrentTimeInUsTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: GetCurrentTimeInUsTest001 start";
    std::string desc = "LifeSpanTimer";
    auto lifeSpanTimer = std::make_shared<JpegHardwareDecoder::LifeSpanTimer>(desc);
    lifeSpanTimer->GetCurrentTimeInUs();
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: GetCurrentTimeInUsTest001 end";
}

/**
 * @tc.name: IsHardwareDecodeSupportedTest005
 * @tc.desc: Test of IsHardwareDecodeSupported
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, IsHardwareDecodeSupportedTest005, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: IsHardwareDecodeSupportedTest005 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    const std::string srcImgFormat;
    PlSize srcImgSize;
    jpegHardwareDecoder->hwDecoder_ = nullptr;
    bool result = jpegHardwareDecoder->IsHardwareDecodeSupported(srcImgFormat, srcImgSize);
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: IsHardwareDecodeSupportedTest005 end";
}

/**
 * @tc.name: IsHardwareDecodeSupportedTest006
 * @tc.desc: Test of IsHardwareDecodeSupported
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, IsHardwareDecodeSupportedTest006, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: IsHardwareDecodeSupportedTest006 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    std::string srcImgFormat = "image/";
    PlSize srcImgSize;
    bool result = jpegHardwareDecoder->IsHardwareDecodeSupported(srcImgFormat, srcImgSize);
    ASSERT_EQ(result, false);
    srcImgFormat = jpegHardwareDecoder->JPEG_FORMAT_DESC;
    result = jpegHardwareDecoder->IsHardwareDecodeSupported(srcImgFormat, srcImgSize);
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: IsHardwareDecodeSupportedTest006 end";
}

/**
 * @tc.name: DecodeTest001
 * @tc.desc: Test of Decode
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, DecodeTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: DecodeTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    SkCodec *codec = nullptr;
    ImagePlugin::InputDataStream *srcStream = nullptr;
    PlSize srcImgSize;
    uint32_t sampleSize = 0;
    CodecImageBuffer outputBuffer;
    uint32_t result = jpegHardwareDecoder->Decode(codec, srcStream, srcImgSize, sampleSize, outputBuffer);
    ASSERT_EQ(result, Media::ERR_IMAGE_DATA_UNSUPPORT);
    jpegHardwareDecoder->hwDecoder_ = nullptr;
    result = jpegHardwareDecoder->Decode(codec, srcStream, srcImgSize, sampleSize, outputBuffer);
    ASSERT_EQ(result, Media::ERR_IMAGE_DECODE_ABNORMAL);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: DecodeTest001 end";
}

/**
 * @tc.name: AssembleComponentInfoTest001
 * @tc.desc: Test of AssembleComponentInfo
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, AssembleComponentInfoTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleComponentInfoTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    std::shared_ptr<jpeg_decompress_struct> jpegCompressInfo = std::make_shared<jpeg_decompress_struct>();
    jpegCompressInfo->num_components = 2;
    bool result = jpegHardwareDecoder->AssembleComponentInfo(jpegCompressInfo.get());
    ASSERT_EQ(result, false);
    jpegCompressInfo->num_components = 3;
    auto compInfo = std::make_shared<jpeg_component_info>();
    jpegCompressInfo->comp_info = compInfo.get();
    result = jpegHardwareDecoder->AssembleComponentInfo(jpegCompressInfo.get());
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleComponentInfoTest001 end";
}

/**
 * @tc.name: PrepareInputDataTest001
 * @tc.desc: Test of PrepareInputData
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, PrepareInputDataTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: PrepareInputDataTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    std::shared_ptr<SkCodec> codec;
    auto srcStream = std::make_shared<MockJpegHwDecode>();
    bool result = jpegHardwareDecoder->PrepareInputData(codec.get(), srcStream.get());
    ASSERT_EQ(result, false);
    codec = nullptr;
    srcStream = nullptr;
    result = jpegHardwareDecoder->PrepareInputData(codec.get(), srcStream.get());
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: PrepareInputDataTest001 end";
}

/**
 * @tc.name: HuffmanTblTransformTest001
 * @tc.desc: Test of HuffmanTblTransform
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, HuffmanTblTransformTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: HuffmanTblTransformTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    JHUFF_TBL* huffTbl = nullptr;
    CodecJpegHuffTable tbl;
    bool result = jpegHardwareDecoder->HuffmanTblTransform(huffTbl, tbl);
    ASSERT_EQ(result, false);
    huffTbl =std::make_shared<JHUFF_TBL>().get();
    huffTbl->bits[1] = 255;
    huffTbl->bits[2] = 128;
    result = jpegHardwareDecoder->HuffmanTblTransform(huffTbl, tbl);
    ASSERT_EQ(result, false);
    huffTbl->bits[1] = 128;
    result = jpegHardwareDecoder->HuffmanTblTransform(huffTbl, tbl);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: HuffmanTblTransformTest001 end";
}

/**
 * @tc.name: AssembleHuffmanTableTest001
 * @tc.desc: Test of AssembleHuffmanTable
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, AssembleHuffmanTableTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleHuffmanTableTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    std::shared_ptr<jpeg_decompress_struct> jpegCompressInfo = std::make_shared<jpeg_decompress_struct>();
    jpegCompressInfo->dc_huff_tbl_ptrs[0] = std::make_shared<JHUFF_TBL>().get();
    jpegCompressInfo->ac_huff_tbl_ptrs[0] = std::make_shared<JHUFF_TBL>().get();
    jpegHardwareDecoder->AssembleHuffmanTable(jpegCompressInfo.get());
    ASSERT_EQ(jpegHardwareDecoder->decodeInfo_.dcHuffTbl.empty(), false);
    ASSERT_EQ(jpegHardwareDecoder->decodeInfo_.acHuffTbl.empty(), false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleHuffmanTableTest001 end";
}

/**
 * @tc.name: AssembleQuantizationTableTest001
 * @tc.desc: Test of AssembleQuantizationTable
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, AssembleQuantizationTableTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleQuantizationTableTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    auto jpegCompressInfo = std::make_shared<jpeg_decompress_struct>();
    jpegHardwareDecoder->AssembleQuantizationTable(jpegCompressInfo.get());
    ASSERT_EQ(jpegHardwareDecoder->decodeInfo_.quantTbl.empty(), false);
    jpegCompressInfo->quant_tbl_ptrs[0] = nullptr;
    jpegCompressInfo->quant_tbl_ptrs[1] = nullptr;
    jpegCompressInfo->quant_tbl_ptrs[2] = nullptr;
    jpegCompressInfo->quant_tbl_ptrs[3] = nullptr;
    jpegHardwareDecoder->AssembleQuantizationTable(jpegCompressInfo.get());
    ASSERT_EQ(jpegHardwareDecoder->decodeInfo_.quantTbl.empty(), false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleQuantizationTableTest001 end";
}

/**
 * @tc.name: AssembleJpegImgHeaderTest001
 * @tc.desc: Test of AssembleJpegImgHeader
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, AssembleJpegImgHeaderTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleJpegImgHeaderTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    auto jpegCompressInfo = std::make_shared<jpeg_decompress_struct>();
    jpegCompressInfo->num_components = 2;
    bool result = jpegHardwareDecoder->AssembleJpegImgHeader(jpegCompressInfo.get());
    ASSERT_EQ(result, false);
    jpegCompressInfo->num_components = 3;
    auto compInfo = std::make_shared<jpeg_component_info>();
    jpegCompressInfo->comp_info = compInfo.get();
    result = jpegHardwareDecoder->AssembleJpegImgHeader(jpegCompressInfo.get());
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: AssembleJpegImgHeaderTest001 end";
}

/**
 * @tc.name: CopySrcImgToDecodeInputBufferTest001
 * @tc.desc: Test of CopySrcImgToDecodeInputBuffer
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, CopySrcImgToDecodeInputBufferTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: CopySrcImgToDecodeInputBufferTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    auto srcStream = std::make_shared<MockJpegHwDecode>();
    bool result = jpegHardwareDecoder->CopySrcImgToDecodeInputBuffer(srcStream.get());
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: CopySrcImgToDecodeInputBufferTest001 end";
}

/**
 * @tc.name: IsStandAloneJpegMarkerTest001
 * @tc.desc: Test of IsStandAloneJpegMarker
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, IsStandAloneJpegMarkerTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: IsStandAloneJpegMarkerTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    uint16_t marker = 0xFFD8;
    bool result = jpegHardwareDecoder->IsStandAloneJpegMarker(marker);
    ASSERT_EQ(result, true);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: IsStandAloneJpegMarkerTest001 end";
}

/**
 * @tc.name: JumpOverCurrentJpegMarkerTest001
 * @tc.desc: Test of JumpOverCurrentJpegMarker
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, JumpOverCurrentJpegMarkerTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: JumpOverCurrentJpegMarkerTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    uint8_t data[10] = {0, 0, 0, 0, 0, 0, 0, 8, 0, 10};
    unsigned int curPos = 0;
    unsigned int totalLen = 3;
    uint16_t marker = 0xFFD8;
    bool result = jpegHardwareDecoder->JumpOverCurrentJpegMarker(data, curPos, totalLen, marker);
    ASSERT_EQ(result, false);
    totalLen = 6;
    result = jpegHardwareDecoder->JumpOverCurrentJpegMarker(data, curPos, totalLen, marker);
    ASSERT_EQ(result, true);
    marker = 0xFF00;
    totalLen = 13;
    result = jpegHardwareDecoder->JumpOverCurrentJpegMarker(data, curPos, totalLen, marker);
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: JumpOverCurrentJpegMarkerTest001 end";
}

/**
 * @tc.name: InitDecoderTest001
 * @tc.desc: Test of InitDecoder
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, InitDecoderTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: InitDecoderTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    bool result = jpegHardwareDecoder->InitDecoder();
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: InitDecoderTest001 end";
}

/**
 * @tc.name: DoDecodeTest001
 * @tc.desc: Test of DoDecode
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, DoDecodeTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: DoDecodeTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    CodecImageBuffer outputBuffer;
    bool result = jpegHardwareDecoder->DoDecode(outputBuffer);
    ASSERT_EQ(result, false);
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: DoDecodeTest001 end";
}

/**
 * @tc.name: RecycleAllocatedResourceTest001
 * @tc.desc: Test of RecycleAllocatedResource
 * @tc.type: FUNC
 */
HWTEST_F(JpegHwDecoderTest, RecycleAllocatedResourceTest001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: RecycleAllocatedResourceTest001 start";
    auto jpegHardwareDecoder = std::make_shared<JpegHardwareDecoder>();
    jpegHardwareDecoder->RecycleAllocatedResource();
    GTEST_LOG_(INFO) << "JpegHwDecoderTest: RecycleAllocatedResourceTest001 end";
}
} // namespace OHOS::Media