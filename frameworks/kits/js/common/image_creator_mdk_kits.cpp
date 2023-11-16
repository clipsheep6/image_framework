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

#include "image_creator_mdk_kits.h"

#include <map>

#include "hilog/log.h"
#include "image_creator_napi_listener.h"
#include "image_napi.h"
#include "log_tags.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "ImageCreatorMdkKits"};
}

namespace OHOS {
namespace Media {
using OHOS::HiviewDFX::HiLog;
using ImageCreatorFunc = int32_t (*)(struct ImageCreatorArgs* args);

#ifdef __cplusplus
extern "C" {
#endif
struct ArgsCheckInfos {
    bool env = false;
    bool napi = false;
    bool inVal = false;
    bool callback = false;
    bool outVal = false;
    bool outNum = false;
};

static int32_t CheckImageCreatorArgs(struct ImageCreatorArgs* args, struct ArgsCheckInfos &info)
{
    if (args == nullptr) {
        HiLog::Error(LABEL, "Input args is null");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (info.env && args->env == nullptr) {
        HiLog::Error(LABEL, "Input env is null");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (info.napi && args->napi == nullptr) {
        HiLog::Error(LABEL, "Input napi is null");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (info.inVal && args->inVal == nullptr) {
        HiLog::Error(LABEL, "Input inVal is null");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (info.callback && args->callback == nullptr) {
        HiLog::Error(LABEL, "Input callback is null");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (info.outVal && args->outVal == nullptr) {
        HiLog::Error(LABEL, "Input outVal is null");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    if (info.outNum && args->outNum == nullptr) {
        HiLog::Error(LABEL, "Input outNum is null");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

static int32_t ImageCreatorNapiCreate(struct ImageCreatorArgs* args)
{
    struct ArgsCheckInfos checkInfos;
    checkInfos.env = true;
    checkInfos.outVal = true;
    auto res = CheckImageCreatorArgs(args, checkInfos);
    if (res != IMAGE_RESULT_SUCCESS) {
        return res;
    }

    ImageCreatorNapiArgs napiArgs;
    napiArgs.width = args->inOpts.width;
    napiArgs.height = args->inOpts.height;
    napiArgs.format = args->inOpts.format;
    napiArgs.capicity = args->inOpts.capicity;
    return ImageCreatorNapi::CreateImageCreatorJsObject(args->env, napiArgs, args->outVal);
}

static int32_t ImageCreatorNapiUnwrap(struct ImageCreatorArgs* args)
{
    struct ArgsCheckInfos checkInfos;
    checkInfos.env = true;
    checkInfos.inVal = true;
    auto res = CheckImageCreatorArgs(args, checkInfos);
    if (res != IMAGE_RESULT_SUCCESS) {
        return res;
    }

    napi_valuetype valueType;
    napi_typeof(args->env, args->inVal, &valueType);
    if (valueType != napi_object) {
        HiLog::Error(LABEL, "Input inVal is not a napi_object");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }

    std::unique_ptr<ImageCreatorNapi> napi = nullptr;
    if (napi_unwrap(args->env, args->inVal, reinterpret_cast<void**>(&napi)) != napi_ok ||
        napi == nullptr) {
        HiLog::Error(LABEL, "napi_unwrap napi failed");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    args->napi = napi.release();
    return IMAGE_RESULT_SUCCESS;
}

static std::shared_ptr<ImageCreator> ImageCreatorGetNative(struct ImageCreatorArgs* args,
    struct ArgsCheckInfos &infos, int32_t &res)
{
    infos.env = true;
    infos.napi = true;
    res = CheckImageCreatorArgs(args, infos);
    if (res != IMAGE_RESULT_SUCCESS) {
        return nullptr;
    }
    auto native = ImageCreatorNapi::GetNative(*args->napi);
    if (native == nullptr) {
        HiLog::Error(LABEL, "ImageCreatorNapi GetNative failed");
        res = IMAGE_RESULT_INVALID_PARAMETER;
    }
    return native;
}

static int32_t ImageCreatorNapiDequeue(struct ImageCreatorArgs* args)
{
    struct ArgsCheckInfos checkInfos;
    checkInfos.outVal = true;
    int32_t res = IMAGE_RESULT_SUCCESS;
    auto native = ImageCreatorGetNative(args, checkInfos, res);
    if (res != IMAGE_RESULT_SUCCESS || native == nullptr) {
        return res;
    }
    auto nativeImage = native->DequeueNativeImage();
    if (nativeImage == nullptr) {
        HiLog::Error(LABEL, "ImageCreatorNapi DequeueNativeImage failed");
        return IMAGE_RESULT_GET_SURFACE_FAILED;
    }
    *(args->outVal) = ImageNapi::Create(args->env, nativeImage);
    if (*(args->outVal) == nullptr) {
        HiLog::Error(LABEL, "ImageNapi Create failed");
        return IMAGE_RESULT_CREATE_SURFACE_FAILED;
    }
    return IMAGE_RESULT_SUCCESS;
}

static int32_t ImageCreatorNapiQueue(struct ImageCreatorArgs* args)
{
    struct ArgsCheckInfos checkInfos;
    checkInfos.inVal = true;
    int32_t res = IMAGE_RESULT_SUCCESS;
    auto native = ImageCreatorGetNative(args, checkInfos, res);
    if (res != IMAGE_RESULT_SUCCESS || native == nullptr) {
        return res;
    }
    auto nativeImage = ImageNapi::GetNativeImage(args->env, args->inVal);
    if (nativeImage == nullptr) {
        HiLog::Error(LABEL, "ImageNapi GetNativeImage failed");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    auto combineRes = nativeImage->CombineYUVComponents();
    if (combineRes != IMAGE_RESULT_SUCCESS) {
        return combineRes;
    }
    native->QueueNativeImage(nativeImage);
    return IMAGE_RESULT_SUCCESS;
}

static int32_t ImageCreatorNapiOn(struct ImageCreatorArgs* args)
{
    struct ArgsCheckInfos checkInfos;
    checkInfos.callback = true;
    int32_t res = IMAGE_RESULT_SUCCESS;
    auto native = ImageCreatorGetNative(args, checkInfos, res);
    if (res != IMAGE_RESULT_SUCCESS || native == nullptr) {
        return res;
    }
    std::shared_ptr<ImageCreatorNapiListener> listener = std::make_shared<ImageCreatorNapiListener>();
    listener->callBack = args->callback;
    native->RegisterBufferReleaseListener((std::shared_ptr<SurfaceBufferReleaseListener> &)listener);
    return IMAGE_RESULT_SUCCESS;
}

static int32_t ImageCreatorNapiGetCapacity(struct ImageCreatorArgs* args)
{
    struct ArgsCheckInfos checkInfos;
    checkInfos.outNum = true;
    int32_t res = IMAGE_RESULT_SUCCESS;
    auto native = ImageCreatorGetNative(args, checkInfos, res);
    if (res != IMAGE_RESULT_SUCCESS || native == nullptr) {
        return res;
    }
    if (native->iraContext_ == nullptr) {
        HiLog::Error(LABEL, "ImageCreator context is nullptr");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    *(args->outNum) = native->iraContext_->GetCapicity();
    return IMAGE_RESULT_SUCCESS;
}

static int32_t ImageCreatorNapiGetFormat(struct ImageCreatorArgs* args)
{
    struct ArgsCheckInfos checkInfos;
    checkInfos.outNum = true;
    int32_t res = IMAGE_RESULT_SUCCESS;
    auto native = ImageCreatorGetNative(args, checkInfos, res);
    if (res != IMAGE_RESULT_SUCCESS || native == nullptr) {
        return res;
    }
    if (native->iraContext_ == nullptr) {
        HiLog::Error(LABEL, "ImageCreator context is nullptr");
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    *(args->outNum) = native->iraContext_->GetFormat();
    return IMAGE_RESULT_SUCCESS;
}

static const std::map<int32_t, ImageCreatorFunc> g_ImageCreatorFuncs = {
    {FUNC_IMAGE_CREATOR_CREATE, ImageCreatorNapiCreate},
    {FUNC_IMAGE_CREATOR_UNWRAP, ImageCreatorNapiUnwrap},
    {FUNC_IMAGE_CREATOR_DEQUEUE, ImageCreatorNapiDequeue},
    {FUNC_IMAGE_CREATOR_QUEUE, ImageCreatorNapiQueue},
    {FUNC_IMAGE_CREATOR_ON, ImageCreatorNapiOn},
    {FUNC_IMAGE_CREATOR_GET_CAPACITY, ImageCreatorNapiGetCapacity},
    {FUNC_IMAGE_CREATOR_GET_FORMAT, ImageCreatorNapiGetFormat},
};

MIDK_EXPORT
int32_t ImageCreatorNativeCall(int32_t mode, struct ImageCreatorArgs* args)
{
    auto funcSearch = g_ImageCreatorFuncs.find(mode);
    if (funcSearch == g_ImageCreatorFuncs.end()) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    return funcSearch->second(args);
}

#ifdef __cplusplus
};
#endif
}  // namespace Media
}  // namespace OHOS
