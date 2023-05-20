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

#ifndef INTERFACES_INNERKITS_INCLUDE_PURGEABLE_PIXEL_MAP_BUILDER_H_
#define INTERFACES_INNERKITS_INCLUDE_PURGEABLE_PIXEL_MAP_BUILDER_H_

#include "hitrace_meter.h"
#include "image_source.h"
#include "image_type.h"
#include "pixel_map.h"

#ifdef IMAGE_PURGEABLE_PIXELMAP
#include "purgeable_ashmem.h"
#include "purgeable_mem_builder.h"
#include "purgeable_resource_manager.h"

#ifndef _WIN32
#include "securec.h"
#else
#include "memory.h"
#endif

#endif // IMAGE_PURGEABLE_PIXELMAP

namespace OHOS {
namespace Media {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001799, "PurgeablePixelMapBuilder" };

class PurgeablePixelMapBuilder : public PurgeableMem::PurgeableMemBuilder {
public:
    PurgeablePixelMapBuilder(uint32_t index, std::unique_ptr<ImageSource> &imageSource,
        DecodeOptions opts, PixelMap *pixelMap)
        : index_(index), opts_(opts), pixelMap_(pixelMap), imageSource_(move(imageSource)) {}

    bool Build(void *data, size_t size)
    {
        HiviewDFX::HiLog::Debug(LABEL, "purgeableMem build in.");
        uint32_t errorCode;
        if (imageSource_ == nullptr) {
            return false;
        }

        StartTraceArgs(HITRACE_TAG_ZIMAGE, "PixelMapPurgeableMemBuilder::Build:%d",
            imageSource_->GetPurgeableMemResourceFd());
        std::unique_ptr<PixelMap> pixelMap = imageSource_->CreatePixelMap(index_, opts_, errorCode);
        if (pixelMap == nullptr || pixelMap_ == nullptr) {
            return false;
        }

        StartTrace(HITRACE_TAG_ZIMAGE, ("PixelMapPurgeableMemBuilder::memcpy_s:" + std::to_string(size)));
        memcpy_s((char *)pixelMap_->GetPixels(), size, (char *)pixelMap->GetPixels(), size);
        FinishTrace(HITRACE_TAG_ZIMAGE); // memcpy_s trace
        FinishTrace(HITRACE_TAG_ZIMAGE); // PixelMapPurgeableMemBuilder::Build trace

        return true;
    }

    size_t GetMetaDataSize()
    {
        return imageSource_ ? imageSource_->GetSourceSize() : 0;
    }

    ~PurgeablePixelMapBuilder() {}

private:
    uint32_t index_;
    DecodeOptions opts_;
    PixelMap *pixelMap_;
    std::unique_ptr<ImageSource> imageSource_;
}; // class PurgeablePixelMapBuilder

bool MakePixelMapToBePurgeable(std::unique_ptr<PixelMap> &pixelMap, std::unique_ptr<ImageSource> &backupImgSrc4Rebuild,
    DecodeOptions &decodeOpts)
{
    HiviewDFX::HiLog::Debug(LABEL, "MakePixelMapToBePurgeable in.");
    if (pixelMap == nullptr || backupImgSrc4Rebuild == nullptr) {
        HiviewDFX::HiLog::Error(LABEL, "PixelMap or backupImgSrc4Rebuild is null.");
        return false;
    }

    // check pixelMap purgeable status
    if (pixelMap->IsPurgeable()) {
        HiviewDFX::HiLog::Error(LABEL, "PixelMap is already purgeable.");
        return false;
    }

    // 2048 * 1536 and 256 * 256 need manage by resource manager
    Size size = decodeOpts.desiredSize;
    if (size.height == 2048 && size.width == 1536) {
        pixelMap->SetIsNeedAddResourceManager(true);
    }

    if (size.height == 256 && size.width == 256) {
        pixelMap->SetIsNeedAddResourceManager(true);
    }

    // save ImageSource to builder
    if (pixelMap->GetIsNeedAddResourceManager()) {
        std::unique_ptr<PurgeableMem::PurgeableMemBuilder> purgeableMemBuilder =
            std::make_unique<PurgeablePixelMapBuilder>(0, backupImgSrc4Rebuild, decodeOpts, pixelMap.get());
        pixelMap->SetBuilderToBePurgeable(purgeableMemBuilder);

        if (pixelMap->IsPurgeable()) {
            HiviewDFX::HiLog::Debug(LABEL, "Set pixelMap purgeable success, add to purgeableResourceManager.");
            PurgeableMem::PurgeableResourceManager::GetInstance().AddResource(pixelMap.get());
        }
    }

    return true;
}
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_PURGEABLE_PIXEL_MAP_BUILDER_H_