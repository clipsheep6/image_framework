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

#ifndef INTERFACES_KITS_JS_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_NAPI_H_
#define INTERFACES_KITS_JS_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_NAPI_H_

#include "image_format_convert.h"
#include "image_type.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "pixel_map_napi.h"

namespace OHOS {
namespace Media {
class ImageFormatConvertNapi {
public:
    ImageFormatConvertNapi();

    static napi_value Init(napi_env env, napi_value exports);
    static napi_ref GetConstructor() {
        return sConstructor_; 
    }

private:
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value YUVToRGB(napi_env env, napi_callback_info info);
    static napi_value RGBToYUV(napi_env env, napi_callback_info info);

    static thread_local napi_ref sConstructor_;
    napi_env env_ = nullptr;
};
} // Media
} // HOOS

#endif // INTERFACES_KITS_JS_COMMON_INCLUDE_IMAGE_FORMAT_CONVERT_NAPI_H_