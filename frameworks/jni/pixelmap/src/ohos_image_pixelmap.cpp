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

#include "ohos_image_pixelmap.h"
#include <fcntl.h>
#include <unistd.h>
#include "hilog/log.h"
#include "log_tags.h"
#include "media_errors.h"
#include "message_parcel.h"
#include "ohos_image_jni_utils.h"
#include "ohos_utils_parcel.h"
#include "parcel.h"
#include "pixel_map.h"
#include "pixel_map_jni_utils.h"
#include "pixel_map_manager.h"
#ifndef _WIN32
#include "securec.h"
#else
#include "memory.h"
#endif
#ifdef _WIN32
#include <cstring>
#include <memory>
#endif

#if !defined(_WIN32) && !defined(_APPLE)
#include <sys/mman.h>
#include "ashmem.h"
#include "image_bitmap_converter.h"
#endif

using namespace OHOS::HiviewDFX;
using namespace OHOS::Media;

namespace {
constexpr int32_t PIXEL_MAP_INFO_MAX_LENGTH = 128;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "PixelMap_JNI" };
jclass g_pixelMapClz;
jmethodID g_pixelMapConstructorMethodId;
jfieldID g_nativePixelMapFieldId;

// java default byte order is big endian, native is little endian, here implements type convert
void PixelFormatToNative(PixelFormat &pixelFormat)
{
    if (pixelFormat == PixelFormat::ARGB_8888) {
        pixelFormat = PixelFormat::RGBA_8888;
    }
}

void PixelFormatToKits(PixelFormat &pixelFormat)
{
    if (pixelFormat == PixelFormat::RGBA_8888) {
        pixelFormat = PixelFormat::ARGB_8888;
    }
}

jobject MakeImageInfoObj(JNIEnv *env)
{
    jclass imageInfoClazz = env->FindClass("ohos/media/image/common/ImageInfo");
    if (imageInfoClazz == nullptr) {
        HiLog::Error(LABEL, "MakeImageInfoObj, ImageInfo class not found .");
        return nullptr;
    }
    jmethodID imgInfoMID = env->GetMethodID(imageInfoClazz, "<init>", "()V");
    if (imgInfoMID == nullptr) {
        HiLog::Error(LABEL, "MakeImageInfoObj, imgInfoMID is null.");
        env->DeleteLocalRef(imageInfoClazz);
        return nullptr;
    }
    return env->NewObject(imageInfoClazz, imgInfoMID);
}

jobject MakeSizeObj(JNIEnv *env, Size &size)
{
    jclass sizeClazz = env->FindClass("ohos/media/image/common/Size");
    if (sizeClazz == nullptr) {
        HiLog::Error(LABEL, "MakeSizeObj, Size class not found .");
        return nullptr;
    }

    jmethodID sizeConstrocMID = env->GetMethodID(sizeClazz, "<init>", "(II)V");
    if (sizeConstrocMID == nullptr) {
        HiLog::Error(LABEL, "MakeSizeObj, sizeConstrocMID is null.");
        env->DeleteLocalRef(sizeClazz);
        return nullptr;
    }

    return env->NewObject(sizeClazz, sizeConstrocMID, size.width, size.height);
}

jobject ohos_media_image_PixelMap_nativeGetImageInfo(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native pointer is null");
        return nullptr;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    ImageInfo imgInfo;
    pixelMap.GetImageInfo(imgInfo);
    jclass imageInfoClazz = env->FindClass("ohos/media/image/common/ImageInfo");
    if (imageInfoClazz == nullptr) {
        HiLog::Error(LABEL, "can't find ImageInfo class");
        return nullptr;
    }
    jfieldID sizeFileID = env->GetFieldID(imageInfoClazz, "size", "Lohos/media/image/common/Size;");
    jmethodID setPixelFormatID = env->GetMethodID(imageInfoClazz, "setPixelFormat", "(I)V");
    jmethodID setColorSpaceID = env->GetMethodID(imageInfoClazz, "setColorSpace", "(I)V");
    jmethodID setAlphaTypeID = env->GetMethodID(imageInfoClazz, "setAlphaType", "(I)V");
    env->DeleteLocalRef(imageInfoClazz);
    if (sizeFileID == nullptr || setPixelFormatID == nullptr || setColorSpaceID == nullptr ||
        setAlphaTypeID == nullptr) {
        HiLog::Error(LABEL, "imageinfo attributes is invalid");
        return nullptr;
    }

    jobject imgInfoObj = MakeImageInfoObj(env);
    if (imgInfoObj == nullptr) {
        HiLog::Error(LABEL, "nativeGetImageInfo obj is null.");
        return nullptr;
    }
    PixelFormatToKits(imgInfo.pixelFormat);
    env->CallVoidMethod(imgInfoObj, setPixelFormatID, jint(imgInfo.pixelFormat));
    env->CallVoidMethod(imgInfoObj, setColorSpaceID, jint(imgInfo.colorSpace));
    env->CallVoidMethod(imgInfoObj, setAlphaTypeID, jint(imgInfo.alphaType));

    jobject sizeObj = MakeSizeObj(env, imgInfo.size);
    if (sizeObj == nullptr) {
        HiLog::Error(LABEL, "nativeGetImageInfo size obj is null.");
        env->DeleteLocalRef(imgInfoObj);
        return nullptr;
    }
    env->SetObjectField(imgInfoObj, sizeFileID, sizeObj);
    env->DeleteLocalRef(sizeObj);
    return imgInfoObj;
}

