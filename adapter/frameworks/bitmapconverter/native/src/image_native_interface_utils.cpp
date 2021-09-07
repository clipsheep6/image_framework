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

#include "hilog/log.h"
#include "image_bitmap_converter.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_map.h"
#include "pixel_map_manager.h"

using namespace OHOS::HiviewDFX;
using namespace OHOS::Media;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "Image_Native_Interface_Utils"
};

namespace OHOS {
namespace Media {
PixelMap *GetNativePixelMap(JNIEnv *env, jobject pixelMapObj)
{
    if (pixelMapObj == nullptr) {
        HiLog::Error(LABEL, "GetNativePixelMapPtr, pixelMap object is null.");
        return nullptr;
    }
    jclass pixelMapClazz = env->GetObjectClass(pixelMapObj);
    if (pixelMapClazz == nullptr) {
        HiLog::Error(LABEL, "GetNativePixelMapPtr, PixelMap class not found.");
        return nullptr;
    }
    jfieldID pixelMapFileId = env->GetFieldID(pixelMapClazz, "nativeImagePixelMap", "J");
    env->DeleteLocalRef(pixelMapClazz);
    if (pixelMapFileId == nullptr) {
        HiLog::Error(LABEL, "GetNativePixelMapPtr get nativeImagePixelMap error.");
        return nullptr;
    }
    jlong pixelMapPtr = env->GetLongField(pixelMapObj, pixelMapFileId);
    PixelMapManager *pixelMapManager = reinterpret_cast<PixelMapManager *>(pixelMapPtr);
    if (pixelMapManager == nullptr || pixelMapManager->Invalid()) {
        HiLog::Error(LABEL, "GetNativeBitmap pixelMap is null.");
        return nullptr;
    }
    return &(pixelMapManager->GetPixelMap());
}

jobject GetNativeBitmap(JNIEnv *env, jobject pixelMapObj)
{
    if (pixelMapObj == nullptr) {
        HiLog::Error(LABEL, "GetNativeBitmap, pixelMap object is null.");
        return nullptr;
    }
    PixelMap *pixelMap = GetNativePixelMap(env, pixelMapObj);
    if (pixelMap == nullptr) {
        HiLog::Error(LABEL, "GetNativeBitmap pixelMap is null.");
        return nullptr;
    }

    jclass bitmapClazz = env->FindClass("android/graphics/Bitmap");
    if (bitmapClazz == nullptr) {
        HiLog::Error(LABEL, "GetNativeBitmap: find android Bitmap class fail, bitmapClazz is null.");
        return nullptr;
    }

    jmethodID bitmapConstructMethodId =
        env->GetMethodID(bitmapClazz, "<init>", "(JIIIZ[BLandroid/graphics/NinePatch$InsetStruct;Z)V");
    if (bitmapConstructMethodId == nullptr) {
        HiLog::Error(LABEL,
                     "GetNativeBitmap: find android Bitmap construct method fail, bitmapConstructMethodId is null.");
        env->DeleteLocalRef(bitmapClazz);
        return nullptr;
    }

    android::BitmapWrapper *bitmapWrapper = ImageBitmapConverter::CreateShadowBitmap(pixelMap);
    if (bitmapWrapper == nullptr) {
        HiLog::Error(LABEL, "GetNativeBitmap bitmap is null.");
        env->DeleteLocalRef(bitmapClazz);
        return nullptr;
    }

    bool heapAlloc = pixelMap->GetAllocatorType() == AllocatorType::HEAP_ALLOC;
    jobject bitmapObj = env->NewObject(bitmapClazz, bitmapConstructMethodId, reinterpret_cast<jlong>(bitmapWrapper),
                                       bitmapWrapper->bitmap().width(), bitmapWrapper->bitmap().height(), -1, true,
                                       nullptr, nullptr, heapAlloc);
    env->DeleteLocalRef(bitmapClazz);

    return bitmapObj;
}

jobject GetShellPixelMap(JNIEnv *env, jobject bitmapObj)
{
    if (bitmapObj == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap, pixelMap object is null.");
        return nullptr;
    }
    jclass bitmapClazz = env->GetObjectClass(bitmapObj);
    if (bitmapClazz == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap, Bitmap class not found.");
        return nullptr;
    }
    jfieldID bitmapWrapperFileId = env->GetFieldID(bitmapClazz, "mNativePtr", "J");
    env->DeleteLocalRef(bitmapClazz);
    if (bitmapWrapperFileId == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap get mNativeBitmap error.");
        return nullptr;
    }
    jlong bitmapWrapperPtr = env->GetLongField(bitmapObj, bitmapWrapperFileId);
    android::BitmapWrapper *bitmapWrapper = reinterpret_cast<android::BitmapWrapper *>(bitmapWrapperPtr);
    if (bitmapWrapper == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap bitmapWrapper is null.");
        return nullptr;
    }

    jclass pixelMapClazz = env->FindClass("ohos/media/image/PixelMap");
    if (pixelMapClazz == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap: find PixelMap class fail, pixelMapClazz is null.");
        return nullptr;
    }

    jmethodID pixelMapConstructMethodId = env->GetMethodID(pixelMapClazz, "<init>", "(JJJ)V");
    if (pixelMapConstructMethodId == nullptr) {
        HiLog::Error(LABEL,
                     "GetShellPixelMap: find PixelMap construct method fail, pixelMapConstructMethodId is null.");
        env->DeleteLocalRef(pixelMapClazz);
        return nullptr;
    }

    PixelMap *pixelMap = ImageBitmapConverter::CreateShellPixelMap(bitmapWrapper);
    if (pixelMap == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap pixelMap is null.");
        env->DeleteLocalRef(pixelMapClazz);
        return nullptr;
    }
    PixelMapManager *pixelMapManager = new (std::nothrow) PixelMapManager(pixelMap);
    if (pixelMapManager == nullptr) {
        HiLog::Error(LABEL, "new pixelMapManager is null.");
        env->DeleteLocalRef(pixelMapClazz);
        return nullptr;
    }
    jobject globalBitmapObj = env->NewGlobalRef(bitmapObj);
    jobject pixelMapObj =
        env->NewObject(pixelMapClazz, pixelMapConstructMethodId, reinterpret_cast<jlong>(pixelMapManager),
                       static_cast<jlong>(pixelMap->GetByteCount()), reinterpret_cast<jlong>(globalBitmapObj));
    env->DeleteLocalRef(pixelMapClazz);
    return pixelMapObj;
}

void PixelMapWriteToParcel(JNIEnv* env, jobject pixelMapObj, jobject zParcel)
{
    if (zParcel == NULL) {
        HiLog::Error(LABEL, "writeToParcel null parcel.");
        return;
    }
    jclass zParcelClazz = env->GetObjectClass(zParcel);
    if (zParcelClazz == NULL) {
        HiLog::Error(LABEL, "writeToParcel null zParcelClazz.");
        return;
    }
    jfieldID zNativeHandleField = env->GetFieldID(zParcelClazz, "nativeHandle", "J");
    env->DeleteLocalRef(zParcelClazz);
    if (zNativeHandleField == nullptr) {
        HiLog::Error(LABEL, "zNativeHandleField is invalid");
        return;
    }
    jlong zParcelNative = env->GetLongField(zParcel, zNativeHandleField);
    auto parcelZ = reinterpret_cast<MessageParcel *>(zParcelNative);
    HiLog::Debug(LABEL, "PixelMapWriteToParcel zParcel Ptr=%p", parcelZ);

    if (parcelZ == nullptr) {
        HiLog::Debug(LABEL, "Failed to get Z native Parcel");
        return;
    }
    if (pixelMapObj == nullptr) {
        HiLog::Error(LABEL, "nativeWriteToParcel, pixelMap object is null.");
        return;
    }
    jclass pixelMapClazz = env->FindClass("ohos/media/image/PixelMap");
    if (pixelMapClazz == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap: find PixelMap class fail, pixelMapClazz is null.");
        return;
    }
    jmethodID pixelMapGetDensityMethodId = env->GetMethodID(pixelMapClazz, "getBaseDensity", "()I");
    if (pixelMapGetDensityMethodId == nullptr) {
        HiLog::Error(LABEL, "GetShellPixelMap: pixelMapGetDensityMethodId is null.");
        env->DeleteLocalRef(pixelMapClazz);
        return;
    }
    env->DeleteLocalRef(pixelMapClazz);
    jint density = env->CallIntMethod(pixelMapObj, pixelMapGetDensityMethodId);
    PixelMap *pixelMap = GetNativePixelMap(env, pixelMapObj);
    if (pixelMap == nullptr) {
        HiLog::Error(LABEL, "nativeWriteToParcel pixelMap is null.");
        return;
    }
    ImageBitmapConverter::PixelMapWriteToParcel(pixelMap, density, parcelZ);
}
} // namespace Media
} // namespace OHOS
