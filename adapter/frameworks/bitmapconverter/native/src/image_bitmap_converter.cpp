/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2015 The Android Open Source Project
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

#include "image_bitmap_converter.h"
#include "SkBitmap.h"
#include "SkImageInfo.h"

#include "media_errors.h"
#if defined(_WIN32) || defined(_APPLE)
#include <string.h>
#else
#include <asm-generic/mman-common.h>
#include <sys/mman.h>
#include "ashmem.h"
#include "securec.h"
#endif

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;

static constexpr uint32_t G_MAX_COLOR_SPACE_SERIALIZED_BYTES = 80;

enum {
    RAW_INPLACE = 0,
    RAW_ASHMEM_IMMUTABLE = 1,
    RAW_ASHMEM_MUTABLE = 2,
};

SkColorType ImageBitmapConverter::GetColorType(PixelFormat pixelFormat)
{
    SkColorType colorType = kUnknown_SkColorType;
    switch (pixelFormat) {
        case PixelFormat::ALPHA_8: {
            colorType = kAlpha_8_SkColorType;
            break;
        }
        case PixelFormat::RGB_565: {
            colorType = kRGB_565_SkColorType;
            break;
        }
        case PixelFormat::RGBA_F16: {
            colorType = kRGBA_F16_SkColorType;
            break;
        }
        case PixelFormat::RGBA_8888:
        case PixelFormat::BGRA_8888: {
            colorType = kN32_SkColorType;
            break;
        }
        default: {
            HiLog::Error(LABEL, "convert from pixel format:%{public}d to color type failed.",
                         static_cast<int32_t>(pixelFormat));
            break;
        }
    }

    return colorType;
}

SkAlphaType ImageBitmapConverter::ConvertToSkAlphaType(AlphaType alphaType)
{
    SkAlphaType skAlphaType = kUnknown_SkAlphaType;
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE: {
            skAlphaType = kOpaque_SkAlphaType;
            break;
        }
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL: {
            skAlphaType = kPremul_SkAlphaType;
            break;
        }
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL: {
            skAlphaType = kUnpremul_SkAlphaType;
            break;
        }
        default: {
            HiLog::Error(LABEL, "unknown alpha type:%{public}d.", static_cast<int32_t>(alphaType));
            break;
        }
    }
    return skAlphaType;
}

void ImageBitmapConverter::FreeBitmap(void *addr, void *context)
{
    // free bitmap do nothing, the bitmap malloc data will be released by pixelmap destruct function.
    HiLog::Debug(LABEL, "free bitmap.");
    return;
}

android::BitmapWrapper *ImageBitmapConverter::CreateShadowBitmap(PixelMap *pixelMap)
{
    if (pixelMap == nullptr) {
        HiLog::Error(LABEL, "create shadow bitmap error, input parameter is null.");
        return nullptr;
    }
    uint8_t *base = const_cast<uint8_t *>(pixelMap->GetPixels());
    if (base == nullptr) {
        HiLog::Error(LABEL, "create shadow bitmap error, pixel addr is null.");
        return nullptr;
    }
    int32_t width = pixelMap->GetWidth();
    int32_t height = pixelMap->GetHeight();
    int32_t rowBytes = pixelMap->GetRowBytes();
    PixelFormat pixelFormat = pixelMap->GetPixelFormat();
    SkColorType colorType = GetColorType(pixelFormat);
    AlphaType alphaType = pixelMap->GetAlphaType();
    SkAlphaType skAlphaType = ConvertToSkAlphaType(alphaType);
    SkImageInfo skImageInfo = SkImageInfo::Make(width, height, colorType, skAlphaType, SkColorSpace::MakeSRGB());
    sk_sp<android::Bitmap> bitmap = nullptr;
    AllocatorType allocType = pixelMap->GetAllocatorType();
    if (allocType == AllocatorType::SHARE_MEM_ALLOC) {
        int32_t *fd = static_cast<int32_t *>(pixelMap->GetFd());
        if (fd == nullptr || *fd < 0) {
            HiLog::Error(LABEL, "CreateShadowBitmap fd is invalid.");
            return nullptr;
        }
        SkBitmap skBitmap;
        if (!skBitmap.setInfo((skImageInfo), rowBytes)) {
            HiLog::Error(LABEL, "CreateShadowBitmap, setInfo failed!");
            return nullptr;
        }
        size_t bitmapSize = skBitmap.computeByteSize();
        int32_t dupFd = fcntl(*fd, F_DUPFD_CLOEXEC, 0);
        if (dupFd < 0) {
            HiLog::Error(LABEL, "CreateShadowBitmap, dupFd < 0.");
            return nullptr;
        }
        bitmap = android::Bitmap::createFrom(skImageInfo, rowBytes, dupFd, nullptr, bitmapSize, false);
        if (bitmap == nullptr) {
            HiLog::Error(LABEL, "CreateShadowBitmap malloc bitmap createFrom failed.");
            ::close(dupFd);
            return nullptr;
        }
    } else {
        SkPixelRef *skPixelRef = new (std::nothrow) SkPixelRef(width, height, base, rowBytes);
        if (skPixelRef == nullptr) {
            HiLog::Error(LABEL, "skPixelRef is nullptr.");
            return nullptr;
        }
        bitmap = android::Bitmap::createFrom(skImageInfo, *skPixelRef);
        if (bitmap.get() == nullptr) {
            HiLog::Error(LABEL, "malloc shadow bitmap error.");
            delete skPixelRef;
            return nullptr;
        }
    }
    android::BitmapWrapper *bitmapWrapper = new (std::nothrow) android::BitmapWrapper(bitmap.get());
    if (bitmapWrapper == nullptr) {
        HiLog::Error(LABEL, "malloc shadow bitmap wrapper error.");
        return nullptr;
    }
    (void)bitmap.release();
    return bitmapWrapper;
}