void ohos_media_image_PixelMap_nativeRelease(JNIEnv *env, jobject thiz, jlong nativePtr, jlong nativeRes)
{
    HiLog::Debug(LABEL, "nativeRelease begin");
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native pointer is null");
        return;
    }
    pixelMapManager->FreePixels();
    env->DeleteGlobalRef(reinterpret_cast<jobject>(nativeRes));
    HiLog::Debug(LABEL, "nativeRelease end");
}

jboolean ohos_media_image_PixelMap_nativeWriteToParcel(JNIEnv *env, jobject thiz, jlong nativePtr, jobject parcel)
{
    if (parcel == NULL) {
        HiLog::Error(LABEL, "writeToParcel null parcel.");
        return false;
    }

    jclass zParcelClazz = env->GetObjectClass(parcel);
    jfieldID zNativeHandleField = env->GetFieldID(zParcelClazz, "nativeHandle", "J");
    env->DeleteLocalRef(zParcelClazz);
    if (zNativeHandleField == nullptr) {
        HiLog::Error(LABEL, "zNativeHandleField is invalid");
        return false;
    }

    jlong zParcelNative = env->GetLongField(parcel, zNativeHandleField);
    auto nativeParcel = reinterpret_cast<OHOS::MessageParcel *>(zParcelNative);

    if (nativeParcel == nullptr) {
        HiLog::Error(LABEL, "get native parcel failed.");
        return false;
    }
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "write to parcel failed, pixel map pointer is null.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    int32_t bufferSize = pixelMap.GetByteCount();
    if (static_cast<size_t>(bufferSize + PIXEL_MAP_INFO_MAX_LENGTH) > nativeParcel->GetDataCapacity() &&
        !nativeParcel->SetDataCapacity(bufferSize + PIXEL_MAP_INFO_MAX_LENGTH)) {
        HiLog::Error(LABEL, "set parcel max capacity:[%{public}d] failed.", bufferSize + PIXEL_MAP_INFO_MAX_LENGTH);
        return false;
    }
    if (!nativeParcel->WriteInt32(pixelMap.GetWidth())) {
        HiLog::Error(LABEL, "write pixel map width:[%{public}d] to parcel failed.", pixelMap.GetWidth());
        return false;
    }
    if (!nativeParcel->WriteInt32(pixelMap.GetHeight())) {
        HiLog::Error(LABEL, "write pixel map height:[%{public}d] to parcel failed.", pixelMap.GetHeight());
        return false;
    }
    if (!nativeParcel->WriteInt32(static_cast<int32_t>(pixelMap.GetPixelFormat()))) {
        HiLog::Error(LABEL, "write pixel map pixel format:[%{public}d] to parcel failed.", pixelMap.GetPixelFormat());
        return false;
    }
    if (!nativeParcel->WriteInt32(static_cast<int32_t>(pixelMap.GetColorSpace()))) {
        HiLog::Error(LABEL, "write pixel map color space:[%{public}d] to parcel failed.", pixelMap.GetColorSpace());
        return false;
    }
    if (!nativeParcel->WriteInt32(static_cast<int32_t>(pixelMap.GetAlphaType()))) {
        HiLog::Error(LABEL, "write pixel map alpha type:[%{public}d] to parcel failed.", pixelMap.GetAlphaType());
        return false;
    }
    if (!nativeParcel->WriteInt32(pixelMap.GetBaseDensity())) {
        HiLog::Error(LABEL, "write pixel map base density:[%{public}d] to parcel failed.", pixelMap.GetBaseDensity());
        return false;
    }
    if (!nativeParcel->WriteInt32(bufferSize)) {
        HiLog::Error(LABEL, "write pixel map buffer size:[%{public}d] to parcel failed.", bufferSize);
        return false;
    }
    if (!nativeParcel->WriteInt32(static_cast<int32_t>(pixelMap.GetAllocatorType()))) {
        HiLog::Error(LABEL, "write pixel map allocator type:[%{public}d] to parcel failed.", pixelMap.GetAllocatorType());
        return false;
    }
    if (pixelMap.GetAllocatorType() == AllocatorType::SHARE_MEM_ALLOC) {
#if !defined(_WIN32) && !defined(_APPLE)
        int *fd = static_cast<int *>(pixelMap.GetFd());
        if (*fd < 0) {
            HiLog::Error(LABEL, "write pixel map failed, fd < 0.");
            return false;
        }
        if (!nativeParcel->WriteFileDescriptor(*fd)) {
            HiLog::Error(LABEL, "write pixel map fd:[%{public}d] to parcel failed.", *fd);
            return false;
        }
#endif
    } else {
        const uint8_t *addr = pixelMap.GetPixels();
        if (addr == nullptr) {
            HiLog::Error(LABEL, "write to parcel failed, pixel memory is null.");
            return false;
        }
        if (!nativeParcel->WriteBuffer(addr, bufferSize)) {
            HiLog::Error(LABEL, "write pixel map buffer to parcel failed.");
            return false;
        }
    }
    return true;
}

