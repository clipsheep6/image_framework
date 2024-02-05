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

#include "image_mdk.h"

#include "common_utils.h"
#include "image_mdk_kits.h"

using namespace OHOS::Media;
#ifdef __cplusplus
extern "C" {
#endif
struct ImageNative_ {
    ImageNapi* napi = nullptr;
};

MIDK_EXPORT
ImageNative* OH_Image_InitImageNative(napi_env env, napi_value source)
{
    ImageNapi* napi = ImageNapi_Unwrap(env, source);
    if (napi == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ImageNative> result = std::make_unique<ImageNative>();
    result->napi = napi;
    return result.release();
}

MIDK_EXPORT
int32_t OH_Image_ClipRect(const ImageNative* native, struct OhosImageRect* rect)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageNapiArgs args;
    args.outRect = rect;
    auto res = ImageNapiNativeCtxCall(CTX_FUNC_IMAGE_CLIP_RECT, native->napi, &args);
    return res;
}

MIDK_EXPORT
int32_t OH_Image_Size(const ImageNative* native, struct OhosImageSize* size)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_JNI_ENV_ABNORMAL;
    }
    ImageNapiArgs args;
    args.outSize = size;
    auto res = ImageNapiNativeCtxCall(CTX_FUNC_IMAGE_SIZE, native->napi, &args);
    return res;
}

MIDK_EXPORT
int32_t OH_Image_Format(const ImageNative* native, int32_t* format)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_JNI_ENV_ABNORMAL;
    }
    ImageNapiArgs args;
    args.outNum0 = format;
    auto res = ImageNapiNativeCtxCall(CTX_FUNC_IMAGE_FORMAT, native->napi, &args);
    return res;
}

MIDK_EXPORT
int32_t OH_Image_GetComponent(const ImageNative* native, int32_t componentType,
    struct OhosImageComponent* componentNative)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_JNI_ENV_ABNORMAL;
    }
    ImageNapiArgs args;
    args.inNum0 = componentType;
    args.outComponent = componentNative;
    auto res = ImageNapiNativeCtxCall(CTX_FUNC_IMAGE_GET_COMPONENT, native->napi, &args);
    return res;
}

MIDK_EXPORT
int32_t OH_Image_Release(ImageNative* native)
{
    if (native != nullptr) {
        delete native;
    }
    return IMAGE_RESULT_SUCCESS;
}

/* Convert Function */
MIDK_EXPORT
int32_t OH_Image_JsToC_Int32(napi_env env, napi_value jsValue, int32_t* out) 
{
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_get_value_int32(env, jsValue, out);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Uint32(napi_env env, napi_value jsValue, uint32_t* out) 
{
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_get_value_uint32(env, jsValue, out);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Int64(napi_env env, napi_value jsValue, int64_t *out)
{
    napi_status status;
    int64_t cValue;
    bool lossless;
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_get_value_bigint_int64(env, jsValue, &cValue, &lossless);
    if (status != napi_ok){
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = cValue;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Uint64(napi_env env, napi_value jsValue, uint64_t *out)
{
    napi_status status;
    uint64_t cValue;
    bool lossless;
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_get_value_bigint_uint64(env, jsValue, &cValue, &lossless);
    if (status != napi_ok){
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = cValue;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Bool(napi_env env, napi_value jsValue, bool *out)
{
    napi_status status;
    bool cValue;
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_get_value_bool(env, jsValue, &cValue);
    if (status != napi_ok){
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = static_cast<bool>(cValue);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_CToJs_NumberInt(napi_env env, int32_t in, napi_value jsValue) 
{
    if (jsValue == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_create_int32(env, in, &jsValue);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_CharStr_Utf8(napi_env env, napi_value jsValue, char* out)
{
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    std::string strValue = "";
    size_t bufLength = 0;
    napi_status status = napi_get_value_string_utf8(env, jsValue, nullptr, 0, &bufLength);
    if (status == napi_ok && bufLength > 0 && bufLength < PATH_MAX) { //PATH_MAX 1024
        char *buffer = reinterpret_cast<char *>(malloc((bufLength + 1) * sizeof(char)));
        if (buffer == nullptr) {
            return IMAGE_RESULT_ERR_SHAMEM_NOT_EXIST;
        }
        status = napi_get_value_string_utf8(env, jsValue, buffer, bufLength + 1, &bufLength);
        if (status == napi_ok) {
            strValue.assign(buffer, 0, bufLength + 1);
	        out = (char*)malloc((strValue.length()) * sizeof(char));
	        strValue.copy(out, strValue.length(), 0);//把当前串中以0开始的s.length()个字符拷贝到以ch为起始位置的字符数组中，返回实际拷贝的数目
        }
        if (buffer != nullptr) {
            free(buffer);
            buffer = nullptr;
        }
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_CToJs_String_Utf8(napi_env env, std::string in, napi_value jsValue)
{
    if (jsValue == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_create_string_utf8(env, in.c_str(), in.length(), &jsValue);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_BufferUint8(napi_env env, napi_value jsValue, uint8_t* outUint, size_t* outByteLength)
{
    if (jsValue == nullptr || outUint == nullptr || outByteLength == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_get_arraybuffer_info(env, jsValue, (void **)&outUint, outByteLength);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_ImageSource(napi_env env, napi_value jsValue, void* out)
{
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_unwrap(env, jsValue, &out);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS; 
}

void Finalizer(napi_env env, void *nativeObject, void *finalize){}

MIDK_EXPORT
int32_t OH_Image_CToJs_ObjImageSource(napi_env env, napi_value jsValue, void* in)
{
    if (jsValue == nullptr || in == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_create_object(env, &jsValue);
    napi_status status = napi_wrap(env, jsValue, in, Finalizer, nullptr, nullptr);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS; 
}

MIDK_EXPORT
int32_t OH_Image_JsToC_PixelMap(napi_env env, napi_value jsValue, void* out)
{
    if (jsValue == nullptr || out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_unwrap(env, jsValue, &out);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS; 
}


MIDK_EXPORT
int32_t OH_Image_CToJs_ObjPixelMap(napi_env env, napi_value jsValue, void* in)
{
    if (jsValue == nullptr || in == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_create_object(env, &jsValue);
    napi_status status = napi_wrap(env, jsValue, in, Finalizer, nullptr, nullptr); 
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS; 
}

MIDK_EXPORT
int32_t OH_Image_CToJs_ObjPixelMapList(napi_env env, napi_value jsValue, void* in[], uint32_t sizeLen)
{
    if (jsValue == nullptr || in == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    int index = 0;
    napi_status status;
    napi_create_array(env, &jsValue);
    for (;index < sizeLen;) {
        napi_value object = nullptr;
        napi_create_object(env, &object);
        status = napi_wrap(env, object, in[index], Finalizer, nullptr, nullptr);  
        if (status != napi_ok) {
            return IMAGE_RESULT_IMAGE_RESULT_BASE;
        }
        status = napi_set_element(env, jsValue, index, object);
        if (status != napi_ok) {
            return IMAGE_RESULT_IMAGE_RESULT_BASE;
        }
        ++ index;
    }
    return IMAGE_RESULT_SUCCESS; 
}
/* Convert Function */

#ifdef __cplusplus
};
#endif
