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
struct ImageCreatorNative_ {
    napi_env env = nullptr;
    ImageCreatorNapi* napi = nullptr;
};

MIDK_EXPORT
int32_t OH_Image_Creator_Create(napi_env env, struct OhosImageCreatorInfo info, napi_value* res)
{
    ImageCreatorArgs args;
    args.env = env;
    args.inInfo.width = info.width;
    args.inInfo.height = info.height;
    args.inInfo.capicity = info.capicity;
    args.inInfo.format = info.format;
    args.outVal = res;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_CREATE, &args);
}

MIDK_EXPORT
ImageCreatorNative* OH_Image_Creator_InitNative(napi_env env, napi_value source)
{
    ImageCreatorArgs args;
    args.env = env;
    args.inVal = source;
    auto resCode = ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_UNWRAP, &args);
    if (resCode != IMAGE_RESULT_SUCCESS || args.napi == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ImageCreatorNative> result = std::make_unique<ImageCreatorNative>();
    result->napi = args.napi;
    result->env = env;
    return result.release();
}

MIDK_EXPORT
int32_t OH_Image_Creator_Dequeue(const ImageCreatorNative* native, napi_value* image)
{
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.outVal = image;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_DEQUEUE, &args);
}

MIDK_EXPORT
int32_t OH_Image_Creator_Queue(const ImageCreatorNative* native, napi_value image)
{
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.inVal = image;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_QUEUE, &args);
}

MIDK_EXPORT
int32_t OH_Image_Creator_On(const ImageCreatorNative* native, OH_Image_Creator_On_Callback callback)
{
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.callback = callback;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_ON, &args);
}

MIDK_EXPORT
int32_t OH_Image_Creator_GetCapacity(const ImageCreatorNative* native, int32_t* capacity)
{
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.outNum = capacity;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_GET_CAPACITY, &args);
}

MIDK_EXPORT
int32_t OH_Image_Creator_GetFormat(const ImageCreatorNative* native, int32_t* format)
{
    ImageCreatorArgs args;
    args.env = native->env;
    args.napi = native->napi;
    args.outNum = format;
    return ImageCreatorNativeCall(FUNC_IMAGE_CREATOR_GET_FORMAT, &args);
}

MIDK_EXPORT
int32_t OH_Image_Creator_Release(ImageCreatorNative* native)
{
    if (native != nullptr) {
        delete native;
    }
    return IMAGE_RESULT_SUCCESS;
}
#ifdef __cplusplus
};
#endif
