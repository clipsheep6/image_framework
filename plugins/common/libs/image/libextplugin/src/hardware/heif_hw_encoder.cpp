/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "hardware/heif_hw_encoder.h"

#include "iservmgr_hdi.h"
#include "media_errors.h"
#include <surface.h>

namespace OHOS::ImagePlugin {
using namespace OHOS::HDI::Codec::Image::V1_0;
using namespace OHOS::HDI::ServiceManager::V1_0;

static std::mutex g_codecMtx;
static sptr<ICodecImage> g_codecMgr;
const static uint32_t PRIMARY_IMAGE_ITEM_ID = 1;
const static std::string IMAGE_ITEM_COMPRESS_TYPE_HEVC = "hevc";

class Listener : public ServStatListenerStub {
public:
    void OnReceive(const ServiceStatus &status) override
    {
        if (status.serviceName == "codec_image_service" && status.status == SERVIE_STATUS_STOP) {
            HEIF_HW_LOGW("codec_image_service died");
            std::lock_guard<std::mutex> lk(g_codecMtx);
            g_codecMgr = nullptr;
        }
    }
};

static sptr<ICodecImage> GetCodecManager()
{
    std::lock_guard<std::mutex> lk(g_codecMtx);
    if (g_codecMgr) {
        return g_codecMgr;
    }
    HEIF_HW_LOGI("need to get ICodecImage");
    sptr<IServiceManager> serviceMng = IServiceManager::Get();
    if (serviceMng) {
        serviceMng->RegisterServiceStatusListener(new Listener(), DEVICE_CLASS_DEFAULT);
    }
    g_codecMgr = ICodecImage::Get();
    return g_codecMgr;
}

HeifHardwareEncoder::LifeSpanTimer::LifeSpanTimer(std::string desc) : desc_(desc)
{
    startTimeInUs_ = GetCurrentTimeInUs();
}

HeifHardwareEncoder::LifeSpanTimer::~LifeSpanTimer()
{
    static constexpr float MILLISEC_TO_MICROSEC = 1000.0f;
    int64_t timeSpanInUs = GetCurrentTimeInUs() - startTimeInUs_;
    HEIF_HW_LOGI("%{public}s cost: %{public}.2f ms",
                 desc_.c_str(), static_cast<float>(timeSpanInUs / MILLISEC_TO_MICROSEC));
}

int64_t HeifHardwareEncoder::LifeSpanTimer::GetCurrentTimeInUs()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

HeifHardwareEncoder::HeifHardwareEncoder()
{
    hwEncoder_ = GetCodecManager();
}

HeifHardwareEncoder::~HeifHardwareEncoder()
{
    hwEncoder_ = nullptr;
}

bool HeifHardwareEncoder::IsHardwareEncodeSupported()
{
    if (hwEncoder_ == nullptr) {
        HEIF_HW_LOGE("failed to get hardware encoder!");
        return false;
    }

    std::vector<CodecImageCapability> capList;
    auto ret = hwEncoder_->GetImageCapability(capList);
    if (ret != HDF_SUCCESS) {
        HEIF_HW_LOGE("failed to get image capability, err=%{public}d!", ret);
        return false;
    }

    for (const CodecImageCapability& cap : capList) {
        HEIF_HW_LOGD("cap info: isSoftwareCodec=%{public}d, type=%{public}d",
                     cap.isSoftwareCodec, cap.type);
        if (!cap.isSoftwareCodec && cap.type == CODEC_IMAGE_TYPE_ENCODER) {
            HEIF_HW_LOGD("encoder(%{public}s) selected", cap.name.c_str());
            return true;
        }
    }
    HEIF_HW_LOGE("no available hardware encoder");
    return false;
}

uint32_t HeifHardwareEncoder::Encode(Media::Picture *picture, PlEncodeOptions opts,
                                     SharedBuffer& outputBuffer)
{
    LifeSpanTimer encodeTimer("heif hardware encode");
    if (hwEncoder_ == nullptr) {
    // if (hwEncoder_ == nullptr || bufferMgr_ == nullptr) {
        HEIF_HW_LOGE("failed to get hardware encoder or failed to get buffer manager!");
        return Media::ERR_IMAGE_ENCODE_FAILED;
    }
    if (!IsHardwareEncodeSupported()) {
        return Media::ERR_IMAGE_DATA_UNSUPPORT;
    }

    bool ret = InitEncoder();
    ret = ret && PrepareInputData(picture, opts);
    ret = ret && DoEncode(outputBuffer);
    RecycleAllocatedResource();
    if (ret) {
        HEIF_HW_LOGI("heif hardware encode succeed");
        return Media::SUCCESS;
    }
    return Media::ERR_IMAGE_ENCODE_FAILED;
}

bool HeifHardwareEncoder::PrepareInputData(Media::Picture *picture, PlEncodeOptions opts)
{
    LifeSpanTimer encodeTimer("prepare input data");
    if (picture == nullptr) {
        HEIF_HW_LOGE("failed to get heif picture");
        return false;
    }
    bool ret = AssembleImageItemList(picture, opts);
    ret = ret && AssembleMetaItemTable(picture);
    ret = ret && AssembleItemRefTable(picture);
    return ret;
}

bool HeifHardwareEncoder::AssembleImageItemList(Media::Picture *picture, PlEncodeOptions opts)
{
    if (picture == nullptr) {
        return false;
    }
    AssemblePrimaryImageItem(picture, opts);
    AssembleAuxiliaryImageItem(picture, opts);
    if (inputImgs_.size() == 0) {
        return false;
    }
    return true;
}

void HeifHardwareEncoder::AssemblePrimaryImageItem(Media::Picture *picture, PlEncodeOptions opts)
{
    auto pixelMap = picture->GetMainPixel();
    if (pixelMap == nullptr) {
        return;
    }
    if (pixelMap->GetAllocatorType() != AllocatorType::DMA_ALLOC && pixelMap->GetFd() == nullptr) {
        return;
    }

    ImageItem item;
    item.id = PRIMARY_IMAGE_ITEM_ID;
    auto *surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
    item.pixelBuffer = sptr<NativeBuffer>::MakeSptr(surfaceBuffer->GetBufferHandle());
    item.isPrimary = true;
    item.isHidden = false;
    item.copmressType = IMAGE_ITEM_COMPRESS_TYPE_HEVC;
    item.quality = opts.quality;
    // TODO: liteProperties or sharedProperties
    inputImgs_.emplace_back(item);
    return;
}

void HeifHardwareEncoder::AssembleAuxiliaryImageItem(Media::Picture *picture, PlEncodeOptions opts)
{
        // auto gainPic = picture->GetAuxiliaryPicture(GAIN_MAP);
    // if (gainPic != nullptr) {
    //     AddImageItem(0, false);
    // }
}

bool HeifHardwareEncoder::AssembleMetaItemTable(Media::Picture *picture)
{
    if (picture == nullptr) {
        return false;
    }

    // auto mainPixel = picture->GetMainPixel();
    // if (mainPixel != nullptr) {
    //     inputMetas_.emplace_back(MetaItem {
    //         .itemName = "Main Pixel",
    //         .id = mainPixel->GetUniqueId(),
    //         .data = SharedBuffer {
    //             .fd = mainPixel->GetFd(),
    //             .fillenLen = mainPixel->GetPixelBytes(),
    //             .capacity = mainPixel->GetCapacity()
    //         },
    //         .properties = ""
    //     });
    // }
    
    if (inputMetas_.size() == 0) {
        return false;
    }
    return true;
}

bool HeifHardwareEncoder::AssembleItemRefTable(Media::Picture *picture)
{
    if (picture == nullptr) {
        return false;
    }

    // auto mainPixel = picture->GetMainPixel();
    // if (mainPixel != nullptr) {
    //     refs_.emplace_back(ItemRef {
    //         .type = DIMG,
    //         .auxType = "",
    //         .from = 0,
    //         .to = 0
    //     });
    // }
    
    if (inputMetas_.size() == 0) {
        return false;
    }
    return true;
}

bool HeifHardwareEncoder::InitEncoder()
{
    LifeSpanTimer encodeTimer("init encoder");
    int32_t ret = hwEncoder_->Init(CODEC_IMAGE_HEIF);
    if (ret != HDF_SUCCESS) {
        HEIF_HW_LOGE("failed to init encoder, err=%{public}d", ret);
        return false;
    }
    return true;
}

void HeifHardwareEncoder::RecycleAllocatedResource()
{
    LifeSpanTimer encodeTimer("recycle resource");
    inputImgs_.clear();
    inputMetas_.clear();
    refs_.clear();
    int32_t ret = hwEncoder_->DeInit(CODEC_IMAGE_HEIF);
    if (ret != HDF_SUCCESS) {
        HEIF_HW_LOGE("failed to deinit encoder, err=%{public}d", ret);
    }
}

bool HeifHardwareEncoder::DoEncode(SharedBuffer& outputBuffer)
{
    LifeSpanTimer encodeTimer("do encode");
    int32_t ret = hwEncoder_->DoHeifEncode(inputImgs_, inputMetas_, refs_, outputBuffer);
    if (ret != HDF_SUCCESS) {
        HEIF_HW_LOGE("failed to do encode, err=%{public}d", ret);
        return false;
    }
    return true;
}
} // namespace OHOS::ImagePlugin