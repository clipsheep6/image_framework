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

#ifndef IMAGE_RECEIVER_ADAPTER_H
#define IMAGE_RECEIVER_ADAPTER_H

#include <cstdint>
#include <string>
#include "hilog/log.h"
#include "image_format.h"
#include "image_receiver_utils.h"
#include "image_type.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "media_errors.h"

namespace OHOS {
namespace Media {
// global jni class cache
struct ImageReceiverCache {
    jfieldID nativeContext;
    jmethodID onEventFromNative;  // callback
} g_imageReceiverCache;

struct ImageCache {
    jfieldID nativeBuffer;
    jfieldID timestamp;
    jfieldID components;
} g_imageCache;

struct ComponentCache {
    jclass clazz;
    jmethodID constructor;
} g_componentCache;

struct SurfaceCache {
    jclass clazz;
    jmethodID constructor;
} g_surfaceCache;

class ImageReceiverAdapter {
public:
    ImageReceiverAdapter() = default;
    ~ImageReceiverAdapter() = default;
    static void InitImageReceiverContext(JNIEnv *env, jobject thiz, jobject weakThiz, jint width, jint height,
                                         jint format, jint capacity, jlong usage);
    static jboolean ReadNextImage(JNIEnv *env, jobject thiz, jobject image);
    static void ReleaseFreeBuffers(JNIEnv *env, jobject thiz);
    static void ReleaseReceiver(JNIEnv *env, jobject thiz);
    static jint GetFormat(JNIEnv *env, jobject thiz, jint receiverFormat);
    static Media::Size GetSize(JNIEnv *env, jobject thiz);
    static jobjectArray GetComponents(JNIEnv *env, jobject thiz, jint componentNum, jint receiverFormat);
    static void ReleaseImage(JNIEnv *env, jobject thiz, jobject imageReceiver);
    static void CacheActiveClass(JNIEnv *env, jclass clazz);
    static jobject GetSurface(JNIEnv *env, jobject thiz);

private:
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
        LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageReceiverAdapter"
    };
    static bool CacheImageClass(JNIEnv *env);
    static bool CacheSurfaceClass(JNIEnv *env);
    static ComponentType GetComponentType(int32_t receiverFormat, int32_t index);
};
} // namespace Media
} // namespace OHOS

#endif // IMAGE_RECEIVER_ADAPTER_H