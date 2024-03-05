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

#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif
#include "common_utils.h"
#include "image_napi_utils.h"
#include "image_pixel_map_impl.h"
#include "image_pixel_map_napi_kits.h"
#include "native_pixel_map.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"

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
    PixelMapCapi **pixelMap)
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

    auto pixelMapCapi = new PixelMapCapi(colors, colorLength, info);
    if (pixelMapCapi == nullptr || pixelMapCapi->GetPixelMapInstances() == nullptr) {
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    *pixelMap = pixelMapCapi;
    return IMAGE_RESULT_SUCCESS;
}

int32_t OH_PixelMapCapi_CreatePixelMapFromSurface(uint64_t surfaceId, OhosImageRect rect, PixelMapCapi **pixelMap)
{
    Rect region;
    region.left = rect.x;
    region.top = rect.y;
    region.width = rect.width;
    region.height = rect.height;
    auto pixelMapCapi = new PixelMapCapi(surfaceId, region);
    if (pixelMapCapi == nullptr || pixelMapCapi->GetPixelMapInstances() == nullptr) {
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    *pixelMap = pixelMapCapi;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_ReadPixelsToBuffer(PixelMapCapi *pixelMap, uint64_t *bufferSize, uint8_t *dst)
{
    if (pixelMap == nullptr || dst == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return pixelMap->GetPixelMapInstances()->ReadPixels(*bufferSize, dst);
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_ReadPixels(PixelMapCapi *pixelMap, OhosPositionArea positionArea)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    Rect region;
    region.left = positionArea.rect.x;
    region.top = positionArea.rect.y;
    region.width = positionArea.rect.width;
    region.height = positionArea.rect.height;
    return pixelMap->GetPixelMapInstances()->ReadPixels(positionArea.size, positionArea.offset, positionArea.stride,
        region, static_cast<uint8_t*>(positionArea.byteBuffer));
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_WritePixels(PixelMapCapi *pixelMap, OhosPositionArea positionArea)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    Rect region;
    region.left = positionArea.rect.x;
    region.top = positionArea.rect.y;
    region.width = positionArea.rect.width;
    region.height = positionArea.rect.height;
    return pixelMap->GetPixelMapInstances()->WritePixels(static_cast<uint8_t*>(positionArea.byteBuffer),
        positionArea.size, positionArea.offset, positionArea.stride, region);
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_WriteBufferToPixels(PixelMapCapi *pixelMap, uint8_t *source, uint64_t *bufferSize)
{
    if (pixelMap == nullptr || source == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return pixelMap->GetPixelMapInstances()->WritePixels(source, *bufferSize);
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetImageInfo(PixelMapCapi *pixelMap, OhosPixelMapInfos *imageInfo)
{
    if (pixelMap == nullptr || imageInfo == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageInfo srcInfo;
    pixelMap->GetPixelMapInstances()->GetImageInfo(srcInfo);
    imageInfo->width = srcInfo.size.width;
    imageInfo->height = srcInfo.size.height;
    imageInfo->rowSize = pixelMap->GetPixelMapInstances()->GetRowStride();
    imageInfo->pixelFormat = static_cast<int32_t>(srcInfo.pixelFormat);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetBytesNumberPerRow(PixelMapCapi *pixelMap, int32_t *bytesNumberPerRow)
{
    if (pixelMap == nullptr || bytesNumberPerRow == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *bytesNumberPerRow = pixelMap->GetPixelMapInstances()->GetRowBytes();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetPixelBytesNumber(PixelMapCapi *pixelMap, int32_t *pixelBytesNumber)
{
    if (pixelMap == nullptr || pixelBytesNumber == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *pixelBytesNumber = pixelMap->GetPixelMapInstances()->GetByteCount();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetDensity(PixelMapCapi *pixelMap, int32_t *density)
{
    if (pixelMap == nullptr || density == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *density = pixelMap->GetPixelMapInstances()->GetBaseDensity();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Opacity(PixelMapCapi *pixelMap, float rate)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelMap->GetPixelMapInstances()->SetAlpha(rate);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_CreateAlphaPixelmap(PixelMapCapi *pixelMap, PixelMapCapi **alphaPixelmap)
{
    if (pixelMap == nullptr || alphaPixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    InitializationOptions opts;
    opts.pixelFormat = PixelFormat::ALPHA_8;
    auto pixelMapCapi = new PixelMapCapi(pixelMap, opts);
    if (alphaPixelmap == nullptr || (*alphaPixelmap)->GetPixelMapInstances() == nullptr) {
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    *alphaPixelmap = pixelMapCapi;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Scale(PixelMapCapi *pixelMap, float x, float y)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelMap->GetPixelMapInstances()->scale(x, y);
    return IMAGE_RESULT_SUCCESS;
}


MIDK_EXPORT
int32_t OH_PixelMapCapi_Translate(PixelMapCapi *pixelMap, float x, float y)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelMap->GetPixelMapInstances()->translate(x, y);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Rotate(PixelMapCapi *pixelMap, float angle)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelMap->GetPixelMapInstances()->rotate(angle);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Flip(PixelMapCapi *pixelMap, bool horizontal, bool vertical)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelMap->GetPixelMapInstances()->flip(horizontal, vertical);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Crop(PixelMapCapi *pixelMap, OhosImageRect rect)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    Rect region;
    region.left = rect.x;
    region.top = rect.y;
    region.width = rect.width;
    region.height = rect.height;
    pixelMap->GetPixelMapInstances()->crop(region);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetColorSpace(PixelMapCapi *pixelMap, ColorSpaceCapi* colorSpace)
{
    if (pixelMap == nullptr || colorSpace == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
#ifdef IMAGE_COLORSPACE_FLAG
    auto colorSpacePtr = pixelMap->GetPixelMapInstances()->InnerGetGrColorSpacePtr().get();
    *colorSpace = static<ColorSpaceCapi>(*colorSpacePtr);
    return IMAGE_RESULT_SUCCESS;
#else
    return IMAGE_RESULT_MEDIA_INVALID_OPERATION;
#endif
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_SetColorSpace(PixelMapCapi *pixelMap, ColorSpaceCapi *colorSpace)
{
    if (pixelMap == nullptr || colorSpace == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
#ifdef IMAGE_COLORSPACE_FLAG
    auto colorSpaceInstance = static_cast<OHOS::ColorManager::ColorSpace *>(colorSpace);
    pixelMap->GetPixelMapInstances()->InnerSetColorSpace(*colorSpaceInstance);
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
    auto colorSpaceInstance = static_cast<OHOS::ColorManager::ColorSpace *>(targetColorSpace);
    return pixelMap->GetPixelMapInstances()->ApplyColorSpace(colorSpaceInstance);
#else
    return IMAGE_RESULT_MEDIA_INVALID_OPERATION;
#endif
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_GetIsEditable(PixelMapCapi *pixelMap, int32_t* editable)
{
    if (pixelMap == nullptr || editable == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *editable = pixelMap->GetPixelMapInstances()->IsEditable();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_IsSupportAlpha(PixelMapCapi *pixelMap, int32_t* alpha)
{
    if (pixelMap == nullptr || alpha == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *alpha = pixelMap->GetPixelMapInstances()->GetAlphaType() != AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_SetAlphaAble(PixelMapCapi *pixelMap, int32_t alpha)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto pixelMapInstances = pixelMap->GetPixelMapInstances();
    auto alphaType = pixelMapInstances->GetAlphaType();
    if ((alpha != NUM_0) && (alphaType == AlphaType::IMAGE_ALPHA_TYPE_OPAQUE)) {
        pixelMapInstances->SetAlphaType(AlphaType::IMAGE_ALPHA_TYPE_PREMUL);
    } else if ((alpha == NUM_0) && !(alphaType == AlphaType::IMAGE_ALPHA_TYPE_OPAQUE)) {
        pixelMapInstances->SetAlphaType(AlphaType::IMAGE_ALPHA_TYPE_OPAQUE);
    } else {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_AccessPixels(PixelMapCapi *pixelMap, void** addr)
{
    if (pixelMap == nullptr || addr == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelMap->LockPixelMap();
    *addr = static_cast<uint8_t*>(pixelMap->GetPixelMapInstances()->GetWritablePixels());
    return IMAGE_RESULT_SUCCESS;
}

int32_t OH_PixelMapCapi_UnAccessPixels(PixelMapCapi *pixelMap)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    pixelMap->UnlockPixelMap();
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_PixelMapCapi_Release(PixelMapCapi *pixelMap)
{
    if (pixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if(pixelMap->IsLockPixelMap())
    {
        return IMAGE_RESULT_MEDIA_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    pixelMap->Release();
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