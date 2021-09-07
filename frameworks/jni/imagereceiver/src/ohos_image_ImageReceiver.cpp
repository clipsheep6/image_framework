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

#include "image_receiver_adapter.h"

using namespace OHOS::HiviewDFX;
using namespace OHOS::Media;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageReceiver_JNI" };

void ohos_media_image_ImageReceiver_nativeInit(JNIEnv *env, jobject thiz, jobject weakThiz, jint width,
                                               jint height, jint format, jint capacity, jlong usage)
{
    HiLog::Debug(LABEL, "nativeInit begin.");
    ImageReceiverAdapter::InitImageReceiverContext(env, thiz, weakThiz, width, height, format, capacity, usage);
    HiLog::Debug(LABEL, "nativeInit end.");
}

jboolean ohos_media_image_ImageReceiver_nativeReadNext(JNIEnv *env, jobject thiz, jobject image)
{
    HiLog::Debug(LABEL, "nativeReadNext begin.");
    jboolean result = ImageReceiverAdapter::ReadNextImage(env, thiz, image);
    HiLog::Debug(LABEL, "nativeReadNext end.");
    return result;
}

void ohos_media_image_ImageReceiver_nativeReleaseFreeBuffers(JNIEnv *env, jobject thiz)
{
    HiLog::Debug(LABEL, "nativeReleaseFreeBuffers begin.");
    ImageReceiverAdapter::ReleaseFreeBuffers(env, thiz);
    HiLog::Debug(LABEL, "nativeReleaseFreeBuffers end.");
}

void ohos_media_image_ImageReceiver_nativeRelease(JNIEnv *env, jobject thiz)
{
    HiLog::Debug(LABEL, "nativeRelease begin.");
    ImageReceiverAdapter::ReleaseReceiver(env, thiz);
    HiLog::Debug(LABEL, "nativeRelease end.");
}

void ohos_media_image_ImageReceiver_nativeCacheClass(JNIEnv *env, jclass clazz)
{
    HiLog::Debug(LABEL, "ImageReceiver nativeCacheClass begin.");
    ImageReceiverAdapter::CacheActiveClass(env, clazz);
    HiLog::Debug(LABEL, "ImageReceiver nativeCacheClass end.");
}

jobject ohos_media_image_ImageReceiver_nativeGetReceiverSurface(JNIEnv *env, jobject thiz)
{
    HiLog::Debug(LABEL, "nativeGetReceiverSurface begin.");
    jobject result = ImageReceiverAdapter::GetSurface(env, thiz);
    ImageReceiverUtils::LogWhenNull(result, "get native surface is null.");
    HiLog::Debug(LABEL, "nativeGetReceiverSurface end.");
    return result;
}

jint ohos_media_image_Image_nativeGetFormat(JNIEnv *env, jobject thiz, jint receiverFormat)
{
    HiLog::Debug(LABEL, "nativeGetFormat begin.");
    jint result = ImageReceiverAdapter::GetFormat(env, thiz, receiverFormat);
    HiLog::Debug(LABEL, "nativeGetFormat end.");
    return result;
}

jobject ohos_media_image_Image_nativeGetSize(JNIEnv *env, jclass thiz)
{
    HiLog::Debug(LABEL, "nativeGetSize begin.");
    Size size = ImageReceiverAdapter::GetSize(env, thiz);
    jclass sizeClazz = env->FindClass("ohos/media/image/common/Size");
    if (ImageReceiverUtils::LogWhenNull(sizeClazz, "can't find Size class.")) {
        return nullptr;
    }
    jmethodID sizeConstructor = env->GetMethodID(sizeClazz, "<init>", "(II)V");
    if (ImageReceiverUtils::LogWhenNull(sizeConstructor, "can't find sizeConstructor.")) {
        env->DeleteLocalRef(sizeClazz);
        return nullptr;
    }
    jobject sizeObj = env->NewObject(sizeClazz, sizeConstructor, size.width, size.width);
    env->DeleteLocalRef(sizeClazz);
    HiLog::Debug(LABEL, "nativeGetSize end.");
    return sizeObj;
}

