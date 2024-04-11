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

#include "vpe_utils.h"

#include "hilog/log.h"
#include "log_tags.h"
#include "image_log.h"
#include "v1_0/buffer_handle_meta_key_type.h"
#include "metadata_convertor.h"
#include "external_window.h"
#include "native_window.h"
#include <dlfcn.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "VpeUtils"

namespace OHOS {
namespace Media {
using namespace OHOS::HDI::Display::Graphic::Common::V1_0;
static constexpr uint32_t TRANSFUNC_OFFSET = 8;
static constexpr uint32_t MATRIX_OFFSET = 16;
static constexpr uint32_t RANGE_OFFSET = 21;
const static char* VPE_SO_NAME = "libvideoprocessingengine.z.so";
void* VpeUtils::vpeHandle_ = nullptr;
int32_t VpeUtils::instanceHdrId_ = VPE_ERROR_FAILED;
int32_t VpeUtils::instanceSrId_ = VPE_ERROR_FAILED;
using CreateT = int32_t (*)(int32_t*);
using ImageProcessT =
    int32_t (*)(int32_t, OHNativeWindowBuffer*, OHNativeWindowBuffer*);
using DestoryT = int32_t (*)(int32_t*);

VpeUtils::VpeUtils()
{
}

VpeUtils::~VpeUtils()
{   
    DetailEnhancerDestory(&instanceSrId_);
    ColorSpaceConverterDestory(&instanceHdrId_);
    dlclose(vpeHandle_);
    instanceSrId_ = VPE_ERROR_FAILED;
    instanceHdrId_ = VPE_ERROR_FAILED;
}

int32_t VpeUtils::ColorSpaceConverterCreate(int32_t* instanceId)
{
    if (!vpeHandle_) {
        vpeHandle_ = dlopen(VPE_SO_NAME, RTLD_LAZY);
    }
    if (vpeHandle_ == nullptr) {
        return VPE_ERROR_FAILED;
    }
    CreateT create = (CreateT)dlsym(vpeHandle_, "ColorSpaceConverter_Create");
    if (!create) {
        return VPE_ERROR_FAILED;
    }
    return create(instanceId);
}

int32_t VpeUtils::ColorSpaceConverterDestory(int32_t* instanceId)
{
    if (*instanceId == VPE_ERROR_FAILED) {
        return VPE_ERROR_FAILED;
    }
    if (!vpeHandle_) {
        vpeHandle_ = dlopen(VPE_SO_NAME, RTLD_LAZY);
        if (vpeHandle_ == nullptr) {
            return VPE_ERROR_FAILED;
        }
    }
    DestoryT destory = (DestoryT)dlsym(vpeHandle_, "ColorSpaceConverter_Destory");
    if (!destory) {
        return VPE_ERROR_FAILED;
    }
    return destory(instanceId);
}

int32_t VpeUtils::ColorSpaceConverterImageProcess(sptr<SurfaceBuffer> & input, sptr<SurfaceBuffer> & output)
{
    std::lock_guard<std::mutex> lock(vpeMtx_);
    if (!vpeHandle_) {
        vpeHandle_ = dlopen(VPE_SO_NAME, RTLD_LAZY);
        if (vpeHandle_ == nullptr) {
            return VPE_ERROR_FAILED;
        }
    }
    int32_t res;
    if (instanceHdrId_ == VPE_ERROR_FAILED) {
        res = ColorSpaceConverterCreate(&instanceHdrId_);
        if (instanceHdrId_ == VPE_ERROR_FAILED || res != VPE_ERROR_OK) {
            instanceHdrId_ = VPE_ERROR_FAILED;
            return VPE_ERROR_FAILED;
        }
    }
    ImageProcessT imageProcess = (ImageProcessT)dlsym(vpeHandle_, "ColorSpaceConverter_ProcessImage");
    if (!imageProcess) {
        return VPE_ERROR_FAILED;
    }
    if (input == nullptr || output == nullptr) {
        return VPE_ERROR_FAILED;
    }
    OHNativeWindowBuffer* sdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&input);

