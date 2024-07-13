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

#include "picture_napi.h"
#include "media_errors.h"
#include "image_log.h"
#include "image_napi_utils.h"
#include "image_napi.h"
#include "pixel_map_napi.h"
#include "auxiliary_picture_napi.h"
#include "auxiliary_picture.h"
#include "napi_message_sequence.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PictureNapi"

namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
}

namespace OHOS {
namespace Media {
static const std::string CLASS_NAME = "Picture";
thread_local napi_ref PictureNapi::sConstructor_ = nullptr;
thread_local std::shared_ptr<Picture> PictureNapi::sPicture_ = nullptr;
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
#endif

struct PictureAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    napi_ref error = nullptr;
    uint32_t status;
    std::shared_ptr<Picture> rPicture;
    PictureNapi *nConstructor;
    std::shared_ptr<PixelMap> rPixelMap;
};

using PictureAsyncContextPtr = std::unique_ptr<PictureAsyncContext>;

struct NapiValues {
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_value* argv = nullptr;
    size_t argc;
    int32_t refCount = 1;
    std::unique_ptr<PictureAsyncContext> context;
};

static bool prepareNapiEnv(napi_env env, napi_callback_info info, struct NapiValues* nVal)
{
    napi_get_undefined(env, &(nVal->result));
    nVal->status = napi_get_cb_info(env, info, &(nVal->argc), nVal->argv, &(nVal->thisVar), nullptr);
    if (nVal->status != napi_ok) {
        IMAGE_LOGE("Fail to napi_get_cb_info");
        return false;
    }
    nVal->context = std::make_unique<PictureAsyncContext>();
    nVal->status = napi_unwrap(env, nVal->thisVar, reinterpret_cast<void**>(&(nVal->context->nConstructor)));
    if (nVal->status != napi_ok) {
        IMAGE_LOGE("Fail to unwrap context");
        return false;
    }
    nVal->context->status = SUCCESS;
    return true;
}

PictureNapi::PictureNapi():env_(nullptr)
{
    static std::atomic<uint32_t> currentId = 0;
    uniqueId_ = currentId.fetch_add(1, std::memory_order_relaxed);
}

PictureNapi::~PictureNapi()
{
    release();
}

napi_value PictureNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor props[] = {
        DECLARE_NAPI_FUNCTION("getMainPixelmap", GetMainPixelmap),
        DECLARE_NAPI_FUNCTION("getAuxiliaryPicture", GetAuxiliaryPicture),
        DECLARE_NAPI_FUNCTION("setAuxiliaryPicture", SetAuxiliaryPicture),
        DECLARE_NAPI_FUNCTION("release", Release),
        DECLARE_NAPI_FUNCTION("marshalling", Marshalling),
    };
    napi_property_descriptor static_prop[] = {};

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

napi_value PictureNapi::Constructor(napi_env env, napi_callback_info info)
{
    napi_value undefineVar = nullptr;
    napi_get_undefined(env, &undefineVar);

    napi_status status;
    napi_value thisVar = nullptr;
    napi_get_undefined(env, &thisVar);
    IMAGE_LOGD("Constructor IN");
    status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    IMG_NAPI_CHECK_RET(IMG_IS_READY(status, thisVar), undefineVar);
    std::unique_ptr<PictureNapi> pPictureNapi = std::make_unique<PictureNapi>();
    if (pPictureNapi != nullptr) {
        pPictureNapi->env_ = env;
        pPictureNapi->nativePicture_ = sPicture_;
        if (pPictureNapi->nativePicture_ == nullptr) {
            IMAGE_LOGE("Failed to set nativePicture_ with null. Maybe a reentrancy error");
        }
        status = napi_wrap(env, thisVar, reinterpret_cast<void *>(pPictureNapi.get()),
                            PictureNapi::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            IMAGE_LOGE("Failure wrapping js to native napi");
            return undefineVar;
        }
    }
    pPictureNapi.release();
    return thisVar;
}

void PictureNapi::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    if (nativeObject != nullptr) {
        IMAGE_LOGD("Destructor PictureNapi");
        delete reinterpret_cast<PictureNapi*>(nativeObject);
        nativeObject = nullptr;
    }
}

