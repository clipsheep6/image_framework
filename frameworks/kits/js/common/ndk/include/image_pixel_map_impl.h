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
#ifndef INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PIXEL_MAP_IMP_H
#define INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PIXEL_MAP_IMP_H
#include <stdint.h>
#include "image_mdk.h"
#include "image_mdk_common.h"
#include "pixel_map.h"
#include "message_parcel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct PixelMapCapi {
public:
    PixelMapCapi(std::shared_ptr<OHOS::Media::PixelMap> pixelMap);
    PixelMapCapi(const uint32_t *colors, uint32_t colorLength, const OHOS::Media::InitializationOptions &opts);
    PixelMapCapi(uint64_t surfaceId, OHOS::Media::Rect region);
    PixelMapCapi(PixelMapCapi *pixelMapCapi, const OHOS::Media::InitializationOptions &opts);
    ~PixelMapCapi();
    bool IsLockPixelMap();
    bool LockPixelMap();
    void UnlockPixelMap();

    void Release();

    std::shared_ptr<OHOS::Media::PixelMap> GetPixelMapInstances();

private:
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap_;
    int32_t lockCount = 0;
};

#ifdef __cplusplus
};
#endif
/** @} */
#endif // INTERFACES_KITS_NATIVE_INCLUDE_IMAGE_PIXEL_MAP_IMP_H