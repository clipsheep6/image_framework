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

#include "image_format_convert_napi.h"
#include <memory>
#include <string>
#include <securec.h>
#include "image_napi_utils.h"
#include "media_errors.h"
#include "hilog/log.h"
#include "log_tags.h"
#include <stdio.h>
#include <ctime>

using OHOS::HiviewDFX::HiLog;
namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE,
    LOG_TAG_DOMAIN_ID_IMAGE_FORAMT_CONVERT_NAPI, "ImageFormatConvertNapi" };
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
}

namespace OHOS {
namespace Media {
static const std::string CLASS_NAME = "ImageConvert";
thread_local napi_ref ImageFormatConvertNapi::sConstructor_ = nullptr;
enum class FormatType:int8_t {
    UNKNOWN,
    YUV,
    RGB
};

struct ImageFormatConvertAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref error = nullptr;
    uint32_t status;
    ImageFormatConvertNapi *nConstructor;
    FormatType srcFormatType = FormatType::UNKNOWN;
    FormatType destFormatType = FormatType::UNKNOWN;
    std::shared_ptr<PixelMap> srcPixelMap = nullptr;
    PixelFormat destPixelFormat = PixelFormat::UNKNOWN;
};

using ImgFmtCvtAsyncCtx = ImageFormatConvertAsyncContext;
using ImgFmtCvtAsyncCtxPtr = ImageFormatConvertAsyncContext *;


static bool IsMatchFormatType(FormatType type, PixelFormat format)
{
    if (type == FormatType::YUV) {
        switch (format) {
            case PixelFormat::NV21:
            case PixelFormat::NV12:{
                return true;
            }
            default:{
                return false;
            }
        }
    } else if (type == FormatType::RGB) {
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

static PixelFormat ParsePixelFormat(napi_env &env, napi_value pixelFormatValue)
{
    napi_status status = napi_invalid_arg;
    int32_t pixelFormatNum = 0;
    status = napi_get_value_int32(env, pixelFormatValue, &pixelFormatNum);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), PixelFormat::UNKNOWN, HiLog::Error(LABEL, "fail to get pixel format!"));
    if (pixelFormatNum >= static_cast<int32_t>(PixelFormat::ARGB_8888) &&
        pixelFormatNum <= static_cast<int32_t>(PixelFormat::YV12)) {
        return PixelFormat(pixelFormatNum);
    }
    return PixelFormat::UNKNOWN;
}

static bool ParsePixelMap(napi_env &env, napi_value &root, std::shared_ptr<PixelMap> &pixelMap)
{
    pixelMap = PixelMapNapi::GetPixelMap(env, root);
    if (pixelMap == nullptr) {
        HiLog::Error(LABEL, "get pixel map failed!");
        return false;
    }
    return true;
}



static void BuildContextError(napi_env env, napi_ref &error, const std::string errMsg, const int32_t errCode)
{
    HiLog::Error(LABEL, "%{public}s", errMsg.c_str());
    napi_value tmpError;
    ImageNapiUtils::CreateErrorObj(env, tmpError, errCode, errMsg);
    napi_create_reference(env, tmpError, NUM_1, &(error));
}

static uint32_t GetNativePixelMapInfo(napi_env &env, napi_value &pixelMapValue,
    napi_value pixelFormatValue, ImgFmtCvtAsyncCtxPtr context)
{
    PixelFormat destPixelFormat = ParsePixelFormat(env, pixelFormatValue);
    IMG_NAPI_CHECK_BUILD_ERROR(IsMatchFormatType(context->destFormatType, destPixelFormat),
        BuildContextError(env, context->error, "dest format is wrong!", ERR_IMAGE_INVALID_PARAMETER),
        context->status = ERR_IMAGE_INVALID_PARAMETER, ERR_IMAGE_INVALID_PARAMETER);
    std::shared_ptr<PixelMap> pixelMap = nullptr;
    IMG_NAPI_CHECK_BUILD_ERROR(ParsePixelMap(env, pixelMapValue, pixelMap),
        BuildContextError(env, context->error, "fail to parse pixel map!", ERR_IMAGE_INVALID_PARAMETER),
        context->status = ERR_IMAGE_INVALID_PARAMETER, ERR_IMAGE_INVALID_PARAMETER);
    IMG_NAPI_CHECK_BUILD_ERROR(IsMatchFormatType(context->srcFormatType, pixelMap->GetPixelFormat()),
        BuildContextError(env, context->error, "source format is wrong!", ERR_IMAGE_INVALID_PARAMETER),
        context->status = ERR_IMAGE_INVALID_PARAMETER, ERR_IMAGE_INVALID_PARAMETER);
    context->destPixelFormat = destPixelFormat;
    context->srcPixelMap = pixelMap;
    return SUCCESS;
}

static void CommonCallbackRoutine(napi_env env, ImgFmtCvtAsyncCtxPtr &asyncContext,
                                  const napi_value &valueParam)
{
    napi_value result[NUM_2] = {0};
    napi_get_undefined(env, &result[NUM_0]);
    napi_get_undefined(env, &result[NUM_1]);

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return;
    }