PixelFormat ImageBitmapConverter::GetPixelFormat(SkColorType colorType)
{
    PixelFormat pixelFormat = PixelFormat::UNKNOWN;
    switch (colorType) {
        case kAlpha_8_SkColorType: {
            pixelFormat = PixelFormat::ALPHA_8;
            break;
        }
        case kRGB_565_SkColorType: {
            pixelFormat = PixelFormat::RGB_565;
            break;
        }
        case kRGBA_F16_SkColorType: {
            pixelFormat = PixelFormat::RGBA_F16;
            break;
        }
        case kRGBA_8888_SkColorType: {
            pixelFormat = PixelFormat::RGBA_8888;
            break;
        }
        case kBGRA_8888_SkColorType: {
            pixelFormat = PixelFormat::BGRA_8888;
            break;
        }
        default: {
            HiLog::Error(LABEL, "convert from color type:%{public}d to pixel format failed.",
                         static_cast<int32_t>(colorType));
            break;
        }
    }
    return pixelFormat;
}

AlphaType ImageBitmapConverter::ConvertToAlphaType(SkAlphaType at)
{
    AlphaType alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    switch (at) {
        case kOpaque_SkAlphaType: {
            alphaType = AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
            break;
        }
        case kPremul_SkAlphaType: {
            alphaType = AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
            break;
        }
        case kUnpremul_SkAlphaType: {
            alphaType = AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
            break;
        }
        default: {
            HiLog::Error(LABEL, "unknown skia alpha type:%{public}d.", static_cast<int32_t>(at));
            break;
        }
    }
    return alphaType;
}

void ImageBitmapConverter::FreePixelMapFunc(void *addr, void *context, uint32_t size)
{
    android::BitmapWrapper *bitmap = static_cast<android::BitmapWrapper *>(context);
    if (bitmap != nullptr) {
        bitmap->freePixels();
    }
}

PixelMap *ImageBitmapConverter::CreateShellPixelMap(android::BitmapWrapper *bitmapWrapper)
{
    if (bitmapWrapper == nullptr) {
        HiLog::Error(LABEL, "create shell pixel map error, input parameter is null.");
        return nullptr;
    }
    SkBitmap skBitmap;
    bitmapWrapper->getSkBitmap(&skBitmap);
    return CreateSkShellPixelMap(skBitmap, bitmapWrapper);
}

void ImageBitmapConverter::PixelMapWriteToParcel(PixelMap *pixelMap, int density, MessageParcel *p)
{
#ifndef _WIN32
    android::BitmapWrapper *bitmapWrapper = ImageBitmapConverter::CreateShadowBitmap(pixelMap);
    SkBitmap bitmap;
    bitmapWrapper->getSkBitmap(&bitmap);

    bool isMutable = pixelMap->IsEditable();
    p->WriteInt32(isMutable);
    p->WriteInt32(bitmap.colorType());
    p->WriteInt32(bitmap.alphaType());
    SkColorSpace* colorSpace = bitmap.colorSpace();
    if (colorSpace != nullptr) {
        sk_sp<SkData> data = colorSpace->serialize();
        size_t size = data->size();
        p->WriteUint32(size);
        if (size > 0) {
            if (size > G_MAX_COLOR_SPACE_SERIALIZED_BYTES) {
                HiLog::Error(LABEL, "Serialized SkColorSpace is larger than expected, ret:%{public}zu.", size);
            }
            p->WriteBuffer(data->data(), size);
        }
    } else {
        p->WriteUint32(0);
    }
    p->WriteInt32(bitmap.width());
    p->WriteInt32(bitmap.height());
    p->WriteInt32(bitmap.rowBytes());
    p->WriteInt32(density);

    // Transfer the underlying ashmem region if we have one and it's immutable.
    android::status_t status;
    int fd = bitmapWrapper->bitmap().getAshmemFd();
    if (fd >= 0 && !isMutable) {
        status = p->WriteFileDescriptor(fd);
        if (status) {
            HiLog::Error(LABEL, "Could not write file descriptor.");
            return;
        }
        ::close(fd);
        return;
    }

    size_t size = bitmap.computeByteSize();
    const void* pSrc = bitmap.getPixels();
    if (pSrc == NULL) {
        if (size == 0) {
            HiLog::Error(LABEL, "malloc failed, size %{public}zu.", size);
            return;
        }
        void* data = malloc(size);
        if (data == nullptr) {
            HiLog::Error(LABEL, "malloc failed, size %{public}zu.", size);
            return;
        }
        if (memset_s(data, size, 0, size) != EOK) {
            HiLog::Error(LABEL, "memset failed");
            free(data);
            return;
        }
        WriteBigData(p, data, size, isMutable);
        free(data);
    } else {
        WriteBigData(p, pSrc, size, isMutable);
    }
#endif
    return;
}

