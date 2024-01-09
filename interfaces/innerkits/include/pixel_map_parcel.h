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

#ifndef INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_PARCEL_H
#define INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_PARCEL_H

#include "message_parcel.h"
#include "pixel_map.h"

namespace OHOS {
namespace Media {
class PixelMapParcel {
public:
    static std::unique_ptr<PixelMap> CreateFromParcel(OHOS::MessageParcel& data);
    static bool WriteAllocatorTypeToParcel(PixelMap* pixelMap, OHOS::MessageParcel& data);
    static bool WriteToParcel(PixelMap* pixelMap, OHOS::MessageParcel& data);

private:
    static void ReleaseMemory(AllocatorType allocType, void *addr, void *context, uint32_t size);
};
}  // namespace Media
}  // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_PIXEL_MAP_PARCEL_H
