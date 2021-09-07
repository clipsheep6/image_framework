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

#include <sstream>
#include "hilog/log.h"
#include "image_native_interface_utils.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "media_errors.h"

using namespace OHOS::HiviewDFX;
using namespace OHOS::Media;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageDoubleFwConverter_JNI"
};

jobject ohos_media_image_inner_ImageDoubleFwConverter_nativeCreateBitmap(JNIEnv *env, jobject thiz,
                                                                         jobject pixelMapObj)
{
    jobject bitmap = GetNativeBitmap(env, pixelMapObj);
    if (bitmap == nullptr) {
        HiLog::Error(LABEL, "nativeCreateBitmap GetNativeBitmap fail");
        return nullptr;
    }

    return bitmap;
}

jobject ohos_media_image_inner_ImageDoubleFwConverter_nativeCreatePixelMap(JNIEnv *env, jobject thiz,
                                                                           jobject bitmapObj)
{
    jobject pixelMapObj = GetShellPixelMap(env, bitmapObj);
    if (pixelMapObj == nullptr) {
        HiLog::Error(LABEL, "nativeCreatePixelMap GetShellPixelMap fail");
        return nullptr;
    }

    return pixelMapObj;
}

void ohos_media_image_inner_ImageDoubleFwConverter_nativeWriteToParcel(JNIEnv *env, jobject thiz,
                                                                       jobject pixelMapObj,
                                                                       jobject parcel)
{
    PixelMapWriteToParcel(env, pixelMapObj, parcel);
    return;
}

static const JNINativeMethod METHODS[] = {
    { "nativeCreateBitmap", "(Lohos/media/image/PixelMap;)Landroid/graphics/Bitmap;",
      reinterpret_cast<void *>(ohos_media_image_inner_ImageDoubleFwConverter_nativeCreateBitmap) },
    { "nativeCreatePixelMap", "(Landroid/graphics/Bitmap;)Lohos/media/image/PixelMap;",
      reinterpret_cast<void *>(ohos_media_image_inner_ImageDoubleFwConverter_nativeCreatePixelMap) },
    { "nativeWriteToParcel", "(Lohos/media/image/PixelMap;Lohos/utils/Parcel;)V",
      reinterpret_cast<void *>(ohos_media_image_inner_ImageDoubleFwConverter_nativeWriteToParcel) }
};

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    HiLog::Debug(LABEL, "JNI_OnLoad begin");
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
        HiLog::Error(LABEL, "JNI_OnLoad: GetEnv failed");
        return ERROR;
    }
    int ret = JkitRegisterNativeMethods(env, "ohos/media/image/inner/ImageDoubleFwConverter", METHODS,
                                        ARRCOUNT(METHODS));
    if (ret == JNI_ERR) {
        HiLog::Error(LABEL, "JkitRegisterNativeMethods failed, ret=%{public}d", ret);
        return ERROR;
    }
    Jkit::nativeInit(vm);
    HiLog::Debug(LABEL, "JNI_OnLoad end");
    return JNI_VERSION_1_4;
}
