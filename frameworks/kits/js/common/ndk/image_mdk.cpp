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
#include "js_native_api.h"
#include "napi/native_node_api.h"
#include "napi/native_api.h"
#include "pixel_map_napi.h"

#include "image_napi_utils.h"

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
int32_t OH_Image_CToJs_PixelMap(napi_env env, PixelMapCapi *pixelMap, napi_value jsPixelMap)
{
    napi_status status;
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    auto pixelMapType = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    status = napi_wrap(env, jsPixelMap, pixelMapType, nullptr, nullptr, nullptr);
    if (status != napi_ok) {
        jsPixelMap = nullptr;
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_CToJs_ColorSpace(napi_env env, ColorSpaceCapi *colorSpace, napi_value jsColorSpace)
{
    if (colorSpace == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
#ifdef IMAGE_COLORSPACE_FLAG
    napi_status status;
    auto colorSpaceType = static_cast<OHOS::ColorManager::ColorSpace *>(colorSpace);
    status = napi_wrap(env, jsColorSpace, colorSpaceType, nullptr, nullptr, nullptr);
    if (status != napi_ok) {
        jsColorSpace = nullptr;
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    return IMAGE_RESULT_SUCCESS;
#else
    return IMAGE_RESULT_MEDIA_INVALID_OPERATION;
#endif
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Uint8(napi_env env, napi_value jsValue, uint8_t *out)
{
    napi_status status;
    uint32_t cValue;
    if (out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_get_value_uint32(env, jsValue, &cValue);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = static_cast<uint8_t>(cValue);

    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Uint64(napi_env env, napi_value jsValue, uint64_t *out)
{
    napi_status status;
    uint64_t cValue;
    bool lossless;
    if (out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_get_value_bigint_uint64(env, jsValue, &cValue, &lossless);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = cValue;

    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Float(napi_env env, napi_value jsValue, float *out)
{
    napi_status status;
    double cValue;
    if (out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_get_value_double(env, jsValue, &cValue);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = static_cast<float>(cValue);

    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Bool(napi_env env, napi_value jsValue, bool *out)
{
    napi_status status;
    bool cValue;
    if (out == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_get_value_bool(env, jsValue, &cValue);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = static_cast<bool>(cValue);

    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_Parcel(napi_env env, napi_value jsParcel, ParcelCapi *parcel)
{
    napi_status status;
    napi_valuetype valueType;
    if (parcel == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto parcelType = static_cast<OHOS::Parcel *>(parcel);
    status = napi_typeof(env, jsParcel, &valueType);
    if (valueType != napi_object) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    status = napi_unwrap(env, jsParcel, reinterpret_cast<void**>(&parcelType));
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_ColorSpace(napi_env env, napi_value jsColorSpace, ColorSpaceCapi *colorSpace)
{
    if (colorSpace == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
#ifdef IMAGE_COLORSPACE_FLAG
    napi_status status;
    napi_valuetype valueType;
    auto colorSpaceType = static_cast<OHOS::ColorManager::ColorSpace *>(colorSpace);
    status = napi_typeof(env, jsColorSpace, &valueType);
    if (valueType != napi_object) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    status = napi_unwrap(env, jsColorSpace, reinterpret_cast<void**>(&colorSpaceType));
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS;
#else
    return IMAGE_RESULT_MEDIA_INVALID_OPERATION;
#endif
}

int32_t OH_Image_JsToC_PixelMap(napi_env env, napi_value JsPixelMap, PixelMapCapi *pixelMap)
{
    napi_status status;
    napi_valuetype valueType;
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapType = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    status = napi_typeof(env, JsPixelMap, &valueType);
    if (valueType != napi_object) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    status = napi_unwrap(env, JsPixelMap, reinterpret_cast<void**>(&pixelMapType));
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    return IMAGE_RESULT_SUCCESS;
}

int32_t OH_Image_JsToC_InitializationOptions(napi_env env, napi_value jsOps, OhosPixelMapCreateOps *ops)
{
    napi_status status;
    napi_valuetype valueType;
    if (ops == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    status = napi_typeof(env, jsOps, &valueType);
    if (valueType != napi_object) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    status = napi_unwrap(env, jsOps, reinterpret_cast<void**>(&ops));
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }

    return IMAGE_RESULT_SUCCESS;
}
#ifdef __cplusplus
};
#endif