jobject CreatePixelMapObject(JNIEnv *env, PixelMapManager *pixelMapManager)
{
    return env->NewObject(g_pixelMapClz, g_pixelMapConstructorMethodId, reinterpret_cast<jlong>(pixelMapManager),
                          static_cast<jlong>(pixelMapManager->GetByteCount()));
}

void ReleaseMemory(OHOS::Media::AllocatorType allocType, void *addr, int fd, uint32_t size, void *base)
{
    if (allocType == AllocatorType::SHARE_MEM_ALLOC) {
#if !defined(_WIN32) && !defined(_APPLE)
        int *fdAddr = static_cast<int *>(addr);
        if (base != nullptr) {
            ::munmap(base, size);
            base = nullptr;
        }
        if (fdAddr != nullptr) {
            ::close(*fdAddr);
            delete fdAddr;
        }
#endif
    } else if (allocType == AllocatorType::HEAP_ALLOC) {
        if (base != nullptr) {
            free(base);
        }
    }
}

jobject ohos_media_image_PixelMap_nativeCreateFromParcel(JNIEnv *env, jobject thiz, jobject parcel)
{
    HiLog::Debug(LABEL, "nativeCreateFromParcel.");
    if (parcel == NULL) {
        HiLog::Error(LABEL, "writeToParcel null parcel.");
        return nullptr;
    }
    jclass zParcelClazz = env->GetObjectClass(parcel);
    jfieldID zNativeHandleField = env->GetFieldID(zParcelClazz, "nativeHandle", "J");
    env->DeleteLocalRef(zParcelClazz);
    if (zNativeHandleField == nullptr) {
        HiLog::Error(LABEL, "zNativeHandleField is invalid");
        return nullptr;
    }

    jlong zParcelNative = env->GetLongField(parcel, zNativeHandleField);
    auto nativeParcel = reinterpret_cast<OHOS::MessageParcel *>(zParcelNative);

    if (nativeParcel == nullptr) {
        HiLog::Error(LABEL, "get native parcel failed.");
        return nullptr;
    }
    ImageInfo imgInfo;
    imgInfo.size.width = nativeParcel->ReadInt32();
    imgInfo.size.height = nativeParcel->ReadInt32();
    imgInfo.pixelFormat = static_cast<PixelFormat>(nativeParcel->ReadInt32());
    imgInfo.colorSpace = static_cast<ColorSpace>(nativeParcel->ReadInt32());
    imgInfo.alphaType = static_cast<AlphaType>(nativeParcel->ReadInt32());
    imgInfo.baseDensity = nativeParcel->ReadInt32();
    int32_t bufferSize = nativeParcel->ReadInt32();
    AllocatorType allocType = static_cast<AllocatorType>(nativeParcel->ReadInt32());
    uint8_t *base = nullptr;
    int fd = 0;
    void *context = nullptr;
    if (allocType == AllocatorType::SHARE_MEM_ALLOC) {
#if !defined(_WIN32) && !defined(_APPLE)
        fd = nativeParcel->ReadFileDescriptor();
        if (fd < 0) {
            HiLog::Error(LABEL, "fd < 0");
            return nullptr;
        }
        HiLog::Debug(LABEL, "ReadFileDescriptor fd %{public}d.", fd);
        void* ptr = ::mmap(nullptr, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            ::close(fd);
            HiLog::Error(LABEL, "shared memory map failed");
            return nullptr;
        }
        context = new int32_t();
        if (context == nullptr) {
            HiLog::Error(LABEL, "alloc sizeof(fd) size:[%{public}zu] error.", sizeof(fd));
            ::munmap(ptr, bufferSize);
            ::close(fd);
            return nullptr;
        }
        *static_cast<int32_t *>(context) = fd;
        base = static_cast<uint8_t *>(ptr);
#endif
    } else {
        const uint8_t *addr = nativeParcel->ReadBuffer(bufferSize);
        if (addr == nullptr) {
            HiLog::Error(LABEL, "read buffer from parcel failed, read buffer addr is null");
            return nullptr;
        }
        base = static_cast<uint8_t *>(malloc(bufferSize));
        if (base == nullptr) {
            HiLog::Error(LABEL, "alloc output pixel memory size:[%{public}d] error.", bufferSize);
            return nullptr;
        }
        if (memcpy_s(base, bufferSize, addr, bufferSize) != 0) {
            free(base);
            base = nullptr;
            HiLog::Error(LABEL, "memcpy pixel data size:[%{public}d] error.", bufferSize);
            return nullptr;
        }
    }
    std::unique_ptr<PixelMap> pixelMap = std::make_unique<PixelMap>();
    if (pixelMap.get() == nullptr) {
        ReleaseMemory(allocType, context, fd, bufferSize, base);
        HiLog::Error(LABEL, "create pixel map failed, malloc error.");
        return nullptr;
    }
    uint32_t ret = pixelMap->SetImageInfo(imgInfo);
    if (ret != SUCCESS) {
        ReleaseMemory(allocType, context, fd, bufferSize, base);
        HiLog::Error(LABEL, "create pixel map from parcel failed, set image info error.");
        return nullptr;
    }
    pixelMap->SetPixelsAddr(base, context, bufferSize, allocType, nullptr);
    PixelMapManager *pixelMapManager = new (std::nothrow) PixelMapManager(pixelMap.release());
    if (pixelMapManager == nullptr) {
        ReleaseMemory(allocType, context, fd, bufferSize, base);
        HiLog::Error(LABEL, "new pixelMap manager failed.");
        return nullptr;
    }
    return CreatePixelMapObject(env, pixelMapManager);
}