    OHNativeWindowBuffer* hdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&output);
    res = imageProcess(instanceHdrId_, sdr, hdr);
    OH_NativeWindow_DestroyNativeWindowBuffer(sdr);
    OH_NativeWindow_DestroyNativeWindowBuffer(hdr);
    return res;
}

int32_t VpeUtils::DetailEnhancerCreate(int32_t* instanceId)
{
    if (!vpeHandle_) {
        vpeHandle_ = dlopen(VPE_SO_NAME, RTLD_LAZY);
    }
    if (vpeHandle_ == nullptr) {
        return VPE_ERROR_FAILED;
    }
    CreateT create = (CreateT)dlsym(vpeHandle_, "DetailEnhancer_Create");
    if (!create) {
        return VPE_ERROR_FAILED;
    }
    return create(instanceId);
}

int32_t VpeUtils::DetailEnhancerDestory(int32_t* instanceId)
{
    if (*instanceId == VPE_ERROR_FAILED) {
        return VPE_ERROR_FAILED;
    }
    if (!vpeHandle_) {
        vpeHandle_ = dlopen(VPE_SO_NAME, RTLD_LAZY);
        if (vpeHandle_ == nullptr) {
            return VPE_ERROR_FAILED;
        }
    }
    DestoryT destory = (DestoryT)dlsym(vpeHandle_, "DetailEnhancer_Destory");
    if (!destory) {
        return VPE_ERROR_FAILED;
    }
    return destory(instanceId);
}

int32_t VpeUtils::DetailEnhancerImageProcess(sptr<SurfaceBuffer> & input, sptr<SurfaceBuffer> & output)
{
    std::lock_guard<std::mutex> lock(vpeMtx_);
    if (!vpeHandle_) {
        vpeHandle_ = dlopen(VPE_SO_NAME, RTLD_LAZY);
        if (vpeHandle_ == nullptr) {
            return VPE_ERROR_FAILED;
        }
    }
    int32_t res;
    if (instanceSrId_ == VPE_ERROR_FAILED) {
        res = DetailEnhancerCreate(&instanceSrId_);
        if (instanceSrId_ == VPE_ERROR_FAILED || res != VPE_ERROR_OK) {
            instanceSrId_ = VPE_ERROR_FAILED;
            return VPE_ERROR_FAILED;
        }
    }
    ImageProcessT imageProcess = (ImageProcessT)dlsym(vpeHandle_, "DetailEnhancer_ProcessImage");
    if (!imageProcess) {
        return VPE_ERROR_FAILED;
    }
    if (input == nullptr || output == nullptr) {
        return VPE_ERROR_FAILED;
    }
    OHNativeWindowBuffer* sdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&input);

    OHNativeWindowBuffer* hdr = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&output);
    res = imageProcess(instanceSrId_, sdr, hdr);
    OH_NativeWindow_DestroyNativeWindowBuffer(sdr);
    OH_NativeWindow_DestroyNativeWindowBuffer(hdr);
    return res;
}

// surfacebuffer metadata
static GSError SetColorSpaceInfo(sptr<SurfaceBuffer>& buffer, const CM_ColorSpaceInfo& colorSpaceInfo)
{
    std::vector<uint8_t> colorSpaceInfoVec;
    auto ret = MetadataManager::ConvertMetadataToVec(colorSpaceInfo, colorSpaceInfoVec);
    if (ret != GSERROR_OK) {
        return ret;
    }
    return buffer->SetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceInfoVec);
}

static bool GetColorSpaceInfo(const sptr<SurfaceBuffer>& buffer, CM_ColorSpaceInfo& colorSpaceInfo)
{
    std::vector<uint8_t> colorSpaceInfoVec;
    auto ret = buffer->GetMetadata(ATTRKEY_COLORSPACE_INFO, colorSpaceInfoVec);
    if (ret != GSERROR_OK) {
        IMAGE_LOGE("GetColorSpaceInfo GetMetadata failed, return value is %{public}d", ret);
        return false;
    }
    return MetadataManager::ConvertVecToMetadata(colorSpaceInfoVec, colorSpaceInfo) == GSERROR_OK;
}