jobjectArray ohos_media_image_Image_nativeGetComponents(JNIEnv *env, jobject thiz, jint componentNum,
                                                        jint receiverFormat)
{
    HiLog::Debug(LABEL, "nativeGetComponents begin.");
    jobjectArray result = ImageReceiverAdapter::GetComponents(env, thiz, componentNum, receiverFormat);
    HiLog::Debug(LABEL, "nativeGetComponents end.");
    return result;
}

void ohos_media_image_Image_nativeReleaseImage(JNIEnv *env, jobject thiz, jobject imageReceiver)
{
    HiLog::Debug(LABEL, "nativeReleaseImage begin.");
    ImageReceiverAdapter::ReleaseImage(env, thiz, imageReceiver);
    HiLog::Debug(LABEL, "nativeReleaseImage end.");
}

static const JNINativeMethod RECEIVER_METHODS[] = {
    { "nativeInit", "(Ljava/lang/Object;IIIIJ)V",
      reinterpret_cast<void *>(ohos_media_image_ImageReceiver_nativeInit) },
    { "nativeReadNext", "(Lohos/media/image/Image;)Z",
      reinterpret_cast<void *>(ohos_media_image_ImageReceiver_nativeReadNext) },
    { "nativeRelease", "()V", reinterpret_cast<void *>(ohos_media_image_ImageReceiver_nativeRelease) },
    { "nativeReleaseFreeBuffers", "()V",
      reinterpret_cast<void *>(ohos_media_image_ImageReceiver_nativeReleaseFreeBuffers) },
    { "nativeCacheClass", "()V", reinterpret_cast<void *>(ohos_media_image_ImageReceiver_nativeCacheClass) },
    { "nativeGetReceiverSurface", "()Lohos/agp/graphics/Surface;",
      reinterpret_cast<void *>(ohos_media_image_ImageReceiver_nativeGetReceiverSurface) }
};

static const JNINativeMethod IMAGE_METHODS[] = {
    { "nativeGetFormat", "(I)I", reinterpret_cast<void *>(ohos_media_image_Image_nativeGetFormat) },
    { "nativeGetSize", "()Lohos/media/image/common/Size;",
      reinterpret_cast<void *>(ohos_media_image_Image_nativeGetSize) },
    { "nativeGetComponents", "(II)[Lohos/media/image/Image$Component;",
      reinterpret_cast<void *>(ohos_media_image_Image_nativeGetComponents) },
    { "nativeReleaseImage", "(Lohos/media/image/ImageReceiver;)V",
      reinterpret_cast<void *>(ohos_media_image_Image_nativeReleaseImage) },
};

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    HiLog::Debug(LABEL, "JNI_OnLoad begin");
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_4) != JNI_OK) {
        HiLog::Error(LABEL, "JNI_OnLoad: GetEnv failed");
        return ERROR;
    }

    int32_t receiveRet = JkitRegisterNativeMethods(env, "ohos/media/image/ImageReceiver", RECEIVER_METHODS,
                                                   ARRCOUNT(RECEIVER_METHODS));
    int32_t imageRet =
        JkitRegisterNativeMethods(env, "ohos/media/image/Image", IMAGE_METHODS, ARRCOUNT(IMAGE_METHODS));
    if (receiveRet == JNI_ERR || imageRet == JNI_ERR) {
        HiLog::Error(LABEL, "JkitRegisterNativeMethods failed, receiveRet = %{public}d, imageRet = %{public}d",
                     receiveRet, imageRet);
        return ERROR;
    }
    Jkit::nativeInit(vm);
    HiLog::Debug(LABEL, "JNI_OnLoad end");
    return JNI_VERSION_1_4;
}