jobject ohos_media_image_PixelMap_nativeCreateByColors(JNIEnv *env, jclass thiz, jintArray colors, jint offset,
                                                       jint stride, jint width, jint height, jint pixelFormat,
                                                       jint alphaType, jboolean editable)
{
    if (colors == nullptr) {
        HiLog::Error(LABEL, "CreatePixelmap : colors is null.");
        return nullptr;
    }
    uint64_t arrayLength = env->GetArrayLength(colors);
    uint32_t *array = reinterpret_cast<uint32_t *>(env->GetIntArrayElements(colors, nullptr));
    uint32_t colorLength = static_cast<uint32_t>(arrayLength);

    InitializationOptions initializationOpts;
    initializationOpts.size.width = width;
    initializationOpts.size.height = height;
    initializationOpts.pixelFormat = static_cast<PixelFormat>(pixelFormat);
    PixelFormatToNative(initializationOpts.pixelFormat);
    initializationOpts.alphaType = static_cast<AlphaType>(alphaType);
    initializationOpts.editable = editable;

    std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(array, colorLength, offset, stride, initializationOpts);
    env->ReleaseIntArrayElements(colors, reinterpret_cast<jint *>(array), JNI_ABORT);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    PixelMapManager *pixelMapManager = new (std::nothrow) PixelMapManager(pixelMap.release());
    if (pixelMapManager == nullptr) {
        return nullptr;
    }
    return CreatePixelMapObject(env, pixelMapManager);
}

jobject ohos_media_image_PixelMap_nativeCreateByOptions(JNIEnv *env, jclass thiz, jint width, jint height,
                                                        jint pixelFormat, jint alphaType, jboolean editable)
{
    InitializationOptions initializationOpts;
    initializationOpts.size.width = width;
    initializationOpts.size.height = height;
    initializationOpts.pixelFormat = static_cast<PixelFormat>(pixelFormat);
    PixelFormatToNative(initializationOpts.pixelFormat);
    initializationOpts.alphaType = static_cast<AlphaType>(alphaType);
    initializationOpts.editable = editable;

    std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(initializationOpts);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    PixelMapManager *pixelMapManager = new (std::nothrow) PixelMapManager(pixelMap.release());
    if (pixelMapManager == nullptr) {
        return nullptr;
    }
    return CreatePixelMapObject(env, pixelMapManager);
}

jobject ohos_media_image_PixelMap_nativeCreateBySource(JNIEnv *env, jclass thiz, jobject srcPixelMap, jint rectX,
                                                       jint rectY, jint rectWidth, jint rectHeight, jint width,
                                                       jint height, jint pixelFormat, jint alphaType,
                                                       jint scaleMode, jboolean useSourceIfMatch,
                                                       jboolean editable)
{
    if (srcPixelMap == nullptr) {
        HiLog::Error(LABEL, "source pixelMap jobject is null");
        return nullptr;
    }
    PixelMapManager *pixelMapManager =
        reinterpret_cast<PixelMapManager *>(env->GetLongField(srcPixelMap, g_nativePixelMapFieldId));
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "source native pixelMap is null");
        return nullptr;
    }
    PixelMap &source = pixelMapManager->GetPixelMap();
    Rect rect;
    rect.left = rectX;
    rect.top = rectY;
    rect.width = rectWidth;
    rect.height = rectHeight;

    InitializationOptions initializationOpts;
    initializationOpts.size.width = width;
    initializationOpts.size.height = height;
    initializationOpts.pixelFormat = static_cast<PixelFormat>(pixelFormat);
    PixelFormatToNative(initializationOpts.pixelFormat);
    initializationOpts.alphaType = static_cast<AlphaType>(alphaType);
    initializationOpts.scaleMode = static_cast<ScaleMode>(scaleMode);
    initializationOpts.useSourceIfMatch = useSourceIfMatch;
    initializationOpts.editable = editable;

    std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(source, rect, initializationOpts);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    if (pixelMap->IsSourceAsResponse()) {
        pixelMap.release();
        return srcPixelMap;
    }
    PixelMapManager *newPixelMapManager = new (std::nothrow) PixelMapManager(pixelMap.release());
    if (newPixelMapManager == nullptr) {
        return nullptr;
    }
    return CreatePixelMapObject(env, newPixelMapManager);
}

jint ohos_media_image_PixelMap_nativeGetBytesNumberPerRow(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native GetBytesNumberPerRow, pixelMap is null.");
        return ERR_MEDIA_INVALID_VALUE;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    return static_cast<jint>(pixelMap.GetRowBytes());
}

