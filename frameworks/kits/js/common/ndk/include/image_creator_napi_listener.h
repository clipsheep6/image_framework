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

#ifndef FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_CREATOR_NAPI_LISTENER_H_
#define FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_CREATOR_NAPI_LISTENER_H_

#include "image_creator.h"
#include "image_creator_mdk.h"

namespace OHOS {
namespace Media {
class ImageCreatorNapiListener : public SurfaceBufferReleaseListener {
public:
    ~ImageCreatorNapiListener() override
    {
        callBack = nullptr;
    }
    void OnSurfaceBufferRelease() __attribute__((no_sanitize("cfi"))) override
    {
        if (callBack != nullptr) {
            callBack();
        }
    }
    OH_Image_Creator_On_Callback callBack = nullptr;
};
} // namespace Media
} // namespace OHOS
#endif // FRAMEWORKS_KITS_JS_COMMON_INCLUDE_IMAGE_CREATOR_NAPI_LISTENER_H_
