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

#ifndef RAW_DECODER_H
#define RAW_DECODER_H

#include <cstdint>
#include <string>
#include "SkCodec.h"
#include "abs_image_decoder.h"
#include "raw_stream.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "plugin_class_base.h"

namespace OHOS {
namespace ImagePlugin {
enum class RawDecodingState : int32_t {
    UNDECIDED = 0,
    SOURCE_INITED = 1,
    BASE_INFO_PARSED = 2,
    IMAGE_DECODING = 3,
    IMAGE_ERROR = 4,
    IMAGE_DECODED = 5
};

class RawDecoder : public AbsImageDecoder, public OHOS::MultimediaPlugin::PluginClassBase {
public:
    RawDecoder() = default;
    virtual ~RawDecoder() override{};
    void SetSource(InputDataStream &sourceStream) override;
    void Reset() override;
    uint32_t SetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info) override;
    uint32_t Decode(uint32_t index, DecodeContext &context) override;
    uint32_t GetImageSize(uint32_t index, PlSize &size) override;
    uint32_t PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context) override;
    uint32_t GetImagePropertyString(uint32_t index, const std::string &key, std::string &value) override;

private:
    DISALLOW_COPY_AND_MOVE(RawDecoder);
    bool DecodeHeader();
    PlAlphaType ConvertToAlphaType(SkAlphaType alphaType);
    SkColorType ConvertToColorType(PlPixelFormat format, PlPixelFormat &outputFormat);
    uint32_t AllocateMemory(SkImageInfo dstInfo, DecodeContext &context);
    std::string ConvertEncodedFormatToString(SkEncodedImageFormat format);
    InputDataStream *stream_ = nullptr;
    std::unique_ptr<SkCodec> codec_ = nullptr;
    SkImageInfo info_;
    SkColorType desireColor_ = kUnknown_SkColorType;
    RawDecodingState state_ = RawDecodingState::UNDECIDED;
};
} // namespace ImagePlugin
} // namespace OHOS

#endif // RAW_DECODER_H
