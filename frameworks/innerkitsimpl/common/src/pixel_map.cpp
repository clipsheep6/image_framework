/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "pixel_map.h"
#include <iostream>
#include <unistd.h>

#include "image_system_properties.h"
#include "image_trace.h"
#include "image_type_converter.h"
#include "image_utils.h"
#include "memory_manager.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "hilog/log.h"
#include "hitrace_meter.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_convert_adapter.h"
#include "pixel_map_utils.h"
#include "post_proc.h"
#include "parcel.h"
#include "pubdef.h"

#ifndef _WIN32
#include "securec.h"
#else
#include "memory.h"
#endif

#ifdef IMAGE_PURGEABLE_PIXELMAP
#include "purgeable_resource_manager.h"
#endif

#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
#include <sys/mman.h>
#include "ashmem.h"
#include "buffer_handle_parcel.h"
#include "ipc_file_descriptor.h"
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
using namespace std;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "PixelMap" };
constexpr int32_t MAX_DIMENSION = INT32_MAX >> 2;
constexpr uint8_t FOUR_BYTE_SHIFT = 2;
constexpr int8_t INVALID_ALPHA_INDEX = -1;
constexpr uint8_t ARGB_ALPHA_INDEX = 0;
constexpr uint8_t BGRA_ALPHA_INDEX = 3;
constexpr uint8_t ALPHA_BYTES = 1;
constexpr uint8_t BGRA_BYTES = 4;
constexpr uint8_t RGBA_F16_BYTES = 8;
constexpr uint8_t PER_PIXEL_LEN = 1;

constexpr uint8_t FILL_NUMBER = 3;
constexpr uint8_t ALIGN_NUMBER = 4;

constexpr int32_t AntiAliasingSize = 350;
PixelMap::~PixelMap()
{
#ifdef IMAGE_PURGEABLE_PIXELMAP
    if (purgeableMemPtr_) {
        PurgeableMem::PurgeableResourceManager::GetInstance().RemoveResource(purgeableMemPtr_);
        purgeableMemPtr_.reset();
        purgeableMemPtr_ = nullptr;
    }
#endif
    FreePixelMap();
}

void PixelMap::FreePixelMap() __attribute__((no_sanitize("cfi")))
{
    if (data_ == nullptr) {
        return;
    }

    if (freePixelMapProc_ != nullptr) {
        freePixelMapProc_(data_, context_, pixelsSize_);
    }
    
    switch (allocatorType_) {
        case AllocatorType::HEAP_ALLOC: {
            free(data_);
            data_ = nullptr;
            break;
        }
        case AllocatorType::CUSTOM_ALLOC: {
            if (custFreePixelMap_ != nullptr) {
                custFreePixelMap_(data_, context_, pixelsSize_);
            }
            data_ = nullptr;
            context_ = nullptr;
            break;
        }
        case AllocatorType::SHARE_MEM_ALLOC: {
            ReleaseSharedMemory(data_, context_, pixelsSize_);
            data_ = nullptr;
            context_ = nullptr;
            break;
        }
        case AllocatorType::DMA_ALLOC: {
#if !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
            ImageUtils::SurfaceBuffer_Unreference(static_cast<SurfaceBuffer*>(context_));
            data_ = nullptr;
            context_ = nullptr;
#endif
            break;
        }
        default: {
            HiLog::Error(LABEL, "unknown allocator type:[%{public}d].", allocatorType_);
            return;
        }
    }
}

void PixelMap::ReleaseSharedMemory(void *addr, void *context, uint32_t size)
{
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    int *fd = static_cast<int *>(context);
    if (addr != nullptr) {
        ::munmap(addr, size);
    }
    if (fd != nullptr) {
        ::close(*fd);
        delete fd;
    }
#endif
}

void PixelMap::SetFreePixelMapProc(CustomFreePixelMap func)
{
    freePixelMapProc_ = func;
}

void PixelMap::SetTransformered(bool isTransformered)
{
    std::unique_lock<std::mutex> lock(*transformMutex_);
    isTransformered_ = isTransformered;
}

void PixelMap::SetPixelsAddr(void *addr, void *context, uint32_t size, AllocatorType type, CustomFreePixelMap func)
{
    if (data_ != nullptr) {
        HiLog::Error(LABEL, "SetPixelsAddr error ");
        FreePixelMap();
    }
    if (type == AllocatorType::SHARE_MEM_ALLOC && context == nullptr) {
        HiLog::Error(LABEL, "SetPixelsAddr error type %{public}d ", type);
    }
    data_ = static_cast<uint8_t *>(addr);
    context_ = context;
    pixelsSize_ = size;
    allocatorType_ = type;
    custFreePixelMap_ = func;
    if (type == AllocatorType::DMA_ALLOC && rowDataSize_ != 0) {
        UpdateImageInfo();
    }
}

bool CheckConvertParmas(const ImageInfo &src, const ImageInfo &dst)
{
    return src.size.width == dst.size.width &&
        src.size.height == dst.size.height &&
        src.alphaType == dst.alphaType;
}

bool CheckPixelmap(std::unique_ptr<PixelMap> &pixelMap, ImageInfo &imageInfo)
{
    if (pixelMap->SetImageInfo(imageInfo) != SUCCESS) {
        HiLog::Error(LABEL, "set image info fail");
        return false;
    }
    uint32_t bufferSize = pixelMap->GetByteCount();
    if (bufferSize == 0 || bufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        HiLog::Error(LABEL, "AllocSharedMemory parameter is zero");
        return false;
    }
    return true;
}

unique_ptr<PixelMap> PixelMap::Create(const uint32_t *colors, uint32_t colorLength, const InitializationOptions &opts)
{
    HiLog::Info(LABEL, "PixelMap::Create1 enter");
    return Create(colors, colorLength, 0, opts.size.width, opts);
}

unique_ptr<PixelMap> PixelMap::Create(const uint32_t *colors, uint32_t colorLength, int32_t offset, int32_t stride,
                                      const InitializationOptions &opts)
{
    HiLog::Info(LABEL, "PixelMap::Create2 enter");
    return Create(colors, colorLength, 0, opts.size.width, opts, false);
}

unique_ptr<PixelMap> PixelMap::Create(const uint32_t *colors, uint32_t colorLength, int32_t offset, int32_t stride,
                                      const InitializationOptions &opts, bool useCustomFormat)
{
    HiLog::Info(LABEL, "PixelMap::Create useCustomFormat enter");
    if (!CheckParams(colors, colorLength, offset, stride, opts)) {
        return nullptr;
    }
    unique_ptr<PixelMap> dstPixelMap = make_unique<PixelMap>();
    if (dstPixelMap == nullptr) {
        HiLog::Error(LABEL, "create pixelMap pointer fail");
        return nullptr;
    }

    PixelFormat format = PixelFormat::BGRA_8888;
    if (useCustomFormat) {
        format = ((opts.srcPixelFormat == PixelFormat::UNKNOWN) ? PixelFormat::BGRA_8888 : opts.srcPixelFormat);
    }
    ImageInfo srcImageInfo =
        MakeImageInfo(stride, opts.size.height, format, AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL);
    PixelFormat dstPixelFormat = (opts.pixelFormat == PixelFormat::UNKNOWN ? PixelFormat::RGBA_8888 : opts.pixelFormat);
    AlphaType dstAlphaType =
        (opts.alphaType == AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN) ? AlphaType::IMAGE_ALPHA_TYPE_PREMUL : opts.alphaType;
    dstAlphaType = ImageUtils::GetValidAlphaTypeByFormat(dstAlphaType, dstPixelFormat);
    ImageInfo dstImageInfo = MakeImageInfo(opts.size.width, opts.size.height, dstPixelFormat, dstAlphaType);
    if (!CheckPixelmap(dstPixelMap, dstImageInfo)) {
        HiLog::Error(LABEL, "check pixelMap pointer fail");
        return nullptr;
    }
    int fd = 0;
    uint32_t bufferSize = dstPixelMap->GetByteCount();
    void *dstPixels = AllocSharedMemory(bufferSize, fd, dstPixelMap->GetUniqueId());
    if (dstPixels == nullptr) {
        HiLog::Error(LABEL, "allocate memory size %{public}u fail", bufferSize);
        return nullptr;
    }

    Position dstPosition;
    if (!CheckConvertParmas(srcImageInfo, dstImageInfo) &&
        !PixelConvertAdapter::WritePixelsConvert(reinterpret_cast<const void *>(colors + offset),
        static_cast<uint32_t>(stride) << FOUR_BYTE_SHIFT, srcImageInfo,
        dstPixels, dstPosition, dstPixelMap->GetRowBytes(), dstImageInfo)) {
        HiLog::Error(LABEL, "pixel convert in adapter failed.");
        ReleaseBuffer(AllocatorType::SHARE_MEM_ALLOC, fd, bufferSize, &dstPixels);
        dstPixels = nullptr;
        return nullptr;
    }

    void *fdBuffer = new int32_t();
    *static_cast<int32_t *>(fdBuffer) = fd;
    dstPixelMap->SetEditable(opts.editable);
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    dstPixelMap->SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    dstPixelMap->SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return dstPixelMap;
}

void PixelMap::ReleaseBuffer(AllocatorType allocatorType, int fd, uint64_t dataSize, void **buffer)
{
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType == AllocatorType::SHARE_MEM_ALLOC) {
        if (*buffer != nullptr) {
            ::munmap(*buffer, dataSize);
            ::close(fd);
        }
        return;
    }
#endif

    if (allocatorType == AllocatorType::HEAP_ALLOC) {
        if (*buffer != nullptr) {
            free(*buffer);
            *buffer = nullptr;
        }
        return;
    }
}

void *PixelMap::AllocSharedMemory(const uint64_t bufferSize, int &fd, uint32_t uniqueId)
{
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    std::string name = "PixelMap RawData, uniqueId: " + std::to_string(getpid()) + '_' + std::to_string(uniqueId);
    fd = AshmemCreate(name.c_str(), bufferSize);
    if (fd < 0) {
        HiLog::Error(LABEL, "AllocSharedMemory fd error");
        return nullptr;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        HiLog::Error(LABEL, "AshmemSetProt error");
        ::close(fd);
        return nullptr;
    }
    void* ptr = ::mmap(nullptr, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        HiLog::Error(LABEL, "mmap error, errno: %{public}s, fd %{public}d, bufferSize %{public}lld",
            strerror(errno), fd, (long long)bufferSize);
        ::close(fd);
        return nullptr;
    }
    return ptr;
#else
    return malloc(bufferSize);
#endif
}

bool PixelMap::CheckParams(const uint32_t *colors, uint32_t colorLength, int32_t offset, int32_t stride,
                           const InitializationOptions &opts)
{
    if (colors == nullptr || colorLength <= 0 || colorLength > PIXEL_MAP_MAX_RAM_SIZE) {
        HiLog::Error(LABEL, "colors invalid");
        return false;
    }
    int32_t dstWidth = opts.size.width;
    int32_t dstHeight = opts.size.height;
    if (dstWidth <= 0 || dstHeight <= 0) {
        HiLog::Error(LABEL, "initial options size invalid");
        return false;
    }
    if (stride < dstWidth) {
        HiLog::Error(LABEL, "stride: %{public}d must >= width: %{public}d", stride, dstWidth);
        return false;
    }
    if (stride > MAX_DIMENSION) {
        HiLog::Error(LABEL, "stride %{public}d is out of range", stride);
        return false;
    }
    int64_t lastLine = static_cast<int64_t>(dstHeight - 1) * stride + offset;
    if (offset < 0 || static_cast<int64_t>(offset) + dstWidth > colorLength || lastLine + dstWidth > colorLength) {
        HiLog::Error(LABEL, "colors length: %{public}u, offset: %{public}d, stride: %{public}d  is invalid",
                     colorLength, offset, stride);
        return false;
    }
    return true;
}

