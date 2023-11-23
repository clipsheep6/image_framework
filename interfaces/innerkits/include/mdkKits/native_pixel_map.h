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

#ifndef NATIVE_PIXEL_MAP_H_
#define NATIVE_PIXEL_MAP_H_

#include "pixel_map_napi.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NativePixelMap_ {
    OHOS::Media::PixelMapNapi* napi = nullptr;
};

#ifdef __cplusplus
};
#endif

std::shared_ptr<OHOS::Media::PixelMap> OH_PixelMapNative_GetPixelMap(struct NativePixelMap_ *nativePixelMap)
{
    return nativePixelMap == nullptr ? nullptr :
	nativePixelMap->napi == nullptr ? nullptr :
	    nativePixelMap->napi->GetPixelNapiInner();
}
#endif // NATIVE_PIXEL_MAP_H_
