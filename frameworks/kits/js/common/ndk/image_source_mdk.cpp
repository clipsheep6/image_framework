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

#include "image_source_mdk.h"

#include "common_utils.h"
#include "image_source_mdk_kits.h"
using namespace OHOS::Media;

#ifdef __cplusplus
extern "C" {
#endif

const size_t SIZE_ZERO = 0;
struct ImageSourceNative_ {
    ImageSourceNapi* napi = nullptr;
    napi_env env = nullptr;
};

MIDK_EXPORT
ImageSourceNative* OH_ImageSource_InitNative(napi_env env, napi_value source)
{
    ImageSourceArgs args;
    args.inEnv = env;
    args.inVal = source;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_UNWRAP, &args);
    if (ret != IMAGE_RESULT_SUCCESS || args.napi == nullptr) {
        return nullptr;
    }
    std::unique_ptr<ImageSourceNative> result = std::make_unique<ImageSourceNative>();
    result->napi = args.napi;
    result->env = env;
    return result.release();
}

MIDK_EXPORT
int32_t OH_ImageSource_Create(napi_env env, struct OhosImageSource* src,
    struct OhosImageSourceOps* ops, napi_value *res)
{
    ImageSourceArgs args;
    args.inEnv = env;
    args.source = src;
    args.sourceOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_CREATE, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreateFromUri(napi_env env, char* uri, size_t size,
    struct OhosImageSourceOps* ops, napi_value *res)
{
    if (uri == nullptr || size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.inEnv = env;
    args.uri = std::string(uri, size);
    args.sourceOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_CREATE_FROM_URI, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreateFromFd(napi_env env, int32_t fd,
    struct OhosImageSourceOps* ops, napi_value *res)
{
    ImageSourceArgs args;
    args.inEnv = env;
    args.fd = fd;
    args.sourceOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_CREATE_FROM_FD, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreateFromData(napi_env env, uint8_t* data, size_t dataSize,
    struct OhosImageSourceOps* ops, napi_value *res)
{
    ImageSourceArgs args;
    DataArray dataArray;
    dataArray.data = data;
    dataArray.dataSize = dataSize;
    args.inEnv = env;
    args.dataArray = dataArray;
    args.sourceOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_CREATE_FROM_DATA, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreateFromRawFile(napi_env env, RawFileDescriptor rawFile,
    struct OhosImageSourceOps* ops, napi_value *res)
{
    ImageSourceArgs args;
    args.inEnv = env;
    args.rawFile = rawFile;
    args.sourceOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_CREATE_FROM_RAW_FILE, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreateIncremental(napi_env env,
    struct OhosImageSource* source, struct OhosImageSourceOps* ops, napi_value *res)
{
    ImageSourceArgs args;
    args.inEnv = env;
    args.source = source;
    args.sourceOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_CREATE_INCREMENTAL, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreateIncrementalFromData(napi_env env, uint8_t* data, size_t dataSize,
    struct OhosImageSourceOps* ops, napi_value *res)
{
    ImageSourceArgs args;
    DataArray dataArray;
    dataArray.data = data;
    dataArray.dataSize = dataSize;
    args.inEnv = env;
    args.dataArray = dataArray;
    args.sourceOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(ENV_FUNC_IMAGE_SOURCE_CREATE_INCREMENTAL, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_GetSupportedFormats(struct OhosImageSourceSupportedFormatList* res)
{
    ImageSourceArgs args;
    args.outFormats = res;
    auto ret = ImageSourceNativeCall(STA_FUNC_IMAGE_SOURCE_GET_SUPPORTED_FORMATS, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreatePixelMap(const ImageSourceNative* native,
    struct OhosImageDecodingOps* ops, napi_value *res)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.inEnv = native->env;
    args.decodingOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_CREATE_PIXELMAP, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_CreatePixelMapList(const ImageSourceNative* native,
    struct OhosImageDecodingOps* ops, napi_value* res)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.inEnv = native->env;
    args.decodingOps = ops;
    args.outVal = res;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_CREATE_PIXELMAP_LIST, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_GetDelayTime(const ImageSourceNative* native,
    struct OhosImageSourceDelayTimeList* res)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.outDelayTimes = res;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_GET_DELAY_TIME, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_GetFrameCount(const ImageSourceNative* native, uint32_t *res)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.outUint32 = res;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_GET_FRAME_COUNT, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_GetImageInfo(const ImageSourceNative* native, int32_t index,
    struct OhosImageSourceInfo* info)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.inInt32 = index;
    args.outInfo = info;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_GET_IMAGE_INFO, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_GetImageProperty(const ImageSourceNative* native,
    struct OhosImageSourceProperty* key, struct OhosImageSourceProperty* value)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.inPropertyKey = key;
    args.propertyVal = value;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_GET_IMAGE_PROPERTY, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_ModifyImageProperty(const ImageSourceNative* native,
    struct OhosImageSourceProperty* key, struct OhosImageSourceProperty* value)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.inPropertyKey = key;
    args.propertyVal = value;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_MODIFY_IMAGE_PROPERTY, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_UpdateData(const ImageSourceNative* native,
    struct OhosImageSourceUpdateData* data)
{
    if (native == nullptr || native->napi == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgs args;
    args.napi = native->napi;
    args.inUpdateData = data;
    auto ret = ImageSourceNativeCall(CTX_FUNC_IMAGE_SOURCE_UPDATE_DATA, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSource_Release(ImageSourceNative* native)
{
    if (native != nullptr) {
        delete native;
    }
    return IMAGE_RESULT_SUCCESS;
}

/** ImageSource Capi */
MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromUri(char* uri, size_t size, ImageSource_Options* ops, void* res)
{
    if (uri == nullptr || size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.uri = std::string(uri, size);
    args.sourceOps = ops;
    args.outImgSrc = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_CREATE_FROM_URI, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromFd(int32_t fd, ImageSource_Options* ops, void* res)
{
    ImageSourceArgsCapi args;
    args.fd = fd;
    args.sourceOps = ops;
    args.outImgSrc = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_CREATE_FROM_FD, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromData(uint8_t* data, size_t dataSize, ImageSource_Options* ops, void* res)
{
    ImageSourceArgsCapi args;
    DataArray dataArray;
    dataArray.data = data;
    dataArray.dataSize = dataSize;
    args.dataArray = dataArray;
    args.sourceOps = ops;
    args.outImgSrc = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_CREATE_FROM_DATA, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromRawFile(RawFileDescriptor rawFile, ImageSource_Options* ops, void* res)
{
    ImageSourceArgsCapi args;
    args.rawFile = rawFile;
    args.sourceOps = ops;
    args.outImgSrc = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_CREATE_FROM_RAW_FILE, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateIncremental(ImageSource_Source* source, ImageSource_Options* ops, void* res)
{
    ImageSourceArgsCapi args;
    args.source = source;
    args.sourceOps = ops;
    args.outImgSrc = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_CREATE_INCREMENTAL, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreatePixelMap(void* imgSrc,
    Image_DecodingOptions* ops, void* resPixMap)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.decodingOps = ops;
    args.outPixMap = resPixMap;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_CREATE_PIXELMAP, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreatePixelMapList(void* imgSrc, Image_DecodingOptions* ops, 
    void* resVecPixMap[], uint32_t* outSize)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.decodingOps = ops;
    args.outVecPixMap[0] = resVecPixMap;
    args.outSize = outSize;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_CREATE_PIXELMAP_LIST, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetSupportedFormats(void* imgSrc, ImageSource_SupportedFormatList* res)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.outFormats = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_GET_SUPPORTED_FORMATS, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetDelayTime(void* imgSrc, ImageSource_DelayTimeList* res)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.outDelayTimes = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_GET_DELAY_TIME, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetImageInfo(void* imgSrc, int32_t index, ImageSource_Info* info)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.inInt32 = index;
    args.outInfo = info;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_GET_IMAGE_INFO, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetImageProperty(void* imgSrc, ImageSource_Property* key, ImageSource_Property* value)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.inPropertyKey = key;
    args.propertyVal = value;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_GET_IMAGE_PROPERTY, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_ModifyImageProperty(void* imgSrc, Image_Resource* imgResource, 
    ImageSource_Property* key, ImageSource_Property* value)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.resource = imgResource;
    args.inPropertyKey = key;
    args.propertyVal = value;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_MODIFY_IMAGE_PROPERTY, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetFrameCount(void* imgSrc, uint32_t *res)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.outUint32 = res;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_GET_FRAME_COUNT, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_UpdateData(void* imgSrc, Image_DecodingOptions* decOption, ImageSource_UpdateData* data)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    args.decodingOps = decOption;
    args.inUpdateData = data;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_UPDATE_DATA, &args);
    return ret;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_Release(void* imgSrc)
{
    if (imgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageSourceArgsCapi args;
    args.inImgSrc = imgSrc;
    auto ret = ImageSourceCapiCall(CAPI_FUNC_IMAGE_SOURCE_UPDATE_RELEASE, &args);
    return ret;
}
/** ImageSource Capi */

#ifdef __cplusplus
};
#endif