napi_value PictureNapi::CreatePicture(napi_env env, std::shared_ptr<Picture> picture)
{
    if (sConstructor_ == nullptr) {
        napi_value exports = nullptr;
        napi_create_object(env, &exports);
        PictureNapi::Init(env, exports);
    }
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status;
    status = napi_get_reference_value(env, sConstructor_, &constructor);
    if (IMG_IS_OK(status)) {
        if (picture != nullptr) {
            sPicture_ = std::move(picture);
            status = napi_new_instance(env, constructor, NUM_0, nullptr, &result);
        } else {
            status = napi_invalid_arg;
            IMAGE_LOGE("New PictureNapi Instance picture is nullptr");
            napi_get_undefined(env, &result);
        }
    }
    if (!IMG_IS_OK(status)) {
        IMAGE_LOGE("CreatePicture | New instance could not be obtained");
        napi_get_undefined(env, &result);
    }
    return result;
}

static AuxiliaryPictureType ParseAuxiliaryPictureType(int32_t val)
{
    if (val >= static_cast<int32_t>(AuxiliaryPictureType::GAIN_MAP)
        && val<= static_cast<int32_t>(AuxiliaryPictureType::MARK_CUT_MAP)) {
        return AuxiliaryPictureType(val);
    }

    return AuxiliaryPictureType::NONE;
}

napi_value PictureNapi::GetAuxiliaryPicture(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = NUM_1;
    uint32_t auxiType = 0;;

    IMAGE_LOGD("GetAuxiliaryPicture IN");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), result, IMAGE_LOGE("fail to arg info"));
    PictureNapi* pictureNapi = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&pictureNapi));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, pictureNapi), result, IMAGE_LOGE("fail to unwrap PictureNapi"));
    status = napi_get_value_uint32(env, argValue[NUM_0], &auxiType);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), result, IMAGE_LOGE("fail to get auxiliary picture Type"));

    AuxiliaryPictureType type = ParseAuxiliaryPictureType(auxiType);

    if (pictureNapi->nativePicture_ != nullptr) {
        auto auxiliaryPic = pictureNapi->nativePicture_->GetAuxiliaryPicture(type);
        if (auxiliaryPic != nullptr) {
            result = AuxiliaryPictureNapi::CreateAuxiliaryPicture(env, std::move(auxiliaryPic));
        } else {
            IMAGE_LOGE("native auxiliary picture is nullptr!");
        }
    } else {
        IMAGE_LOGE("native picture is nullptr!");
    }
    return result;
}

napi_value PictureNapi::SetAuxiliaryPicture(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_2] = {0};
    size_t argCount = NUM_2;
    uint32_t auxiType = 0;;


    IMAGE_LOGD("SetAuxiliaryPictureSync IN");
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
    PictureNapi* pictureNapi = nullptr;
    status = napi_unwrap(env, thisVar, reinterpret_cast<void**>(&pictureNapi));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, pictureNapi), result, IMAGE_LOGE("fail to unwrap PictureNapi"));

    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), result, IMAGE_LOGE("fail to arg info"));
    status = napi_get_value_uint32(env, argValue[NUM_0], &auxiType);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), result, IMAGE_LOGE("fail to get auxiliary picture Type"));
    AuxiliaryPictureType type = ParseAuxiliaryPictureType(auxiType);

    AuxiliaryPictureNapi* auxiliaryPictureNapi = nullptr;
    status = napi_unwrap(env, argValue[NUM_1], reinterpret_cast<void**>(&auxiliaryPictureNapi));
    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(status, pictureNapi), result,
                                    IMAGE_LOGE("fail to unwrap AuxiliaryPictureNapi"));

    if (pictureNapi->nativePicture_ != nullptr) {
        auto auxiliaryPicturePtr = auxiliaryPictureNapi->GetNativeAuxiliaryPic();
        if (auxiliaryPicturePtr != nullptr) {
            pictureNapi->nativePicture_->SetAuxiliaryPicture(type, auxiliaryPicturePtr);
        } else {
            IMAGE_LOGE("native auxiliary picture is nullptr!");
        }
    } else {
        IMAGE_LOGE("native picture is nullptr!");
    }

    return result;
}

