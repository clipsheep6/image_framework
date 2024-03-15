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
#ifndef FRAMEWORKS_KITS_JS_COMMON_INCLUDE_PIXELMAP2_IMPL_H
#define FRAMEWORKS_KITS_JS_COMMON_INCLUDE_PIXELMAP2_IMPL_H  
#include <stdint.h>
#include "pixel_map.h"

#ifdef __cplusplus
extern "C" {
#endif

struct OH_Pixelmap {
public:
    OH_Pixelmap(std::shared_ptr<OHOS::Media::PixelMap> pixelMap);
    OH_Pixelmap(const uint32_t *colors, uint32_t colorLength, const OHOS::Media::InitializationOptions &opts);
    OH_Pixelmap(uint64_t surfaceId, OHOS::Media::Rect region);
    OH_Pixelmap(OH_Pixelmap *OH_Pixelmap, const OHOS::Media::InitializationOptions &opts);
    ~OH_Pixelmap();
    bool IsLockPixelMap();
    bool LockPixelMap();
    void UnlockPixelMap();
    // void Release();

    std::shared_ptr<OHOS::Media::PixelMap> GetInnerPixelMap();

private:
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap_;
    int32_t lockCount = 0;
};

#ifdef __cplusplus
};
#endif
/** @} */
#endif // FRAMEWORKS_KITS_JS_COMMON_INCLUDE_PIXELMAP2_IMPL_H