    if (asyncContext == nullptr) {
        return;
    }
    if (asyncContext->status == SUCCESS) {
        result[NUM_1] = valueParam;
    } else if (asyncContext->error != nullptr) {
        napi_get_reference_value(env, asyncContext->error, &result[NUM_0]);
        napi_delete_reference(env, asyncContext->error);
    } else {
        napi_create_uint32(env, asyncContext->status, &result[NUM_0]);
    }

    if (asyncContext->deferred) {
        if (asyncContext->status == SUCCESS) {
            napi_resolve_deferred(env, asyncContext->deferred, result[NUM_1]);
        } else {
            napi_reject_deferred(env, asyncContext->deferred, result[NUM_0]);
        }
    }

    napi_delete_async_work(env, asyncContext->work);
    napi_close_handle_scope(env, scope);

    delete asyncContext;
    asyncContext = nullptr;
}

STATIC_COMPLETE_FUNC(ConvertWithPixelMap)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    auto context = static_cast<ImgFmtCvtAsyncCtxPtr>(data);
    std::shared_ptr<PixelMap> destPixelMap = nullptr;
    context->status = ImageFormatConvert::ConvertImageFormat(context->srcPixelMap, destPixelMap,
                                                             context->destPixelFormat);
    if (context->status == SUCCESS) {
        result = PixelMapNapi::CreatePixelMap(env, destPixelMap);
        if (ImageNapiUtils::getType(env, result) == napi_undefined) {
            context->status = ERR_MEDIA_INVALID_VALUE;
            BuildContextError(env, context->error, "napi create dest pixel map failed!", context->status);
        }
    } else {
        BuildContextError(env, context->error, "napi pixel format convert failed!", context->status);
    }

    CommonCallbackRoutine(env, context, result);
}

STATIC_COMPLETE_FUNC(GeneralError)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    auto context = static_cast<ImgFmtCvtAsyncCtxPtr>(data);
    CommonCallbackRoutine(env, context, result);
}

STATIC_EXEC_FUNC(ConvertWithPixelMap)
{
    auto context = static_cast<ImgFmtCvtAsyncCtxPtr>(data);
    context->status = SUCCESS;
}

STATIC_EXEC_FUNC(GeneralError)
{
    auto context = static_cast<ImgFmtCvtAsyncCtxPtr>(data);
    context->status = IMAGE_RESULT_CREATE_FORMAT_CONVERT_FAILED;
}

static uint32_t GetNativeConvertInfo(napi_env &env, napi_callback_info &info, ImgFmtCvtAsyncCtxPtr context)
{
    napi_status status = napi_invalid_arg;
    napi_value thisVar = nullptr;
    size_t argc = NUM_2;
    napi_value argv[NUM_2] = {nullptr};
    IMG_JS_ARGS(env, info, status, argc, argv, thisVar);
    IMG_NAPI_CHECK_BUILD_ERROR(IMG_IS_OK(status),
        BuildContextError(env, context->error, "fail to napi_get_cb_info", ERROR), context->status = ERROR, ERROR);
    IMG_NAPI_CHECK_BUILD_ERROR(argc == NUM_2,
        BuildContextError(env, context->error, "incorrect number of parametersarguments!", ERR_IMAGE_INVALID_PARAMETER),
        context->status = ERR_IMAGE_INVALID_PARAMETER, ERR_IMAGE_INVALID_PARAMETER);
    napi_value constructor = nullptr;
    napi_value global = nullptr;
    napi_get_global(env, &global);
    status = napi_get_named_property(env, global, "PixelMap", &constructor);
    IMG_NAPI_CHECK_BUILD_ERROR(IMG_IS_OK(status),
        BuildContextError(env, context->error, "Get PixelMapNapi property failed!", ERR_IMAGE_PROPERTY_NOT_EXIST),
        context->status = ERR_IMAGE_PROPERTY_NOT_EXIST, ERR_IMAGE_PROPERTY_NOT_EXIST);
    bool isPixelMap = false;
    status = napi_instanceof(env, argv[NUM_0], constructor, &isPixelMap);
    IMG_NAPI_CHECK_BUILD_ERROR(IMG_IS_OK(status),
        BuildContextError(env, context->error, "fail to napi_instanceof!", ERROR), context->status = ERROR, ERROR);
    if (isPixelMap) {
        return GetNativePixelMapInfo(env, argv[NUM_0], argv[NUM_1], context);
    }
    IMG_NAPI_CHECK_BUILD_ERROR(false,
        BuildContextError(env, context->error, "wrong arguments!", ERR_IMAGE_INVALID_PARAMETER),
        context->status = ERR_IMAGE_INVALID_PARAMETER, ERR_IMAGE_INVALID_PARAMETER);
}