#ifndef _WIN32
void ImageBitmapConverter::WriteBigData(MessageParcel *p, const void* data, size_t size, bool isMutable)
{
    HiLog::Debug(LABEL, "WriteBigData size:[%{public}lld] isMutable:[%{public}d]", (long long)size, isMutable);
    if (size < SIZE_LIMIT) {
        p->WriteInt32(RAW_INPLACE);
        p->WriteBuffer(data, size);
        return;
    }
    int fd = AshmemCreate("Parcel RawData", size);
    if (fd < 0) {
        return;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        ::close(fd);
        return;
    }
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        ::close(fd);
        return;
    }
    if (isMutable) {
        p->WriteInt32(RAW_ASHMEM_MUTABLE);
    } else {
        result = AshmemSetProt(fd, PROT_READ);
        if (result < 0) {
            ::munmap(ptr, size);
            ::close(fd);
            return;
        }
        p->WriteInt32(RAW_ASHMEM_IMMUTABLE);
    }
    if (!p->WriteFileDescriptor(fd)) {
        ::munmap(ptr, size);
        ::close(fd);
        return;
    }
    if (memcpy_s(ptr, size, data, size) != EOK) {
        ::munmap(ptr, size);
        ::close(fd);
        return;
    }
    ::munmap(ptr, size);
    ::close(fd);
    return;
}
#endif
PixelMap *ImageBitmapConverter::CreateSkShellPixelMap(SkBitmap &skBitmap, void *context, PixelFormat pixelFormat,
    ColorSpace colorSpace, bool copy)
{
    void *base = skBitmap.getPixels();
    if (base == nullptr) {
        HiLog::Error(LABEL, "create skbitmap shell pixel map error, bitmap addr is null.");
        return nullptr;
    }
    int32_t width = skBitmap.width();
    int32_t height = skBitmap.height();
    SkColorType colorType = skBitmap.colorType();
    SkAlphaType skAlphaType = skBitmap.alphaType();
    size_t byteCount = skBitmap.rowBytes() * height;
    PixelMap *pixelMap = new (std::nothrow) PixelMap();
    if (pixelMap == nullptr) {
        HiLog::Error(LABEL, "malloc skbitmap shell pixel map error.");
        return nullptr;
    }
    ImageInfo info;
    info.size.width = width;
    info.size.height = height;
    info.pixelFormat = (pixelFormat == PixelFormat::UNKNOWN ? GetPixelFormat(colorType) : pixelFormat);
    info.alphaType = ConvertToAlphaType(skAlphaType);
    info.colorSpace = colorSpace;
    uint32_t ret = pixelMap->SetImageInfo(info);
    if (ret != SUCCESS) {
        delete pixelMap;
        HiLog::Error(LABEL, "skbitmap pixel map set image info failed, ret:%{public}u.", ret);
        return nullptr;
    }
    if (copy) {
        void *dstPixels = malloc(byteCount);
        if (dstPixels == nullptr) {
            HiLog::Error(LABEL, "allocate memory byteCount %{public}zu fail", byteCount);
            return nullptr;
        }
        if (memcpy_s(dstPixels, byteCount, base, byteCount) != EOK) {
            free(dstPixels);
            return nullptr;
        }
        skBitmap.reset();
        pixelMap->SetPixelsAddr(dstPixels, nullptr, byteCount, AllocatorType::HEAP_ALLOC, nullptr);
    } else {
        pixelMap->SetPixelsAddr(base, context, byteCount, AllocatorType::CUSTOM_ALLOC, FreePixelMapFunc);
    }
    return pixelMap;
}

PixelMap *ImageBitmapConverter::createFromAlpha(PixelMap *pixelMapSrc)
{
    if (pixelMapSrc) {
        SkBitmap src;
        SkBitmap dst;
        CreateShadowBitmap(pixelMapSrc)->getSkBitmap(&src);
        if (!src.extractAlpha(&dst) || (dst.getPixels() == nullptr && src.getPixels() != nullptr)) {
            HiLog::Error(LABEL, "failed to allocate pixels for alpha");
            return nullptr;
        }
        return CreateSkShellPixelMap(dst, nullptr, PixelFormat::ALPHA_8, ColorSpace::SRGB, true);
    } else {
        HiLog::Error(LABEL, "input PixelMap is null, unable to get alpha from a null PixelMap");
    }
    return nullptr;
}
} // namespace Media
} // namespace OHOS