jlong ohos_media_image_PixelMap_nativeGetBytesNumber(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native GetBytesNumber, pixelMap is null.");
        return ERR_MEDIA_INVALID_VALUE;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    return pixelMap.GetByteCount();
}

jlong ohos_media_image_PixelMap_nativeGetPixelBytesCapacity(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native GetPixelBytesCapacity, pixelMap is null.");
        return ERR_MEDIA_INVALID_VALUE;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    return pixelMap.GetCapacity();
}

jboolean ohos_media_image_PixelMap_nativeIsEditable(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native IsEditable, pixelMap is null.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    return pixelMap.IsEditable();
}

jboolean ohos_media_image_PixelMap_nativeIsSameImage(JNIEnv *env, jclass thiz, jlong nativePtr,
                                                     jlong otherNativePtr)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    PixelMapManager *other = reinterpret_cast<PixelMapManager *>(otherNativePtr);
    if (pixelMapManager == nullptr || other == nullptr || pixelMapManager->Invalid() || other->Invalid()) {
        HiLog::Error(LABEL, "native IsSameImage, curPixelMap or otherPixelMap is null.");
        return false;
    }
    PixelMap &curPixelMap = pixelMapManager->GetPixelMap();
    return curPixelMap.IsSameImage(other->GetPixelMap());
}

jint ohos_media_image_PixelMap_nativeReadPixel(JNIEnv *env, jclass thiz, jlong nativePtr, jint x, jint y)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native read pixel by pos, pixelMap is null.");
        ThrowIllegalStateException(env, "native read pixel by pos, pixelMap is null");
        return ERR_MEDIA_INVALID_VALUE;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    Position pos{ x, y };
    uint32_t dst = 0;
    uint32_t ret = pixelMap.ReadPixel(pos, dst);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "native read pixel by pos, ReadPixel fail.ret=%{public}u", ret);
        if (ret == ERR_IMAGE_INVALID_PARAMETER) {
            ThrowIllegalArgumentException(env, "native read pixel by pos, invalid input parameters");
            return ERR_MEDIA_INVALID_VALUE;
        }
        ThrowIllegalStateException(env, "native read pixel by pos, some error occurred in pixelmap");
        return ERR_MEDIA_INVALID_VALUE;
    }
    return static_cast<jint>(dst);
}

jboolean ohos_media_image_PixelMap_nativeReadPixelsToIntArray(JNIEnv *env, jclass thiz, jlong nativePtr,
                                                              jintArray nativePixels, jint nativeOffset,
                                                              jint nativeStride, jint x, jint y, jint width,
                                                              jint height)
{
    if (nativePixels == nullptr) {
        HiLog::Error(LABEL, "native read pixels by rect, nativePixels is null.");
        return false;
    }
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native read pixels by rect, pixelMap is null.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    Rect srcRegion{ x, y, width, height };
    jint *dst = env->GetIntArrayElements(nativePixels, nullptr);
    if (dst == nullptr) {
        HiLog::Error(LABEL, "native read pixels by rect GetIntArrayElements return dst is null.");
        return false;
    }
    uint64_t dstLen = static_cast<uint64_t>(env->GetArrayLength(nativePixels)) * 4;  // jintArray is 4 bytes
    uint32_t dstRowBytes = static_cast<uint32_t>(nativeStride) * 4;                  // "4" is pixel format size.
    uint32_t offset = static_cast<uint32_t>(nativeOffset) * 4;  // shift as bytes count, need need multiply 4
    uint32_t ret = pixelMap.ReadPixels(dstLen, offset, dstRowBytes, srcRegion, reinterpret_cast<uint8_t *>(dst));
    if (ret != SUCCESS) {
        if (ret == ERR_IMAGE_INVALID_PARAMETER) {
            ThrowIllegalArgumentException(env, "native read pixel by rect, invalid input parameters");
        }
        HiLog::Error(LABEL, "native read pixels by rect, ReadPixels fail, ret=%{public}u", ret);
        env->ReleaseIntArrayElements(nativePixels, dst, 0);
        return false;
    }
    env->ReleaseIntArrayElements(nativePixels, dst, 0);
    return true;
}

void *GetBufferAddress(JNIEnv *env, jobject jbuffer)
{
    jlong pointer = 0L;
    if (!GetBufferPointer(env, jbuffer, pointer)) {
        HiLog::Error(LABEL, "GetBufferAddress call GetNioBufferPointer fail.");
        return nullptr;
    }
    jarray array = nullptr;
    void *elements = nullptr;
    void *dstAddress = nullptr;
    if (pointer != 0L) {
        // Buffer is backed by a direct buffer.
        dstAddress = reinterpret_cast<void *>(pointer);
    } else {
        // Buffer is backed by a managed array.
        jint byteOffset = 0;
        if (!PixelMapJniUtilsAdapter::GetBufferBaseArrayOffset(env, jbuffer, byteOffset)) {
            HiLog::Error(LABEL, "GetBufferAddress call GetNioBufferBaseArrayOffset fail.");
            return nullptr;
        }
        array = PixelMapJniUtilsAdapter::GetBufferBaseArray(env, jbuffer);
        if (array == nullptr) {
            HiLog::Error(LABEL, "GetBufferAddress call GetNioBufferBaseArray fail.");
            return nullptr;
        }
        elements = env->GetPrimitiveArrayCritical(array, nullptr);
        dstAddress = reinterpret_cast<void *>(reinterpret_cast<char *>(elements) + byteOffset);
        env->ReleasePrimitiveArrayCritical(array, elements, 0);
    }
    return dstAddress;
}

