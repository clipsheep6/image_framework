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
#include "image_napi_utils.h"
#include "image_packer_impl.h"
#include "image_packer_napi.h"
#include "image_pixel_map_impl.h"
#include "image_source_napi.h"
#include "image_source_impl.h"
#include "js_native_api.h"
#include "napi/native_node_api.h"
#include "napi/native_api.h"
#include "pixel_map_napi.h"

#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif

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
int32_t OH_Image_CToJs_PixelMap(napi_env env, PixelMapCapi *pixelMap, napi_value *jsPixelMap)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *jsPixelMap = PixelMapNapi::CreatePixelMap(env, pixelMap->GetPixelMapInstances());
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

int32_t OH_Image_JsToC_PixelMap(napi_env env, napi_value JsPixelMap, PixelMapCapi **pixelMap)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapPtr = PixelMapNapi::GetPixelMap(env, JsPixelMap);
    *pixelMap = new PixelMapCapi(pixelMapPtr);
    return IMAGE_RESULT_SUCCESS;
}

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
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *out = cValue;
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
int32_t OH_Image_CToJs_String_Utf8(napi_env env, char *in, size_t len, napi_value jsValue)
{
    if (jsValue == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    napi_status status = napi_create_string_utf8(env, in, len, &jsValue);
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_JsToC_ImageSource(napi_env env, napi_value jsValue, ImageSourceCapi** imageSourceCapi)
{
    if (jsValue == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    std::unique_ptr<ImageSourceNapi> imageSourceNapi = nullptr;
    napi_status status = napi_unwrap(env, jsValue, reinterpret_cast<void**>(&imageSourceNapi));
    if (status != napi_ok) {
        return IMAGE_RESULT_IMAGE_RESULT_BASE;
    }
    *imageSourceCapi = new ImageSourceCapi(imageSourceNapi->nativeImgSrc);
    return IMAGE_RESULT_SUCCESS;
}

void Finalizer(napi_env env, void *nativeObject, void *finalize) {}

static ImageSourceNapi* UnwrapNativeObject(napi_env env, napi_value value)
{
    napi_valuetype valueType;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_object) {
        return nullptr;
    }
    std::unique_ptr<ImageSourceNapi> napi = nullptr;
    napi_status status = napi_unwrap(env, value, reinterpret_cast<void**>(&napi));
    if ((status == napi_ok) && napi != nullptr) {
        return napi.release();
    }
    return nullptr;
}

MIDK_EXPORT
int32_t OH_Image_CToJs_ObjImageSource(napi_env env, ImageSourceCapi* imageSourceCapi, napi_value* jsValue)
{
    if (imageSourceCapi == nullptr)
    {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto imageSource = imageSourceCapi->GetInnerImageSource();
    if (ImageSourceNapi::CreateImageSourceNapi(env, jsValue) != IMAGE_RESULT_SUCCESS) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto napi = UnwrapNativeObject(env, *(jsValue));
    if (napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (imageSource != nullptr) {
        napi->SetNativeImageSource(imageSource);
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_Image_CToJs_ObjPixelMapList(napi_env env, napi_value jsValue, void* in[], uint32_t sizeLen)
{
    if (jsValue == nullptr || in == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint32_t index = 0;
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

int32_t OH_Image_CToJs_Array(napi_env env, uint8_t* outData, int64_t size, napi_value tsArrayBuffer)
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
        auto imagePacker = napi->GetNative(napi.get());
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
