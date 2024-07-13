/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <memory.h>
#include "auxiliary_picture_napi.h"
#include "media_errors.h"
#include "image_log.h"
#include "image_napi.h"
#include "image_napi_utils.h"
#include "pixel_map_napi.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "AuxiliaryPictureNapi"

namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
    constexpr uint32_t NUM_3 = 3;
}

namespace OHOS {
namespace Media {
static const std::string CLASS_NAME = "AuxiliaryPicture";
thread_local napi_ref AuxiliaryPictureNapi::sConstructor_ = nullptr;
thread_local std::shared_ptr<AuxiliaryPicture> AuxiliaryPictureNapi::sAuxiliaryPic_ = nullptr;
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#endif

struct AuxiliaryPictureNapiAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    napi_ref error = nullptr;
    uint32_t status;
    AuxiliaryPictureNapi *nConstructor;
    std::shared_ptr<Picture> rPicture;
    std::shared_ptr<AuxiliaryPicture> auxPicture;
    void *arrayBuffer;
    size_t arrayBufferSize;
    Size size;
    AuxiliaryPictureType type;
};

using AuxiliaryPictureNapiAsyncContextPtr = std::unique_ptr<AuxiliaryPictureNapiAsyncContext>;

struct NapiValues {
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_value* argv = nullptr;
    size_t argc;
    int32_t refCount = 1;
    std::unique_ptr<AuxiliaryPictureNapiAsyncContext> context;
};

AuxiliaryPictureNapi::AuxiliaryPictureNapi():env_(nullptr)
{
    static std::atomic<uint32_t> currentId = 0;
    uniqueId_ = currentId.fetch_add(1, std::memory_order_relaxed);
}

AuxiliaryPictureNapi::~AuxiliaryPictureNapi()
{
    release();
}

napi_value AuxiliaryPictureNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor props[] = {};
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createAuxiliaryPicture", CreateAuxiliaryPicture),
    };

    napi_value constructor = nullptr;

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH,
                          Constructor, nullptr, IMG_ARRAY_SIZE(props),
                          props, &constructor)),
        nullptr, IMAGE_LOGE("define class fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_create_reference(env, constructor, 1, &sConstructor_)),
        nullptr, IMAGE_LOGE("create reference fail")
    );

    napi_value global = nullptr;
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_get_global(env, &global)),
        nullptr, IMAGE_LOGE("Init:get global fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor)),
        nullptr, IMAGE_LOGE("Init:set global named property fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor)),
        nullptr, IMAGE_LOGE("set named property fail")
    );

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(
        napi_define_properties(env, exports, IMG_ARRAY_SIZE(static_prop), static_prop)),
        nullptr, IMAGE_LOGE("define properties fail")
    );

    IMAGE_LOGD("Init success");
    return exports;
}

napi_value AuxiliaryPictureNapi::Constructor(napi_env env, napi_callback_info info)
{
    napi_value undefineVar = nullptr;
    napi_get_undefined(env, &undefineVar);

    napi_status status;
    napi_value thisVar = nullptr;
    napi_get_undefined(env, &thisVar);
    IMAGE_LOGD("Constructor IN");
    status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    IMG_NAPI_CHECK_RET(IMG_IS_READY(status, thisVar), undefineVar);
    std::unique_ptr<AuxiliaryPictureNapi> pAuxiliaryPictureNapi = std::make_unique<AuxiliaryPictureNapi>();
    if (pAuxiliaryPictureNapi != nullptr) {
        pAuxiliaryPictureNapi->env_ = env;
        pAuxiliaryPictureNapi->nativeAuxiliaryPicture_ = sAuxiliaryPic_;
        if (pAuxiliaryPictureNapi->nativeAuxiliaryPicture_ == nullptr) {
            IMAGE_LOGE("Failed to set nativeImageSource with null. Maybe a reentrancy error");
        }
        status = napi_wrap(env, thisVar, reinterpret_cast<void *>(pAuxiliaryPictureNapi.get()),
                            AuxiliaryPictureNapi::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            IMAGE_LOGE("Failure wrapping js to native napi");
            return undefineVar;
        }
    }
    pAuxiliaryPictureNapi.release();
    return thisVar;
}

void AuxiliaryPictureNapi::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    if (nativeObject != nullptr) {
        IMAGE_LOGD("Destructor PictureNapi");
        delete reinterpret_cast<AuxiliaryPictureNapi*>(nativeObject);
        nativeObject = nullptr;
    }
}

