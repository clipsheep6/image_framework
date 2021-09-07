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

#ifndef HW_JPEG_DECOMPRESSOR_H
#define HW_JPEG_DECOMPRESSOR_H

#include <cstdint>
#include <string>
#include "abs_image_decoder.h"
#include "abs_image_decompress_component.h"
#include "hilog/log.h"
#include "hw_jpeg_decompressor_wrapper.h"
#include "input_data_stream.h"
#include "jerror.h"
#include "jpeglib.h"
#include "log_tags.h"
#include "plugin_class_base.h"

namespace OHOS {
namespace ImagePlugin {
class HwJpegDecompressor : public AbsImageDecompressComponent, public OHOS::MultimediaPlugin::PluginClassBase {
public:
    HwJpegDecompressor();
    ~HwJpegDecompressor() = default;
    uint32_t Decompress(void *decompressInfo, InputDataStream *stream, DecodeContext &context) override;

private:
    DISALLOW_COPY_AND_MOVE(HwJpegDecompressor);
    bool CheckOriginalImageSize(uint32_t srcWidth, uint32_t srcHeight, uint32_t comps);
    bool CheckOutputImageSize(uint32_t outWidth, uint32_t comps);
    uint8_t *GetInputBuffer(size_t &bufferSize);
    uint32_t DoDecompress(DecodeContext &context);
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
        LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "HwJpegDecompressor"
    };
    jpeg_decompress_struct *decodeInfo_ = nullptr;
    InputDataStream *stream_ = nullptr;
    std::unique_ptr<HwJpegDecompressorInterface> hwDecompressor_ = nullptr;
};
} // namespace ImagePlugin
} // namespace OHOS

#endif // HW_JPEG_DECOMPRESSOR_H
