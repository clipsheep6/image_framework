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

#ifndef HEIF_DECODER_ADAPTER_H
#define HEIF_DECODER_ADAPTER_H

#include <image/image_plugin_type.h>
#include "abs_image_decoder.h"
#include "heif_decoder_interface.h"
#include "heif_stream_wrapper.h"
#include "image_plugin_type.h"
#include "nocopyable.h"

namespace OHOS {
namespace Media {
class HeifDecoderAdapter : public ImagePlugin::HeifDecoderInterface {
public:
    HeifDecoderAdapter() = default;
    explicit HeifDecoderAdapter(HeifDecoder *heifDecoder);
    virtual ~HeifDecoderAdapter() override{};
    static std::unique_ptr<HeifDecoderInterface> MakeFromStream(ImagePlugin::InputDataStream &stream);
    virtual uint32_t OnGetPixels(const ImagePlugin::PlSize &dstSize, const uint32_t dstRowBytes,
                                 ImagePlugin::DecodeContext &context) override;
    virtual void SetAllowPartial(const bool isAllowPartialImage) override;
    virtual void GetHeifSize(ImagePlugin::PlSize &size) override;
    virtual bool ConversionSupported(const ImagePlugin::PlPixelFormat &plPixelFormat, int32_t &bytesPerPixel) override;
    uint32_t ReadRows(const uint32_t rowBytes, const uint32_t count, ImagePlugin::DecodeContext &context);

private:
    DISALLOW_COPY_AND_MOVE(HeifDecoderAdapter);
    std::unique_ptr<HeifDecoder> heifDecoder_ = nullptr;
    bool isAllowPartialImage_ = false;
    static HeifFrameInfo heifFrameInfo_;
};
} // namespace Media
} // namespace OHOS

#endif // HEIF_DECODER_ADAPTER_H