napi_value PictureNapi::GetMainPixelmap(napi_env env, napi_callback_info info)
{
    NapiValues nVal;
    napi_get_undefined(env, &nVal.result);
    nVal.argc = NUM_0;
    IMG_JS_ARGS(env, info, nVal.status, nVal.argc, nullptr, nVal.thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(nVal.status), nVal.result, IMAGE_LOGE("Fail to arg info"));

    PictureNapi* pictureNapi = nullptr;
    nVal.status = napi_unwrap(env, nVal.thisVar, reinterpret_cast<void**>(&pictureNapi));

    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(nVal.status, pictureNapi), nVal.result, IMAGE_LOGE("Fail to unwrap context"));
    
    if (pictureNapi->nativePicture_ != nullptr) {
        auto pixelmap = pictureNapi->nativePicture_->GetMainPixel();
        nVal.result = PixelMapNapi::CreatePixelMap(env, pixelmap);
    } else {
        IMAGE_LOGE("Native picture is nullptr!");
    }
    return nVal.result;
}

napi_value PictureNapi::Release(napi_env env, napi_callback_info info)
{
    NapiValues nVal;
    nVal.result = nullptr;
    napi_get_undefined(env, &nVal.result);
    nVal.argc = NUM_0;
    std::unique_ptr<PictureAsyncContext> asyncContext = std::make_unique<PictureAsyncContext>();
    IMG_JS_ARGS(env, info, nVal.status, nVal.argc, nullptr, nVal.thisVar);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(nVal.status), nVal.result, IMAGE_LOGE("Fail to call napi_get_cb_info"));
    nVal.status = napi_unwrap(env, nVal.thisVar, reinterpret_cast<void**>(&asyncContext->nConstructor));

    IMG_NAPI_CHECK_RET_D(IMG_IS_READY(nVal.status, asyncContext->nConstructor), 
            nVal.result, IMAGE_LOGE("Fail to unwrap context"));
    asyncContext.release();
    return nVal.result;
}

napi_value PictureNapi::Marshalling(napi_env env, napi_callback_info info)
{
#if !defined(IOS_PLATFORM) && !defined(ANDROID_PLATFORM)
    NapiValues nVal;
    nVal.argc = NUM_1;
    napi_value argValue[NUM_1] = {0};
    nVal.argv = argValue;
    if (!prepareNapiEnv(env, info, &nVal)) {
        return ImageNapiUtils::ThrowExceptionError(
            env, ERR_IMAGE_INVALID_PARAMETER, "Fail to unwrap context");
    }
    nVal.context->rPicture = nVal.context->nConstructor->nativePicture_;
    if (nVal.argc != NUM_0 && nVal.argc != NUM_1) {
        return ImageNapiUtils::ThrowExceptionError(
            env, ERR_IMAGE_INVALID_PARAMETER, "Invalid args count");
    }
    NAPI_MessageSequence *napiSequence = nullptr;
    napi_get_cb_info(env, info, &nVal.argc, nVal.argv, nullptr, nullptr);
    napi_unwrap(env, nVal.argv[0], reinterpret_cast<void**>(&napiSequence));
        auto messageParcel = napiSequence->GetMessageParcel();
    if (messageParcel == nullptr) {
        return ImageNapiUtils::ThrowExceptionError(
            env, ERR_IPC, "Marshalling picture to parcel failed.");
    }
    bool st = nVal.context->rPicture->Marshalling(*messageParcel);
    if (!st) {
        return ImageNapiUtils::ThrowExceptionError(
            env, ERR_IPC, "Marshalling picture to parcel failed.");
    }
    return nVal.result;
#else
    return napi_value(nullptr);
#endif
}

void PictureNapi::release()
{
    if (!isRelease) {
        if (nativePicture_ != nullptr) {
            nativePicture_ = nullptr;
        }
        isRelease = true;
    }
}
}  // namespace Media
}  // namespace OHOS