jboolean ohos_media_image_PixelMap_nativeReadPixelsToBuffer(JNIEnv *env, jclass thiz, jlong nativePtr,
                                                            jobject nioBuffer, jlong nioBufferSize)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (nioBuffer == nullptr || pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native read pixels to buffer, nioBuffer or pixelMap is null.");
        return false;
    }
    void *dstAddress = GetBufferAddress(env, nioBuffer);
    if (dstAddress == nullptr) {
        HiLog::Error(LABEL, "native read pixels to buffer, dstAddress is null.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    uint32_t ret = pixelMap.ReadPixels(nioBufferSize, static_cast<uint8_t *>(dstAddress));
    if (ret != SUCCESS) {
        if (ret == ERR_IMAGE_INVALID_PARAMETER) {
            ThrowIllegalArgumentException(env, "native read pixel to buffer, invalid input parameters");
        }
        HiLog::Error(LABEL, "native read pixels to buffer, ReadPixels fail, ret=%{public}u", ret);
        return false;
    }
    return true;
}

jboolean ohos_media_image_PixelMap_nativeResetConfig(JNIEnv *env, jclass thiz, jlong nativePtr, jint width,
                                                     jint height, jint nativePixelFormat)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr) {
        HiLog::Error(LABEL, "nativeResetConfig input parameters invalid.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    Size size{ width, height };
    uint32_t ret = pixelMap.ResetConfig(size, static_cast<const PixelFormat>(nativePixelFormat));
    if (ret != SUCCESS) {
        if (ret == ERR_IMAGE_INVALID_PARAMETER) {
            ThrowIllegalArgumentException(env, "native reset config, invalid input parameters");
        }
        HiLog::Error(LABEL, "nativeResetConfig to call ResetConfig fail,ret=%{public}u", ret);
        return false;
    }
    return true;
}

jboolean ohos_media_image_PixelMap_nativeSetAlphaType(JNIEnv *env, jclass thiz, jlong nativePtr,
                                                      jint nativeAlphaType)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "nativeSetAlphaType input parameters invalid.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    if (!pixelMap.SetAlphaType(static_cast<const AlphaType>(nativeAlphaType))) {
        HiLog::Error(LABEL, "nativeSetAlphaType call SetAlphaType to set PixelNap AlphaType fail.");
        return false;
    }
    return true;
}

jboolean ohos_media_image_PixelMap_nativeWritePixel(JNIEnv *env, jclass thiz, jlong nativePtr, jint x, jint y,
                                                    jint nativeColor)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native write pixel by pos input parameters invalid.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    Position pos{ x, y };
    uint32_t ret = pixelMap.WritePixel(pos, static_cast<uint32_t>(nativeColor));
    if (ret != SUCCESS) {
        if (ret == ERR_IMAGE_INVALID_PARAMETER) {
            ThrowIllegalArgumentException(env, "native write pixel by pos, invalid input parameters");
        }
        HiLog::Error(LABEL, "native write pixel by pos call WritePixel func fail, ret=%{public}d", ret);
        return false;
    }
    return true;
}

