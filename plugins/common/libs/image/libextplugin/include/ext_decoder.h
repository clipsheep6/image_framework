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

#ifndef EXT_DECODER_H
#define EXT_DECODER_H

#include <cstdint>
#include <string>

#include "abs_image_decoder.h"
#include "ext_stream.h"
#include "nocopyable.h"
#include "plugin_class_base.h"
#include "SkCodec.h"

namespace OHOS {
namespace ImagePlugin {
class ExtDecoder : public AbsImageDecoder, public OHOS::MultimediaPlugin::PluginClassBase, NoCopyable {
public:
    ExtDecoder();
    virtual ~ExtDecoder() override {};
    bool HasProperty(std::string key) override;
    uint32_t Decode(uint32_t index, DecodeContext &context) override;
    uint32_t GetImageSize(uint32_t index, PlSize &size) override;
    uint32_t PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context) override;
    uint32_t SetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info) override;
    uint32_t GetImagePropertyString(uint32_t index, const std::string &key, std::string &value) override;
    void Reset() override;
    void SetSource(InputDataStream &sourceStream) override;
#ifdef IMAGE_COLORSPACE_FLAG
    bool IsSupportICCProfile() override;
#endif
private:
    bool CheckCodec();
    bool CheckIndexVailed(uint32_t index);
    bool DecodeHeader();
    bool ConvertInfoToAlphaType(SkAlphaType &alphaType, PlAlphaType &outputType);
    bool ConvertInfoToColorType(SkColorType &format, PlPixelFormat &outputFormat);
    SkAlphaType ConvertToAlphaType(PlAlphaType desiredType, PlAlphaType &outputType);
    SkColorType ConvertToColorType(PlPixelFormat format, PlPixelFormat &outputFormat);
    uint32_t SetContextPixelsBuffer(uint64_t byteCount, DecodeContext &context);
    ImagePlugin::InputDataStream *stream_;
    std::unique_ptr<SkCodec> codec_;
    SkImageInfo info_;
    SkImageInfo dstInfo_;
    SkCodec::Options dstOptions_;
    SkIRect dstSubset_;
    int32_t frameCount_;
};
} // namespace ImagePlugin
} // namespace OHOS

#endif // EXT_DECODER_H
