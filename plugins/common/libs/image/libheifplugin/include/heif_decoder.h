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

#ifndef HEIF_DECODER_H
#define HEIF_DECODER_H

#include "abs_image_decoder.h"
#include "hilog/log.h"
#include "input_data_stream.h"
#include "log_tags.h"
#include "plugin_class_base.h"
#include "heif.h"

namespace OHOS {
namespace ImagePlugin {
using HeifContextUPrt =
    std::unique_ptr<struct heif_context, std::function<void(struct heif_context*)>>;
struct HeifImageInfo {
    heif_item_id id;
    std::shared_ptr<struct heif_image_handle> handle = nullptr;
    bool hasGetInfo = false;
    int width = -1;
    int height = -1;
    bool hasAlpha = false;
    heif_colorspace decodeColorspace;
    heif_chroma decodeChroma;
    PlPixelFormat pixelFormat;
};
class HeifDecoder : public AbsImageDecoder, public OHOS::MultimediaPlugin::PluginClassBase {
public:
    HeifDecoder();
    ~HeifDecoder() override;
    void SetSource(InputDataStream &sourceStream) override;
    void Reset() override;
    uint32_t SetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info) override;
    uint32_t Decode(uint32_t index, DecodeContext &context) override;
    uint32_t PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context) override;
    uint32_t GetImageSize(uint32_t index, PlSize &size) override;

#ifdef IMAGE_COLORSPACE_FLAG
    OHOS::ColorManager::ColorSpace getGrColorSpace();
    bool IsSupportICCProfile();
#endif

private:
    // private function
    DISALLOW_COPY_AND_MOVE(HeifDecoder);
    bool IsSourceReady(uint32_t index);
    bool InitHeifEnv();
    std::shared_ptr<struct heif_image_handle> GetImageHandle(uint32_t index);
    uint32_t GetHeifInfo(uint32_t index);
    void ReleaseHandles();
    uint32_t DecodePixels(uint32_t index, struct heif_image* image, DecodeContext &context);

    // private members
    InputDataStream *inputStreamPtr_ = nullptr;
    HeifContextUPrt heifContext_ = nullptr;
    std::vector<std::unique_ptr<struct HeifImageInfo>> heifImageInfos_;
    PixelDecodeOptions opts_;
    uint32_t currentIndex_ = 0;
#ifdef IMAGE_COLORSPACE_FLAG
    std::unique_ptr<OHOS::ColorManager::ColorSpace> colorSpace_ = nullptr;
#endif
};
} // namespace ImagePlugin
} // namespace OHOS

#endif // HEIF_DECODER_H
