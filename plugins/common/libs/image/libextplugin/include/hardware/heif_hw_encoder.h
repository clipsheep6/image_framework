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

#ifndef HEIF_HARDWARE_ENCODER_H
#define HEIF_HARDWARE_ENCODER_H

#include <cinttypes>
#include "v1_0/icodec_image.h"
#include "image_log.h"
#include "picture.h"
#include "abs_image_encoder.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "HEIFHWENCODER"

#ifdef __FILE_NAME__
#define FILENAME __FILE_NAME__
#else
#define FILENAME __FILE__
#endif

#define LOG_FMT "[%{public}s][%{public}s %{public}d] "
#define HEIF_HW_LOGE(x, ...) \
    IMAGE_LOGE(LOG_FMT x, FILENAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HEIF_HW_LOGW(x, ...) \
    IMAGE_LOGW(LOG_FMT x, FILENAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HEIF_HW_LOGI(x, ...) \
    IMAGE_LOGI(LOG_FMT x, FILENAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HEIF_HW_LOGD(x, ...) \
    IMAGE_LOGD(LOG_FMT x, FILENAME, __FUNCTION__, __LINE__, ##__VA_ARGS__)

namespace OHOS {
namespace ImagePlugin {
class HeifHardwareEncoder {
public:
    HeifHardwareEncoder();
    ~HeifHardwareEncoder();

    bool IsHardwareEncodeSupported();
    uint32_t Encode(Media::Picture *picture, PlEncodeOptions opts,
                    OHOS::HDI::Codec::Image::V1_0::SharedBuffer& outputBuffer);
private:
    class LifeSpanTimer {
    public:
        explicit LifeSpanTimer(std::string desc);
        ~LifeSpanTimer();
    private:
        int64_t GetCurrentTimeInUs();
    private:
        int64_t startTimeInUs_;
        std::string desc_;
    };
private:
    bool InitEncoder();
    bool PrepareInputData(Media::Picture *picture, PlEncodeOptions opts);
    bool AssembleImageItemList(Media::Picture *picture, PlEncodeOptions opts);
    void AssemblePrimaryImageItem(Media::Picture *picture, PlEncodeOptions opts);
    void AssembleAuxiliaryImageItem(Media::Picture *picture, PlEncodeOptions opts);
    bool AssembleMetaItemTable(Media::Picture *picture);
    bool AssembleItemRefTable(Media::Picture *picture);
    bool DoEncode(OHOS::HDI::Codec::Image::V1_0::SharedBuffer& outputBuffer);
    void RecycleAllocatedResource();
private:
    OHOS::sptr<OHOS::HDI::Codec::Image::V1_0::ICodecImage> hwEncoder_;
    std::vector<OHOS::HDI::Codec::Image::V1_0::ImageItem> inputImgs_;
    std::vector<OHOS::HDI::Codec::Image::V1_0::MetaItem> inputMetas_;
    std::vector<OHOS::HDI::Codec::Image::V1_0::ItemRef> refs_;
};
} // namespace ImagePlugin
} // namespace OHOS

#endif // HEIF_HARDWARE_ENCODER_H