unique_ptr<PixelMap> PixelMap::Create(const InitializationOptions &opts)
{
    HiLog::Info(LABEL, "PixelMap::Create3 enter");
    unique_ptr<PixelMap> dstPixelMap = make_unique<PixelMap>();
    if (dstPixelMap == nullptr) {
        HiLog::Error(LABEL, "create pixelMap pointer fail");
        return nullptr;
    }
    PixelFormat dstPixelFormat = (opts.pixelFormat == PixelFormat::UNKNOWN ? PixelFormat::RGBA_8888 : opts.pixelFormat);
    AlphaType dstAlphaType =
        (opts.alphaType == AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN) ? AlphaType::IMAGE_ALPHA_TYPE_PREMUL : opts.alphaType;
    dstAlphaType = ImageUtils::GetValidAlphaTypeByFormat(dstAlphaType, dstPixelFormat);
    ImageInfo dstImageInfo = MakeImageInfo(opts.size.width, opts.size.height, dstPixelFormat, dstAlphaType);
    if (dstPixelMap->SetImageInfo(dstImageInfo) != SUCCESS) {
        HiLog::Error(LABEL, "set image info fail");
        return nullptr;
    }
    uint32_t bufferSize = dstPixelMap->GetByteCount();
    if (bufferSize == 0 || bufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        HiLog::Error(LABEL, "calloc parameter bufferSize:[%{public}d] error.", bufferSize);
        return nullptr;
    }
    int fd = 0;
    void *dstPixels = AllocSharedMemory(bufferSize, fd, dstPixelMap->GetUniqueId());
    if (dstPixels == nullptr) {
        HiLog::Error(LABEL, "allocate memory size %{public}u fail", bufferSize);
        return nullptr;
    }
    // update alpha opaque
    UpdatePixelsAlpha(dstImageInfo.alphaType, dstImageInfo.pixelFormat,
                      static_cast<uint8_t *>(dstPixels), *dstPixelMap.get());
    void *fdBuffer = new int32_t();
    *static_cast<int32_t *>(fdBuffer) = fd;
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    dstPixelMap->SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    dstPixelMap->SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);
#endif
    dstPixelMap->SetEditable(opts.editable);
    return dstPixelMap;
}

void PixelMap::UpdatePixelsAlpha(const AlphaType &alphaType, const PixelFormat &pixelFormat, uint8_t *dstPixels,
                                 PixelMap dstPixelMap)
{
    if (alphaType == AlphaType::IMAGE_ALPHA_TYPE_OPAQUE) {
        int8_t alphaIndex = -1;
        if (pixelFormat == PixelFormat::RGBA_8888 || pixelFormat == PixelFormat::BGRA_8888) {
            alphaIndex = BGRA_ALPHA_INDEX;
        } else if (pixelFormat == PixelFormat::ARGB_8888) {
            alphaIndex = 0;
        }
        if (alphaIndex != -1) {
            uint8_t pixelBytes = dstPixelMap.GetPixelBytes();
            uint32_t bufferSize = dstPixelMap.GetByteCount();
            uint32_t i = alphaIndex;
            while (i < bufferSize) {
                dstPixels[i] = ALPHA_OPAQUE;
                i += pixelBytes;
            }
        }
    }
}

unique_ptr<PixelMap> PixelMap::Create(PixelMap &source, const InitializationOptions &opts)
{
    HiLog::Info(LABEL, "PixelMap::Create4 enter");
    Rect rect;
    return Create(source, rect, opts);
}

unique_ptr<PixelMap> PixelMap::Create(PixelMap &source, const Rect &srcRect, const InitializationOptions &opts)
{
    HiLog::Info(LABEL, "PixelMap::Create5 enter");
    ImageInfo srcImageInfo;
    source.GetImageInfo(srcImageInfo);
    PostProc postProc;
    Rect sRect = srcRect;
    CropValue cropType = PostProc::ValidCropValue(sRect, srcImageInfo.size);
    if (cropType == CropValue::INVALID) {
        HiLog::Error(LABEL, "src crop range is invalid");
        return nullptr;
    }
    ImageInfo dstImageInfo;
    InitDstImageInfo(opts, srcImageInfo, dstImageInfo);
    Size targetSize = dstImageInfo.size;
    // use source if match
    bool isHasConvert = postProc.HasPixelConvert(srcImageInfo, dstImageInfo);
    if (opts.useSourceIfMatch && !source.IsEditable() && !opts.editable && (cropType == CropValue::NOCROP) &&
        !isHasConvert && IsSameSize(srcImageInfo.size, dstImageInfo.size)) {
        source.useSourceAsResponse_ = true;
        return unique_ptr<PixelMap>(&source);
    }
    unique_ptr<PixelMap> dstPixelMap = make_unique<PixelMap>();
    if (dstPixelMap == nullptr) {
        HiLog::Error(LABEL, "create pixelmap pointer fail");
        return nullptr;
    }
    if (cropType == CropValue::VALID) {
        dstImageInfo.size.width = sRect.width;
        dstImageInfo.size.height = sRect.height;
    } else {
        dstImageInfo.size = srcImageInfo.size;
    }
    if (dstPixelMap->SetImageInfo(dstImageInfo) != SUCCESS) {
        return nullptr;
    }
    // dst pixelmap is source crop and convert pixelmap
    if ((cropType == CropValue::VALID) || isHasConvert) {
        if (!SourceCropAndConvert(source, srcImageInfo, dstImageInfo, sRect, *dstPixelMap.get())) {
            return nullptr;
        }
    } else {
        // only maybe size changed, copy source as scale operation
        if (!CopyPixelMap(source, *dstPixelMap.get())) {
            return nullptr;
        }
    }
    if (!ScalePixelMap(targetSize, dstImageInfo.size, opts.scaleMode, *dstPixelMap.get())) {
        return nullptr;
    }
    dstPixelMap->SetEditable(opts.editable);
    return dstPixelMap;
}

bool PixelMap::SourceCropAndConvert(PixelMap &source, const ImageInfo &srcImageInfo, const ImageInfo &dstImageInfo,
                                    const Rect &srcRect, PixelMap &dstPixelMap)
{
    uint32_t bufferSize = dstPixelMap.GetByteCount();
    if (bufferSize == 0 || bufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        HiLog::Error(LABEL, "AllocSharedMemory  parameter bufferSize:[%{public}d] error.", bufferSize);
        return false;
    }
    int fd = 0;
    void *dstPixels = nullptr;
    if (source.GetAllocatorType() == AllocatorType::SHARE_MEM_ALLOC) {
        dstPixels = AllocSharedMemory(bufferSize, fd, dstPixelMap.GetUniqueId());
    } else {
        dstPixels = malloc(bufferSize);
    }
    if (dstPixels == nullptr) {
        HiLog::Error(LABEL, "source crop allocate memory fail allocatetype: %{public}d ", source.GetAllocatorType());
        return false;
    }

    if (memset_s(dstPixels, bufferSize, 0, bufferSize) != EOK) {
        HiLog::Error(LABEL, "dstPixels memset_s failed.");
    }
    Position srcPosition { srcRect.left, srcRect.top };
    if (!PixelConvertAdapter::ReadPixelsConvert(source.GetPixels(), srcPosition, source.GetRowStride(), srcImageInfo,
        dstPixels, dstPixelMap.GetRowStride(), dstImageInfo)) {
        HiLog::Error(LABEL, "pixel convert in adapter failed.");
        ReleaseBuffer(fd > 0 ? AllocatorType::SHARE_MEM_ALLOC : AllocatorType::HEAP_ALLOC, fd, bufferSize, &dstPixels);
        return false;
    }

    if (fd <= 0) {
        dstPixelMap.SetPixelsAddr(dstPixels, nullptr, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);
        return true;
    }
    void *fdBuffer = new int32_t();
    *static_cast<int32_t *>(fdBuffer) = fd;
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    dstPixelMap.SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    dstPixelMap.SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return true;
}

bool PixelMap::ScalePixelMap(const Size &targetSize, const Size &dstSize, const ScaleMode &scaleMode,
                             PixelMap &dstPixelMap)
{
    if (dstSize.width == targetSize.width && dstSize.height == targetSize.height) {
        return true;
    }
    PostProc postProc;
    if (scaleMode == ScaleMode::FIT_TARGET_SIZE) {
        if (!postProc.ScalePixelMap(targetSize, dstPixelMap)) {
            HiLog::Error(LABEL, "scale FIT_TARGET_SIZE fail");
            return false;
        }
    }
    if (scaleMode == ScaleMode::CENTER_CROP) {
        if (!postProc.CenterScale(targetSize, dstPixelMap)) {
            HiLog::Error(LABEL, "scale CENTER_CROP fail");
            return false;
        }
    }
    return true;
}

void PixelMap::InitDstImageInfo(const InitializationOptions &opts, const ImageInfo &srcImageInfo,
                                ImageInfo &dstImageInfo)
{
    dstImageInfo.size = opts.size;
    if (dstImageInfo.size.width == 0 && dstImageInfo.size.height == 0) {
        dstImageInfo.size = srcImageInfo.size;
    }
    dstImageInfo.pixelFormat = opts.pixelFormat;
    if (dstImageInfo.pixelFormat == PixelFormat::UNKNOWN) {
        dstImageInfo.pixelFormat = srcImageInfo.pixelFormat;
    }
    dstImageInfo.alphaType = opts.alphaType;
    if (dstImageInfo.alphaType == AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN) {
        dstImageInfo.alphaType = srcImageInfo.alphaType;
    }
}

bool PixelMap::CopyPixMapToDst(PixelMap &source, void* &dstPixels, int &fd, uint32_t bufferSize)
{
    if (source.GetAllocatorType() == AllocatorType::DMA_ALLOC) {
        ImageInfo imageInfo;
        source.GetImageInfo(imageInfo);
        for (int i = 0; i < imageInfo.size.height; ++i) {
            errno_t ret = memcpy_s(dstPixels, source.GetRowBytes(),
                                   source.GetPixels() + i * source.GetRowStride(), source.GetRowBytes());
            if (ret != 0) {
                HiLog::Error(LABEL, "copy source memory size %{public}u fail", bufferSize);
                ReleaseBuffer(AllocatorType::DMA_ALLOC, fd, bufferSize, &dstPixels);
                return false;
            }
            // Move the destination buffer pointer to the next row
            dstPixels = reinterpret_cast<uint8_t *>(dstPixels) + source.GetRowBytes();
        }
    } else {
        if (memcpy_s(dstPixels, bufferSize, source.GetPixels(), bufferSize) != 0) {
            HiLog::Error(LABEL, "copy source memory size %{public}u fail", bufferSize);
            ReleaseBuffer(fd > 0 ? AllocatorType::SHARE_MEM_ALLOC : AllocatorType::HEAP_ALLOC,
                          fd, bufferSize, &dstPixels);
            return false;
        }
    }
    return true;
}

