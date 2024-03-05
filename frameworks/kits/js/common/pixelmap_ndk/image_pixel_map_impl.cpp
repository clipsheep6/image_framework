/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "image_pixel_map_impl.h"
#include "pixel_map_from_surface.h"

using namespace OHOS;
using namespace Media;

#ifdef __cplusplus
extern "C" {
#endif

PixelMapCapi::PixelMapCapi(std::shared_ptr<PixelMap> pixelMap)
{
    if (pixelMap == nullptr) {
        pixelMap_ = nullptr;
        return;
    }
    pixelMap_ = pixelMap;
}

PixelMapCapi::PixelMapCapi(const uint32_t *colors, uint32_t colorLength, const InitializationOptions &opts)
{
    auto tmpPixelmap = PixelMap::Create(colors, colorLength, opts);
    pixelMap_ = std::move(tmpPixelmap);
}

PixelMapCapi::PixelMapCapi(uint64_t surfaceId, OHOS::Media::Rect region)
{
    auto tmpPixelmap = CreatePixelMapFromSurfaceId(surfaceId, region);
    pixelMap_ = std::move(tmpPixelmap);
}

PixelMapCapi::PixelMapCapi(PixelMapCapi *pixelMapCapi, const InitializationOptions &opts)
{
    if (pixelMapCapi == nullptr) {
        pixelMap_ = nullptr;
        return;
    }
    auto pixelMapPtr = pixelMapCapi->GetPixelMapInstances().get();
    auto tmpPixelmap = PixelMap::Create(*pixelMapPtr, opts);
    pixelMap_ = std::move(tmpPixelmap);
}

std::shared_ptr<OHOS::Media::PixelMap> PixelMapCapi::GetPixelMapInstances()
{
    return pixelMap_;
}

bool PixelMapCapi::IsLockPixelMap()
{
    return (lockCount > 0);
}

bool PixelMapCapi::LockPixelMap()
{
    lockCount++;
    return true;
}

void PixelMapCapi::UnlockPixelMap()
{
    if (lockCount > 0) {
        lockCount--;
    }
}

void PixelMapCapi::Release(){
    if (pixelMap_) {
        pixelMap_ = nullptr;
    }
}

#ifdef __cplusplus
};
#endif