jboolean ohos_media_image_PixelMap_nativeWritePixelsByIntArray(JNIEnv *env, jclass thiz, jlong nativePtr,
                                                               jintArray nativePixels, jint nativeOffset,
                                                               jint nativeStride, jint x, jint y, jint width,
                                                               jint height)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (nativePixels == nullptr || pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native write pixel by rect input parameters is invalid.");
        return false;
    }
    Rect dstRegion{ x, y, width, height };
    jint *srcPixels = env->GetIntArrayElements(nativePixels, nullptr);
    if (srcPixels == nullptr) {
        HiLog::Error(LABEL, "native write pixel by rect call GetIntArrayElements is null.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    uint64_t srcPixelsLen = static_cast<uint64_t>(env->GetArrayLength(nativePixels)) * 4;  // jintArray is 4 bytes
    uint32_t rowDataSize = static_cast<uint32_t>(nativeStride) * 4;                        // jintArray is 4 bytes
    uint32_t offset = static_cast<uint32_t>(nativeOffset) * 4;  // shift as bytes count, need need multiply 4
    uint32_t ret =
        pixelMap.WritePixels(reinterpret_cast<uint8_t *>(srcPixels), srcPixelsLen, offset, rowDataSize, dstRegion);
    if (ret != SUCCESS) {
        if (ret == ERR_IMAGE_INVALID_PARAMETER) {
            ThrowIllegalArgumentException(env, "native write pixel by rect, invalid input parameters");
        }
        HiLog::Error(LABEL, "native write pixel by rect call WritePixels fail, ret=%{public}u", ret);
        env->ReleaseIntArrayElements(nativePixels, srcPixels, 0);
        return false;
    }
    env->ReleaseIntArrayElements(nativePixels, srcPixels, 0);
    return true;
}

jboolean ohos_media_image_PixelMap_nativeWritePixelsFromBuffer(JNIEnv *env, jclass thiz, jlong nativePtr,
                                                               jobject nativeBuffer, jlong nativeBufferSize)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (nativeBuffer == nullptr || pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native write pixel by buffer input parameters is invalid.");
        return false;
    }
    void *srcAddress = GetBufferAddress(env, nativeBuffer);
    if (srcAddress == nullptr) {
        HiLog::Error(LABEL, "native write pixel by buffer srcAddress is null.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    uint32_t ret = pixelMap.WritePixels(static_cast<const uint8_t *>(srcAddress), nativeBufferSize);
    if (ret != SUCCESS) {
        if (ret == ERR_IMAGE_INVALID_PARAMETER) {
            ThrowIllegalArgumentException(env, "native write pixel from buffer, invalid input parameters");
        }
        HiLog::Error(LABEL, "native write pixel by buffer failed,ret=%{public}u.", ret);
        return false;
    }

    return true;
}

jboolean ohos_media_image_PixelMap_nativeWritePixelsFromInt(JNIEnv *env, jclass thiz, jlong nativePtr,
                                                            jint nativeColor)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native erase by color input parameters is invalid.");
        return false;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    if (!pixelMap.WritePixels(nativeColor)) {
        HiLog::Error(LABEL, "native erase by color call WritePixels return fail.");
        return false;
    }
    return true;
}

#if !defined(_WIN32) && !defined(_APPLE)
jobject ohos_media_image_PixelMap_nativeCreateFromAlpha(JNIEnv *env, jclass thiz, jlong nativePtr)
{
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(nativePtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "native CreateFromAlpha input parameters is invalid.");
        return nullptr;
    }
    PixelMap &pixelMapSrc = pixelMapManager->GetPixelMap();
    PixelMap *pixelMapRet = ImageBitmapConverter::createFromAlpha(&pixelMapSrc);

    if (pixelMapRet == nullptr) {
        HiLog::Error(LABEL, "native CreateFromAlpha get alpha pixelmap failed.");
    }

    PixelMapManager *newPixelMapManager = new (std::nothrow) PixelMapManager(pixelMapRet);
    if (newPixelMapManager == nullptr) {
        HiLog::Error(LABEL, "native CreateFromAlpha create pixelmapmanager failed.");
        return nullptr;
    }
    return CreatePixelMapObject(env, newPixelMapManager);
}
#endif

void FreePixelMap(PixelMapManager *manager)
{
    if (manager != nullptr) {
        delete manager;
    }
}

jlong ohos_media_image_PixelMap_nativeGetFreeFunction(JNIEnv *env, jclass thiz)
{
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(&FreePixelMap));
}

const JNINativeMethod METHODS[] = {
    { "nativeGetImageInfo", "(J)Lohos/media/image/common/ImageInfo;",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeGetImageInfo) },
    { "nativeRelease", "(JJ)V", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeRelease) },
    { "nativeWriteToParcel", "(JLohos/utils/Parcel;)Z",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeWriteToParcel) },
    { "nativeCreateFromParcel", "(Lohos/utils/Parcel;)Lohos/media/image/PixelMap;",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeCreateFromParcel) },
    { "nativeCreate", "([IIIIIIIZ)Lohos/media/image/PixelMap;",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeCreateByColors) },
    { "nativeCreate", "(IIIIZ)Lohos/media/image/PixelMap;",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeCreateByOptions) },
    { "nativeCreate", "(Lohos/media/image/PixelMap;IIIIIIIIIZZ)Lohos/media/image/PixelMap;",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeCreateBySource) },
    { "nativeGetBytesNumberPerRow", "(J)I",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeGetBytesNumberPerRow) },
    { "nativeGetBytesNumber", "(J)J", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeGetBytesNumber) },
    { "nativeGetPixelBytesCapacity", "(J)J",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeGetPixelBytesCapacity) },
    { "nativeIsEditable", "(J)Z", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeIsEditable) },
    { "nativeIsSameImage", "(JJ)Z", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeIsSameImage) },
    { "nativeReadPixel", "(JII)I", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeReadPixel) },
    { "nativeReadPixels", "(J[IIIIIII)Z",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeReadPixelsToIntArray) },
    { "nativeReadPixels", "(JLjava/nio/Buffer;J)Z",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeReadPixelsToBuffer) },
    { "nativeResetConfig", "(JIII)Z", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeResetConfig) },
    { "nativeSetAlphaType", "(JI)Z", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeSetAlphaType) },
    { "nativeWritePixel", "(JIII)Z", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeWritePixel) },
    { "nativeWritePixels", "(J[IIIIIII)Z",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeWritePixelsByIntArray) },
    { "nativeWritePixels", "(JLjava/nio/Buffer;J)Z",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeWritePixelsFromBuffer) },
    { "nativeWritePixels", "(JI)Z", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeWritePixelsFromInt) },
    { "nativeGetFreeFunction", "()J", reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeGetFreeFunction) },
