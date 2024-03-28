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

#include "image_source2.h"

#include "common_utils.h"
#include "image_source.h"
#include "image_source2_impl.h"
#include "pixelmap2_impl.h"
#ifndef _WIN32
#include "securec.h"
#else
#include "memory.h"
#endif

using namespace OHOS;
using namespace Media;
#ifdef __cplusplus
extern "C" {
#endif

const uint32_t DEFAULT_INDEX = 0;
const int32_t ERR_MEDIA_INVALID_VALUE = -1;
constexpr size_t SIZE_ZERO = 0;
constexpr uint32_t INVALID_SAMPLE_SIZE = 0;
const int32_t INVALID_FD = -1;

struct OH_ImageSource_DecodingOptions {
    int32_t pixelFormat;
    uint32_t index;
    uint32_t sampleSize;
    uint32_t rotate;
    DynamicRange decodingDynamicRange;
    ResolutionQuality resolutionQuality;
    struct Image_Size desiredSize;
    struct Image_Region desiredRegion;
};

struct OH_ImageSource_ImageInfo {
    /** Image width, in pixels. */
    int32_t width;
    /** Image height, in pixels. */
    int32_t height;
    /** Image is Hdr, in pixels. */
    bool isHdr;
};

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreateDecodingOptions(OH_ImageSource_DecodingOptions **opts)
{
    *opts = new OH_ImageSource_DecodingOptions();
    if (*opts == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetPixelFormat(OH_ImageSource_DecodingOptions *opts,
    int32_t *pixelFormat)
{
    if (opts == nullptr || pixelFormat == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *pixelFormat = opts->pixelFormat;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetPixelFormat(OH_ImageSource_DecodingOptions *opts,
    int32_t pixelFormat)
{
    if (opts == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->pixelFormat = pixelFormat;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetIndex(OH_ImageSource_DecodingOptions *opts, uint32_t *index)
{
    if (opts == nullptr || index == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *index = opts->index;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetIndex(OH_ImageSource_DecodingOptions *opts, uint32_t index)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->index = index;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetRotate(OH_ImageSource_DecodingOptions *opts, float *rotate)
{
    if (opts == nullptr || rotate == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *rotate = opts->rotate;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetRotate(OH_ImageSource_DecodingOptions *opts, float rotate)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->rotate = rotate;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetDesiredSize(OH_ImageSource_DecodingOptions *opts,
    Image_Size *desiredSize)
{
    if (opts == nullptr || desiredSize == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    desiredSize->width = opts->desiredSize.width;
    desiredSize->height = opts->desiredSize.height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetDesiredSize(OH_ImageSource_DecodingOptions *opts,
    Image_Size* desiredSize)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->desiredSize.width = desiredSize->width;
    opts->desiredSize.height = desiredSize->height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetDesiredRegion(OH_ImageSource_DecodingOptions *opts,
    Image_Region *desiredRegion)
{
    if (opts == nullptr || desiredRegion == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    desiredRegion->x = opts->desiredRegion.x;
    desiredRegion->y = opts->desiredRegion.y;
    desiredRegion->width = opts->desiredRegion.width;
    desiredRegion->height = opts->desiredRegion.height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetDesiredRegion(OH_ImageSource_DecodingOptions *opts,
    Image_Region* desiredRegion)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->desiredRegion.x = desiredRegion->x;
    opts->desiredRegion.y = desiredRegion->y;
    opts->desiredRegion.width = desiredRegion->width;
    opts->desiredRegion.height = desiredRegion->height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetResolutionQuality(OH_ImageSource_DecodingOptions *opts,
    ResolutionQuality *resolutionQuality)
{
    if (opts == nullptr || resolutionQuality == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    resolutionQuality = opts->resolutionQuality;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetResolutionQuality(OH_ImageSource_DecodingOptions *opts,
    ResolutionQuality resolutionQuality)
{
    if (opts == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->resolutionQuality = resolutionQuality;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsGetDesiredDynamicRange(OH_ImageSource_DecodingOptions *opts,
    DynamicRange *dynamicRange)
{
    if (opts == nullptr || dynamicRange == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    dynamicRange = opts->dynamicRange;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_DecodingOptionsSetDesiredDynamicRange(OH_ImageSource_DecodingOptions *opts,
    DynamicRange dynamicRange)
{
    if (opts == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    opts->dynamicRange = dynamicRange;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_ReleaseDecodingOptions(OH_ImageSource_DecodingOptions *opts)
{
    if (opts == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    delete opts;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreateImageInfo(OH_ImageSource_ImageInfo **info)
{
    *info = new OH_ImageSource_ImageInfo();
    if (*info == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_ImageInfoGetWidth(OH_ImageSource_ImageInfo *info, uint32_t *width)
{
    if (info == nullptr || width == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *width = info->width;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_ImageInfoGetHeight(OH_ImageSource_ImageInfo *info, uint32_t *height)
{
    if (info == nullptr || height == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *height = info->height;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_ImageInfoGetDynamicRange(OH_ImageSource_ImageInfo *info, bool *isHdr)
{
    if (info == nullptr || height == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    *isHdr = info->isHdr;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_ReleaseImageInfo(OH_ImageSource_ImageInfo *info)
{
    if (info == nullptr ) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    delete info;
    return IMAGE_RESULT_SUCCESS;
}


std::string OH_ImageSource::UrlToPath(const std::string &path)
{
    const std::string filePrefix = "file://";
    if (path.size() > filePrefix.size() &&
        (path.compare(0, filePrefix.size(), filePrefix) == 0)) {
        return path.substr(filePrefix.size());
    }
    return path;
}

static void ParseDecodingOps(DecodeOptions &decOps, struct OH_ImageSource_DecodingOptions* ops)
{
    if (ops->sampleSize != INVALID_SAMPLE_SIZE) {
        decOps.sampleSize = ops->sampleSize;
    }
    decOps.rotateNewDegrees = ops->rotate;
    decOps.desiredSize.width = ops->desiredSize.width;
    decOps.desiredSize.height = ops->desiredSize.height;
    decOps.desiredRegion.left = ops->desiredRegion.x;
    decOps.desiredRegion.top = ops->desiredRegion.y;
    decOps.desiredRegion.width = ops->desiredRegion.width;
    decOps.desiredRegion.height = ops->desiredRegion.height;
    decOps.decodingDynamicRange = ops->decodingDynamicRange;
    decOps.resolutionQuality = ops->resolutionQuality;
    if (ops->pixelFormat <= static_cast<int32_t>(PixelFormat::CMYK)) {
        decOps.desiredPixelFormat = PixelFormat(ops->pixelFormat);
    }
}

static void ParseImageSourceInfo(struct OH_ImageSource_ImageInfo* source, ImageInfo &info)
{
    source->width = info.size.width;
    source->height = info.size.height;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreateFromUri(char* uri, size_t uriSize, OH_ImageSource** res)
{
    if (uri == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    SourceOptions opts;
    auto imageSource = new OH_ImageSource(uri, uriSize, opts);
    if ((imageSource) == nullptr) {
        *res = nullptr;
        return IMAGE_RESULT_SOURCE_DATA;
    }
    std::string tmp(uri, uriSize);
    imageSource->filePath_ = tmp;
    *res = imageSource;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreateFromFd(int32_t fd, OH_ImageSource** res)
{
    SourceOptions opts;
    auto imageSource = new OH_ImageSource(fd, opts);
    if ((imageSource) == nullptr) {
        *res = nullptr;
        return IMAGE_RESULT_SOURCE_DATA;
    }
    imageSource->fileDescriptor_ = fd;
    *res = imageSource;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreateFromData(uint8_t* data, size_t dataSize, OH_ImageSource** res)
{
    if (data == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    SourceOptions opts;
    auto imageSource = new OH_ImageSource(data, dataSize, opts);
    if ((imageSource) == nullptr) {
        *res = nullptr;
        return IMAGE_RESULT_SOURCE_DATA;
    }
    imageSource->fileBuffer_ = (void*)data;
    imageSource->fileBufferSize_ = dataSize;
    *res = imageSource;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreateFromRawFile(RawFileDescriptor rawFile, OH_ImageSource** res)
{
    SourceOptions opts;
    auto imageSource = new OH_ImageSource(rawFile, opts);
    if ((imageSource) == nullptr) {
        *res = nullptr;
        return IMAGE_RESULT_SOURCE_DATA;
    }
    *res = imageSource;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreatePixelMap(OH_ImageSource* source, OH_ImageSource_DecodingOptions* ops,
    OH_Pixelmap** resPixMap)
{
    if (source == nullptr || ops == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    DecodeOptions decOps;
    uint32_t index = DEFAULT_INDEX;
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    ParseDecodingOps(decOps, ops);
    index = ops->index;
    std::unique_ptr<PixelMap> tmpPixelmap = source->GetInnerImageSource()->CreatePixelMapEx(index, decOps, errorCode);
    if (tmpPixelmap == nullptr) {
        return IMAGE_RESULT_ADD_PIXEL_MAP_FAILED;
    }
    std::shared_ptr<PixelMap> nativePixelMap = std::move(tmpPixelmap);
    OH_Pixelmap* stPixMap = new OH_Pixelmap(nativePixelMap);
    *resPixMap = stPixMap;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_CreatePixelMapList(OH_ImageSource* source, OH_ImageSource_DecodingOptions* ops,
        OH_Pixelmap** resVecPixMap[], size_t* outSize)
{
    if (source == nullptr || ops == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    DecodeOptions decOps;
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    if (ops != nullptr) {
        ParseDecodingOps(decOps, ops);
    }
    auto pixelMapList = source->GetInnerImageSource()->CreatePixelMapList(decOps, errorCode);
    if (pixelMapList == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    size_t index = 0;
    for (auto &item : *pixelMapList) {
        std::shared_ptr<PixelMap> tempPixMap = std::move(item);
        OH_Pixelmap* stPixMap = new OH_Pixelmap(tempPixMap);
        *resVecPixMap[index] = stPixMap;
        index ++;
    }
    *outSize = index;
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_GetDelayTime(OH_ImageSource* source, int32_t* delayTimeList, size_t* size)
{
    if (source == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    auto delayTimes = source->GetInnerImageSource()->GetDelayTime(errorCode);
    if (delayTimes == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    size_t actCount = (*delayTimes).size();
    if (delayTimeList == nullptr) {
        *size = actCount;
        return IMAGE_RESULT_SUCCESS;
    }
    if (actCount > *size) {
        actCount = *size;
    }
    for (size_t i = SIZE_ZERO; i < actCount; i++) {
        delayTimeList[i] = (*delayTimes)[i];
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_GetImageInfo(OH_ImageSource* source, int32_t index,
    struct OH_ImageSource_ImageInfo* info)
{
    if (source == nullptr || info == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ImageInfo imageInfo;
    uint32_t errorCode = source->GetInnerImageSource()->GetImageInfo(index, imageInfo);
    if (errorCode != IMAGE_RESULT_SUCCESS) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    ParseImageSourceInfo(info, imageInfo);
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_GetImageProperty(OH_ImageSource* source, Image_String* key,
    Image_String* value)
{
    if (source == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (key == nullptr || key->format == nullptr || key->size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (value == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    std::string keyString(key->format, key->size);
    std::string val;
    uint32_t errorCode = source->GetInnerImageSource()->GetImagePropertyString(DEFAULT_INDEX, keyString, val);
    if (errorCode != IMAGE_RESULT_SUCCESS || val.empty()) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (value->format == nullptr) {
        value->size = val.size();
        return IMAGE_RESULT_SUCCESS;
    }
    memcpy_s(value->format, value->size, val.c_str(), val.size());
    if (value->size > val.size()) {
        value->size = val.size();
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_ModifyImageProperty(OH_ImageSource* source, Image_String* key,
    Image_String* value)
{
    if (source == nullptr || source == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (key == nullptr || key->format == nullptr || key->size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (value == nullptr || value->format == nullptr || value->size == SIZE_ZERO) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }

    std::string keyStr(key->format, key->size);
    std::string val(value->format, value->size);
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    if (!(source->filePath_.empty())) {
        errorCode = source->GetInnerImageSource()->ModifyImageProperty(DEFAULT_INDEX, keyStr, val, source->filePath_);
    } else if (source->fileDescriptor_ != INVALID_FD) {
        errorCode = source->GetInnerImageSource()->ModifyImageProperty(DEFAULT_INDEX, keyStr, val,
            source->fileDescriptor_ );
    } else if (source->fileBuffer_ != nullptr && source->fileBufferSize_ != 0) {
        errorCode = source->GetInnerImageSource()->ModifyImageProperty(DEFAULT_INDEX, keyStr, val,
            static_cast<uint8_t *>(source->fileBuffer_), source->fileBufferSize_);
    } else {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    if (errorCode == IMAGE_RESULT_SUCCESS) {
        return IMAGE_RESULT_SUCCESS;
    }
    return IMAGE_RESULT_BAD_PARAMETER;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_GetFrameCount(OH_ImageSource* source, uint32_t* frameCount)
{
    if (source == nullptr || frameCount == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    uint32_t errorCode = ERR_MEDIA_INVALID_VALUE;
    *frameCount = source->GetInnerImageSource()->GetFrameCount(errorCode);
    if (errorCode != IMAGE_RESULT_SUCCESS) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    return IMAGE_RESULT_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageSource2_Release(OH_ImageSource* source)
{
    if (source == nullptr) {
        return IMAGE_RESULT_BAD_PARAMETER;
    }
    source->~OH_ImageSource();
    return IMAGE_RESULT_SUCCESS;
}
#ifdef __cplusplus
};
#endif