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

#include "image_test_sendable_napi.h"
#include "image_log.h"
#include "media_errors.h"
#include "image_format.h"
#include "image_napi_utils.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ImageTestSendableNapi"

namespace OHOS {
namespace Media {

    napi_value ImageTestSendableNapi::CreateImageSource(napi_env env, napi_callback_info info) {
        IMAGE_ERR("ImageTestSendableNapi::CreateImageSourc");
        napi_value result = nullptr;
        napi_get_undefined(env, &result);
        return result;
    }
    
    napi_value ImageTestSendableNapi::Init(napi_env env, napi_value exports){
        IMAGE_ERR("ImageTestSendableNapi::Init");
        napi_property_descriptor static_prop[] = {
            DECLARE_NAPI_STATIC_FUNCTION("createImageSource", CreateImageSource)
        };

        napi_status status = napi_define_properties(env, exports, sizeof(static_prop) / sizeof(static_prop[0]), static_prop);
        if(status != napi_ok) {
            IMAGE_ERR("ImageTestSendableNapi define property fail");
            return nullptr;
        }
        IMAGE_ERR("ImageTestSendableNapi::End");
        return exports;
    }

}
}