static napi_value DoInitAfter(napi_env env, napi_value exports, napi_value constructor, 
                              size_t property_count, const napi_property_descriptor* properties)
{
    napi_value global = nullptr;
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_get_global(env, &global)),
        nullptr, HiLog::Error(LABEL, "Init:get global fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor)),
        nullptr, HiLog::Error(LABEL, "Init:set global named property fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor)),
        nullptr, HiLog::Error(LABEL, "set named property fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_define_properties(env, exports, property_count, properties)),
        nullptr, HiLog::Error(LABEL, "define properties fail")
    );
    return exports;
}

static napi_value Convert(napi_env &env, napi_callback_info &info, FormatType srcFormatType,
    FormatType destFormatType, std::string workName)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    std::unique_ptr<ImgFmtCvtAsyncCtx> asyncContext = std::make_unique<ImgFmtCvtAsyncCtx>();
    asyncContext->status = SUCCESS;
    asyncContext->srcFormatType = srcFormatType;
    asyncContext->destFormatType = destFormatType;
    uint32_t ret = GetNativeConvertInfo(env, info, asyncContext.get());
    napi_create_promise(env, &(asyncContext->deferred), &result);

    IMG_NAPI_CHECK_BUILD_ERROR(asyncContext->error == nullptr,
        asyncContext->status = IMAGE_RESULT_CREATE_FORMAT_CONVERT_FAILED,
        IMG_CREATE_CREATE_ASYNC_WORK(env, status, (workName + "GeneralError").c_str(),
        GeneralErrorExec, GeneralErrorComplete, asyncContext, asyncContext->work),
        result);
    IMG_NAPI_CHECK_BUILD_ERROR(ret == SUCCESS,
        BuildContextError(env, asyncContext->error, "get native convert info failed!", ret),
        IMG_CREATE_CREATE_ASYNC_WORK(env, status, (workName + "GeneralError").c_str(),
        GeneralErrorExec, GeneralErrorComplete, asyncContext, asyncContext->work),
        result);
    IMG_CREATE_CREATE_ASYNC_WORK(env, status, workName.c_str(),
    ConvertWithPixelMapExec, ConvertWithPixelMapComplete, asyncContext, asyncContext->work);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status),
        nullptr, HiLog::Error(LABEL, "fail to create async work"));
    return result;
}

ImageFormatConvertNapi::ImageFormatConvertNapi():env_(nullptr)
{
}

napi_value ImageFormatConvertNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor props[] = {
    };
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("imageConvertYuvToRgb", YUVToRGB),
        DECLARE_NAPI_STATIC_FUNCTION("imageConvertRgbToYuv", RGBToYUV),
    };

    napi_value constructor = nullptr;

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH,
                          Constructor, nullptr, IMG_ARRAY_SIZE(props),
                          props, &constructor)),
        nullptr, HiLog::Error(LABEL, "define class fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_create_reference(env, constructor, NUM_1, &sConstructor_)),
        nullptr, HiLog::Error(LABEL, "create reference fail")
    );

    auto result = DoInitAfter(env, exports, constructor,
        IMG_ARRAY_SIZE(static_prop), static_prop);

    HiLog::Debug(LABEL, "Init success");
    return result;
}

napi_value ImageFormatConvertNapi::Constructor(napi_env env, napi_callback_info info)
{
    napi_value undefineValue = nullptr;
    napi_get_undefined(env, &undefineValue);

    napi_status status;
    napi_value thisVar = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    if (status == napi_ok && thisVar != nullptr) {
        std::unique_ptr<ImageFormatConvertNapi> pImgFmtCvtNapi = std::make_unique<ImageFormatConvertNapi>();
        if (pImgFmtCvtNapi != nullptr) {
            pImgFmtCvtNapi->env_ = env;
            status = napi_wrap(env, thisVar, reinterpret_cast<void *>(pImgFmtCvtNapi.get()),
                               ImageFormatConvertNapi::Destructor, nullptr, nullptr);
            if (status == napi_ok) {
                pImgFmtCvtNapi.release();
                return thisVar;
            } else {
                HiLog::Error(LABEL, "Failure wrapping js to native napi");
            }
        }
    }

    return undefineValue;
}

void ImageFormatConvertNapi::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    HiLog::Debug(LABEL, "ImageFormatConvertNapi::Destructor");
}

napi_value ImageFormatConvertNapi::YUVToRGB(napi_env env, napi_callback_info info)
{
    return Convert(env, info, FormatType::YUV, FormatType::RGB, "YUVToRGB");
}

napi_value ImageFormatConvertNapi::RGBToYUV(napi_env env, napi_callback_info info)
{
    return Convert(env, info, FormatType::RGB, FormatType::YUV, "RGBToYUV");
}

} // Media
} // OHOS