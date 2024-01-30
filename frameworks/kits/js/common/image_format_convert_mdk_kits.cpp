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

#include "image_format_convert_mdk_kits.h"
#include <map>
#include <memory>
#include "image_mdk_common.h"
#include "common_utils.h"
#include "hilog/log.h"
#include "log_tags.h"

namespace OHOS {
namespace Media {
using OHOS::HiviewDFX::HiLog;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageFormatConvertMdk"};

using ImageConvertFunc = int32_t(*)(ImageFormatConvertArgs*);
static std::unique_ptr<ImageFormatConvert> g_nativeImgFmtCvt = nullptr;
#ifdef __cplusplus
extern "C" {
#endif

static bool IsMatchType(int32_t type, PixelFormat format)
{
    if (type == YUV_TYPE) {
        switch (format) {
            case PixelFormat::NV21:
            case PixelFormat::NV12:
            case PixelFormat::YU12:
            case PixelFormat::YV12:{
                return true;
            }
            default:{
                return false;
            }
        }
    } else if (type == RGB_TYPE) {
        switch (format) {
            case PixelFormat::ARGB_8888:
            case PixelFormat::RGB_565:
            case PixelFormat::RGBA_8888:
            case PixelFormat::BGRA_8888:
            case PixelFormat::RGB_888:
            case PixelFormat::RGBA_F16:{
                return true;
            }
            default:{
                return false;
            }
        }
    } else {
        return false;
    }
}

static int32_t CreateImageConvertNative(ImageFormatConvertArgs *args)
{
    if (g_nativeImgFmtCvt != nullptr) {
        return IMAGE_RESULT_SUCCESS;
    }

    if (args == nullptr) {
        g_nativeImgFmtCvt = std::make_unique<ImageFormatConvert>();
    } else {
        if (args->srcPixelMap == nullptr) {
            HiLog::Error(LABEL, "parameter is invalid");
            return IMAGE_RESULT_INVALID_PARAMETER;
        }
        std::shared_ptr<PixelMap> srcPixelMap = nullptr;
        srcPixelMap.reset(args->srcPixelMap);
        g_nativeImgFmtCvt = std::make_unique<ImageFormatConvert>(srcPixelMap, args->destPixelFormat);
    }

    if (g_nativeImgFmtCvt == nullptr) {
        HiLog::Error(LABEL, "create image format convert failed!");
        return IMAGE_RESULT_CREATE_FORMAT_CONVERT_FAILED;
    }

    return IMAGE_RESULT_SUCCESS;
}

static int32_t ImageConvertExec(ImageFormatConvertArgs *args)
{
    if (args == nullptr || args->srcPixelMap == nullptr) {
        HiLog::Error(LABEL, "parameter is invalid!");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    if (g_nativeImgFmtCvt == nullptr) {
        HiLog::Error(LABEL, "native image format convert is null");
        return IMAGE_RESULT_MEDIA_NULL_POINTER;
    }

    PixelFormat srcPixelFormat = args->srcPixelMap->GetPixelFormat();
    if (!IsMatchType(args->srcFormatType, srcPixelFormat) ||
        !IsMatchType(args->destFormatType, args->destPixelFormat)) {
        HiLog::Error(LABEL, "format mismatch");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    if (!g_nativeImgFmtCvt->SetDestinationFormat(args->destPixelFormat)) {
        HiLog::Error(LABEL, "set destination pixel format failed!");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    srcPixelMap.reset(args->srcPixelMap);
    if (!g_nativeImgFmtCvt->SetSourcePixelMap(srcPixelMap)) {
        HiLog::Error(LABEL, "set source pixel map failed!");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    std::unique_ptr<PixelMap> destPixelMap = nullptr;
    uint32_t ret = g_nativeImgFmtCvt->ConvertImageFormat(destPixelMap);
    if (ret != IMAGE_RESULT_SUCCESS) {
        HiLog::Error(LABEL, "fail to convert format");
        return ret;
    }
    args->destPixelMap = destPixelMap.get();
    destPixelMap.release();
    return IMAGE_RESULT_SUCCESS;
}

static int32_t ImageConvertRelease(ImageFormatConvertArgs *args)
{
    (void)args;
    if (g_nativeImgFmtCvt != nullptr) {
        g_nativeImgFmtCvt.reset(nullptr);
    }
    return IMAGE_RESULT_SUCCESS;
}

static const std::map<int32_t, ImageConvertFunc> g_Functions = {
    {CTX_FUNC_IMAGE_CONVERT_CREATE, CreateImageConvertNative},
    {CTX_FUNC_IMAGE_CONVERT_EXEC, ImageConvertExec},
    {CTX_FUNC_IMAGE_CONVERT_RELEASE, ImageConvertRelease}
};

MIDK_EXPORT
int32_t ImageConvertNativeCall(int32_t mode, ImageFormatConvertArgs *args)
{
    auto funcSearch = g_Functions.find(mode);
    if (funcSearch == g_Functions.end()) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    return funcSearch->second(args);
}
#ifdef __cplusplus
};
#endif
} // Media
} // OHOS