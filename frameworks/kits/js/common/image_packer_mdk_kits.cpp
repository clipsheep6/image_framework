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

#include "image_packer_mdk_kits.h"

#include <map>
#include "hilog/log.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "ImagePackerMdk"};
    constexpr size_t SIZE_ZERO = 0;
    constexpr int INVALID_FD = -1;
    constexpr int32_t TYPE_INVALID = -1;
    constexpr int32_t TYPE_IMAGE_SOURCE = 0;
    constexpr int32_t TYPE_PIXEL_MAP = 1;
}

namespace OHOS {
namespace Media {
using OHOS::HiviewDFX::HiLog;
using ImagePackerNativeFunc = int32_t (*)(struct ImagePackerArgs* args);
#ifdef __cplusplus
extern "C" {
#endif

static bool IsInstanceOf(napi_env env, napi_value value, napi_value global, const char* type)
{
    napi_value constructor = nullptr;
    if (napi_get_named_property(env, global, type, &constructor) != napi_ok) {
        HiLog::Error(LABEL, "Get constructor property failed!");
        return false;
    }

    bool isInstance = false;
    if (napi_instanceof(env, value, constructor, &isInstance) == napi_ok && isInstance) {
        return true;
    }
    return false;
}

static int32_t ParserPackingArgumentType(napi_env env, napi_value source)
{
    napi_value global = nullptr;
    if (napi_get_global(env, &global) != napi_ok) {
        HiLog::Error(LABEL, "Get global property failed!");
        return TYPE_INVALID;
    }

    if (IsInstanceOf(env, source, global, "ImageSource")) {
        HiLog::Debug(LABEL, "This is ImageSource!");
        return TYPE_IMAGE_SOURCE;
    } else if (IsInstanceOf(env, source, global, "PixelMap")) {
        HiLog::Debug(LABEL, "This is PixelMap!");
        return TYPE_PIXEL_MAP;
    }

    HiLog::Error(LABEL, "Invalid type!");
    return TYPE_INVALID;
}

static int32_t ImagePackerNapiCreate(struct ImagePackerArgs* args)
{
    if (args == nullptr || args->inEnv == nullptr || args->outVal == nullptr) {
        HiLog::Error(LABEL, "ImagePackerNapiCreate bad parameter");
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *(args->outVal) = ImagePackerNapi::CreateImagePacker(args->inEnv, nullptr);
    if (*(args->outVal) == nullptr) {
        HiLog::Error(LABEL, "ImageSourceNapiCreate native create failed");
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    HiLog::Debug(LABEL, "ImagePackerNapiCreate success");
    return IMAGE_RESULT_SUCCESS;
}

static std::shared_ptr<ImageSource> GetNativeImageSouce(napi_env env, napi_value source)
{
    std::unique_ptr<ImageSourceNapi> napi = nullptr;
    napi_status status = napi_unwrap(env, source, reinterpret_cast<void**>(&napi));
    if ((status == napi_ok) && napi != nullptr) {
        return napi.release()->nativeImgSrc;
    }
    return nullptr;
}

static int32_t DoStartPacking(std::shared_ptr<ImagePacker> &packer, struct ImagePackerArgs* args)
{
    PackOption option;
    option.format = args->inOpts->foramt;
    option.quality = args->inOpts->quality;
    if (args->outBuffer != nullptr && args->bufferSize != SIZE_ZERO) {
        return packer->StartPacking(args->outBuffer, args->bufferSize, option);
    } else if (args->inNum0 > INVALID_FD) {
        return packer->StartPacking(args->inNum0, option);
    }
    HiLog::Error(LABEL, "DoNativePacking StartPacking failed");
    return IMAGE_RESULT_BAD_PARAMETER;
}

static int32_t DoAddImage(std::shared_ptr<ImagePacker> &packer, int32_t type, struct ImagePackerArgs* args)
{
    if (type == TYPE_IMAGE_SOURCE) {
        auto image = GetNativeImageSouce(args->inEnv, args->inOpts->source);
        if (image != nullptr) {
            return packer->AddImage(*image);
        } else {
            HiLog::Error(LABEL, "DoNativePacking get image source native failed");
        }
    } else if (type == TYPE_PIXEL_MAP) {
        auto pixel = PixelMapNapi::GetPixelMap(args->inEnv, args->inOpts->source);
        if (pixel != nullptr) {
            return packer->AddImage(*pixel);
        } else {
            HiLog::Error(LABEL, "DoNativePacking get pixelmap native failed");
        }
    }
    HiLog::Error(LABEL, "DoNativePacking unsupport packing source type %{public}d", type);
    return IMAGE_RESULT_BAD_PARAMETER;
}

static int32_t DoNativePacking(struct ImagePackerArgs* args)
{
    auto type = ParserPackingArgumentType(args->inEnv, args->inOpts->source);
    if (type == TYPE_INVALID) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    auto nativeImagePacker = ImagePackerNapi::GetNative(args->inNapi);
    if (nativeImagePacker == nullptr) {
        HiLog::Error(LABEL, "DoNativePacking get native failed");
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    int32_t res = DoStartPacking(nativeImagePacker, args);
    if (res != IMAGE_RESULT_SUCCESS) {
        HiLog::Error(LABEL, "DoNativePacking StartPacking failed");
        return res;
    }
    res = DoAddImage(nativeImagePacker, type, args);
    if (res != IMAGE_RESULT_SUCCESS) {
        HiLog::Error(LABEL, "DoNativePacking AddImage failed");
        return res;
    }
    int64_t packedSize = SIZE_ZERO;
    res = nativeImagePacker->FinalizePacking(packedSize);
    args->bufferSize = packedSize;
    return res;
}
static int32_t ImagePackerNapiPacking(struct ImagePackerArgs* args)
{
    if (args == nullptr || args->inEnv == nullptr ||
        args->inNapi == nullptr || args->inOpts == nullptr ||
        args->inOpts->source == nullptr || args->outBuffer == nullptr ||
        args->bufferSize == SIZE_ZERO) {
        HiLog::Error(LABEL, "ImagePackerNapiPacking bad parameter");
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return DoNativePacking(args);
}

static int32_t ImagePackerNapiPackingToFile(struct ImagePackerArgs* args)
{
    if (args == nullptr || args->inEnv == nullptr ||
        args->inNapi == nullptr || args->inOpts == nullptr ||
        args->inOpts->source == nullptr || args->inNum0 <= INVALID_FD) {
        HiLog::Error(LABEL, "ImagePackerNapiPacking bad parameter");
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return DoNativePacking(args);
}

static const std::map<int32_t, ImagePackerNativeFunc> g_CtxFunctions = {
    {ENV_FUNC_IMAGEPACKER_CREATE, ImagePackerNapiCreate},
    {CTX_FUNC_IMAGEPACKER_PACKING, ImagePackerNapiPacking},
    {CTX_FUNC_IMAGEPACKER_PACKING_TO_FILE, ImagePackerNapiPackingToFile},
};

MIDK_EXPORT
int32_t ImagePackerNativeCall(int32_t mode, struct ImagePackerArgs* args)
{
    auto funcSearch = g_CtxFunctions.find(mode);
    if (funcSearch == g_CtxFunctions.end()) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return funcSearch->second(args);
}

MIDK_EXPORT
ImagePackerNapi* ImagePackerNapi_Unwrap(napi_env env, napi_value value)
{
    napi_valuetype valueType;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_object) {
        return nullptr;
    }
    std::unique_ptr<ImagePackerNapi> napi = nullptr;
    napi_status status = napi_unwrap(env, value, reinterpret_cast<void**>(&napi));
    if ((status == napi_ok) && napi != nullptr) {
        return napi.release();
    }
    return nullptr;
}
#ifdef __cplusplus
};
#endif
}  // namespace Media
}  // namespace OHOS