bool VpeUtils::SetSbColorSpaceType(sptr<SurfaceBuffer>& buffer, const CM_ColorSpaceType& colorSpaceType)
{
    CM_ColorSpaceInfo colorSpaceInfo;
    uint32_t colorSpace = static_cast<uint32_t>(colorSpaceType);
    colorSpaceInfo.primaries = static_cast<CM_ColorPrimaries>(colorSpace & CM_PRIMARIES_MASK);
    colorSpaceInfo.transfunc = static_cast<CM_TransFunc>((colorSpace & CM_TRANSFUNC_MASK) >> TRANSFUNC_OFFSET);
    colorSpaceInfo.matrix = static_cast<CM_Matrix>((colorSpace & CM_MATRIX_MASK) >> MATRIX_OFFSET);
    colorSpaceInfo.range = static_cast<CM_Range>((colorSpace & CM_RANGE_MASK) >> RANGE_OFFSET);
    auto ret = SetColorSpaceInfo(buffer, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        IMAGE_LOGE("SetColorSpaceInfo GetMetadata failed, return value is %{public}d", ret);
        return false;
    }
    return true;
}

bool VpeUtils::GetSbColorSpaceType(const sptr<SurfaceBuffer>& buffer, CM_ColorSpaceType& colorSpaceType)
{
    CM_ColorSpaceInfo colorSpaceInfo;
    if (!GetColorSpaceInfo(buffer, colorSpaceInfo)) {
        return false;
    }
    uint32_t primaries = static_cast<uint32_t>(colorSpaceInfo.primaries);
    uint32_t transfunc = static_cast<uint32_t>(colorSpaceInfo.transfunc);
    uint32_t matrix = static_cast<uint32_t>(colorSpaceInfo.matrix);
    uint32_t range = static_cast<uint32_t>(colorSpaceInfo.range);
    colorSpaceType = static_cast<CM_ColorSpaceType>(primaries | (transfunc << TRANSFUNC_OFFSET) |
        (matrix << MATRIX_OFFSET) | (range << RANGE_OFFSET));
    return true;
}

bool VpeUtils::SetSbMetadataType(sptr<SurfaceBuffer>& buffer, const CM_HDR_Metadata_Type& metadataType)
{
    std::vector<uint8_t> hdrMetadataTypeVec;
    auto ret = MetadataManager::ConvertMetadataToVec(metadataType, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        return false;
    }
    ret = buffer->SetMetadata(ATTRKEY_HDR_METADATA_TYPE, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        return false;
    }
    return true;
}

bool VpeUtils::GetSbMetadataType(const sptr<SurfaceBuffer>& buffer, CM_HDR_Metadata_Type& metadataType)
{
    std::vector<uint8_t> hdrMetadataTypeVec;
    auto ret = buffer->GetMetadata(ATTRKEY_HDR_METADATA_TYPE, hdrMetadataTypeVec);
    if (ret != GSERROR_OK) {
        IMAGE_LOGE("HdrUtils::GetHDRMetadataType GetMetadata failed, return value is %{public}d", ret);
        return false;
    }
    return MetadataManager::ConvertVecToMetadata(hdrMetadataTypeVec, metadataType) == GSERROR_OK;
}

bool VpeUtils::SetSbDynamicMetadata(sptr<SurfaceBuffer>& buffer, const std::vector<uint8_t>& dynamicMetadata)
{
    return buffer->SetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, dynamicMetadata) == GSERROR_OK;
}

bool VpeUtils::GetSbDynamicMetadata(const sptr<SurfaceBuffer>& buffer, std::vector<uint8_t>& dynamicMetadata)
{
    return buffer->GetMetadata(ATTRKEY_HDR_DYNAMIC_METADATA, dynamicMetadata) == GSERROR_OK;
}

bool VpeUtils::SetSbStaticMetadata(sptr<SurfaceBuffer>& buffer, const std::vector<uint8_t>& staticMetadata)
{
    return buffer->SetMetadata(ATTRKEY_HDR_STATIC_METADATA, staticMetadata) == GSERROR_OK;
}

bool VpeUtils::GetSbStaticMetadata(const sptr<SurfaceBuffer>& buffer, std::vector<uint8_t>& staticMetadata)
{
    return buffer->GetMetadata(ATTRKEY_HDR_STATIC_METADATA, staticMetadata) == GSERROR_OK;
}
}
}