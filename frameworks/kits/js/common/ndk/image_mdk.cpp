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
#include "image_packer.h"
#include "image_packer_napi.h"
#include "image_source_napi.h"

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

MIDK_EXPORT
int32_t OH_Image_CToJs_Array(napi_env env, napi_value tsArrayBuffer, uint8_t* outData, int64_t size)
{
    napi_status status = napi_ok;
    size_t bufferSize = static_cast<size_t>(size);
    status = napi_create_arraybuffer(env, bufferSize, (void**)&outData, &tsArrayBuffer);
    if (status != napi_ok) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_ImagePacker(napi_env env, napi_value tsImagePacker, ImagePackerCapi** imagePackerCapi)
{
    std::unique_ptr<ImagePackerNapi> napi = nullptr;
    napi_status status = napi_unwrap(env, tsImagePacker, reinterpret_cast<void**>(&napi));
    if ((status == napi_ok) && napi != nullptr) {
        ImagePacker* imagePacker = napi.GetNative();
        *imagePackerCapi = new ImagePackerCapi(imagePacker);
        return IMAGE_RESULT_SUCCESS;
    }
    return IMAGE_RESULT_BAD_PARAMETER;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Int64_t(napi_env env, napi_value bufferSize, int64_t* result)
{
    napi_status status = napi_ok;
    status = napi_get_value_int64(env, bufferSize, result);
    if (status != napi_ok) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Int(napi_env env, napi_value fd, int* result)
{
    napi_status status = napi_ok;
    status = napi_get_value_int32(env, fd, result);
    if (status != napi_ok) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

#ifdef __cplusplus
};
#endif