bool PixelMap::CopyPixelMap(PixelMap &source, PixelMap &dstPixelMap)
{
    uint32_t bufferSize = source.GetByteCount();
    if (source.GetPixels() == nullptr) {
        HiLog::Error(LABEL, "source pixelMap data invalid");
        return false;
    }
    if (bufferSize == 0 || bufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
        HiLog::Error(LABEL, "AllocSharedMemory parameter bufferSize:[%{public}d] error.", bufferSize);
        return false;
    }
    int fd = 0;
    void *dstPixels = nullptr;
    if (source.GetAllocatorType() == AllocatorType::SHARE_MEM_ALLOC) {
        dstPixels = AllocSharedMemory(bufferSize, fd, dstPixelMap.GetUniqueId());
    } else {
        dstPixels = malloc(bufferSize);
    }
    if (dstPixels == nullptr) {
        HiLog::Error(LABEL, "source crop allocate memory fail allocatetype: %{public}d ", source.GetAllocatorType());
        return false;
    }
    void* tmpDstPixels = dstPixels;
    if (!CopyPixMapToDst(source, tmpDstPixels, fd, bufferSize)) {
        return false;
    }
    if (fd <= 0) {
        dstPixelMap.SetPixelsAddr(dstPixels, nullptr, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);
        return true;
    }
    void *fdBuffer = new int32_t();
    *static_cast<int32_t *>(fdBuffer) = fd;
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    dstPixelMap.SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    dstPixelMap.SetPixelsAddr(dstPixels, fdBuffer, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return true;
}

bool PixelMap::IsSameSize(const Size &src, const Size &dst)
{
    return (src.width == dst.width) && (src.height == dst.height);
}

bool PixelMap::GetPixelFormatDetail(const PixelFormat format)
{
    switch (format) {
        case PixelFormat::RGBA_8888: {
            pixelBytes_ = ARGB_8888_BYTES;
            colorProc_ = RGBA8888ToARGB;
            break;
        }
        case PixelFormat::BGRA_8888: {
            pixelBytes_ = ARGB_8888_BYTES;
            colorProc_ = BGRA8888ToARGB;
            break;
        }
        case PixelFormat::ARGB_8888: {
            pixelBytes_ = ARGB_8888_BYTES;
            colorProc_ = ARGB8888ToARGB;
            break;
        }
        case PixelFormat::ALPHA_8: {
            pixelBytes_ = ALPHA_8_BYTES;
            colorProc_ = ALPHA8ToARGB;
            break;
        }
        case PixelFormat::RGB_565: {
            pixelBytes_ = RGB_565_BYTES;
            colorProc_ = RGB565ToARGB;
            break;
        }
        case PixelFormat::RGB_888: {
            pixelBytes_ = RGB_888_BYTES;
            colorProc_ = RGB888ToARGB;
            break;
        }
        case PixelFormat::NV12:
        case PixelFormat::NV21: {
            pixelBytes_ = YUV420_BYTES;
            break;
        }
        case PixelFormat::CMYK:
            pixelBytes_ = ARGB_8888_BYTES;
            break;
        case PixelFormat::RGBA_F16:
            pixelBytes_ = BGRA_F16_BYTES;
            break;
        default: {
            HiLog::Error(LABEL, "pixel format:[%{public}d] not supported.", format);
            return false;
        }
    }
    return true;
}

void PixelMap::SetRowStride(uint32_t stride)
{
    rowStride_ = stride;
}

void PixelMap::UpdateImageInfo()
{
    SetImageInfo(imageInfo_, true);
}

uint32_t PixelMap::SetImageInfo(ImageInfo &info)
{
    return SetImageInfo(info, false);
}

uint32_t PixelMap::SetImageInfo(ImageInfo &info, bool isReused)
{
    if (info.size.width <= 0 || info.size.height <= 0) {
        HiLog::Error(LABEL, "pixel map image info invalid.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    if (!GetPixelFormatDetail(info.pixelFormat)) {
        return ERR_IMAGE_DATA_UNSUPPORT;
    }

    if (pixelBytes_ <= 0) {
        ResetPixelMap();
        HiLog::Error(LABEL, "pixel map bytes is invalid.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }

    if ((static_cast<uint64_t>(pixelBytes_) * info.size.width) > PIXEL_MAP_MAX_RAM_SIZE) {
        ResetPixelMap();
        HiLog::Error(LABEL, "image size is out of range.");
        return ERR_IMAGE_TOO_LARGE;
    }
    if (info.pixelFormat == PixelFormat::ALPHA_8) {
        rowDataSize_ = pixelBytes_ * ((info.size.width + FILL_NUMBER) / ALIGN_NUMBER * ALIGN_NUMBER);
        SetRowStride(rowDataSize_);
        HiLog::Info(LABEL, "ALPHA_8 rowDataSize_ %{public}d.", rowDataSize_);
    } else {
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
        if (allocatorType_ == AllocatorType::DMA_ALLOC) {
            if (context_ == nullptr) {
                HiLog::Error(LABEL, "set imageInfo context_ null");
                return ERR_IMAGE_DATA_ABNORMAL;
            }
            SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*>(context_);
            SetRowStride(sbBuffer->GetStride());
        } else {
            SetRowStride(pixelBytes_ * info.size.width);
        }
#endif
        rowDataSize_ = pixelBytes_ * info.size.width;
    }
    if (rowDataSize_ != 0 && info.size.height > (PIXEL_MAP_MAX_RAM_SIZE / rowDataSize_)) {
        ResetPixelMap();
        HiLog::Error(LABEL, "pixel map byte count out of range.");
        return ERR_IMAGE_TOO_LARGE;
    }
    if (!isReused) {
        FreePixelMap();
    }
    imageInfo_ = info;
    return SUCCESS;
}

const uint8_t *PixelMap::GetPixel8(int32_t x, int32_t y)
{
    if (!CheckValidParam(x, y) || (pixelBytes_ != ALPHA_8_BYTES)) {
        HiLog::Error(LABEL, "get addr8 pixel position:(%{public}d, %{public}d) pixel bytes:%{public}d invalid.", x, y,
                     pixelBytes_);
        return nullptr;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    return (data_ + y * rowStride_ + x);
#else
    return (data_ + y * rowDataSize_  + x);
#endif
}

const uint16_t *PixelMap::GetPixel16(int32_t x, int32_t y)
{
    if (!CheckValidParam(x, y) || (pixelBytes_ != RGB_565_BYTES)) {
        HiLog::Error(LABEL, "get addr16 pixel position:(%{public}d, %{public}d) pixel bytes:%{public}d invalid.", x, y,
                     pixelBytes_);
        return nullptr;
    }
    // convert uint8_t* to uint16_t*
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    return reinterpret_cast<uint16_t *>(data_ + y * rowStride_ + (static_cast<uint32_t>(x) << RGB_565_SHIFT));
#else
    return reinterpret_cast<uint16_t *>(data_ + y * rowDataSize_ + (static_cast<uint32_t>(x) << RGB_565_SHIFT));
#endif
}

const uint32_t *PixelMap::GetPixel32(int32_t x, int32_t y)
{
    if (!CheckValidParam(x, y) || (pixelBytes_ != ARGB_8888_BYTES)) {
        HiLog::Error(LABEL, "get addr32 pixel position:(%{public}d, %{public}d) pixel bytes:%{public}d invalid.", x, y,
                     pixelBytes_);
        return nullptr;
    }
    // convert uint8_t* to uint32_t*
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    return reinterpret_cast<uint32_t *>(data_ + y * rowStride_ + (static_cast<uint32_t>(x) << ARGB_8888_SHIFT));
#else
    return reinterpret_cast<uint32_t *>(data_ + y * rowDataSize_ + (static_cast<uint32_t>(x) << ARGB_8888_SHIFT));
#endif
}

const uint8_t *PixelMap::GetPixel(int32_t x, int32_t y)
{
    if (!CheckValidParam(x, y)) {
        HiLog::Error(LABEL, "input pixel position:(%{public}d, %{public}d) invalid.", x, y);
        return nullptr;
    }
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    return (data_ + y * rowStride_ + (static_cast<uint32_t>(x) * pixelBytes_));
#else
    return (data_ + y * rowDataSize_ + (static_cast<uint32_t>(x) * pixelBytes_));
#endif
}

bool PixelMap::GetARGB32Color(int32_t x, int32_t y, uint32_t &color)
{
    if (colorProc_ == nullptr) {
        HiLog::Error(LABEL, "pixel format not supported.");
        return false;
    }
    const uint8_t *src = GetPixel(x, y);
    if (src == nullptr) {
        HiLog::Error(LABEL, "get pixel color error.");
        return false;
    }
    // use founction point for frequently called interface
    return colorProc_(src, ONE_PIXEL_SIZE * pixelBytes_, &color, ONE_PIXEL_SIZE);
}

bool PixelMap::ALPHA8ToARGB(const uint8_t *in, uint32_t inCount, uint32_t *out, uint32_t outCount)
{
    if (inCount != outCount) {
        HiLog::Error(LABEL, "input count:%{public}u is not match to output count:%{public}u.", inCount, outCount);
        return false;
    }
    const uint8_t *src = in;
    for (uint32_t i = 0; i < outCount; i++) {
        *out++ = GetColorARGB(*src++, BYTE_ZERO, BYTE_ZERO, BYTE_ZERO);
    }
    return true;
}

bool PixelMap::RGB565ToARGB(const uint8_t *in, uint32_t inCount, uint32_t *out, uint32_t outCount)
{
    if (((inCount / RGB_565_BYTES) != outCount) && ((inCount % RGB_565_BYTES) != 0)) {
        HiLog::Error(LABEL, "input count:%{public}u is not match to output count:%{public}u.", inCount, outCount);
        return false;
    }
    const uint16_t *src = reinterpret_cast<const uint16_t *>(in);
    for (uint32_t i = 0; i < outCount; i++) {
        uint16_t color = *src++;
        *out++ = GetColorARGB(BYTE_FULL, RGB565ToR32(color), RGB565ToG32(color), RGB565ToB32(color));
    }
    return true;
}

bool PixelMap::ARGB8888ToARGB(const uint8_t *in, uint32_t inCount, uint32_t *out, uint32_t outCount)
{
    if (((inCount / ARGB_8888_BYTES) != outCount) && ((inCount % ARGB_8888_BYTES) != 0)) {
        HiLog::Error(LABEL, "input count:%{public}u is not match to output count:%{public}u.", inCount, outCount);
        return false;
    }
    const uint32_t *src = reinterpret_cast<const uint32_t *>(in);
    for (uint32_t i = 0; i < outCount; i++) {
        uint32_t color = *src++;
        *out++ = GetColorARGB(GetColorComp(color, ARGB32_A_SHIFT), GetColorComp(color, ARGB32_R_SHIFT),
                              GetColorComp(color, ARGB32_G_SHIFT), GetColorComp(color, ARGB32_B_SHIFT));
    }
    return true;
}

bool PixelMap::RGBA8888ToARGB(const uint8_t *in, uint32_t inCount, uint32_t *out, uint32_t outCount)
{
    if (((inCount / ARGB_8888_BYTES) != outCount) && ((inCount % ARGB_8888_BYTES) != 0)) {
        HiLog::Error(LABEL, "input count:%{public}u is not match to output count:%{public}u.", inCount, outCount);
        return false;
    }
    const uint32_t *src = reinterpret_cast<const uint32_t *>(in);
    for (uint32_t i = 0; i < outCount; i++) {
        uint32_t color = *src++;
        *out++ = GetColorARGB(GetColorComp(color, RGBA32_A_SHIFT), GetColorComp(color, RGBA32_R_SHIFT),
                              GetColorComp(color, RGBA32_G_SHIFT), GetColorComp(color, RGBA32_B_SHIFT));
    }
    return true;
}

bool PixelMap::BGRA8888ToARGB(const uint8_t *in, uint32_t inCount, uint32_t *out, uint32_t outCount)
{
    if (((inCount / ARGB_8888_BYTES) != outCount) && ((inCount % ARGB_8888_BYTES) != 0)) {
        HiLog::Error(LABEL, "input count:%{public}u is not match to output count:%{public}u.", inCount, outCount);
        return false;
    }
    const uint32_t *src = reinterpret_cast<const uint32_t *>(in);
    for (uint32_t i = 0; i < outCount; i++) {
        uint32_t color = *src++;
        *out++ = GetColorARGB(GetColorComp(color, BGRA32_A_SHIFT), GetColorComp(color, BGRA32_R_SHIFT),
                              GetColorComp(color, BGRA32_G_SHIFT), GetColorComp(color, BGRA32_B_SHIFT));
    }
    return true;
}

bool PixelMap::RGB888ToARGB(const uint8_t *in, uint32_t inCount, uint32_t *out, uint32_t outCount)
{
    if (((inCount / RGB_888_BYTES) != outCount) && ((inCount % RGB_888_BYTES) != 0)) {
        HiLog::Error(LABEL, "input count:%{public}u is not match to output count:%{public}u.", inCount, outCount);
        return false;
    }
    const uint8_t *src = in;
    for (uint32_t i = 0; i < outCount; i++) {
        uint8_t colorR = *src++;
        uint8_t colorG = *src++;
        uint8_t colorB = *src++;
        *out++ = GetColorARGB(BYTE_FULL, colorR, colorG, colorB);
    }
    return true;
}

int32_t PixelMap::GetPixelBytes()
{
    return pixelBytes_;
}

int32_t PixelMap::GetRowBytes()
{
    return rowDataSize_;
}

int32_t PixelMap::GetByteCount()
{
    HiLog::Debug(LABEL, "GetByteCount");
    return rowDataSize_ * imageInfo_.size.height;
}

int32_t PixelMap::GetWidth()
{
    return imageInfo_.size.width;
}

int32_t PixelMap::GetHeight()
{
    return imageInfo_.size.height;
}

int32_t PixelMap::GetBaseDensity()
{
    return imageInfo_.baseDensity;
}

void PixelMap::GetImageInfo(ImageInfo &imageInfo)
{
    imageInfo = imageInfo_;
}

PixelFormat PixelMap::GetPixelFormat()
{
    return imageInfo_.pixelFormat;
}

ColorSpace PixelMap::GetColorSpace()
{
    return imageInfo_.colorSpace;
}

AlphaType PixelMap::GetAlphaType()
{
    return imageInfo_.alphaType;
}

const uint8_t *PixelMap::GetPixels()
{
    return data_;
}

uint8_t PixelMap::GetARGB32ColorA(uint32_t color)
{
    return (color >> ARGB_A_SHIFT) & ARGB_MASK;
}

uint8_t PixelMap::GetARGB32ColorR(uint32_t color)
{
    return (color >> ARGB_R_SHIFT) & ARGB_MASK;
}

uint8_t PixelMap::GetARGB32ColorG(uint32_t color)
{
    return (color >> ARGB_G_SHIFT) & ARGB_MASK;
}

uint8_t PixelMap::GetARGB32ColorB(uint32_t color)
{
    return (color >> ARGB_B_SHIFT) & ARGB_MASK;
}

bool PixelMap::IsSameImage(const PixelMap &other)
{
    if (data_ == nullptr || other.data_ == nullptr) {
        HiLog::Error(LABEL, "IsSameImage data_ is nullptr.");
        return false;
    }
    if (imageInfo_.size.width != other.imageInfo_.size.width ||
        imageInfo_.size.height != other.imageInfo_.size.height ||
        imageInfo_.pixelFormat != other.imageInfo_.pixelFormat || imageInfo_.alphaType != other.imageInfo_.alphaType) {
        HiLog::Error(LABEL, "IsSameImage imageInfo check not OK.");
        return false;
    }
    uint64_t size = static_cast<uint64_t>(rowDataSize_) * imageInfo_.size.height;
    if (memcmp(data_, other.data_, size) != 0) {
        HiLog::Error(LABEL, "IsSameImage mmemcmp check not OK.");
        return false;
    }
    return true;
}

uint32_t PixelMap::ReadPixels(const uint64_t &bufferSize, uint8_t *dst)
{
    StartTrace(HITRACE_TAG_ZIMAGE, "ReadPixels by bufferSize");
    if (dst == nullptr) {
        HiLog::Error(LABEL, "read pixels by buffer input dst address is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "read pixels by buffer current PixelMap data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    if (bufferSize < static_cast<uint64_t>(pixelsSize_)) {
        HiLog::Error(LABEL, "read pixels by buffer input dst buffer(%{public}llu) < current pixelmap size(%{public}u).",
                     static_cast<unsigned long long>(bufferSize), pixelsSize_);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Copy the actual pixel data without padding bytes
    for (int i = 0; i < imageInfo_.size.height; ++i) {
        errno_t ret = memcpy_s(dst, rowDataSize_, data_ + i * rowStride_, rowDataSize_);
        if (ret != 0) {
            HiLog::Error(LABEL, "read pixels by buffer memcpy the pixelmap data to dst fail, error:%{public}d", ret);
            return ERR_IMAGE_READ_PIXELMAP_FAILED;
        }
        dst += rowDataSize_; // Move the destination buffer pointer to the next row
    }
    FinishTrace(HITRACE_TAG_ZIMAGE);
    return SUCCESS;
}

bool PixelMap::CheckPixelsInput(const uint8_t *dst, const uint64_t &bufferSize, const uint32_t &offset,
                                const uint32_t &stride, const Rect &region)
{
    if (dst == nullptr) {
        HiLog::Error(LABEL, "CheckPixelsInput input dst address is null.");
        return false;
    }

    if (bufferSize == 0) {
        HiLog::Error(LABEL, "CheckPixelsInput input buffer size is 0.");
        return false;
    }

    if (region.left < 0 || region.top < 0 || stride > numeric_limits<int32_t>::max() ||
        static_cast<uint64_t>(offset) > bufferSize) {
        HiLog::Error(
            LABEL,
            "CheckPixelsInput left(%{public}d) or top(%{public}d) or stride(%{public}u) or offset(%{public}u) < 0.",
            region.left, region.top, stride, offset);
        return false;
    }
    if (region.width <= 0 || region.height <= 0 || region.width > MAX_DIMENSION || region.height > MAX_DIMENSION) {
        HiLog::Error(LABEL, "CheckPixelsInput width(%{public}d) or height(%{public}d) is < 0.", region.width,
                     region.height);
        return false;
    }
    if (region.left > GetWidth() - region.width) {
        HiLog::Error(LABEL, "CheckPixelsInput left(%{public}d) + width(%{public}d) is > pixelmap width(%{public}d).",
                     region.left, region.width, GetWidth());
        return false;
    }
    if (region.top > GetHeight() - region.height) {
        HiLog::Error(LABEL, "CheckPixelsInput top(%{public}d) + height(%{public}d) is > pixelmap height(%{public}d).",
                     region.top, region.height, GetHeight());
        return false;
    }
    uint32_t regionStride = static_cast<uint32_t>(region.width) * 4;  // bytes count, need multiply by 4
    if (stride < regionStride) {
        HiLog::Error(LABEL, "CheckPixelsInput stride(%{public}d) < width*4 (%{public}d).", stride, regionStride);
        return false;
    }

    if (bufferSize < regionStride) {
        HiLog::Error(LABEL, "CheckPixelsInput input buffer size is < width * 4.");
        return false;
    }
    uint64_t lastLinePos = offset + static_cast<uint64_t>(region.height - 1) * stride;  // "1" is except the last line.
    if (static_cast<uint64_t>(offset) > (bufferSize - regionStride) || lastLinePos > (bufferSize - regionStride)) {
        HiLog::Error(LABEL,
                     "CheckPixelsInput fail, height(%{public}d), width(%{public}d), lastLine(%{public}llu), "
                     "offset(%{public}u), bufferSize:%{public}llu.",
                     region.height, region.width, static_cast<unsigned long long>(lastLinePos), offset,
                     static_cast<unsigned long long>(bufferSize));
        return false;
    }
    return true;
}

uint32_t PixelMap::ReadPixels(const uint64_t &bufferSize, const uint32_t &offset, const uint32_t &stride,
                              const Rect &region, uint8_t *dst)
{
    if (!CheckPixelsInput(dst, bufferSize, offset, stride, region)) {
        HiLog::Error(LABEL, "read pixels by rect input parameter fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "read pixels by rect this pixel data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    ImageInfo dstImageInfo =
        MakeImageInfo(region.width, region.height, PixelFormat::BGRA_8888, AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL);
    Position srcPosition { region.left, region.top };
    if (!PixelConvertAdapter::ReadPixelsConvert(data_, srcPosition, rowStride_, imageInfo_, dst + offset, stride,
        dstImageInfo)) {
        HiLog::Error(LABEL, "read pixels by rect call ReadPixelsConvert fail.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

uint32_t PixelMap::ReadPixel(const Position &pos, uint32_t &dst)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= GetWidth() || pos.y >= GetHeight()) {
        HiLog::Error(LABEL, "read pixel by pos input invalid exception. [x(%{public}d), y(%{public}d)]", pos.x, pos.y);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "read pixel by pos source data is null.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    ImageInfo dstImageInfo =
        MakeImageInfo(PER_PIXEL_LEN, PER_PIXEL_LEN, PixelFormat::BGRA_8888, AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL);
    uint32_t dstRowBytes = BGRA_BYTES;
    Position srcPosition { pos.x, pos.y };
    if (!PixelConvertAdapter::ReadPixelsConvert(data_, srcPosition, rowStride_, imageInfo_, &dst, dstRowBytes,
        dstImageInfo)) {
        HiLog::Error(LABEL, "read pixel by pos call ReadPixelsConvert fail.");
        return ERR_IMAGE_READ_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

uint32_t PixelMap::ResetConfig(const Size &size, const PixelFormat &format)
{
    if (size.width <= 0 || size.height <= 0) {
        HiLog::Error(LABEL, "ResetConfig reset input width(%{public}d) or height(%{public}d) is < 0.", size.width,
                     size.height);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    uint32_t bytesPerPixel = ImageUtils::GetPixelBytes(format);
    if (bytesPerPixel == 0) {
        HiLog::Error(LABEL, "ResetConfig get bytes by per pixel fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    uint64_t dstSize = static_cast<uint64_t>(size.width) * size.height * bytesPerPixel;
    if (dstSize > static_cast<uint64_t>(pixelsSize_)) {
        HiLog::Error(LABEL, "ResetConfig reset dstSize(%{public}llu) > current(%{public}u).",
                     static_cast<unsigned long long>(dstSize), pixelsSize_);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    AlphaType dstAlphaType = ImageUtils::GetValidAlphaTypeByFormat(GetAlphaType(), format);
    if (dstAlphaType == AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN) {
        HiLog::Error(LABEL, "ResetConfig Failed to get validate alpha type.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    ImageInfo dstInfo = MakeImageInfo(size.width, size.height, format, dstAlphaType);
    uint32_t ret = SetImageInfo(dstInfo, true);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "ResetConfig call SetImageInfo Failed. ret:%{public}u", ret);
        return ERR_IMAGE_CONFIG_FAILED;
    }
    return SUCCESS;
}

bool PixelMap::SetAlphaType(const AlphaType &alphaType)
{
    AlphaType type = ImageUtils::GetValidAlphaTypeByFormat(alphaType, imageInfo_.pixelFormat);
    if (type == AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN) {
        HiLog::Error(LABEL, "SetAlphaType Failed to get validate alpha type.");
        return false;
    }
    ImageInfo dstInfo = imageInfo_;
    dstInfo.alphaType = type;
    uint32_t ret = SetImageInfo(dstInfo, true);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "SetAlphaType call SetImageInfo Failed. ret:%{public}u", ret);
        return false;
    }
    return true;
}

uint32_t PixelMap::WritePixel(const Position &pos, const uint32_t &color)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= GetWidth() || pos.y >= GetHeight()) {
        HiLog::Error(LABEL,
                     "write pixel by pos but input position is invalid. [x(%{public}d), y(%{public}d)]"\
                     "Width() %{public}d,  Height() %{public}d, ",
                     pos.x, pos.y, GetWidth(), GetHeight());
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!IsEditable()) {
        HiLog::Error(LABEL, "write pixel by pos pixelmap is not editable.");
        return ERR_IMAGE_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    if (!ImageUtils::IsValidImageInfo(imageInfo_)) {
        HiLog::Error(LABEL, "write pixel by pos current pixelmap image info is invalid.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "write pixel by pos but current pixelmap data is nullptr.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    ImageInfo srcImageInfo =
        MakeImageInfo(PER_PIXEL_LEN, PER_PIXEL_LEN, PixelFormat::BGRA_8888, AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL);
    uint32_t srcRowBytes = BGRA_BYTES;
    Position dstPosition { pos.x, pos.y };  // source is per pixel.
    if (!PixelConvertAdapter::WritePixelsConvert(&color, srcRowBytes, srcImageInfo, data_, dstPosition, rowStride_,
        imageInfo_)) {
        HiLog::Error(LABEL, "write pixel by pos call WritePixelsConvert fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

uint32_t PixelMap::WritePixels(const uint8_t *source, const uint64_t &bufferSize, const uint32_t &offset,
                               const uint32_t &stride, const Rect &region)
{
    if (!CheckPixelsInput(source, bufferSize, offset, stride, region)) {
        HiLog::Error(LABEL, "write pixel by rect input parameter fail.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!IsEditable()) {
        HiLog::Error(LABEL, "write pixel by rect pixelmap data is not editable.");
        return ERR_IMAGE_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    if (!ImageUtils::IsValidImageInfo(imageInfo_)) {
        HiLog::Error(LABEL, "write pixel by rect current pixelmap image info is invalid.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "write pixel by rect current pixel map data is null.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    uint32_t bytesPerPixel = ImageUtils::GetPixelBytes(imageInfo_.pixelFormat);
    if (bytesPerPixel == 0) {
        HiLog::Error(LABEL, "write pixel by rect get bytes by per pixel fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    Position dstPosition { region.left, region.top };
    ImageInfo srcInfo =
        MakeImageInfo(region.width, region.height, PixelFormat::BGRA_8888, AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL);
    if (!PixelConvertAdapter::WritePixelsConvert(source + offset, stride, srcInfo, data_, dstPosition, rowStride_,
        imageInfo_)) {
        HiLog::Error(LABEL, "write pixel by rect call WritePixelsConvert fail.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    return SUCCESS;
}

uint32_t PixelMap::WritePixels(const uint8_t *source, const uint64_t &bufferSize)
{
    StartTrace(HITRACE_TAG_ZIMAGE, "WritePixels");
    if (source == nullptr || bufferSize < static_cast<uint64_t>(pixelsSize_)) {
        HiLog::Error(LABEL, "write pixels by buffer source is nullptr or size(%{public}llu) < pixelSize(%{public}u).",
                     static_cast<unsigned long long>(bufferSize), pixelsSize_);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (!IsEditable()) {
        HiLog::Error(LABEL, "write pixels by buffer pixelmap data is not editable.");
        return ERR_IMAGE_PIXELMAP_NOT_ALLOW_MODIFY;
    }
    if (!ImageUtils::IsValidImageInfo(imageInfo_)) {
        HiLog::Error(LABEL, "write pixels by buffer current pixelmap image info is invalid.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "write pixels by buffer current pixelmap data is nullptr.");
        return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
    }

    for (int i = 0; i < imageInfo_.size.height; ++i) {
        const uint8_t* sourceRow = source + i * rowDataSize_;
        errno_t ret = memcpy_s(data_ + i * rowStride_, rowDataSize_, sourceRow, rowDataSize_);
        if (ret != 0) {
            HiLog::Error(LABEL, "write pixels by buffer memcpy the pixelmap data to dst fail, error:%{public}d", ret);
            return ERR_IMAGE_WRITE_PIXELMAP_FAILED;
        }
    }
    FinishTrace(HITRACE_TAG_ZIMAGE);
    return SUCCESS;
}

bool PixelMap::WritePixels(const uint32_t &color)
{
    if (!IsEditable()) {
        HiLog::Error(LABEL, "erase pixels by color pixelmap data is not editable.");
        return false;
    }
    if (!ImageUtils::IsValidImageInfo(imageInfo_)) {
        HiLog::Error(LABEL, "erase pixels by color current pixelmap image info is invalid.");
        return false;
    }
    if (data_ == nullptr) {
        HiLog::Error(LABEL, "erase pixels by color current pixel map data is null.");
        return false;
    }
    ImageInfo srcInfo =
        MakeImageInfo(imageInfo_.size.width, imageInfo_.size.height, imageInfo_.pixelFormat, imageInfo_.alphaType);
    if (!PixelConvertAdapter::EraseBitmap(data_, rowStride_, srcInfo, color)) {
        HiLog::Error(LABEL, "erase pixels by color call EraseBitmap fail.");
        return false;
    }
    return true;
}

AllocatorType PixelMap::GetAllocatorType()
{
    return allocatorType_;
}

void *PixelMap::GetFd() const
{
    return context_;
}

void PixelMap::ReleaseMemory(AllocatorType allocType, void *addr, void *context, uint32_t size)
{
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    if (allocType == AllocatorType::SHARE_MEM_ALLOC) {
        int *fd = static_cast<int *>(context);
        if (addr != nullptr) {
            ::munmap(addr, size);
        }
        if (fd != nullptr) {
            ::close(*fd);
        }
    } else if (allocType == AllocatorType::HEAP_ALLOC) {
        if (addr != nullptr) {
            free(addr);
            addr = nullptr;
        }
    } else if (allocType == AllocatorType::DMA_ALLOC) {
        ImageUtils::SurfaceBuffer_Unreference(static_cast<SurfaceBuffer*>(context));
    }
#else
    if (addr != nullptr) {
        free(addr);
        addr = nullptr;
    }
#endif
}

bool PixelMap::WriteImageData(Parcel &parcel, size_t size) const
{
    const uint8_t *data = data_;
    if (data == nullptr) {
        HiLog::Error(LABEL, "write to parcel failed, pixel memory is null.");
        return false;
    }
    if (data == nullptr || size > MAX_IMAGEDATA_SIZE) {
        return false;
    }

    if (!parcel.WriteInt32(size)) {
        return false;
    }
    if (size <= MIN_IMAGEDATA_SIZE) {
        return parcel.WriteUnpadBuffer(data, size);
    }
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    uint32_t id = GetUniqueId();
    std::string name = "Parcel ImageData, uniqueId: " + std::to_string(getpid()) + '_' + std::to_string(id);
    int fd = AshmemCreate(name.c_str(), size);
    HiLog::Info(LABEL, "AshmemCreate:[%{public}d].", fd);
    if (fd < 0) {
        return false;
    }

    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    HiLog::Info(LABEL, "AshmemSetProt:[%{public}d].", result);
    if (result < 0) {
        ::close(fd);
        return false;
    }
    void *ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        ::close(fd);
        HiLog::Error(LABEL, "WriteImageData map failed, errno:%{public}d", errno);
        return false;
    }
    HiLog::Info(LABEL, "mmap success");

    if (memcpy_s(ptr, size, data, size) != EOK) {
        ::munmap(ptr, size);
        ::close(fd);
        HiLog::Error(LABEL, "WriteImageData memcpy_s error");
        return false;
    }

    if (!WriteFileDescriptor(parcel, fd)) {
        ::munmap(ptr, size);
        ::close(fd);
        HiLog::Error(LABEL, "WriteImageData WriteFileDescriptor error");
        return false;
    }
    HiLog::Debug(LABEL, "WriteImageData WriteFileDescriptor success");
    ::munmap(ptr, size);
    ::close(fd);
    HiLog::Debug(LABEL, "WriteImageData End");
#endif
    return true;
}

uint8_t *PixelMap::ReadImageData(Parcel &parcel, int32_t bufferSize)
{
    uint8_t *base = nullptr;
#if !defined(_WIN32) && !defined(_APPLE) &&!defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    if (static_cast<unsigned int>(bufferSize) <= MIN_IMAGEDATA_SIZE) {
        if (bufferSize <= 0) {
            HiLog::Error(LABEL, "malloc parameter bufferSize:[%{public}d] error.", bufferSize);
            return nullptr;
        }

        const uint8_t *ptr = parcel.ReadUnpadBuffer(bufferSize);
        if (ptr == nullptr) {
            HiLog::Error(LABEL, "read buffer from parcel failed, read buffer addr is null");
            return nullptr;
        }

        base = static_cast<uint8_t *>(malloc(bufferSize));
        if (base == nullptr) {
            HiLog::Error(LABEL, "alloc output pixel memory size:[%{public}d] error.", bufferSize);
            return nullptr;
        }
        if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
            free(base);
            base = nullptr;
            HiLog::Error(LABEL, "memcpy pixel data size:[%{public}d] error.", bufferSize);
            return nullptr;
        }
    } else {
        int fd = ReadFileDescriptor(parcel);
        if (fd < 0) {
            HiLog::Error(LABEL, "read fd :[%{public}d] error", fd);
            return nullptr;
        }
        if (bufferSize <= 0 || bufferSize > PIXEL_MAP_MAX_RAM_SIZE) {
            HiLog::Error(LABEL, "malloc parameter bufferSize:[%{public}d] error.", bufferSize);
            return nullptr;
        }

        void *ptr = ::mmap(nullptr, bufferSize, PROT_READ, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            // do not close fd here. fd will be closed in FileDescriptor, ::close(fd)
            HiLog::Error(LABEL, "ReadImageData map failed, errno:%{public}d", errno);
            return nullptr;
        }

        base = static_cast<uint8_t *>(malloc(bufferSize));
        if (base == nullptr) {
            ::munmap(ptr, bufferSize);
            HiLog::Error(LABEL, "alloc output pixel memory size:[%{public}d] error.", bufferSize);
            return nullptr;
        }
        if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
            ::munmap(ptr, bufferSize);
            free(base);
            base = nullptr;
            HiLog::Error(LABEL, "memcpy pixel data size:[%{public}d] error.", bufferSize);
            return nullptr;
        }

        ReleaseMemory(AllocatorType::SHARE_MEM_ALLOC, ptr, &fd, bufferSize);
    }
#else
    if (bufferSize <= 0) {
        HiLog::Error(LABEL, "malloc parameter bufferSize:[%{public}d] error.", bufferSize);
        return nullptr;
    }

    const uint8_t *ptr = parcel.ReadUnpadBuffer(bufferSize);
    if (ptr == nullptr) {
        HiLog::Error(LABEL, "read buffer from parcel failed, read buffer addr is null");
        return nullptr;
    }

    base = static_cast<uint8_t *>(malloc(bufferSize));
    if (base == nullptr) {
        HiLog::Error(LABEL, "alloc output pixel memory size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
    if (memcpy_s(base, bufferSize, ptr, bufferSize) != 0) {
        free(base);
        base = nullptr;
        HiLog::Error(LABEL, "memcpy pixel data size:[%{public}d] error.", bufferSize);
        return nullptr;
    }
#endif
    return base;
}

bool PixelMap::WriteFileDescriptor(Parcel &parcel, int fd)
{
#if !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    if (fd < 0) {
        return false;
    }
    int dupFd = dup(fd);
    if (dupFd < 0) {
        return false;
    }
    sptr<IPCFileDescriptor> descriptor = new IPCFileDescriptor(dupFd);
    return parcel.WriteObject<IPCFileDescriptor>(descriptor);
#else
    return false;
#endif
}

int PixelMap::ReadFileDescriptor(Parcel &parcel)
{
#if !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    sptr<IPCFileDescriptor> descriptor = parcel.ReadObject<IPCFileDescriptor>();
    if (descriptor == nullptr) {
        return -1;
    }
    int fd = descriptor->GetFd();
    if (fd < 0) {
        return -1;
    }
    return dup(fd);
#else
    return -1;
#endif
}

bool PixelMap::WriteImageInfo(Parcel &parcel) const
{
    if (!parcel.WriteInt32(imageInfo_.size.width)) {
        HiLog::Error(LABEL, "write image info width:[%{public}d] to parcel failed.", imageInfo_.size.width);
        return false;
    }
    if (!parcel.WriteInt32(imageInfo_.size.height)) {
        HiLog::Error(LABEL, "write image info height:[%{public}d] to parcel failed.", imageInfo_.size.height);
        return false;
    }
    if (!parcel.WriteInt32(static_cast<int32_t>(imageInfo_.pixelFormat))) {
        HiLog::Error(LABEL, "write image info pixel format:[%{public}d] to parcel failed.", imageInfo_.pixelFormat);
        return false;
    }
    if (!parcel.WriteInt32(static_cast<int32_t>(imageInfo_.colorSpace))) {
        HiLog::Error(LABEL, "write image info color space:[%{public}d] to parcel failed.", imageInfo_.colorSpace);
        return false;
    }
    if (!parcel.WriteInt32(static_cast<int32_t>(imageInfo_.alphaType))) {
        HiLog::Error(LABEL, "write image info alpha type:[%{public}d] to parcel failed.", imageInfo_.alphaType);
        return false;
    }
    if (!parcel.WriteInt32(imageInfo_.baseDensity)) {
        HiLog::Error(LABEL, "write image info base density:[%{public}d] to parcel failed.", imageInfo_.baseDensity);
        return false;
    }
    return true;
}

bool PixelMap::WriteInfoToParcel(Parcel &parcel) const
{
    if (!WriteImageInfo(parcel)) {
        HiLog::Error(LABEL, "write image info to parcel failed.");
        return false;
    }

    if (!parcel.WriteBool(editable_)) {
        HiLog::Error(LABEL, "write pixel map editable to parcel failed.");
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(allocatorType_))) {
        HiLog::Error(LABEL, "write pixel map allocator type:[%{public}d] to parcel failed.",
                     allocatorType_);
        return false;
    }
    return true;
}

bool PixelMap::Marshalling(Parcel &parcel) const
{
    int32_t PIXEL_MAP_INFO_MAX_LENGTH = 128;
    int32_t bufferSize = rowDataSize_ * imageInfo_.size.height;
    if (static_cast<size_t>(bufferSize) <= MIN_IMAGEDATA_SIZE &&
        static_cast<size_t>(bufferSize + PIXEL_MAP_INFO_MAX_LENGTH) > parcel.GetDataCapacity() &&
        !parcel.SetDataCapacity(bufferSize + PIXEL_MAP_INFO_MAX_LENGTH)) {
        HiLog::Error(LABEL, "set parcel max capacity:[%{public}d] failed.", bufferSize + PIXEL_MAP_INFO_MAX_LENGTH);
        return false;
    }
    if (!WriteInfoToParcel(parcel)) {
        HiLog::Error(LABEL, "write info to parcel failed.");
        return false;
    }
    if (!parcel.WriteInt32(static_cast<int32_t>(rowDataSize_))) {
        HiLog::Error(LABEL, "write image info rowStride_:[%{public}d] to parcel failed.", rowDataSize_);
        return false;
    }
#if !defined(_WIN32) && !defined(_APPLE) &&!defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::SHARE_MEM_ALLOC) {
        if (!parcel.WriteInt32(bufferSize)) {
            return false;
        }

        int *fd = static_cast<int *>(context_);
        if (fd == nullptr || *fd < 0) {
            HiLog::Error(LABEL, "write pixel map failed, fd is [%{public}d] or fd < 0.", fd == nullptr ? 1 : 0);
            return false;
        }

        if (!WriteFileDescriptor(parcel, *fd)) {
            HiLog::Error(LABEL, "write pixel map fd:[%{public}d] to parcel failed.", *fd);
            return false;
        }
    } else if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (!parcel.WriteInt32(bufferSize)) {
            return false;
        }
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*> (context_);
        sbBuffer->WriteToMessageParcel(static_cast<MessageParcel&>(parcel));
    } else {
        if (!WriteImageData(parcel, bufferSize)) {
            HiLog::Error(LABEL, "write pixel map buffer to parcel failed.");
            return false;
        }
    }
#else
    if (!WriteImageData(parcel, bufferSize)) {
        HiLog::Error(LABEL, "write pixel map buffer to parcel failed.");
        return false;
    }
#endif
    return true;
}

bool PixelMap::ReadImageInfo(Parcel &parcel, ImageInfo &imgInfo)
{
    imgInfo.size.width = parcel.ReadInt32();
    HiLog::Debug(LABEL, "read pixel map width:[%{public}d] to parcel.", imgInfo.size.width);
    imgInfo.size.height = parcel.ReadInt32();
    HiLog::Debug(LABEL, "read pixel map height:[%{public}d] to parcel.", imgInfo.size.height);
    imgInfo.pixelFormat = static_cast<PixelFormat>(parcel.ReadInt32());
    HiLog::Debug(LABEL, "read pixel map pixelFormat:[%{public}d] to parcel.", imgInfo.pixelFormat);
    imgInfo.colorSpace = static_cast<ColorSpace>(parcel.ReadInt32());
    HiLog::Debug(LABEL, "read pixel map colorSpace:[%{public}d] to parcel.", imgInfo.colorSpace);
    imgInfo.alphaType = static_cast<AlphaType>(parcel.ReadInt32());
    HiLog::Debug(LABEL, "read pixel map alphaType:[%{public}d] to parcel.", imgInfo.alphaType);
    imgInfo.baseDensity = parcel.ReadInt32();
    return true;
}

PixelMap *PixelMap::Unmarshalling(Parcel &parcel)
{
    PixelMap *pixelMap = new PixelMap();
    if (pixelMap == nullptr) {
        return nullptr;
    }

    ImageInfo imgInfo;
    if (!pixelMap->ReadImageInfo(parcel, imgInfo)) {
        HiLog::Error(LABEL, "read imageInfo fail");
        delete pixelMap;
        return nullptr;
    }

    bool isEditable = parcel.ReadBool();
    pixelMap->SetEditable(isEditable);
    
    AllocatorType allocType = static_cast<AllocatorType>(parcel.ReadInt32());
    int32_t rowDataSize = parcel.ReadInt32();
    int32_t bufferSize = parcel.ReadInt32();
    int32_t bytesPerPixel = ImageUtils::GetPixelBytes(imgInfo.pixelFormat);
    if (bytesPerPixel == 0) {
        delete pixelMap;
        HiLog::Error(LABEL, "unmarshalling get bytes by per pixel fail.");
        return nullptr;
    }
    if (bufferSize != rowDataSize * imgInfo.size.height) {
        delete pixelMap;
        HiLog::Error(LABEL, "unmarshalling bufferSize parcelling error");
        return nullptr;
    }
    uint8_t *base = nullptr;
    void *context = nullptr;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) &&!defined(A_PLATFORM)
    if (allocType == AllocatorType::SHARE_MEM_ALLOC) {
        int fd = ReadFileDescriptor(parcel);
        if (fd < 0) {
            HiLog::Error(LABEL, "fd < 0");
            delete pixelMap;
            return nullptr;
        }
        void* ptr = ::mmap(nullptr, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            ptr = ::mmap(nullptr, bufferSize, PROT_READ, MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED) {
                ::close(fd);
                delete pixelMap;
                HiLog::Error(LABEL, "shared memory map in memalloc failed, errno:%{public}d", errno);
                return nullptr;
            }
        }
        context = new int32_t();
        if (context == nullptr) {
            ::munmap(ptr, bufferSize);
            ::close(fd);
            delete pixelMap;
            return nullptr;
        }
        *static_cast<int32_t *>(context) = fd;
        base = static_cast<uint8_t *>(ptr);
    } else if (allocType == AllocatorType::DMA_ALLOC) {
        sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
        surfaceBuffer->ReadFromMessageParcel(static_cast<MessageParcel&>(parcel));
        uint8_t* virAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
        void* nativeBuffer = surfaceBuffer.GetRefPtr();
        ImageUtils::SurfaceBuffer_Reference(nativeBuffer);
        base = virAddr;
        context = nativeBuffer;
    } else {
        base = ReadImageData(parcel, bufferSize);
        if (base == nullptr) {
            HiLog::Error(LABEL, "get pixel memory size:[%{public}d] error.", bufferSize);
            delete pixelMap;
            return nullptr;
        }
    }
#else
    base = ReadImageData(parcel, bufferSize);
    if (base == nullptr) {
        HiLog::Error(LABEL, "get pixel memory size:[%{public}d] error.", bufferSize);
        delete pixelMap;
        return nullptr;
    }
#endif

    uint32_t ret = pixelMap->SetImageInfo(imgInfo);
    if (ret != SUCCESS) {
        if (pixelMap->freePixelMapProc_ != nullptr) {
            pixelMap->freePixelMapProc_(base, context, bufferSize);
        }
        ReleaseMemory(allocType, base, context, bufferSize);
        if (context != nullptr) {
            delete static_cast<int32_t *>(context);
        }
        delete pixelMap;
        HiLog::Error(LABEL, "create pixel map from parcel failed, set image info error.");
        return nullptr;
    }
    pixelMap->SetPixelsAddr(base, context, bufferSize, allocType, nullptr);
    return pixelMap;
}

void PixelMap::WriteUint8(std::vector<uint8_t> &buff, uint8_t value) const
{
    buff.push_back(value);
}

uint8_t PixelMap::ReadUint8(std::vector<uint8_t> &buff, int32_t &cursor)
{
    if (static_cast<size_t>(cursor + 1) > buff.size()) {
        HiLog::Error(LABEL, "ReadUint8 out of range");
        return TLV_END;
    }
    return buff[cursor++];
}

uint8_t PixelMap::GetVarintLen(int32_t value) const
{
    uint8_t len = 1;
    while (value > TLV_VARINT_MASK) {
        len++;
        value >>= TLV_VARINT_BITS;
    }
    return len;
}

void PixelMap::WriteVarint(std::vector<uint8_t> &buff, int32_t value) const
{
    while (value > TLV_VARINT_MASK) {
        buff.push_back(TLV_VARINT_MORE | uint8_t(value & TLV_VARINT_MASK));
        value >>= TLV_VARINT_BITS;
    }
    buff.push_back(uint8_t(value));
}

int32_t PixelMap::ReadVarint(std::vector<uint8_t> &buff, int32_t &cursor)
{
    int32_t value = 0;
    uint8_t shift = 0;
    int32_t item = 0;
    do {
        if (static_cast<size_t>(cursor + 1) > buff.size()) {
            HiLog::Error(LABEL, "ReadVarint out of range");
            return static_cast<int32_t>(TLV_END);
        }
        item = int32_t(buff[cursor++]);
        value |= (item & TLV_VARINT_MASK) << shift;
        shift += TLV_VARINT_BITS;
    } while ((item & TLV_VARINT_MORE) != 0);
    return value;
}

void PixelMap::WriteData(std::vector<uint8_t> &buff, const uint8_t *data, int32_t size) const
{
    for (int32_t offset = 0; offset < size; offset++) {
        buff.push_back(*(data + offset));
    }
}

uint8_t *PixelMap::ReadData(std::vector<uint8_t> &buff, int32_t size, int32_t &cursor)
{
    if (size <= 0) {
        HiLog::Error(LABEL, "pixel map tlv read data fail: invalid size[%{public}d]", size);
        return nullptr;
    }
    if (static_cast<size_t>(cursor + size) > buff.size()) {
        HiLog::Error(LABEL, "ReadData out of range");
        return nullptr;
    }
    uint8_t *data = static_cast<uint8_t *>(malloc(size));
    if (data == nullptr) {
        HiLog::Error(LABEL, "pixel map tlv read data fail: malloc memory size[%{public}d]", size);
        return nullptr;
    }
    for (int32_t offset = 0; offset < size; offset++) {
        *(data + offset) = buff[cursor++];
    }
    return data;
}

bool PixelMap::EncodeTlv(std::vector<uint8_t> &buff) const
{
    WriteUint8(buff, TLV_IMAGE_WIDTH);
    WriteVarint(buff, GetVarintLen(imageInfo_.size.width));
    WriteVarint(buff, imageInfo_.size.width);
    WriteUint8(buff, TLV_IMAGE_HEIGHT);
    WriteVarint(buff, GetVarintLen(imageInfo_.size.height));
    WriteVarint(buff, imageInfo_.size.height);
    WriteUint8(buff, TLV_IMAGE_PIXELFORMAT);
    WriteVarint(buff, GetVarintLen(static_cast<int32_t>(imageInfo_.pixelFormat)));
    WriteVarint(buff, static_cast<int32_t>(imageInfo_.pixelFormat));
    WriteUint8(buff, TLV_IMAGE_COLORSPACE);
    WriteVarint(buff, GetVarintLen(static_cast<int32_t>(imageInfo_.colorSpace)));
    WriteVarint(buff, static_cast<int32_t>(imageInfo_.colorSpace));
    WriteUint8(buff, TLV_IMAGE_ALPHATYPE);
    WriteVarint(buff, GetVarintLen(static_cast<int32_t>(imageInfo_.alphaType)));
    WriteVarint(buff, static_cast<int32_t>(imageInfo_.alphaType));
    WriteUint8(buff, TLV_IMAGE_BASEDENSITY);
    WriteVarint(buff, GetVarintLen(imageInfo_.baseDensity));
    WriteVarint(buff, imageInfo_.baseDensity);
    WriteUint8(buff, TLV_IMAGE_ALLOCATORTYPE);
    AllocatorType tmpAllocatorType = allocatorType_;
    if (allocatorType_ == AllocatorType::SHARE_MEM_ALLOC) {
        tmpAllocatorType = AllocatorType::HEAP_ALLOC;
        HiLog::Info(LABEL, "pixel map tlv encode unsupport SHARE_MEM_ALLOC, use HEAP_ALLOC."\
                    "width: %{piblic}d, height: %{public}d",
                    imageInfo_.size.width, imageInfo_.size.height);
    }
    WriteVarint(buff, GetVarintLen(static_cast<int32_t>(tmpAllocatorType)));
    WriteVarint(buff, static_cast<int32_t>(tmpAllocatorType));
    WriteUint8(buff, TLV_IMAGE_DATA);
    const uint8_t *data = data_;
    int32_t dataSize = rowDataSize_ * imageInfo_.size.height;
    if (data == nullptr || size_t(dataSize) > MAX_IMAGEDATA_SIZE || dataSize <= 0) {
        WriteVarint(buff, 0); // L is zero and no value
        WriteUint8(buff, TLV_END); // end tag
        HiLog::Error(LABEL, "pixel map tlv encode fail: no data");
        return false;
    }
    WriteVarint(buff, dataSize);
    WriteData(buff, data, dataSize);
    WriteUint8(buff, TLV_END); // end tag
    return true;
}

void PixelMap::ReadTlvAttr(std::vector<uint8_t> &buff, ImageInfo &info, int32_t &type, int32_t &size, uint8_t **data)
{
    int cursor = 0;
    for (uint8_t tag = ReadUint8(buff, cursor); tag != TLV_END; tag = ReadUint8(buff, cursor)) {
        int32_t len = ReadVarint(buff, cursor);
        if (len <= 0 || static_cast<size_t>(cursor + len) > buff.size()) {
            HiLog::Error(LABEL, "ReadTlvAttr out of range");
            return;
        }
        switch (tag) {
            case TLV_IMAGE_WIDTH:
                info.size.width = ReadVarint(buff, cursor);
                break;
            case TLV_IMAGE_HEIGHT:
                info.size.height = ReadVarint(buff, cursor);
                break;
            case TLV_IMAGE_PIXELFORMAT:
                info.pixelFormat = static_cast<PixelFormat>(ReadVarint(buff, cursor));
                break;
            case TLV_IMAGE_COLORSPACE:
                info.colorSpace = static_cast<ColorSpace>(ReadVarint(buff, cursor));
                break;
            case TLV_IMAGE_ALPHATYPE:
                info.alphaType = static_cast<AlphaType>(ReadVarint(buff, cursor));
                break;
            case TLV_IMAGE_BASEDENSITY:
                info.baseDensity = ReadVarint(buff, cursor);
                break;
            case TLV_IMAGE_ALLOCATORTYPE:
                type = ReadVarint(buff, cursor);
                break;
            case TLV_IMAGE_DATA:
                size = len;
                *data = ReadData(buff, size, cursor);
                break;
            default:
                cursor += len; // skip unknown tag
                HiLog::Warn(LABEL, "pixel map tlv decode warn: unknown tag[%{public}d]", tag);
                break;
        }
    }
}

PixelMap *PixelMap::DecodeTlv(std::vector<uint8_t> &buff)
{
    PixelMap *pixelMap = new PixelMap();
    if (pixelMap == nullptr) {
        HiLog::Error(LABEL, "pixel map tlv decode fail: new PixelMap error");
        return nullptr;
    }
    ImageInfo imageInfo;
    int32_t dataSize = 0;
    uint8_t *data = nullptr;
    int32_t allocType = static_cast<int32_t>(AllocatorType::DEFAULT);
    ReadTlvAttr(buff, imageInfo, allocType, dataSize, &data);
    if (data == nullptr) {
        delete pixelMap;
        HiLog::Error(LABEL, "pixel map tlv decode fail: no data");
        return nullptr;
    }
    uint32_t ret = pixelMap->SetImageInfo(imageInfo);
    if (ret != SUCCESS) {
        free(data);
        delete pixelMap;
        HiLog::Error(LABEL, "pixel map tlv decode fail: set image info error[%{public}d]", ret);
        return nullptr;
    }
    if (dataSize != pixelMap->GetByteCount()) {
        delete pixelMap;
        HiLog::Error(LABEL, "pixel map tlv decode fail: dataSize not match");
        return nullptr;
    }
    pixelMap->SetPixelsAddr(data, nullptr, dataSize, static_cast<AllocatorType>(allocType), nullptr);
    return pixelMap;
}

static const string GetNamedAlphaType(const AlphaType alphaType)
{
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return "Alpha Type Unknown";
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return "Alpha Type Opaque";
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return "Alpha Type Premul";
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return "Alpha Type Unpremul";
        default:
            return "Alpha Type Unknown";
    }
    return "Alpha Type Unknown";
}

static const string GetNamedPixelFormat(const PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::UNKNOWN:
            return "Pixel Format UNKNOWN";
        case PixelFormat::RGB_565:
            return "Pixel Format RGB_565";
        case PixelFormat::RGB_888:
            return "Pixel Format RGB_888";
        case PixelFormat::NV21:
            return "Pixel Format NV21";
        case PixelFormat::NV12:
            return "Pixel Format NV12";
        case PixelFormat::CMYK:
            return "Pixel Format CMYK";
        case PixelFormat::ARGB_8888:
            return "Pixel Format ARGB_8888";
        case PixelFormat::ALPHA_8:
            return "Pixel Format ALPHA_8";
        case PixelFormat::RGBA_8888:
            return "Pixel Format RGBA_8888";
        case PixelFormat::BGRA_8888:
            return "Pixel Format BGRA_8888";
        case PixelFormat::RGBA_F16:
            return "Pixel Format RGBA_F16";
        default:
            return "Pixel Format UNKNOWN";
    }
    return "Pixel Format UNKNOWN";
}

constexpr uint8_t HALF_LOW_BYTE = 0;
constexpr uint8_t HALF_HIGH_BYTE = 1;

static float HalfTranslate(const uint8_t* ui)
{
    return HalfToFloat(U8ToU16(ui[HALF_HIGH_BYTE], ui[HALF_LOW_BYTE]));
}

static void HalfTranslate(const float pixel, uint8_t* ui)
{
    uint16_t val = FloatToHalf(pixel);
    ui[HALF_LOW_BYTE] = static_cast<uint8_t>((val >> SHIFT_8_BIT) & UINT8_MAX);
    ui[HALF_HIGH_BYTE] = static_cast<uint8_t>(val & UINT8_MAX);
}
constexpr uint8_t RGBA_F16_R_OFFSET = 0;
constexpr uint8_t RGBA_F16_G_OFFSET = 2;
constexpr uint8_t RGBA_F16_B_OFFSET = 4;
constexpr uint8_t RGBA_F16_A_OFFSET = 6;

static constexpr float FLOAT_NUMBER_NEAR_ZERO = 0.000001;
static constexpr float FLOAT_ZERO = 0.0f;
static float ProcessPremulF16Pixel(float mulPixel, float alpha, const float percent)
{
    if (alpha < FLOAT_NUMBER_NEAR_ZERO && alpha > -FLOAT_NUMBER_NEAR_ZERO) {
        return FLOAT_ZERO;
    }
    float res = mulPixel * percent / alpha;
    return res > MAX_HALF ? MAX_HALF : res;
}

static void SetF16PixelAlpha(uint8_t *pixel, const float percent, bool isPixelPremul)
{
    float A = HalfTranslate(pixel + RGBA_F16_A_OFFSET);
    if (isPixelPremul) {
        float R = HalfTranslate(pixel + RGBA_F16_R_OFFSET);
        float G = HalfTranslate(pixel + RGBA_F16_G_OFFSET);
        float B = HalfTranslate(pixel + RGBA_F16_B_OFFSET);
        R = ProcessPremulF16Pixel(R, A, percent);
        G = ProcessPremulF16Pixel(G, A, percent);
        B = ProcessPremulF16Pixel(B, A, percent);
        HalfTranslate(R, pixel + RGBA_F16_R_OFFSET);
        HalfTranslate(G, pixel + RGBA_F16_G_OFFSET);
        HalfTranslate(B, pixel + RGBA_F16_B_OFFSET);
    }
    A = percent * MAX_HALF;
    HalfTranslate(A, pixel + RGBA_F16_A_OFFSET);
}

static constexpr uint8_t U_ZERO = 0;
static uint8_t ProcessPremulPixel(uint8_t mulPixel, uint8_t alpha, const float percent)
{
    // mP = oP * oAlpha / UINT8_MAX
    // => oP = mP * UINT8_MAX / oAlpha
    // nP = oP * percent
    // => nP = mP * UINT8_MAX * percent / oAlpha
    if (alpha == 0) {
        return U_ZERO;
    }
    float nPixel = mulPixel * percent * UINT8_MAX / alpha;
    if ((nPixel + HALF_ONE) >= UINT8_MAX) {
        return UINT8_MAX;
    }
    return static_cast<uint8_t>(nPixel + HALF_ONE);
}

static void SetUintPixelAlpha(uint8_t *pixel, const float percent,
    uint8_t pixelByte, int8_t alphaIndex, bool isPixelPremul)
{
    if (isPixelPremul) {
        for (int32_t pixelIndex = 0; pixelIndex < pixelByte; pixelIndex++) {
            if (pixelIndex != alphaIndex) {
                pixel[pixelIndex] = ProcessPremulPixel(pixel[pixelIndex],
                    pixel[alphaIndex], percent);
            }
        }
    }
    pixel[alphaIndex] = static_cast<uint8_t>(UINT8_MAX * percent + HALF_ONE);
}

static int8_t GetAlphaIndex(const PixelFormat& pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::ARGB_8888:
        case PixelFormat::ALPHA_8:
            return ARGB_ALPHA_INDEX;
        case PixelFormat::RGBA_8888:
        case PixelFormat::BGRA_8888:
        case PixelFormat::RGBA_F16:
            return BGRA_ALPHA_INDEX;
        default:
            return INVALID_ALPHA_INDEX;
    }
}

uint32_t PixelMap::SetAlpha(const float percent)
{
    auto alphaType = GetAlphaType();
    if (alphaType == AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN ||
        alphaType == AlphaType::IMAGE_ALPHA_TYPE_OPAQUE) {
        HiLog::Error(LABEL,
            "Could not set alpha on %{public}s",
            GetNamedAlphaType(alphaType).c_str());
        return ERR_IMAGE_DATA_UNSUPPORT;
    }

    if (percent <= 0 || percent > 1) {
        HiLog::Error(LABEL,
            "Set alpha input should (0 < input <= 1). Current input %{public}f",
            percent);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    bool isPixelPremul = alphaType == AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    auto pixelFormat = GetPixelFormat();
    uint32_t pixelsSize = GetByteCount();
    int8_t alphaIndex = GetAlphaIndex(pixelFormat);
    if (alphaIndex == INVALID_ALPHA_INDEX) {
        HiLog::Error(LABEL, "Could not set alpha on %{public}s",
            GetNamedPixelFormat(pixelFormat).c_str());
        return ERR_IMAGE_DATA_UNSUPPORT;
    }

    if ((pixelFormat == PixelFormat::ALPHA_8 && pixelBytes_ != ALPHA_BYTES) ||
        (pixelFormat == PixelFormat::RGBA_F16 && pixelBytes_ != RGBA_F16_BYTES)) {
        HiLog::Error(LABEL, "Pixel format %{public}s mismatch pixelByte %{public}d",
            GetNamedPixelFormat(pixelFormat).c_str(), pixelBytes_);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    for (uint32_t i = 0; i < pixelsSize;) {
        uint8_t* pixel = data_ + i;
        if (pixelFormat == PixelFormat::RGBA_F16) {
            SetF16PixelAlpha(pixel, percent, isPixelPremul);
        } else {
            SetUintPixelAlpha(pixel, percent, pixelBytes_, alphaIndex, isPixelPremul);
        }
        i += pixelBytes_;
    }
    return SUCCESS;
}

static sk_sp<SkColorSpace> ToSkColorSpace(PixelMap *pixelmap)
{
#ifdef IMAGE_COLORSPACE_FLAG
    if (pixelmap->InnerGetGrColorSpacePtr() == nullptr) {
        return nullptr;
    }
    return pixelmap->InnerGetGrColorSpacePtr()->ToSkColorSpace();
#else
    return nullptr;
#endif
}

static SkImageInfo ToSkImageInfo(ImageInfo &info, sk_sp<SkColorSpace> colorSpace)
{
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(info.alphaType);
    HiLog::Debug(LABEL, "ToSkImageInfo w %{public}d, h %{public}d", info.size.width, info.size.height);
    HiLog::Debug(LABEL,
        "ToSkImageInfo pf %{public}s, at %{public}s, skpf %{public}s, skat %{public}s",
        ImageTypeConverter::ToName(info.pixelFormat).c_str(),
        ImageTypeConverter::ToName(info.alphaType).c_str(),
        ImageTypeConverter::ToName(colorType).c_str(),
        ImageTypeConverter::ToName(alphaType).c_str()
    );
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

static void ToImageInfo(ImageInfo &info, SkImageInfo &skInfo, bool sizeOnly = true)
{
    info.size.width = skInfo.width();
    info.size.height = skInfo.height();
    if (!sizeOnly) {
        info.alphaType = ImageTypeConverter::ToAlphaType(skInfo.alphaType());
        info.pixelFormat = ImageTypeConverter::ToPixelFormat(skInfo.colorType());
    }
}

struct SkTransInfo {
    SkRect r;
    SkImageInfo info;
    SkBitmap bitmap;
};

struct TransMemoryInfo {
    AllocatorType allocType;
    std::unique_ptr<AbsMemory> memory = nullptr;
};

constexpr float HALF = 0.5f;

static inline int FloatToInt(float a)
{
    return static_cast<int>(a + HALF);
}

#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
static void GenSrcTransInfo(SkTransInfo &srcInfo, ImageInfo &imageInfo, PixelMap* pixelmap,
    sk_sp<SkColorSpace> colorSpace)
{
    srcInfo.r = SkRect::MakeIWH(imageInfo.size.width, imageInfo.size.height);
    srcInfo.info = ToSkImageInfo(imageInfo, colorSpace);
    uint64_t rowStride = srcInfo.info.minRowBytes();
    if (pixelmap->GetAllocatorType() == AllocatorType::DMA_ALLOC) {
        if (pixelmap->GetFd() == nullptr) {
            HiLog::Error(LABEL, "GenSrcTransInfo get surfacebuffer failed");
        }
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*>(pixelmap->GetFd());
        rowStride = sbBuffer->GetStride();
    }
    srcInfo.bitmap.installPixels(srcInfo.info, static_cast<uint8_t *>(pixelmap->GetWritablePixels()), rowStride);
}
#else
static void GenSrcTransInfo(SkTransInfo &srcInfo, ImageInfo &imageInfo, uint8_t* pixels,
    sk_sp<SkColorSpace> colorSpace)
{
    srcInfo.r = SkRect::MakeIWH(imageInfo.size.width, imageInfo.size.height);
    srcInfo.info = ToSkImageInfo(imageInfo, colorSpace);
    srcInfo.bitmap.installPixels(srcInfo.info, pixels, srcInfo.info.minRowBytes());
}
#endif

static bool GendstTransInfo(SkTransInfo &srcInfo, SkTransInfo &dstInfo, SkMatrix &matrix,
    TransMemoryInfo &memoryInfo)
{
    dstInfo.r = matrix.mapRect(srcInfo.r);
    int width = FloatToInt(dstInfo.r.width());
    int height = FloatToInt(dstInfo.r.height());
    if (matrix.isTranslate()) {
        width += dstInfo.r.fLeft;
        height += dstInfo.r.fTop;
    }
    dstInfo.info = srcInfo.info.makeWH(width, height);
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    Size desiredSize = {dstInfo.info.width(), dstInfo.info.height()};
    MemoryData memoryData = {nullptr, dstInfo.info.computeMinByteSize(), "Trans ImageData", desiredSize};
#else
    MemoryData memoryData = {nullptr, dstInfo.info.computeMinByteSize(), "Trans ImageData"};
#endif
    std::unique_ptr<AbsMemory> dstMemory = MemoryManager::CreateMemory(memoryInfo.allocType, memoryData);
    if (dstMemory == nullptr) {
        HiLog::Error(LABEL, "CreateMemory falied");
        return false;
    }
    memoryInfo.memory = std::move(dstMemory);
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    uint64_t rowStride = dstInfo.info.minRowBytes();
    if (memoryInfo.allocType == AllocatorType::DMA_ALLOC) {
        if (memoryInfo.memory->extend.data == nullptr) {
            HiLog::Error(LABEL, "GendstTransInfo get surfacebuffer failed");
        }
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*>(memoryInfo.memory->extend.data);
        rowStride = sbBuffer->GetStride();
    }
    dstInfo.bitmap.installPixels(dstInfo.info, memoryInfo.memory->data.data, rowStride);
#else
    dstInfo.bitmap.installPixels(dstInfo.info, memoryInfo.memory->data.data, dstInfo.info.minRowBytes());
#endif
    return true;
}

struct TransInfos {
    SkMatrix matrix;
};

bool IsSupportAntiAliasing(const ImageInfo& imageInfo)
{
    return imageInfo.size.width <= AntiAliasingSize &&
            imageInfo.size.height <= AntiAliasingSize;
}

bool PixelMap::DoTranslation(TransInfos &infos)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);

    TransMemoryInfo dstMemory;
    if (allocatorType_ == AllocatorType::CUSTOM_ALLOC) {
        // We dont know how custom alloc memory
        dstMemory.allocType = AllocatorType::DEFAULT;
    } else {
        dstMemory.allocType = allocatorType_;
    }

    SkTransInfo src;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    GenSrcTransInfo(src, imageInfo, this, ToSkColorSpace(this));
#else
    GenSrcTransInfo(src, imageInfo, data_, ToSkColorSpace(this));
#endif

    SkTransInfo dst;
    if (!GendstTransInfo(src, dst, infos.matrix, dstMemory)) {
        HiLog::Error(LABEL, "GendstTransInfo dstMemory falied");
        return false;
    }

    SkCanvas canvas(dst.bitmap);
    if (!infos.matrix.isTranslate()) {
        if (!EQUAL_TO_ZERO(dst.r.fLeft) || !EQUAL_TO_ZERO(dst.r.fTop)) {
            canvas.translate(-dst.r.fLeft, -dst.r.fTop);
        }
    }
    canvas.concat(infos.matrix);
    auto skimage = SkImage::MakeFromBitmap(src.bitmap);
    if (ImageSystemProperties::GetAntiAliasingEnabled() && IsSupportAntiAliasing(imageInfo)) {
        canvas.drawImage(skimage, FLOAT_ZERO, FLOAT_ZERO,
            SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear));
    } else {
        canvas.drawImage(skimage, FLOAT_ZERO, FLOAT_ZERO);
    }

    ToImageInfo(imageInfo, dst.info);
#ifdef IMAGE_COLORSPACE_FLAG
    if (dst.bitmap.refColorSpace() != nullptr) {
        grColorSpace_ = make_shared<OHOS::ColorManager::ColorSpace>(dst.bitmap.refColorSpace());
    }
#endif
    auto m = dstMemory.memory.get();
    SetPixelsAddr(m->data.data, m->extend.data, m->data.size, m->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    return true;
}

void PixelMap::scale(float xAxis, float yAxis)
{
    TransInfos infos;
    infos.matrix.setScale(xAxis, yAxis);
    if (!DoTranslation(infos)) {
        HiLog::Error(LABEL, "scale falied");
    }
}

bool PixelMap::resize(float xAxis, float yAxis)
{
    TransInfos infos;
    infos.matrix.setScale(xAxis, yAxis);
    if (!DoTranslation(infos)) {
        HiLog::Error(LABEL, "resize falied");
        return false;
    }
    return true;
}

void PixelMap::translate(float xAxis, float yAxis)
{
    TransInfos infos;
    infos.matrix.setTranslate(xAxis, yAxis);
    if (!DoTranslation(infos)) {
        HiLog::Error(LABEL, "translate falied");
    }
}

void PixelMap::rotate(float degrees)
{
    TransInfos infos;
    infos.matrix.setRotate(degrees);
    if (!DoTranslation(infos)) {
        HiLog::Error(LABEL, "rotate falied");
    }
}

void PixelMap::flip(bool xAxis, bool yAxis)
{
    if (xAxis == false && yAxis == false) {
        return;
    }
    scale(xAxis ? -1 : 1, yAxis ? -1 : 1);
}

uint32_t PixelMap::crop(const Rect &rect)
{
    ImageInfo imageInfo;
    GetImageInfo(imageInfo);

    SkTransInfo src;
#if !defined(_WIN32) && !defined(_APPLE) && !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    GenSrcTransInfo(src, imageInfo, this, ToSkColorSpace(this));
#else
    GenSrcTransInfo(src, imageInfo, data_, ToSkColorSpace(this));
#endif

    SkTransInfo dst;
    SkIRect dstIRect = SkIRect::MakeXYWH(rect.left, rect.top, rect.width, rect.height);
    dst.r = SkRect::Make(dstIRect);
    if (dst.r == src.r) {
        return SUCCESS;
    }

    if (!src.r.contains(dst.r)) {
        HiLog::Error(LABEL, "Invalid crop rect");
        return ERR_IMAGE_CROP;
    }
    dst.info = src.info.makeWH(dstIRect.width(), dstIRect.height());
    Size desiredSize = {dst.info.width(), dst.info.height()};
    MemoryData memoryData = {nullptr, dst.info.computeMinByteSize(), "Trans ImageData", desiredSize};
    auto m = MemoryManager::CreateMemory(allocatorType_, memoryData);
    if (m == nullptr) {
        HiLog::Error(LABEL, "crop CreateMemory failed");
        return ERR_IMAGE_CROP;
    }
    uint64_t rowStride = dst.info.minRowBytes();
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    if (allocatorType_ == AllocatorType::DMA_ALLOC) {
        if (m->extend.data == nullptr) {
            HiLog::Error(LABEL, "GendstTransInfo get surfacebuffer failed");
        }
        SurfaceBuffer* sbBuffer = reinterpret_cast<SurfaceBuffer*>(m->extend.data);
        rowStride = sbBuffer->GetStride();
    }
#endif
    if (!src.bitmap.readPixels(dst.info, m->data.data, rowStride,
        dstIRect.fLeft, dstIRect.fTop)) {
        HiLog::Error(LABEL, "ReadPixels failed");
        return ERR_IMAGE_CROP;
    }
    ToImageInfo(imageInfo, dst.info);
#ifdef IMAGE_COLORSPACE_FLAG
    if (dst.info.refColorSpace() != nullptr) {
        grColorSpace_ = make_shared<OHOS::ColorManager::ColorSpace>(dst.info.refColorSpace());
    }
#endif
    SetPixelsAddr(m->data.data, m->extend.data, m->data.size, m->GetType(), nullptr);
    SetImageInfo(imageInfo, true);
    return SUCCESS;
}

#ifdef IMAGE_COLORSPACE_FLAG
    void PixelMap::InnerSetColorSpace(const OHOS::ColorManager::ColorSpace &grColorSpace)
    {
        grColorSpace_ = std::make_shared<OHOS::ColorManager::ColorSpace>(grColorSpace.ToSkColorSpace(),
            grColorSpace.GetColorSpaceName());
    }

    OHOS::ColorManager::ColorSpace PixelMap::InnerGetGrColorSpace()
    {
        if (grColorSpace_ == nullptr) {
            grColorSpace_ =
                std::make_shared<OHOS::ColorManager::ColorSpace>(OHOS::ColorManager::ColorSpaceName::SRGB);
        }
        return *grColorSpace_;
    }
#endif
} // namespace Media
} // namespace OHOS
