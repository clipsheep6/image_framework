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

#ifndef JPEG_HARDWARE_DECODER_H
#define JPEG_HARDWARE_DECODER_H

// #include "jpeg_decoder.h"
#include "v1_0/icodec_image_jpeg.h"
#include "v1_0/icodec_image_callback.h"
#include "v1_0/display_buffer_type.h"
#include "idisplay_buffer.h"
#include "display_type.h"
#include "abs_image_decoder.h"
#include "media_errors.h"
#include "jpeg_utils.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "hdf_log.h"
#include <ashmem.h>
#include <cerrno>
#include <cstring>
#include <securec.h>
namespace OHOS {
namespace ImagePlugin {
using namespace HDI::Codec::Image::V1_0;
using namespace HDI::Display::Buffer::V1_0;
using namespace Media;

struct JpegHardwareDecodeOpt {
    InputDataStream *inputStream = nullptr;//inputStream
    uint32_t width = 0;
    uint32_t height = 0;
    DecodeContext context;
};
class CodecJpegCallbackService;

class JpegHardwareDecoder {
public:
    JpegHardwareDecoder();
    ~JpegHardwareDecoder();
        enum JpegMarker : int16_t {
        SOF0 = 0xffc0,
        DHT = 0xffc4,
        SOI = 0xffd8,
        EOI = 0xffd9,
        SOS = 0xffda,
        DQT = 0xffdb,
        DRI = 0xffdd,
        UNKNOWN = 0xffff
    };
    // explicit CodecJpegHelper() = default;
    // ~CodecJpegHelper() = default;
    int32_t JpegAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t fd);
    bool DessambleJpeg(int8_t *buffer, size_t bufferLen, struct CodecJpegDecInfo &decInfo,
                       std::unique_ptr<int8_t[]> &compressBuffer, uint32_t &comBufLen);
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
    int32_t Init();
    int32_t DeInit();
    int32_t OnEvent(int32_t error);
    uint32_t GetJpegBuffer(InputDataStream *sourceStream);
    uint32_t AllocBuffer(uint32_t width, uint32_t height);
    uint32_t Decode(JpegHardwareDecodeOpt &opt);

private:
    uint32_t inline AlignUp(uint32_t width)
    {
        return (((width) + alignment_ - 1) & (~(alignment_ - 1)));
    }

private:
    int32_t FindMarker(int8_t *start);
    int32_t JpegDqtAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos);
    int32_t JpegDriAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos);
    int32_t JpegDhtAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos);
    int32_t JpegDhtAssemble(const std::vector<CodecJpegHuffTable> &table, int8_t *buffer, int32_t curPos,
                            bool dc = true);
    int32_t JpegSofAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos);
    int32_t JpegSosAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos);
    int32_t JpegDataAssemble(int8_t *buffer, int32_t curPos, int32_t fd);
    int32_t DessambleSof(int8_t *buffer, struct CodecJpegDecInfo &decInfo);
    int32_t DessambleSos(int8_t *buffer, struct CodecJpegDecInfo &decInfo);
    int32_t DessambleCompressData(int8_t *buffer, std::unique_ptr<int8_t[]> &compressBuffer, uint32_t &comBufLen);
    int32_t DessambleDqt(int8_t *buffer, struct CodecJpegDecInfo &decInfo);
    int32_t DessambleDht(int8_t *buffer, struct CodecJpegDecInfo &decInfo);
    int32_t PutInt16(int8_t *buffer, int32_t curPos, int16_t value);
    int32_t PutInt8(int8_t *buffer, int32_t curPos, int8_t value);
    int32_t GetInt8(int8_t *buffer);
    int32_t GetInt16(int8_t *buffer);

//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
    static constexpr int32_t MAX_BUFFER_LEN = 128;
    uint32_t streamPosition_ = 0;
    DataStreamBuffer jpegBuffer;//save jpegBuffer
    uint32_t bufferLen_;//length of jpegBuffer
    OHOS::HDI::Codec::Image::V1_0::CodecJpegDecInfo decInfo_;
    OHOS::sptr<OHOS::HDI::Codec::Image::V1_0::ICodecImageCallback> callback_;
    std::unique_ptr<int8_t[]> compressBuffer_;
    uint32_t compDataLen_;
    OHOS::sptr<ICodecImageJpeg> hdiDecoder_;
    OHOS::HDI::Codec::Image::V1_0::CodecImageBuffer inBuffer_;
    OHOS::HDI::Codec::Image::V1_0::CodecImageBuffer outBuffer_;
    OHOS::HDI::Display::Buffer::V1_0::IDisplayBuffer *hdiBuffer_;
    DecodeContext context;
    uint32_t alignment_ = 16;
};

class CodecJpegCallbackService : public OHOS::HDI::Codec::Image::V1_0::ICodecImageCallback {
public:
    CodecJpegCallbackService(std::shared_ptr<JpegHardwareDecoder> decoder)
    {
        decoder_ = decoder;
    };

    virtual ~CodecJpegCallbackService(){};

    int32_t OnImageEvent(int32_t error) override
    {
        decoder_->OnEvent(error);
        return SUCCESS;
    };

private:
    std::shared_ptr<JpegHardwareDecoder> decoder_;
};

}// namespace ImagePlugin
} // namespace OHOS

#endif // JPEG_HARDWARE_DECODER_H