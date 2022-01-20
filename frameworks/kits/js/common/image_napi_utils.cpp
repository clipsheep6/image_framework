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

#include "image_napi_utils.h"
#include <securec.h>

namespace OHOS {
namespace Media {
const size_t NUM0 = 0;
const size_t NUM1 = 1;
bool ImageNapiUtils::GetBufferByName(napi_env env, napi_value root, const char* name, void **res, size_t* len)
{
    napi_value tempValue = nullptr;
    
    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_named_property(env, root, name, &tempValue)), false);

    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_arraybuffer_info(env, tempValue, res, len)), false);

    return true;
}

bool ImageNapiUtils::GetUint32ByName(napi_env env, napi_value root, const char* name, uint32_t *res)
{
    napi_value tempValue = nullptr;
    
    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_named_property(env, root, name, &tempValue)), false);

    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_value_uint32(env, tempValue, res)), false);

    return true;
}

bool ImageNapiUtils::GetInt32ByName(napi_env env, napi_value root, const char* name, int32_t *res)
{
    napi_value tempValue = nullptr;
    
    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_named_property(env, root, name, &tempValue)), false);

    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_value_int32(env, tempValue, res)), false);

    return true;
}

bool ImageNapiUtils::GetBoolByName(napi_env env, napi_value root, const char* name, bool *res)
{
    napi_value tempValue = nullptr;
    
    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_named_property(env, root, name, &tempValue)), false);

    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_value_bool(env, tempValue, res)), false);

    return true;
}

bool ImageNapiUtils::GetNodeByName(napi_env env, napi_value root, const char* name, napi_value *res)
{
    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_named_property(env, root, name, res)), false);

    return true;
}

bool ImageNapiUtils::GetUtf8String(napi_env env, napi_value root, std::string &res)
{
    size_t bufferSize = NUM0;
    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_value_string_utf8(env, root, nullptr,
        NUM0, &bufferSize)) && bufferSize > NUM0, false);

    size_t resultSize = NUM0;
    bufferSize = bufferSize + NUM1;
    std::vector<char> buffer(bufferSize);
    IMG_NAPI_CHECK_RET(IMG_IS_OK(napi_get_value_string_utf8(env, root, &(buffer[NUM0]),
        bufferSize, &resultSize)) && resultSize > NUM0, false);
    res.assign(buffer.begin(), buffer.end());
    return true;
}

bool ImageNapiUtils::CreateArrayBuffer(napi_env env, void* src, size_t srcLen, napi_value *res)
{
    if (src == nullptr || srcLen == 0) {
        return false;
    }

    void *nativePtr = nullptr;
    if (napi_create_arraybuffer(env, srcLen, &nativePtr, res) != napi_ok || nativePtr == nullptr) {
        return false;
    }

    if (memcpy_s(nativePtr, srcLen, src, srcLen) != 0) {
        return false;
    }
    return true;
}

napi_valuetype ImageNapiUtils::getType(napi_env env, napi_value root)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, root, &res);
    return res;
}

}  // namespace Media
}  // namespace OHOS