#if !defined(_WIN32) && !defined(_APPLE)
    { "nativeCreateFromAlpha", "(J)Lohos/media/image/PixelMap;",
      reinterpret_cast<void *>(ohos_media_image_PixelMap_nativeCreateFromAlpha) },
#endif
};

jint RegisterGlobalCache(JNIEnv *env)
{
    if (env != nullptr) {
        jobject pixelMapClz = env->FindClass("ohos/media/image/PixelMap");
        if (pixelMapClz == nullptr) {
            HiLog::Error(LABEL, "find pixel map class failed.");
            return ERROR;
        }
        g_pixelMapClz = static_cast<jclass>(env->NewGlobalRef(pixelMapClz));
        if (g_pixelMapClz == nullptr) {
            HiLog::Error(LABEL, "register global pixelMap clz failed.");
            return ERROR;
        }
        g_pixelMapConstructorMethodId = env->GetMethodID(g_pixelMapClz, "<init>", "(JJ)V");
        if (g_pixelMapConstructorMethodId == nullptr) {
            HiLog::Error(LABEL, "get pixelmap constructor method Id failed.");
            return ERROR;
        }
        g_nativePixelMapFieldId = env->GetFieldID(g_pixelMapClz, "nativeImagePixelMap", "J");
        if (g_nativePixelMapFieldId == nullptr) {
            HiLog::Error(LABEL, "get native pixelmap fieldId failed.");
            return ERROR;
        }
        return SUCCESS;
    }
    return ERROR;
}
} // namespace

namespace OHOS {
namespace Media {
void ohos_media_image_GetImageInfo(JNIEnv *env, jobject pixelMapObject, OhosPixelMapInfo &info)
{
    PixelMapManager *pixelMapManager =
        reinterpret_cast<PixelMapManager *>(env->GetLongField(pixelMapObject, g_nativePixelMapFieldId));
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "getImageInfo:source native pixelMap is null");
        return;
    }
    PixelMap &pixelMap = pixelMapManager->GetPixelMap();
    info.width = pixelMap.GetWidth();
    info.height = pixelMap.GetHeight();
    info.rowSize = pixelMap.GetRowBytes();
    PixelFormat pixelFormat = pixelMap.GetPixelFormat();
    HiLog::Error(LABEL, "getImageInfo:format:%{public}d", pixelFormat);
    switch (pixelFormat) {
        case PixelFormat::RGBA_8888:
            info.pixelFormat = OHOS_PIXEL_MAP_FORMAT_RGBA_8888;
            break;
        case PixelFormat::RGB_565:
            info.pixelFormat = OHOS_PIXEL_MAP_FORMAT_RGB_565;
            break;
        default:
            info.pixelFormat = OHOS_PIXEL_MAP_FORMAT_NONE;
            break;
    }
}

void *ohos_media_image_AccessPixels(JNIEnv *env, jobject pixelMapObject)
{
    if (pixelMapObject == nullptr) {
        HiLog::Error(LABEL, "accessPixels:pixelMapObject is null");
        return nullptr;
    }

    PixelMapManager *pixelMapManager =
        reinterpret_cast<PixelMapManager *>(env->GetLongField(pixelMapObject, g_nativePixelMapFieldId));
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "accessPixels:source native pixelMap is null");
        return nullptr;
    }

    pixelMapManager->Ref();
    return pixelMapManager->GetPixelMap().GetWritablePixels();
}

bool ohos_media_image_UnAccessPixels(JNIEnv *env, jobject pixelMapObject)
{
    if (pixelMapObject == nullptr) {
        HiLog::Error(LABEL, "unAccessPixels:pixelMapObject is null");
        return false;
    }
    PixelMapManager *pixelMapManager =
        reinterpret_cast<PixelMapManager *>(env->GetLongField(pixelMapObject, g_nativePixelMapFieldId));
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "unAccessPixels::source native pixelMap is null");
        return false;
    }
    if (pixelMapManager->GetPixelMap().GetSptrRefCount() == 0) {
        HiLog::Error(LABEL, "unAccessPixels::native ptr ref count is zero");
        return false;
    }
    pixelMapManager->UnRef();
    return true;
}
} // namespace Media
} // namespace OHOS

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    HiLog::Debug(LABEL, "JNI_OnLoad begin");
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
        HiLog::Error(LABEL, "JNI_OnLoad: GetEnv failed");
        return ERROR;
    }
    int ret = JkitRegisterNativeMethods(env, "ohos/media/image/PixelMap", METHODS, ARRCOUNT(METHODS));
    if (ret == JNI_ERR) {
        HiLog::Error(LABEL, "JkitRegisterNativeMethods failed, ret=%{public}d", ret);
        return ERROR;
    }
    Jkit::nativeInit(vm);
    HiLog::Debug(LABEL, "JNI_OnLoad end");

    if (RegisterGlobalCache(env) == ERROR) {
        HiLog::Error(LABEL, "register global cache failed.");
        return ERROR;
    }
    return JNI_VERSION_1_4;
}
