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
#include "surface_buffer.h"
#include "image_type.h"
#include "securec.h"
#include "image_type_converter.h"
#include "include/core/SkData.h"
#include "src/images/SkImageEncoderFns.h"
#include "file_packer_stream.h"

namespace OHOS::ImagePlugin {
using namespace Media;
using namespace OHOS::HDI::Codec::Image::V1_0;
using namespace OHOS::HDI::ServiceManager::V1_0;

static std::mutex g_codecMtx;
static sptr<ICodecImage> g_codecMgr;
const uint32_t PRIMARY_IMAGE_ITEM_ID = 1;
const uint32_t DEFAULT_METADATA_ITEM_ID = 50;
const std::string IMAGE_ITEM_COMPRESS_TYPE_HEVC = "hevc";
const std::string ICC_ASHMEM_TAG = "Heif Hardware Encoder";
const std::map<AuxiliaryPictureType, uint32_t> AUXI_IMAGE_ID = {
    { AuxiliaryPictureType::GAIN_MAP, PRIMARY_IMAGE_ITEM_ID + 1},
    { AuxiliaryPictureType::DEPTH_MAP, PRIMARY_IMAGE_ITEM_ID + 2},
    { AuxiliaryPictureType::UNREFOCUS_MAP, PRIMARY_IMAGE_ITEM_ID + 3},
    { AuxiliaryPictureType::LINEAR_MAP, PRIMARY_IMAGE_ITEM_ID + 4},
    { AuxiliaryPictureType::MARK_CUT_MAP, PRIMARY_IMAGE_ITEM_ID + 5}
};
const size_t DEFAULT_OUTPUT_SIZE = 25 * 1024 * 1024;
const std::map<AuxiliaryPictureType, std::string> AUXI_REF_TYPE = {
    { AuxiliaryPictureType::GAIN_MAP, "gain map"},
    { AuxiliaryPictureType::DEPTH_MAP, "depth map"},
    { AuxiliaryPictureType::UNREFOCUS_MAP, "unrefocus map"},
    { AuxiliaryPictureType::LINEAR_MAP, "linear map"},
    { AuxiliaryPictureType::MARK_CUT_MAP, "mark cut map"}
};

static SkImageInfo ToSkInfo(PixelMap *pixelMap)
{
    ImageInfo info;
    pixelMap->GetImageInfo(info);
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(info.alphaType);
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
#ifdef IMAGE_COLORSPACE_FLAG
    if (pixelMap->InnerGetGrColorSpacePtr() != nullptr) {
        colorSpace = pixelMap->InnerGetGrColorSpacePtr()->ToSkColorSpace();
    }
#endif
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

static size_t GetBufferSize(size_t size)
{
    return sizeof(PropertyType) + sizeof(int) + size;
}

static bool GetAuxType(AuxiliaryPictureType auxType, std::string &type)
{
    auto iter = AUXI_REF_TYPE.find(auxType);
    if (iter != AUXI_REF_TYPE.end()) {
        type = iter->second;
        return true;
    }
    return false;
}

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

uint32_t HeifHardwareEncoder::Encode(Picture *picture, PlEncodeOptions opts,
                                     SharedBuffer& outputBuffer)
{
    LifeSpanTimer encodeTimer("heif hardware encode");
    if (hwEncoder_ == nullptr) {
        HEIF_HW_LOGE("failed to get hardware encoder or failed to get buffer manager!");
        return Media::ERR_IMAGE_ENCODE_FAILED;
    }
    if (!IsHardwareEncodeSupported()) {
        return Media::ERR_IMAGE_DATA_UNSUPPORT;
    }

    bool ret = InitEncoder();
    ret = ret && PrepareInputData(picture, opts);
    ret = ret && DoEncode(outputBuffer);
    if (ret) {
        HEIF_HW_LOGI("heif hardware encode succeed");
        return Media::SUCCESS;
    }
    return Media::ERR_IMAGE_ENCODE_FAILED;
}

bool HeifHardwareEncoder::PrepareInputData(Picture *picture, PlEncodeOptions opts)
{
    LifeSpanTimer encodeTimer("prepare input data");
    if (picture == nullptr) {
        HEIF_HW_LOGE("failed to get heif picture");
        return false;
    }
    bool ret = AssembleImageItem(picture, opts);
    ret = ret && AssembleMetaItem(picture);
    return ret;
}

bool HeifHardwareEncoder::AssembleImageItem(Picture *picture, PlEncodeOptions opts)
{
    if (picture == nullptr) {
        return false;
    }
    bool ret = AssemblePrimaryImageItem(picture, opts);
    if (ret) {
        ret = AssembleAuxiliaryImageItem(picture, opts);
    }
    if (!ret || inputImgs_.size() == 0) {
        return false;
    }
    return true;
}

bool HeifHardwareEncoder::AssemblePrimaryImageItem(Picture *picture, PlEncodeOptions opts)
{
    auto pixelMap = picture->GetMainPixel();
    if (pixelMap == nullptr) {
        return false;
    }
    if (pixelMap->GetAllocatorType() != AllocatorType::DMA_ALLOC && pixelMap->GetFd() == nullptr) {
        return false;
    }

    ImageItem item;
    item.id = PRIMARY_IMAGE_ITEM_ID;
    auto *surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
    item.pixelBuffer = sptr<NativeBuffer>::MakeSptr(surfaceBuffer->GetBufferHandle());
    item.isPrimary = true;
    item.isHidden = false;
    item.copmressType = IMAGE_ITEM_COMPRESS_TYPE_HEVC;
    item.quality = opts.quality;
    bool ret = AssembleImagePropertiesSharedBuffer(pixelMap, item.sharedProperties);
    if (!ret) {
        return false;
    }
    inputImgs_.emplace_back(item);
    return true;
}

bool HeifHardwareEncoder::AssembleAuxiliaryImageItem(Picture *picture, PlEncodeOptions opts)
{
    for (const auto& it : AUXI_IMAGE_ID) {
        auto auxiPicture = picture->GetAuxiliaryPicture(it.first);
        if (auxiPicture == nullptr) {
            continue;
        }
        auto pixelMap = auxiPicture->GetContentPixel();
        if (pixelMap) {
            continue;
        }
        if (pixelMap->GetAllocatorType() != AllocatorType::DMA_ALLOC && pixelMap->GetFd() == nullptr) {
            continue;
        }
        ImageItem item;
        item.id = it.second;
        auto *surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
        item.pixelBuffer = sptr<NativeBuffer>::MakeSptr(surfaceBuffer->GetBufferHandle());
        item.isPrimary = false;
        item.isHidden = true;
        item.copmressType = IMAGE_ITEM_COMPRESS_TYPE_HEVC;
        item.quality = opts.quality;
        bool ret = AssembleImagePropertiesSharedBuffer(pixelMap, item.sharedProperties);
        if (!ret) {
            return false;
        }
        inputImgs_.emplace_back(item);
        if (it.first == AuxiliaryPictureType::GAIN_MAP) {
            ret = AssembleImageRef(DIMG, it.first, it.second);
        } else {
            ret = AssembleImageRef(AUXL, it.first, it.second);
        }
        
        if (!ret) {
            return false;
        }
    }
    return true;
}

bool HeifHardwareEncoder::AssembleImageRef(ReferenceType refType, AuxiliaryPictureType type,
                                           unsigned int auxiId)
{
    std::string type_str;
    if (!GetAuxType(type, type_str)) {
        return false;
    }
    ItemRef ref;
    ref.type = refType;
    ref.auxType = type_str;
    ref.from = PRIMARY_IMAGE_ITEM_ID;
    ref.to.emplace_back(auxiId);
    refs_.emplace_back(ref);
    return true;
}

bool HeifHardwareEncoder::AssembleImagePropertiesSharedBuffer(std::shared_ptr<PixelMap> pixelMap,
                                                              SharedBuffer& outputBuffer)
{
    if (pixelMap == nullptr) {
        return false;
    }
    sk_sp<SkData> iccProfile = icc_from_color_space(ToSkInfo(pixelMap.get()));
    std::shared_ptr<AbsMemory> propertiesMem = AllocateShareMem(GetBufferSize(iccProfile->size()), ICC_ASHMEM_TAG);
    if (propertiesMem == nullptr) {
        return false;
    }
    releaseMems_.emplace_back(propertiesMem);
    bool ret = FillImagePropertiesItem(propertiesMem, 0, PropertyType::ICC_PROFILE,
                                       iccProfile->data(), iccProfile->size());
    if (ret) {
        outputBuffer.fd = *static_cast<int *>(propertiesMem->extend.data);
        outputBuffer.fillenLen = propertiesMem->data.size;
        outputBuffer.capacity = propertiesMem->data.size;
    }
    return ret;
}

std::shared_ptr<AbsMemory> HeifHardwareEncoder::AllocateShareMem(size_t size, std::string tag)
{
    MemoryData data;
    data.size = size;
    data.tag = tag.empty() ? ICC_ASHMEM_TAG.c_str() : tag.c_str();
    std::unique_ptr<AbsMemory> memory = MemoryManager::CreateMemory(AllocatorType::SHARE_MEM_ALLOC, data);
    std::shared_ptr<AbsMemory> res = std::move(memory);
    return res;
}

bool HeifHardwareEncoder::FillImagePropertiesItem(const std::shared_ptr<AbsMemory> &memory, const size_t offset,
                                                  PropertyType type, const void *payloadData, size_t payloadSize)
{
    uint8_t *memData = reinterpret_cast<uint8_t *>(memory->data.data);
    size_t memSize = memory->data.size;
    bool res = memcpy_s(memData + offset, memSize - offset, &type, sizeof(type)) == EOK &&
        memcpy_s(memData + offset + sizeof(type), memSize - offset - sizeof(type),
                 &payloadSize, sizeof(payloadSize)) == EOK &&
        memcpy_s(memData + offset + sizeof(type) + sizeof(payloadSize),
                 memSize - offset - sizeof(type) - sizeof(payloadSize), payloadData, payloadSize) == EOK;
    return res;
}

bool HeifHardwareEncoder::AssembleOuputSharedBuffer(SharedBuffer& outputBuffer,
                                                    std::shared_ptr<AbsMemory> &outputMem,
                                                    OutputDataStream* outputStream)
{
    if (outputStream == nullptr) {
        return false;
    }
    OutputStreamType outType = outputStream->GetType();
    if (outType == OutputStreamType::FILE_PACKER) {
        auto fileOutput = reinterpret_cast<FilePackerStream *>(outputStream);
        outputBuffer.fd = fileOutput->GetFd();
        return true;
    }
    size_t capacitySize = DEFAULT_OUTPUT_SIZE;
    outputStream->GetCapicity(capacitySize);
    std::shared_ptr<AbsMemory> mem = AllocateShareMem(capacitySize, ICC_ASHMEM_TAG);
    if (mem == nullptr) {
        return false;
    }
    releaseMems_.emplace_back(mem);
    outputMem = mem;
    outputBuffer.fd = *static_cast<int *>(mem->extend.data);
    outputBuffer.capacity = capacitySize;
    return true;
}

bool HeifHardwareEncoder::AssembleMetaItem(Picture *picture)
{
    if (picture == nullptr) {
        return false;
    }

    uint32_t metaId = DEFAULT_METADATA_ITEM_ID;
    bool ret = AssemblePrimaryImageMeta(picture, metaId);
    if (ret) {
        ret = AssembleAuxiliaryImageMeta(picture, metaId);
    }    
    if (!ret || inputMetas_.size() == 0) {
        return false;
    }
    return true;
}

bool HeifHardwareEncoder::AssemblePrimaryImageMeta(Picture *picture, uint32_t &metaId)
{
    std::shared_ptr<PixelMap> pixelMap = picture->GetMainPixel();
    if (pixelMap == nullptr) {
        return false;
    }
    std::shared_ptr<ExifMetadata> meta = pixelMap->GetExifMetadata();
    if (meta == nullptr) {
        return false;
    }
    MetaItem item;
    item.id = metaId;
    // TODO: 解析Meta properties. 
    // @List: {ColourInfo, ContentLightLevel, MasteringDisplayColourVolume, ToneMapChannel, ToneMapMetadata}
    // 以ColourInfo举例
    // 1. 从Metadata中获取ColourInfo的值
    // 2. 计算MetaItem property size, sizeof(PropertyType) + sizeof(ColourInfo)
    // 3. 申请MetaItem property内存
    // 4. 备份MetaItem property内存
    // 5. 创建ColourInfo的对象, 并赋值
    // 6. 拷贝ColourInfo数据到3中申请的内存中
    // 7. 将property内容添加到MetaItem.properties中
    // Code Demo:
    // auto info = meta->GetColourInfo();
    // size_t propertySize = sizeof(PropertyType) + sizeof(ColourInfo);
    // std::shared_ptr<AbsMemory> metaMem = AllocateShareMem(propertySize, ICC_ASHMEM_TAG);
    // if (metaMem == nullptr) {
    //     return false;
    // }
    // releaseMems_.emplace_back(metaMem);
    // std::shared_ptr<ColourInfo> colorInfo;
    // colorInfo->colourPrimaries = info.primaries;
    // colorInfo->transferCharacteristics = info.characteristics;
    // colorInfo->matrixCoefficients = info.coefficients;
    // colorInfo->fullRangeFlag = true;
    // bool ret = FillMetaPropertiesItem(metaMem, 0, PropertyType::COLOR_INFO, colorInfo.get(), sizeof(ColourInfo));
    // if (!ret) {
    //     return false;
    // }
    // item.properties.emplace_back(*metaMem->data.data);
    inputMetas_.emplace_back(item);
    bool ret = AssembleMetaRef(CDSC, metaId, PRIMARY_IMAGE_ITEM_ID);
    return ret;
}

bool HeifHardwareEncoder::AssembleAuxiliaryImageMeta(Picture *picture, uint32_t &metaId)
{
    for (const auto& it : AUXI_IMAGE_ID) {
        std::shared_ptr<AuxiliaryPicture> auxPic = picture->GetAuxiliaryPicture(it.first);
        if (auxPic == nullptr) {
            continue;
        }
        auto exifMeta = auxPic->GetMetadata(MetadataType::EXIF);
        if (exifMeta != nullptr) {
            // TODO: AssemblePrimaryImageMeta
            metaId += 1;
            bool ret = AssembleMetaRef(CDSC, metaId, it.second);
            if (!ret) {
                return false;
            }
        }
        auto markcutMeta = auxPic->GetMetadata(MetadataType::MARK_CUT);
        if (markcutMeta != nullptr) {
            // TODO: AssemblePrimaryImageMeta
            metaId += 1;
            bool ret = AssembleMetaRef(CDSC, metaId, it.second);
            if (!ret) {
                return false;
            }
        }
    }
    return true;
}

bool HeifHardwareEncoder::FillMetaPropertiesItem(const std::shared_ptr<AbsMemory> &memory, const size_t offset,
                                                  PropertyType type, const void *payloadData, size_t payloadSize)
{
    uint8_t *memData = reinterpret_cast<uint8_t *>(memory->data.data);
    size_t memSize = memory->data.size;
    bool res = memcpy_s(memData + offset, memSize - offset, &type, sizeof(type)) == EOK &&
        memcpy_s(memData + offset + sizeof(type),
                 memSize - offset - sizeof(type), payloadData, payloadSize) == EOK;
    return res;
}

bool HeifHardwareEncoder::AssembleMetaRef(ReferenceType refType,
                                          uint32_t metaId, uint32_t imageId)
{
    ItemRef ref;
    ref.type = refType;
    ref.from = metaId;
    ref.to.emplace_back(imageId);
    refs_.emplace_back(ref);
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
    for (std::shared_ptr<AbsMemory> &memory : releaseMems_) {
        if (memory == nullptr) {
            continue;
        }
        memory->Release();
    }
    releaseMems_.clear();
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