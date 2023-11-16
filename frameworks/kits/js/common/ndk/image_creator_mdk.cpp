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

#include "image_creator_mdk.h"

#include "common_utils.h"
#include "image_creator_mdk_kits.h"

using namespace OHOS::Media;
#ifdef __cplusplus
extern "C" {
#endif
struct ImageCreator_Native_ {
    napi_env env = nullptr;
    ImageCreatorNapi* napi = nullptr;
};

MIDK_EXPORT
int32_t OH_ImageCreator_Create(napi_env env, ImageCreator_Opts opts, napi_value* res)
{
    ImageCreatorArgs args;
    args.env = env;
    args.inOpts.width = opts.width;
    args.inOpts.height = opts.height;
    args.inOpts.capicity = opts.capicity;
    args.inOpts.format = opts.format;
    args.outVal = res;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_CREATE, &args);
}

MIDK_EXPORT
ImageCreator_Native* OH_ImageCreator_InitNative(napi_env env, napi_value source)
{
    ImageCreatorArgs args;
    args.env = env;
    args.inVal = source;
    auto resCode = ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_UNWRAP, &args);
    if (resCode != IMAGE_RESULT_SUCCESS || args.napi == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ImageCreator_Native> result = std::make_unique<ImageCreator_Native>();
    result->napi = args.napi;
    result->env = env;
    return result.release();
}

MIDK_EXPORT
int32_t OH_ImageCreator_Dequeue(const ImageCreator_Native* native, napi_value* image)
{
    if (native == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.outVal = image;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_DEQUEUE, &args);
}

MIDK_EXPORT
int32_t OH_ImageCreator_Queue(const ImageCreator_Native* native, napi_value image)
{
    if (native == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.inVal = image;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_QUEUE, &args);
}

MIDK_EXPORT
int32_t OH_ImageCreator_On(const ImageCreator_Native* native, OH_ImageCreator_On_Callback callback)
{
    if (native == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.callback = callback;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_ON, &args);
}

MIDK_EXPORT
int32_t OH_ImageCreator_GetCapacity(const ImageCreator_Native* native, int32_t* capacity)
{
    if (native == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.outNum = capacity;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_GET_CAPACITY, &args);
}

MIDK_EXPORT
int32_t OH_ImageCreator_GetFormat(const ImageCreator_Native* native, int32_t* format)
{
    if (native == nullptr) {
        return IMAGE_RESULT_INVALID_PARAMETER;
    }
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.outNum = format;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_GET_FORMAT, &args);
}

MIDK_EXPORT
int32_t OH_ImageCreator_Release(ImageCreator_Native* native)
{
    if (native != nullptr) {
        delete native;
    }
    return IMAGE_RESULT_SUCCESS;
}
#ifdef __cplusplus
};
#endif
