/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "heifimpl_fuzzer.h"
#define private public

#include <cstdint>
#include <string>
#include <unistd.h>
#include <fcntl.h>

#include "image_source.h"
#include "HeifDecoderImpl.h"
#include "include/core/SKStream.h"
#include "buffer_source_stream.h"
#include "ext_decoder.h"

namespace OHOS {
namespace Media {
    struct HeifStreamMock : public HeifStream{
    explicit HeifStreamMock(SKStream* stream) : fStream(stream){}
    
    ~HeifStreamMock() override{}
    
    size_t read(void* buffer, size_t size) override{
        return fStream->read(buffer,size);
    }

    bool rewind() override{
        return fStream->rewind();
    }

    bool seek(size_t position){
        return fStream->seek(position);
    }

    bool hasLength() const override{
        return fStream->hasLength();
    }

    size_t getLength() const override{
        return fStream->getLength();
    }

    bool hasPosition() const override{
        return fStream->hasPosition();
    }

    size_t getPosition() const override{
        return fStream->getPosition();
    }

    private:
    std::unique_ptr<SKStream> fStream;

    }    

void HeifDecodeImplTest001(ImagePlugin::HeifDecoderImpl *heifdecoderimpl, HeifFrameInfo *frameInfo)
{
    size_t frameCount = 0;
    enum SkHeifColorFormat heifColor = kHeifColorFormat_RGB565;
    int frameIndex = 0;
    uint8_t dstBuffer = 0;
    void *context = nullptr;

    heifdecoderimpl->getSequenceInfo(frameInfo, frameCount);
    heifdecoderimpl->setOutputColor(heifColor);
    heifdecoderimpl->decode(frameInfo);
    heifdecoderimpl->decodeSequence(frameIndex, frameInfo);
    heifdecoderimpl->setDstBuffer(&dstBuffer, frameCount, context);
    heifdecoderimpl->getScanline(&dstBuffer);
    heifdecoderimpl->skipScanlines(frameIndex);
    heifdecoderimpl->getImageInfo(frameInfo);
    heifdecoderimpl->decodeGainmap();
    heifdecoderimpl->setGainmapDstBuffer(&dstBuffer, frameCount);
    heifdecoderimpl->getGainmapInfo(frameInfo);
    heifdecoderimpl->getTmapInfo(frameInfo);
    heifdecoderimpl->getHdrType();
    heifdecoderimpl->getVividMetadata(std::vector<uint8_t>& uwaInfo, std::vector<uint8_t>& displayInfo,std::vector<uint8_t>& lightInfo);
    heifdecoderimpl->getISOMetadata(std::vector<uint8_t>& isoMetadata);
    heifdecoderimpl->getErrMsg(std::string& errMsg);
    heifdecoderimpl->Reinit(HeifFrameInfo *frameInfo);
    heifdecoderimpl->InitFrameInfo(HeifFrameInfo *frameInfo, const std::shared_ptr<HeifImage> &image);
    heifdecoderimpl->GetTileSize(const std::shared_ptr<HeifImage> &image, uint32_t &tileWidth, uint32_t &tileHeight);
    heifdecoderimpl->SetRowColNum();
    heifdecoderimpl->ProcessChunkHead(uint8_t *data, size_t len);
    heifdecoderimpl->DecodeGrids(sptr<SurfaceBuffer> &hwBuffer, bool isGainmap = false);
    heifdecoderimpl->DecodeIdenImage(sptr<SurfaceBuffer> &hwBuffer, bool isGainmap = false);
    heifdecoderimpl->DecodeSingleImage(std::shared_ptr<HeifImage> &image, sptr<SurfaceBuffer> &hwBuffer, bool isGainmap = false);
    heifdecoderimpl->ConvertHwBufferPixelFormat(sptr<SurfaceBuffer> &hwBuffer, bool isGainmap = false);
    heifdecoderimpl->IsDirectYUVDecode();
    heifdecoderimpl->SetColorSpaceInfo(HeifFrameInfo* info, const std::shared_ptr<HeifImage>& image);
    heifdecoderimpl->SetHardwareDecodeErrMsg(const uint32_t width, const uint32_t height);
}

void HeifImplFuzzTest001(const uint8_t* data, size_t size)
{
    Media::SourceOptions opts;
    //HeifDecodeImpl.cpp create/init/fuzztest
    auto heifdecoder = CreateHeifDecoderImpl();
    void *obj = dynamic_cast<void*>(heifdecoder);
    ImagePlugin::HeifDecoderImpl *heifdecoderimpl = static_cast<ImagePlugin::HeifDecoderImpl*>(obj);
    std::unique_ptr<ImagePlugin::InputDataStream> stream_ = BufferSourceStream::CreateSourceStream(data, size);
    auto skStream = std::make_shared<ImagePlugin::ExtStream>(stream_.release());
    HeifFrameInfo heifInfo;
    heifdecoderimpl->init(new HeifStreamMock(skStream.release()), &heifInfo);
    HeifDecodeImplTest001(heifdecoderimpl, &heifInfo);
    

}
} //namespace media
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Media::HeifImplFuzzTest001(data, size);
    return 0;
}