STATIC_EXEC_FUNC(CreateAuxiliaryPicture)
{
    auto context = static_cast<AuxiliaryPictureNapiAsyncContext*>(data);
    InitializationOptions opts;
    opts.size = context->size;
    auto colors = static_cast<uint32_t*>(context->arrayBuffer);
    auto tmpPixelmap = PixelMap::Create(colors, context->arrayBufferSize, opts);
    std::shared_ptr<PixelMap> pixelmap = std::move(tmpPixelmap);
    auto picture = AuxiliaryPicture::Create(pixelmap, context->type, context->size);
    context->auxPicture = std::move(picture);
    if (IMG_NOT_NULL(context->auxPicture)) {
        context->status = SUCCESS;
    } else {
        context->status = ERROR;
    }
}

static bool ParseSize(napi_env env, napi_value root, int32_t& width, int32_t& height)
{
    if (!GET_INT32_BY_NAME(root, "width", width) || !GET_INT32_BY_NAME(root, "height", height)) {
        return false;
    }
    return true;
}

static AuxiliaryPictureType ParseAuxiliaryPictureType(int32_t val)
{
    if (val >= static_cast<int32_t>(AuxiliaryPictureType::GAIN_MAP)
        && val <= static_cast<int32_t>(AuxiliaryPictureType::MARK_CUT_MAP)) {
        return AuxiliaryPictureType(val);
    }
    return AuxiliaryPictureType::NONE;
}

napi_value AuxiliaryPictureNapi::CreateAuxiliaryPicture(napi_env env, napi_callback_info info)
{
    if (AuxiliaryPictureNapi::GetConstructor() == nullptr) {
        napi_value exports = nullptr;
        napi_create_object(env, &exports);
        AuxiliaryPictureNapi::Init(env, exports);
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napi_value constructor = nullptr;
    napi_status status;
    napi_value thisVar = nullptr;
    size_t argCount = NUM_3;
    napi_value argValue[NUM_3] = {0};
    uint32_t auxiType = 0;
    std::unique_ptr<AuxiliaryPictureNapiAsyncContext> asyncContext = 
        std::make_unique<AuxiliaryPictureNapiAsyncContext>();
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, IMAGE_LOGE("Call napi_get_cb_info failed"));
    
    IMG_NAPI_CHECK_RET_D(argCount == NUM_3,ImageNapiUtils::ThrowExceptionError(env, COMMON_ERR_INVALID_PARAMETER,
        "Invalid args count"),IMAGE_LOGE("Invalid args count %{public}zu", argCount));

    status = napi_get_arraybuffer_info(env, argValue[NUM_0], &(asyncContext->arrayBuffer),
            &(asyncContext->arrayBufferSize));
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, IMAGE_LOGE("Fail to get auxiliary picture buffer"));
    if (asyncContext->arrayBuffer == nullptr || asyncContext->arrayBufferSize < NUM_0) {
        IMAGE_LOGE("Auxiliary picture buffer invalid or Auxiliary picture buffer size invalid");
        return result;
    }
    
    if (!ParseSize(env, argValue[NUM_1], asyncContext->size.width, asyncContext->size.height)) {
        IMAGE_LOGE("Fail to get auxiliary picture size");
        return result;
    }
    status = napi_get_value_uint32(env, argValue[NUM_2], &auxiType);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), result, IMAGE_LOGE("Fail to get auxiliary picture Type"));
    asyncContext->type = ParseAuxiliaryPictureType(auxiType);
    if (val < static_cast<int32_t>(AuxiliaryPictureType::GAIN_MAP)
        || val > static_cast<int32_t>(AuxiliaryPictureType::MARK_CUT_MAP)) {
        IMAGE_LOGE("AuxiliaryFigureType is invalid");
        return result;
    }
    CreateAuxiliaryPictureExec(env, static_cast<void*>((asyncContext).get()));
    status = napi_get_reference_value(env, sConstructor_, &constructor);
    if (IMG_IS_OK(status)) {
        sAuxiliaryPic_ = std::move(asyncContext->auxPicture);
        status = napi_new_instance(env, constructor, NUM_0, nullptr, &result);
    }
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, IMAGE_LOGE("Fail to create picture sync"));
    return result;
}

void AuxiliaryPictureNapi::release()
{
    if (!isRelease) {
        if (nativeAuxiliaryPicture_ != nullptr) {
            nativeAuxiliaryPicture_ = nullptr;
        }
        isRelease = true;
    }
}
}  // namespace Media
}  // namespace OHOS
