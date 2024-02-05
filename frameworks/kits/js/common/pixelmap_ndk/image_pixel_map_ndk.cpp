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

#include "image_pixel_map_mdk.h"
#include "image_pixel_map_napi_kits.h"
#include "native_pixel_map.h"
#include "common_utils.h"
#include "image_napi_utils.h"
#include "pixel_map_from_surface.h"

using namespace OHOS::Media;
#ifdef __cplusplus
extern "C" {
#endif
uint32_t NUM_0 = 0;
struct NativePixelMap_ {
    OHOS::Media::PixelMapNapi* napi = nullptr;
};

MIDK_EXPORT
NativePixelMap* OH_PixelMap_InitNativePixelMap(napi_env env, napi_value source)
{
    PixelMapNapi* napi = PixelMapNapi_Unwrap(env, source);
    if (napi == nullptr) {
        return nullptr;
    }
    std::unique_ptr<NativePixelMap> result = std::make_unique<NativePixelMap>();
    result->napi = napi;
    return result.release();
}

MIDK_EXPORT
int32_t OH_PixelMap_CreatePixelMap(napi_env env, OhosPixelMapCreateOps info,
    void* buf, size_t len, napi_value* res)
{
    PixelMapNapiArgs args;
    args.createOptions.width = info.width;
    args.createOptions.height = info.height;
    args.createOptions.pixelFormat = info.pixelFormat;
    args.createOptions.editable = info.editable;
    args.createOptions.alphaType = info.alphaType;
    args.createOptions.scaleMode = info.scaleMode;
    args.inBuffer = buf;
    args.bufferLen = len;
    args.outValue = res;
    return PixelMapNapiNativeEnvCall(ENV_FUNC_CREATE, env, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_CreateAlphaPixelMap(napi_env env, napi_value source, napi_value* alpha)
{
    PixelMapNapiArgs args;
    args.inValue = source;
    args.outValue = alpha;
    return PixelMapNapiNativeEnvCall(ENV_FUNC_CREATE_ALPHA, env, &args);
}


MIDK_EXPORT
int32_t OH_PixelMap_GetBytesNumberPerRow(const NativePixelMap* native, int32_t* num)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.outNum = num;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_GET_ROW_BYTES, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_GetIsEditable(const NativePixelMap* native, int32_t* editable)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.outNum = editable;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_IS_EDITABLE, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_IsSupportAlpha(const NativePixelMap* native, int32_t* alpha)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.outNum = alpha;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_IS_SUPPORT_ALPHA, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_SetAlphaAble(const NativePixelMap* native, int32_t alpha)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inNum0 = alpha;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_SET_ALPHAABLE, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_GetDensity(const NativePixelMap* native, int32_t* density)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.outNum = density;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_GET_DENSITY, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_SetDensity(const NativePixelMap* native, int32_t density)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inNum0 = density;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_SET_DENSITY, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_SetOpacity(const NativePixelMap* native, float opacity)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inFloat0 = opacity;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_SET_OPACITY, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_Scale(const NativePixelMap* native, float x, float y)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inFloat0 = x;
    args.inFloat1 = y;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_SCALE, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_Translate(const NativePixelMap* native, float x, float y)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inFloat0 = x;
    args.inFloat1 = y;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_TRANSLATE, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_Rotate(const NativePixelMap* native, float angle)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inFloat0 = angle;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_ROTATE, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_Flip(const NativePixelMap* native, int32_t x, int32_t y)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inNum0 = x;
    args.inNum1 = y;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_FLIP, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_Crop(const NativePixelMap* native, int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.inNum0 = x;
    args.inNum1 = y;
    args.inNum2 = width;
    args.inNum3 = height;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_CROP, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_GetImageInfo(const NativePixelMap* native, OhosPixelMapInfos *info)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.outInfo = info;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_GET_IMAGE_INFO, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_AccessPixels(const NativePixelMap* native, void** addr)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    args.outAddr = addr;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_ACCESS_PIXELS, native->napi, &args);
}

MIDK_EXPORT
int32_t OH_PixelMap_UnAccessPixels(const NativePixelMap* native)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    PixelMapNapiArgs args;
    return PixelMapNapiNativeCtxCall(CTX_FUNC_UNACCESS_PIXELS, native->napi, &args);
}

static PixelFormat ParsePixelForamt(int32_t val)
{
    if (val <= static_cast<int32_t>(PixelFormat::CMYK)) {
        return PixelFormat(val);
    }

    return PixelFormat::UNKNOWN;
}
static AlphaType ParseAlphaType(int32_t val)
{
    if (val <= static_cast<int32_t>(AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL)) {
        return AlphaType(val);
    }

    return AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
}

static ScaleMode ParseScaleMode(int32_t val)
{
    if (val <= static_cast<int32_t>(ScaleMode::CENTER_CROP)) {
        return ScaleMode(val);
    }

    return ScaleMode::FIT_TARGET_SIZE;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_CreatePixelMap(uint32_t *colors, uint32_t colorLength, OhosPixelMapCreateOps *opts,
    PixelMapCapi *pixelMap)
{
    if (colors == nullptr || opts == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    InitializationOptions info;
    info.alphaType = ParseAlphaType(opts->alphaType);
    info.editable = opts->editable != NUM_0;
    info.pixelFormat = ParsePixelForamt(opts->pixelFormat);
    info.scaleMode = ParseScaleMode(opts->scaleMode);
    info.size.height = opts->height;
    info.size.width = opts->width;
    auto pixelMapInstance = PixelMap::Create(colors, colorLength, info).get();
    pixelMap = static_cast<Capi *>(pixelMapInstance);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_CreatePixelMapFromParcel(MessageParcelCapi *parcel, PixelMapCapi *pixelMap)
#if defined(IOS_PLATFORM) || defined(A_PLATFORM)
{
    pixelMap = nullptr;
    return IMAGE_RESULT_IMAGE_RESULT_BASE;
}
#else
{
    if (parcel == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto parcelInstance = static_cast<OHOS::MessageParcel *>(parcel);
    PIXEL_MAP_ERR error;
    auto pixelMapInstance = PixelMap::Unmarshalling(*parcelInstance, error);
    if (pixelMapInstance == nullptr) {
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    pixelMap = static_cast<Capi *>(pixelMapInstance);
    return IMAGE_RESULT_SUCCESS;
}
#endif

int32_t OH_PixelMapCapi_CreatePixelMapFromSurface(uint64_t surfaceId, int32_t x, int32_t y, int32_t width,
    int32_t height, PixelMapCapi *pixelMap)
{
    Rect region;
    region.left = x;
    region.top = y;
    region.width = width;
    region.height = height;
    auto pixelMapInstance = CreatePixelMapFromSurfaceId(surfaceId, region).get();
    if (pixelMapInstance == nullptr) {
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    pixelMap = static_cast<Capi *>(pixelMapInstance);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_ReadPixelsToBuffer(PixelMapCapi *pixelMap, uint64_t *bufferSize, uint8_t *dst)
{
    if (pixelMap == nullptr || dst == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    return pixelMapInstance->ReadPixels(*bufferSize, dst);
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_ReadPixels(PixelMapCapi *pixelMap, void* pixels, size_t size, uint32_t offset,
    uint32_t stride, int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    Rect region;
    region.left = x;
    region.top = y;
    region.width = width;
    region.height = height;
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    return pixelMapInstance->ReadPixels(size, offset, stride,
        region, static_cast<uint8_t*>(pixels));
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_WritePixels(PixelMapCapi *pixelMap, void* pixels, size_t size, uint32_t offset,
    uint32_t stride, int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    Rect region;
    region.left = x;
    region.top = y;
    region.width = width;
    region.height = height;
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    return pixelMapInstance->WritePixels(static_cast<uint8_t*>(pixels), size,
        offset, stride, region);
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_WriteBufferToPixels(PixelMapCapi *pixelMap, uint8_t *source, uint64_t *bufferSize)
{
    if (pixelMap == nullptr || source == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    return pixelMapInstance->WritePixels(source, *bufferSize);
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetImageInfo(PixelMapCapi *pixelMap, OhosPixelMapInfos *imageInfo)
{
    if (pixelMap == nullptr || imageInfo == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    ImageInfo srcInfo;
    pixelMapInstance->GetImageInfo(srcInfo);
    imageInfo->width = srcInfo.size.width;
    imageInfo->height = srcInfo.size.height;
    imageInfo->rowSize = pixelMapInstance->GetRowStride();
    imageInfo->pixelFormat = static_cast<int32_t>(srcInfo.pixelFormat);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetBytesNumberPerRow(PixelMapCapi *pixelMap, int32_t *bytesNumberPerRow)
{
    if (pixelMap == nullptr || bytesNumberPerRow == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    *bytesNumberPerRow = pixelMapInstance->GetRowBytes();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetPixelBytesNumber(PixelMapCapi *pixelMap, int32_t *pixelBytesNumber)
{
    if (pixelMap == nullptr || pixelBytesNumber == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    *pixelBytesNumber = pixelMapInstance->GetByteCount();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetDensity(PixelMapCapi *pixelMap, int32_t *density)
{
    if (pixelMap == nullptr || density == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    *density = pixelMapInstance->GetBaseDensity();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Opacity(PixelMapCapi *pixelMap, float rate)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    pixelMapInstance->SetAlpha(rate);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_CreateAlphaPixelmap(PixelMapCapi *pixelMap, PixelMapCapi *alphaPixelmap)
{
    if (pixelMap == nullptr || alphaPixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    InitializationOptions opts;
    opts.pixelFormat = PixelFormat::ALPHA_8;
    auto tmpPixelMap = pixelMapInstance->Create(*(pixelMapInstance), opts);
    alphaPixelmap = static_cast<Capi *>(std::move(tmpPixelMap).get());
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Scale(PixelMapCapi *pixelMap, float x, float y)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    pixelMapInstance->scale(x, y);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Translate(PixelMapCapi *pixelMap, float x, float y)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    pixelMapInstance->translate(x, y);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Rotate(PixelMapCapi *pixelMap, float angle)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    pixelMapInstance->rotate(angle);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Flip(PixelMapCapi *pixelMap, bool horizontal, bool vertical)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    pixelMapInstance->flip(horizontal, vertical);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Crop(PixelMapCapi *pixelMap, int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    Rect region;
    region.left = x;
    region.top = y;
    region.width = width;
    region.height = height;
    pixelMapInstance->crop(region);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetColorSpace(PixelMapCapi *pixelMap, ColorSpaceCapi* colorSpace)
{
    if (pixelMap == nullptr || colorSpace == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
#ifdef IMAGE_COLORSPACE_FLAG
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    *colorSpace = pixelMapInstance->InnerGetGrColorSpacePtr();
    return IMAGE_RESULT_SUCCESS;
#else
    return IMAGE_RESULT_MEDIA_INVALID_OPERATION;
#endif
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Marshalling(PixelMapCapi *pixelMap, ParcelCapi *parcel)
{
    if (pixelMap == nullptr || parcel == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    auto parcelInstance = static_cast<OHOS::Parcel *>(parcel);
    bool res = pixelMapInstance->Marshalling(*parcelInstance);
    if (!res) {
        return IMAGE_RESULT_ERR_IPC;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Unmarshalling(ParcelCapi *parcel, PixelMapCapi *rPixelMap)
{
    if (parcel == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto parcelInstance = static_cast<OHOS::Parcel *>(parcel);
    std::unique_ptr<OHOS::Media::PixelMap> pixelmap_ptr(PixelMap::Unmarshalling(*parcelInstance));
    rPixelMap = static_cast<Capi *>(std::move(pixelmap_ptr).get());
    if (rPixelMap == nullptr) {
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_SetColorSpace(PixelMapCapi *pixelMap, ColorSpaceCapi *colorSpace)
{
    if (pixelMap == nullptr || colorSpace == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
#ifdef IMAGE_COLORSPACE_FLAG
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    auto colorSpaceInstance = static_cast<OHOS::ColorManager::ColorSpace *>(colorSpace);
    pixelMapInstance->InnerSetColorSpace(*colorSpaceInstance);
    return IMAGE_RESULT_SUCCESS;
#else
    return IMAGE_RESULT_MEDIA_INVALID_OPERATION;
#endif
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_ApplyColorSpace(PixelMapCapi *pixelMap,
    ColorSpaceCapi *targetColorSpace)
{
    if (pixelMap == nullptr || targetColorSpace == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
#ifdef IMAGE_COLORSPACE_FLAG
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    auto colorSpaceInstance = static_cast<OHOS::ColorManager::ColorSpace *>(targetColorSpace);
    return pixelMapInstance->ApplyColorSpace(colorSpaceInstance);
#else
    return IMAGE_RESULT_MEDIA_INVALID_OPERATION;
#endif
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Release(PixelMapCapi *pixelMap)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstance = static_cast<OHOS::Media::PixelMap *>(pixelMap);
    pixelMapInstance->~PixelMap();
    return IMAGE_RESULT_SUCCESS;
}

#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
PixelMapNapi *OH_PixelMapNative_GetPixelMapNapi(struct NativePixelMap_ *nativePixelMap)
{
    return nativePixelMap == nullptr ? nullptr : nativePixelMap->napi;
}
}
}