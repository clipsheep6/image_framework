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
#include "image_source_impl.h"
#include "common_utils.h"
#include "image_source_mdk_kits.h"
#include "securec.h"
#include "image_pixel_map_impl.h"
using namespace OHOS::Media;

#ifdef __cplusplus
extern "C" {
#endif

const size_t SIZE_ZERO = 0;
const uint32_t SUCCESS_CAPI = 0;
const int32_t INVALID_FD_CAPI = -1;
const int32_t ERR_MEDIA_INVALID_VALUE_CAPI = -1;
const uint32_t DEFAULT_INDEX_CAPI = 0;
const uint32_t INVALID_SAMPLE_SIZE_CAPI = 0;
const int8_t INT8_TRUE_CAPI = 1;

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
static void ParseImageSourceOpsCapi(SourceOptions &opts, struct OhosImageSourceOps* ops)
{
    opts.baseDensity = ops->density;
    opts.pixelFormat = static_cast<PixelFormat>(ops->pixelFormat);
    opts.size.width = ops->size.width;
    opts.size.height = ops->size.height;
}

static void ParseImageSourceCapi(ImageSourceImpl* imgSrcImp, struct OhosImageSource* imgSrc)
{
    imgSrcImp->buffer = imgSrc->buffer;
    imgSrcImp->bufferSize = imgSrc->bufferSize;
    imgSrcImp->fd = imgSrc->fd;
    imgSrcImp->uri = imgSrc->uri;
    imgSrcImp->uriSize = imgSrc->uriSize;
}

static void ParseDecodingOpsCapi(DecodeOptions &decOps, struct OhosImageDecodingOps* ops)
{
    if (ops->sampleSize != INVALID_SAMPLE_SIZE_CAPI) {
        decOps.sampleSize = ops->sampleSize;
    }
    decOps.rotateNewDegrees = ops->rotate;
    decOps.editable = (ops->editable == INT8_TRUE_CAPI);
    decOps.desiredSize.width = ops->size.width;
    decOps.desiredSize.height = ops->size.height;
    decOps.desiredRegion.left = ops->region.x;
    decOps.desiredRegion.top = ops->region.y;
    decOps.desiredRegion.width = ops->region.width;
    decOps.desiredRegion.height = ops->region.height;
    if (ops->pixelFormat <= static_cast<int32_t>(PixelFormat::CMYK)) {
        decOps.desiredPixelFormat = PixelFormat(ops->pixelFormat);
    }
    decOps.fitDensity = ops->fitDensity;
}

static void ParseImageSourceInfoCapi(struct OhosImageSourceInfo* source, ImageInfo &info)
{
    source->alphaType = static_cast<int32_t>(info.alphaType);
    source->colorSpace = static_cast<int32_t>(info.colorSpace);
    source->density = info.baseDensity;
    source->pixelFormat = static_cast<int32_t>(info.pixelFormat);
    source->size.width = info.size.width;
    source->size.height = info.size.height;
}

static uint32_t MathMin(uint32_t a, uint32_t b)
{
    if (a < b) {
        return a;
    }
    return b;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromUri(char* uri, size_t size, OhosImageSourceOps* ops, ImageSourceCapi** res)
{
    SourceOptions opts;
    if (ops != nullptr) {
        ParseImageSourceOpsCapi(opts, ops);
    }
    *res = new ImageSourceCapi(uri, size, opts);
    if ((*res) == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromFd(int32_t fd, OhosImageSourceOps* ops, ImageSourceCapi** res)
{
    SourceOptions opts;
    if (ops != nullptr) {
        ParseImageSourceOpsCapi(opts, ops);
    }
    *res = new ImageSourceCapi(fd, opts);
    if ((*res) == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromData(uint8_t* data, size_t dataSize,
    OhosImageSourceOps* ops, ImageSourceCapi** res)
{
    SourceOptions opts;
    if (ops != nullptr) {
        ParseImageSourceOpsCapi(opts, ops);
    }
    *res = new ImageSourceCapi(data, dataSize, opts);
    if ((*res) == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateFromRawFile(RawFileDescriptor rawFile, OhosImageSourceOps* ops, ImageSourceCapi** res)
{
    SourceOptions opts;
    if (ops != nullptr) {
        ParseImageSourceOpsCapi(opts, ops);
    }
    *res = new ImageSourceCapi(rawFile, opts);
    if ((*res) == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreateIncremental(OhosImageSource* source,
    OhosImageSourceOps* ops, ImageSourceCapi** res)
{
    ImageSourceImpl* imgSrcImpl = nullptr;
    ParseImageSourceCapi(imgSrcImpl, source);
    IncrementalSourceOptions incOpts;
    if (ops != nullptr) {
        ParseImageSourceOpsCapi(incOpts.sourceOptions, ops);
    }
    *res = new ImageSourceCapi(imgSrcImpl, incOpts);
    if ((*res) == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreatePixelMap(ImageSourceCapi* stImgSrc,
    OhosImageDecodingOps* ops, PixelMapCapi** resPixMap)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    DecodeOptions decOps;
    uint32_t index = DEFAULT_INDEX_CAPI;
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE_CAPI;
    if (ops != nullptr) {
        ParseDecodingOpsCapi(decOps, ops);
        index = ops->index;
    }
    std::unique_ptr<PixelMap> tmpPixelmap = stImgSrc->GetInnerImageSource()->CreatePixelMapEx(index, decOps, errorCode);
    if (tmpPixelmap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    std::shared_ptr<PixelMap> nativePixelMap = std::move(tmpPixelmap);
    PixelMapCapi* stPixMap = new PixelMapCapi(nativePixelMap);
    *resPixMap = stPixMap;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_CreatePixelMapList(ImageSourceCapi* stImgSrc, OhosImageDecodingOps* ops,
    PixelMapCapi** resPixMap[], uint32_t* outSize)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    DecodeOptions decOps;
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE_CAPI;
    if (ops != nullptr) {
        ParseDecodingOpsCapi(decOps, ops);
    }
    //unique_ptr<vector<unique_ptr<PixelMap>>>
    auto pixelMapList = stImgSrc->GetInnerImageSource()->CreatePixelMapList(decOps, errorCode);
    if (pixelMapList == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint32_t index = 0;
    for (auto &item : *pixelMapList) {
        std::shared_ptr<PixelMap> tempPixMap = std::move(item);
        PixelMapCapi* stPixMap = new PixelMapCapi(tempPixMap);
        *resPixMap[index] = stPixMap;
        index ++;
    }
    *outSize = index;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetSupportedFormats(ImageSourceCapi* stImgSrc, OhosImageSourceSupportedFormatList* resFormats)
{
    if (stImgSrc == nullptr || resFormats == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    std::set<std::string> formatSet;
    uint32_t errorCode = stImgSrc->GetInnerImageSource()->GetSupportedFormats(formatSet);
    if (errorCode != SUCCESS_CAPI) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    size_t formatCount = formatSet.size();
    if (resFormats->supportedFormatList == nullptr) {
        resFormats->size = formatCount;
        return IMAGE_RESULT_SUCCESS;
    } else {
        formatCount = resFormats->size;
    }

    auto formatList = resFormats->supportedFormatList;
    size_t i = SIZE_ZERO;
    for (const std::string& formatStr: formatSet) {
        if (i >= formatCount) {
            break;
        }
        if (formatList[i] == nullptr || formatList[i]->format == nullptr) {
            return IMAGE_RESULT_BAD_PARAMETER;
        }
        memcpy_s(formatList[i]->format, formatList[i]->size, formatStr.c_str(), formatStr.size());
        if (formatList[i]->size > formatStr.size()) {
            formatList[i]->size = formatStr.size();
        }
        i++;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetDelayTime(ImageSourceCapi* stImgSrc, OhosImageSourceDelayTimeList* resDelayTimes)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE_CAPI;
    auto delayTimes = stImgSrc->GetInnerImageSource()->GetDelayTime(errorCode);
    if (delayTimes == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    size_t actCount = (*delayTimes).size();
    if (resDelayTimes->delayTimeList == nullptr) {
        resDelayTimes->size = actCount; //get times count only
        return IMAGE_RESULT_SUCCESS;
    }
    if (actCount > resDelayTimes->size) {
        actCount = resDelayTimes->size;
    }
    for (size_t i = SIZE_ZERO; i < actCount; i++) {
        resDelayTimes->delayTimeList[i] = (*delayTimes)[i];
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetImageInfo(ImageSourceCapi* stImgSrc, int32_t index, OhosImageSourceInfo* info)
{
    if (stImgSrc == nullptr || info == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageInfo imageInfo;
    uint32_t errorCode = stImgSrc->GetInnerImageSource()->GetImageInfo(index, imageInfo);
    if (errorCode != SUCCESS_CAPI) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ParseImageSourceInfoCapi(info, imageInfo);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetImageProperty(ImageSourceCapi* stImgSrc, OhosImageSourceProperty* propertyKey,
    OhosImageSourceProperty* propertyVal)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (propertyKey == nullptr || propertyKey->value == nullptr || propertyKey->size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (propertyVal == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    std::string key(propertyKey->value, propertyKey->size);
    std::string val;
    uint32_t errorCode = stImgSrc->GetInnerImageSource()->GetImagePropertyString(DEFAULT_INDEX_CAPI, key, val);
    if (errorCode != SUCCESS_CAPI || val.empty()) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (propertyVal->value == nullptr) {
        propertyVal->size = val.size();
        return IMAGE_RESULT_SUCCESS;
    }
    memcpy_s(propertyVal->value, propertyVal->size, val.c_str(), val.size());
    if (propertyVal->size > val.size()) {
        propertyVal->size = val.size();
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_ModifyImageProperty(ImageSourceCapi* stImgSrc, OhosImageResource* imageResource,
    OhosImageSourceProperty* propertyKey, OhosImageSourceProperty* propertyVal)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (propertyKey == nullptr || propertyKey->value == nullptr || propertyKey->size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (propertyVal == nullptr || propertyVal->value == nullptr || propertyVal->size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    std::string key(propertyKey->value, propertyKey->size);
    std::string val(propertyVal->value, propertyVal->size);
    std::string path(imageResource->pathBuffer, imageResource->pathSize);
    auto type = imageResource->type;
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE_CAPI;
    if (type == ImageResourceType_::IMAGE_RESOURCE_INVAILD) {
        return IMAGE_RESULT_BAD_PARAMETER;
    } else if (type == ImageResourceType_::IMAGE_RESOURCE_FD && imageResource->fd != INVALID_FD_CAPI) {
        errorCode = stImgSrc->GetInnerImageSource()->ModifyImageProperty(DEFAULT_INDEX_CAPI, key, val, imageResource->fd);
    } else if (type == ImageResourceType_::IMAGE_RESOURCE_PATH && !path.empty()) {
        errorCode = stImgSrc->GetInnerImageSource()->ModifyImageProperty(DEFAULT_INDEX_CAPI, key, val, path);
    } else if (type == ImageResourceType_::IMAGE_RESOURCE_BUFFER &&
        imageResource->buffer != nullptr && imageResource->bufferSize > SIZE_ZERO) {
        errorCode = stImgSrc->GetInnerImageSource()->ModifyImageProperty(DEFAULT_INDEX_CAPI, key, val,
            imageResource->buffer, imageResource->bufferSize);
    } else {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (errorCode == SUCCESS_CAPI) {
        return IMAGE_RESULT_SUCCESS;
    }
    return IMAGE_RESULT_BAD_PARAMETER;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_GetFrameCount(ImageSourceCapi* stImgSrc, uint32_t* frameCount)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE_CAPI;
    *frameCount = stImgSrc->GetInnerImageSource()->GetFrameCount(errorCode);
    if (errorCode != SUCCESS_CAPI) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_UpdateData(ImageSourceCapi* stImgSrc, OhosImageDecodingOps* decOption,
    OhosImageSourceUpdateData* data)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (data == nullptr || data->buffer == nullptr || data->bufferSize == SIZE_ZERO ||
        data->offset >= data->bufferSize) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint32_t actSize = MathMin((data->bufferSize - data->offset), data->updateLength);
    bool completed = data->isCompleted == INT8_TRUE_CAPI;
    uint32_t errCode = stImgSrc->GetInnerImageSource()->UpdateData((data->buffer + data->offset), actSize, completed);
    if (errCode != SUCCESS_CAPI) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    DecodeOptions decodeOpts;
    uint32_t index = DEFAULT_INDEX_CAPI;
    if (decOption != nullptr) {
        ParseDecodingOpsCapi(decodeOpts, decOption);
        index = decOption->index;
    }
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE_CAPI;
    std::unique_ptr<IncrementalPixelMap> incPixelMap = stImgSrc->GetInnerImageSource()->CreateIncrementalPixelMap(
        index, decodeOpts, errorCode);
    if (incPixelMap == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint8_t tmpProgress = 0;
    uint32_t errCodePro = incPixelMap->PromoteDecoding(tmpProgress);
    if (completed) {
        incPixelMap->DetachFromDecoding();
    }
    if (errCodePro != SUCCESS_CAPI ){ //|| (errCodePro == ERR_IMAGE_SOURCE_DATA_INCOMPLETE && !completed)) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
int32_t OH_ImageSourceCapi_Release(ImageSourceCapi* stImgSrc)
{
    if (stImgSrc == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    stImgSrc->GetInnerImageSource()->~ImageSource();
    return IMAGE_RESULT_SUCCESS;
}
/** ImageSource Capi */

#ifdef __cplusplus
};
#endif