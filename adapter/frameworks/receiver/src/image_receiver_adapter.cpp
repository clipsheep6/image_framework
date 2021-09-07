/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2019 The Android Open Source Project
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
#include "image_receiver_private.h"

namespace OHOS {
namespace Media {
static constexpr int32_t DEFAULT_SIZE = -1;
static constexpr int32_t MAX_COMPONENT_NUM = 3;

using namespace OHOS::HiviewDFX;

void ImageReceiverAdapter::InitImageReceiverContext(JNIEnv *env, jobject thiz, jobject weakThiz, jint width,
                                                    jint height, jint format, jint capacity, jlong usage)
{
    HiLog::Debug(LABEL,
                 "width:%{public}d, height:%{public}d, format:%{public}d, capacity:%{public}d, "
                 "usage:%{public}ld.",
                 width, height, format, capacity, static_cast<long>(usage));
    android::PublicFormat publicFormat = ImageReceiverCommon::ConvertReceiverFormatToAndroidPublic(format);
    int nativeFormat = android::mapPublicFormatToHalFormat(publicFormat);
    android_dataspace nativeDataspace = android::mapPublicFormatToHalDataspace(publicFormat);
    android::sp<android::IGraphicBufferProducer> graphicProducer;
    android::sp<android::IGraphicBufferConsumer> graphicConsumer;
    android::BufferQueue::createBufferQueue(&graphicProducer, &graphicConsumer);
    uint64_t consumerUsage = usage;  // ImageReceiver default use HardwareBuffer::USAGE_CPU_READ_OFTEN
    android::sp<android::BufferItemConsumer> bufferConsumer =
        new (std::nothrow) android::BufferItemConsumer(graphicConsumer, consumerUsage, capacity, true);
    if (bufferConsumer == nullptr) {
        std::string errMsg =
            ImageReceiverUtils::GetFmtMsg("allocate cumsumer buffer(format:0x%x) failed.", nativeFormat);
        ImageReceiverUtils::ThrowException(env, errMsg);
        return;
    }
    if (consumerUsage & GRALLOC_USAGE_PROTECTED) {
        graphicConsumer->setConsumerIsProtected(true);
    }
    jclass receiverClazz = env->GetObjectClass(thiz);
    if (ImageReceiverUtils::ThrowExceptionWhenNull(receiverClazz, env, "get ImageReceiver class fail.")) {
        return;
    }
    android::sp<ImageReceiverContext> context(
        new (std::nothrow)ImageReceiverContext(env, weakThiz, receiverClazz, capacity));
    env->DeleteLocalRef(receiverClazz);
    if (context == nullptr) {
        ImageReceiverUtils::ThrowException(env, "allocate image receiver context memory error.");
        return;
    }
    uint32_t uniqueId = ImageReceiverCommon::GetProcessUniqueId();
    android::String8 consumerName = android::String8::format("ImageReceiver-%dx%df%xc%d-%d-%d", width, height, format,
                                                             capacity, getpid(), uniqueId);
    if (!ImageReceiverCommon::InitBufferConsumer(env, context, bufferConsumer, consumerName, width, height,
        nativeFormat, nativeDataspace)) {
        HiLog::Error(LABEL, "init buffer consumer failed.");
        return;
    }
    context->SetBufferConsumer(bufferConsumer);
    context->SetProducer(graphicProducer);
    context->SetBufferFormat(nativeFormat);
    context->SetBufferDataspace(nativeDataspace);
    context->SetBufferWidth(width);
    context->SetBufferHeight(height);
    ImageReceiverCommon::SetNativeContext(env, thiz, context);
}

jboolean ImageReceiverAdapter::ReadNextImage(JNIEnv *env, jobject thiz, jobject image)
{
    ImageReceiverContext *context = ImageReceiverCommon::GetImageReceiverContext(env, thiz);
    if (ImageReceiverUtils::ThrowExceptionWhenNull(context, env, "ImageReceiver is not initialized or was closed.")) {
        return false;
    }
    android::BufferItem *buffer = context->PopBufferItem();
    if (ImageReceiverUtils::LogWhenNull(buffer, "buffer item is null, maybe exceed maximum of buffers.")) {
        return false;
    }
    android::BufferItemConsumer *bufferConsumer = context->GetBufferConsumer();
    if (ImageReceiverUtils::LogWhenNull(bufferConsumer, "buffer consumer is null.")) {
        return false;
    }
    int32_t ret = bufferConsumer->acquireBuffer(buffer, 0);
    if (ret != SUCCESS) {
        context->PushBufferItem(buffer);
        if (ret != android::BufferQueue::NO_BUFFER_AVAILABLE) {
            if (ret == android::INVALID_OPERATION) {
                HiLog::Error(LABEL, "get image exceed maximum of buffers.");
            } else {
                std::string errMsg = ImageReceiverUtils::GetFmtMsg("get image occurs unknown error:%d.", ret);
                ImageReceiverUtils::ThrowException(env, errMsg);
            }
        }
        return false;
    }
    if (!ImageReceiverCommon::CheckNoOpaqueFormat(env, context, buffer, bufferConsumer)) {
        HiLog::Error(LABEL, "check no opaque format failed.");
        return false;
    }
    // set image instance member variables
    env->SetLongField(image, g_imageCache.nativeBuffer, reinterpret_cast<jlong>(buffer));
    env->SetLongField(image, g_imageCache.timestamp, static_cast<jlong>(buffer->mTimestamp));
    return true;
}

void ImageReceiverAdapter::ReleaseFreeBuffers(JNIEnv *env, jobject thiz)
{
    ImageReceiverContext *context = ImageReceiverCommon::GetImageReceiverContext(env, thiz);
    if (ImageReceiverUtils::ThrowExceptionWhenNull(context, env, "ImageReceiver is not initialized or was closed.")) {
        return;
    }
    android::BufferItemConsumer *bufferConsumer = context->GetBufferConsumer();
    if (ImageReceiverUtils::ThrowExceptionWhenNull(bufferConsumer, env, "consumer buffer item is null.")) {
        return;
    }
    int32_t ret = bufferConsumer->discardFreeBuffers();
    if (ret != SUCCESS) {
        std::string errMsg = ImageReceiverUtils::GetFmtMsg("release free buffers failed:%d", ret);
        ImageReceiverUtils::ThrowException(env, errMsg);
    }
}

void ImageReceiverAdapter::CacheActiveClass(JNIEnv *env, jclass clazz)
{
    if (g_componentCache.clazz != nullptr && g_surfaceCache.clazz != nullptr) {
        HiLog::Debug(LABEL, "global active class was initialized.");
        return;
    }
    // ImageReceiver mapping
    g_imageReceiverCache.nativeContext = env->GetFieldID(clazz, "nativeContext", "J");
    if (ImageReceiverUtils::LogWhenNull(g_imageReceiverCache.nativeContext,
        "can't find ImageReceiver.nativeContext.")) {
        return;
    }
    g_imageReceiverCache.onEventFromNative =
        env->GetStaticMethodID(clazz, "onEventFromNative", "(Ljava/lang/Object;)V");
    if (ImageReceiverUtils::LogWhenNull(g_imageReceiverCache.onEventFromNative,
        "can't find ImageReceiver.onEventFromNative.")) {
        return;
    }
    if (!CacheImageClass(env)) {
        HiLog::Error(LABEL, "cache image attribute failed.");
        return;
    }
    if (!CacheSurfaceClass(env)) {
        HiLog::Error(LABEL, "cache surface attribute failed.");
        return;
    }
}

bool ImageReceiverAdapter::CacheImageClass(JNIEnv *env)
{
    jclass imageClazz = env->FindClass("ohos/media/image/Image");
    if (ImageReceiverUtils::LogWhenNull(imageClazz, "find Image class fail.")) {
        return false;
    }
    g_imageCache.nativeBuffer = env->GetFieldID(imageClazz, "nativeBuffer", "J");
    if (ImageReceiverUtils::LogWhenNull(g_imageCache.nativeBuffer, "can't find Image.nativeBuffer.")) {
        env->DeleteLocalRef(imageClazz);
        return false;
    }
    g_imageCache.timestamp = env->GetFieldID(imageClazz, "timestamp", "J");
    if (ImageReceiverUtils::LogWhenNull(g_imageCache.timestamp, "can't find Image.timestamp.")) {
        env->DeleteLocalRef(imageClazz);
        return false;
    }
    g_imageCache.components = env->GetFieldID(imageClazz, "components", "[Lohos/media/image/Image$Component;");
    if (ImageReceiverUtils::LogWhenNull(g_imageCache.components, "can't find Image.component.")) {
        env->DeleteLocalRef(imageClazz);
        return false;
    }
    // Image$Component constructor mapping
    jclass componentClazz = env->FindClass("ohos/media/image/Image$Component");
    if (ImageReceiverUtils::LogWhenNull(componentClazz, "can't find Image$Component class.")) {
        env->DeleteLocalRef(imageClazz);
        return false;
    }
    g_componentCache.constructor = env->GetMethodID(componentClazz, "<init>", "(IIILjava/nio/ByteBuffer;)V");
    if (ImageReceiverUtils::LogWhenNull(g_componentCache.constructor, "can't find Component constructor.")) {
        env->DeleteLocalRef(imageClazz);
        env->DeleteLocalRef(componentClazz);
        return false;
    }
    env->DeleteLocalRef(imageClazz);
    g_componentCache.clazz = reinterpret_cast<jclass>(env->NewGlobalRef(componentClazz));
    return true;
}

bool ImageReceiverAdapter::CacheSurfaceClass(JNIEnv *env)
{
    // Surface constructor mapping
    jclass surfaceClazz = env->FindClass("ohos/agp/graphics/Surface");
    if (ImageReceiverUtils::LogWhenNull(surfaceClazz, "can't find Surface class.")) {
        return false;
    }
    g_surfaceCache.constructor = env->GetMethodID(surfaceClazz, "<init>", "(J)V");
    if (ImageReceiverUtils::LogWhenNull(g_surfaceCache.constructor, "can't find Surface constructor.")) {
        env->DeleteLocalRef(surfaceClazz);
        return false;
    }
    g_surfaceCache.clazz = reinterpret_cast<jclass>(env->NewGlobalRef(surfaceClazz));
    return true;
}

jobject ImageReceiverAdapter::GetSurface(JNIEnv *env, jobject thiz)
{
    ImageReceiverContext *context = ImageReceiverCommon::GetImageReceiverContext(env, thiz);
    if (ImageReceiverUtils::ThrowExceptionWhenNull(context, env, "ImageReceiver is not initialized or was closed.")) {
        return nullptr;
    }
    android::IGraphicBufferProducer *graphicProducer = context->GetProducer();
    if (ImageReceiverUtils::ThrowExceptionWhenNull(graphicProducer, env, "graphic buffer producer is null.")) {
        return nullptr;
    }
    const android::sp<android::IGraphicBufferProducer> &bufferProducer = graphicProducer;
    AGP::UISurface *zSurface = new (std::nothrow) AGP::UISurface(bufferProducer, true);
    if (zSurface == nullptr) {
        HiLog::Error(LABEL, "create surface pointer fail.");
        return nullptr;
    }
    jobject surfaceObj =
        env->NewObject(g_surfaceCache.clazz, g_surfaceCache.constructor, reinterpret_cast<jlong>(zSurface));
    if (ImageReceiverUtils::LogWhenNull(surfaceObj, "create surface jni class fail.")) {
        delete zSurface;
        zSurface = nullptr;
        return nullptr;
    }
    return surfaceObj;
}

void ImageReceiverAdapter::ReleaseReceiver(JNIEnv *env, jobject thiz)
{
    ImageReceiverContext *context = ImageReceiverCommon::GetImageReceiverContext(env, thiz);
    if (ImageReceiverUtils::LogWhenNull(context, "ImageReceiver is not initialized or was closed.")) {
        return;
    }
    android::BufferItemConsumer *consumer = context->GetBufferConsumer();
    if (consumer != nullptr) {
        consumer->abandon();
        consumer->setFrameAvailableListener(nullptr);
    }
    ImageReceiverCommon::SetNativeContext(env, thiz, nullptr);
}

jint ImageReceiverAdapter::GetFormat(JNIEnv *env, jobject thiz, jint receiverFormat)
{
    android::PublicFormat publicFormat = ImageReceiverCommon::ConvertReceiverFormatToAndroidPublic(receiverFormat);
    if (android::isFormatOpaque(static_cast<int32_t>(publicFormat))) {
        return static_cast<jint>(ImageFormat::UNKNOWN);
    }
    int32_t nativeFormat = android::mapPublicFormatToHalFormat(publicFormat);
    android::BufferItem *buffer = ImageReceiverCommon::GetBufferItem(env, thiz);
    if (ImageReceiverUtils::LogWhenNull(buffer, "image buffer item is null.")) {
        return static_cast<jint>(ImageFormat::UNKNOWN);
    }
    int32_t actualFormat = android::applyFormatOverrides(buffer->mGraphicBuffer->getPixelFormat(), nativeFormat);
    // override the image format to HAL_PIXEL_FORMAT_YCbCr_420_888 if the actual format is NV21 or YV12.
    if (android::isPossiblyYUV(actualFormat)) {
        actualFormat = HAL_PIXEL_FORMAT_YCbCr_420_888;
    }
    android::PublicFormat publicFmt = android::mapHalFormatDataspaceToPublicFormat(actualFormat, buffer->mDataSpace);
    ImageFormat imageFmt = ImageReceiverCommon::ConvertAndroidPublicToReceiverFormat(publicFmt);
    return static_cast<jint>(imageFmt);
}

Media::Size ImageReceiverAdapter::GetSize(JNIEnv *env, jobject thiz)
{
    Size size;
    android::BufferItem *buffer = ImageReceiverCommon::GetBufferItem(env, thiz);
    if (ImageReceiverUtils::LogWhenNull(buffer, "image buffer item is null.")) {
        return size;
    }
    size.width = android::getBufferWidth(buffer);
    size.height = android::getBufferHeight(buffer);
    return size;
}

jobjectArray ImageReceiverAdapter::GetComponents(JNIEnv *env, jobject thiz, jint componentNum, jint receiverFormat)
{
    android::PublicFormat publicFormat = ImageReceiverCommon::ConvertReceiverFormatToAndroidPublic(receiverFormat);
    int32_t nativeFormat = android::mapPublicFormatToHalFormat(publicFormat);
    if (android::isFormatOpaque(nativeFormat) && componentNum > 0) {
        std::string errMsg = ImageReceiverUtils::GetFmtMsg("format 0x%x is opaque, components number %d must be 0",
                                                           nativeFormat, componentNum);
        ImageReceiverUtils::ThrowException(env, errMsg);
        return nullptr;
    }
    jobjectArray components = env->NewObjectArray(componentNum, g_componentCache.clazz, nullptr);
    if (ImageReceiverUtils::ThrowExceptionWhenNull(components, env, "create components array failed.")) {
        return nullptr;
    }
    if (android::isFormatOpaque(nativeFormat)) {
        return components;  // opaque components size is 0
    }
    android::LockedImage lockedImage = android::LockedImage();
    if (!ImageReceiverCommon::GetLockedImage(env, thiz, &lockedImage)) {
        env->DeleteLocalRef(components);
        return nullptr;
    }

    int32_t rowStride = 0;
    int32_t pixelStride = 0;
    uint8_t *dataPtr = nullptr;
    uint32_t dataSize = 0;
    jobject byteBuffer = nullptr;
    for (int32_t index = 0; index < componentNum; index++) {
        int32_t ret = android::getLockedImageInfo(&lockedImage, index, nativeFormat, &dataPtr, &dataSize, &pixelStride,
                                                  &rowStride);
        if (ret != SUCCESS) {
            env->DeleteLocalRef(components);
            HiLog::Error(LABEL, "get lock image info error:%{public}d", ret);
            return nullptr;
        }
        byteBuffer = env->NewDirectByteBuffer(dataPtr, dataSize);
        if (byteBuffer == nullptr) {
            env->DeleteLocalRef(components);
            ImageReceiverUtils::ThrowException(env, "allocate byte buffer failed, maybe memory not enough.");
            return nullptr;
        }
        int32_t componentType = static_cast<int32_t>(GetComponentType(receiverFormat, index));
        jobject component = env->NewObject(g_componentCache.clazz, g_componentCache.constructor, componentType,
                                           rowStride, pixelStride, byteBuffer);
        env->SetObjectArrayElement(components, index, component);
        env->DeleteLocalRef(component);
        env->DeleteLocalRef(byteBuffer);
    }
    return components;
}

void ImageReceiverAdapter::ReleaseImage(JNIEnv *env, jobject thiz, jobject imageReceiver)
{
    ImageReceiverContext *context = ImageReceiverCommon::GetImageReceiverContext(env, imageReceiver);
    if (ImageReceiverUtils::LogWhenNull(context, "maybe ImageReceiver#release called before Image#release.")) {
        return;
    }
    android::BufferItem *buffer = ImageReceiverCommon::GetBufferItem(env, thiz);
    if (ImageReceiverUtils::LogWhenNull(buffer, "image buffer item is null.")) {
        return;
    }
    android::sp<android::Fence> releaseFence = ImageReceiverCommon::UnlockImageIfLocked(env, thiz);
    android::BufferItemConsumer *bufferConsumer = context->GetBufferConsumer();
    if (ImageReceiverUtils::LogWhenNull(buffer, "image buffer consumer is null.")) {
        return;
    }
    bufferConsumer->releaseBuffer(*buffer, releaseFence);
    env->SetLongField(thiz, g_imageCache.nativeBuffer, reinterpret_cast<jlong>(nullptr));
    context->PushBufferItem(buffer);
    HiLog::Debug(LABEL, "image (format:0x%{public}x) has been released.", context->GetBufferFormat());
}

ComponentType ImageReceiverAdapter::GetComponentType(int32_t receiverFormat, int32_t index)
{
    ImageFormat imageFormat = static_cast<ImageFormat>(receiverFormat);
    ComponentType result = ComponentType::UNKNOWN;
    switch (imageFormat) {
        case ImageFormat::NV21:
        case ImageFormat::YUV420_888:
            if (index >= MAX_COMPONENT_NUM) {
                break;
            }
            result = (index == 0) ? ComponentType::YUV_Y : ((index == 1) ? ComponentType::YUV_U : ComponentType::YUV_V);
            break;
        case ImageFormat::JPEG:
            result = (index == 0) ? ComponentType::JPEG : ComponentType::UNKNOWN;
            break;
        case ImageFormat::RAW10:
            result = (index == 0) ? ComponentType::RAW10 : ComponentType::UNKNOWN;
            break;
        case ImageFormat::RAW16:
            result = (index == 0) ? ComponentType::RAW16 : ComponentType::UNKNOWN;
            break;
        case ImageFormat::H264:
            result = (index == 0) ? ComponentType::H264 : ComponentType::UNKNOWN;
            break;
        case ImageFormat::H265:
            result = (index == 0) ? ComponentType::H265 : ComponentType::UNKNOWN;
            break;
        default:
            HiLog::Error(LABEL, "unknown image format:%d", receiverFormat);
            break;
    }
    return result;
}

ImageReceiverContext::ImageReceiverContext(JNIEnv *env, jobject weakThiz, jclass clazz, int32_t capacity)
{
    weakThiz_ = env->NewGlobalRef(weakThiz);
    clazz_ = reinterpret_cast<jclass>(env->NewGlobalRef(clazz));
    format_ = static_cast<int32_t>(ImageFormat::UNKNOWN);
    dataSpace_ = HAL_DATASPACE_UNKNOWN;
    width_ = DEFAULT_SIZE;
    height_ = DEFAULT_SIZE;
    for (int32_t i = 0; i < capacity; i++) {
        android::BufferItem *buffer = new (std::nothrow) android::BufferItem;
        if (buffer == nullptr) {
            HiLog::Error(LABEL, "allocate buffer item %{public}d memory error.", i);
            continue;
        }
        buffers_.push_back(buffer);
    }
}

ImageReceiverContext::~ImageReceiverContext()
{
    Jkit jkit;
    JNIEnv *jniEnv = jkit.Get();
    if (jniEnv != nullptr) {
        jniEnv->DeleteGlobalRef(weakThiz_);
        jniEnv->DeleteGlobalRef(clazz_);
    } else {
        HiLog::Warn(LABEL, "leaking jni global object references.");
    }
    // delete buffer items
    for (android::List<android::BufferItem *>::iterator iter = buffers_.begin(); iter != buffers_.end(); ++iter) {
        delete *iter;
        *iter = nullptr;
    }
    buffers_.clear();
    if (bufferConsumer_ != nullptr) {
        bufferConsumer_.clear();
    }
    bufferConsumer_ = nullptr;
}

// android virtual method
void ImageReceiverContext::onFrameAvailable(const android::BufferItem &item)
{
    Jkit jkit;
    JNIEnv *jniEnv = jkit.Get();
    if (jniEnv != nullptr) {
        jniEnv->CallStaticVoidMethod(clazz_, g_imageReceiverCache.onEventFromNative, weakThiz_);
        HiLog::Debug(LABEL, "onFrameAvailable event post success.");
    } else {
        HiLog::Warn(LABEL, "onFrameAvailable event will not post cause by jniEnv is null.");
    }
}

android::BufferItem *ImageReceiverContext::PopBufferItem()
{
    if (buffers_.empty()) {
        return nullptr;
    }
    // return a buffer item and remove it from the list
    android::List<android::BufferItem *>::iterator iter = buffers_.begin();
    android::BufferItem *buffer = *iter;
    buffers_.erase(iter);
    return buffer;
}

void ImageReceiverContext::PushBufferItem(android::BufferItem *buffer)
{
    // clear the graphic buffer and push buffer item back to the list
    buffer->mGraphicBuffer = nullptr;
    buffers_.push_back(buffer);
}

bool ImageReceiverCommon::InitBufferConsumer(JNIEnv *env, android::sp<ImageReceiverContext> context,
                                             android::sp<android::BufferItemConsumer> bufferConsumer,
                                             android::String8 consumerName, jint width, jint height, int nativeFormat,
                                             android_dataspace nativeDataspace)
{
    bufferConsumer->setName(consumerName);
    bufferConsumer->setFrameAvailableListener(context);
    int32_t ret = bufferConsumer->setDefaultBufferSize(width, height);
    if (ret != SUCCESS) {
        std::string errMsg = ImageReceiverUtils::GetFmtMsg("set format 0x%x default buffer size:[%d, %d] failed:%d.",
                                                           nativeFormat, width, height, ret);
        ImageReceiverUtils::ThrowException(env, errMsg);
        return false;
    }
    ret = bufferConsumer->setDefaultBufferFormat(nativeFormat);
    if (ret != SUCCESS) {
        std::string errMsg =
            ImageReceiverUtils::GetFmtMsg("set default buffer format 0x%x failed:%d.", nativeFormat, ret);
        ImageReceiverUtils::ThrowException(env, errMsg);
        return false;
    }
    ret = bufferConsumer->setDefaultBufferDataSpace(nativeDataspace);
    if (ret != SUCCESS) {
        std::string errMsg =
            ImageReceiverUtils::GetFmtMsg("set default buffer data space 0x%x failed:%d.", nativeDataspace, ret);
        ImageReceiverUtils::ThrowException(env, errMsg);
        return false;
    }
    return true;
}

bool ImageReceiverCommon::CheckNoOpaqueFormat(JNIEnv *env, android::sp<ImageReceiverContext> context,
                                              android::BufferItem *buffer, android::BufferItemConsumer *bufferConsumer)
{
    if (!android::isFormatOpaque(context->GetBufferFormat())) {
        android::Point point = buffer->mCrop.leftTop();
        if (point.x != 0 || point.y != 0) {
            std::string errMsg =
                ImageReceiverUtils::GetFmtMsg("crop left top [%d, %d] must to be at origin", point.x, point.y);
            ImageReceiverUtils::ThrowException(env, errMsg);
            return false;
        }
        // check producer buffer settings is same as image receiver settings
        int32_t producerWidth = getBufferWidth(buffer);
        int32_t producerHeight = getBufferHeight(buffer);
        int32_t producerFormat = buffer->mGraphicBuffer->getPixelFormat();
        int32_t receiverWidth = context->GetBufferWidth();
        int32_t receiverHeight = context->GetBufferHeight();
        int32_t receiverFormat = context->GetBufferFormat();
        if ((producerFormat != HAL_PIXEL_FORMAT_BLOB) && (receiverFormat != HAL_PIXEL_FORMAT_BLOB) &&
            (producerWidth != receiverWidth || producerHeight != receiverHeight)) {
            HiLog::Debug(LABEL,
                         "producer size [%{public}d, %{public}d] mismatch receiver size [%{public}d, "
                         "%{public}d]",
                         producerWidth, producerHeight, receiverWidth, receiverHeight);
        }
        if (producerFormat != receiverFormat) {
            if (receiverFormat == HAL_PIXEL_FORMAT_YCbCr_420_888 && android::isPossiblyYUV(producerFormat)) {
                HiLog::Debug(LABEL, "treat producer format 0x%{public}x as HAL_PIXEL_FORMAT_YCbCr_420_888.",
                             producerFormat);
            } else if (receiverFormat == HAL_PIXEL_FORMAT_BLOB && producerFormat == HAL_PIXEL_FORMAT_RGBA_8888) {
                HiLog::Debug(LABEL, "receiving JPEG in HAL_PIXEL_FORMAT_RGBA_8888 buffer.");
            } else {
                bufferConsumer->releaseBuffer(*buffer);
                context->PushBufferItem(buffer);
                std::string errMsg = ImageReceiverUtils::GetFmtMsg("producer format:0x%x mismatch receiver format:0x%x",
                                                                   producerFormat, receiverFormat);
                ImageReceiverUtils::ThrowException(env, errMsg);
                return false;
            }
        }
    }
    return true;
}

void ImageReceiverCommon::SetNativeContext(JNIEnv *env, jobject thiz, android::sp<ImageReceiverContext> context)
{
    if (context != nullptr) {
        context->incStrong(reinterpret_cast<void *>(SetNativeContext));
        env->SetLongField(thiz, g_imageReceiverCache.nativeContext, reinterpret_cast<jlong>(context.get()));
    } else {
        ImageReceiverContext *ctx = GetImageReceiverContext(env, thiz);
        if (ctx != nullptr) {
            ctx->decStrong(reinterpret_cast<void *>(SetNativeContext));
            env->SetLongField(thiz, g_imageReceiverCache.nativeContext, reinterpret_cast<jlong>(nullptr));
        }
    }
}

ImageReceiverContext *ImageReceiverCommon::GetImageReceiverContext(JNIEnv *env, jobject thiz)
{
    ImageReceiverContext *context =
        reinterpret_cast<ImageReceiverContext *>(env->GetLongField(thiz, g_imageReceiverCache.nativeContext));
    return context;
}

bool ImageReceiverCommon::GetLockedImage(JNIEnv *env, jobject thiz, android::LockedImage *image)
{
    android::BufferItem *buffer = GetBufferItem(env, thiz);
    if (ImageReceiverUtils::ThrowExceptionWhenNull(buffer, env, "lock image buffer item is null.")) {
        return false;
    }
    int fenceFd = buffer->mFence->dup();
    int32_t ret = android::lockImageFromBuffer(buffer, GRALLOC_USAGE_SW_READ_OFTEN, fenceFd, image);
    if (ret != SUCCESS) {
        ::close(fenceFd);
        std::string errMsg = ImageReceiverUtils::GetFmtMsg("lock image buffer (format:0x%x) failed:%d",
                                                           buffer->mGraphicBuffer->getPixelFormat(), ret);
        ImageReceiverUtils::ThrowException(env, errMsg);
        return false;
    }
    // assignment
    image->crop = buffer->mCrop;
    image->timestamp = buffer->mTimestamp;
    image->dataSpace = buffer->mDataSpace;
    image->frameNumber = buffer->mFrameNumber;
    return true;
}

android::sp<android::Fence> ImageReceiverCommon::UnlockImageIfLocked(JNIEnv *env, jobject image)
{
    android::BufferItem *buffer = GetBufferItem(env, image);
    if (ImageReceiverUtils::ThrowExceptionWhenNull(buffer, env, "unlock image buffer item is null.")) {
        return android::Fence::NO_FENCE;
    }
    bool isBufferLocked = false;
    jobject components = nullptr;
    if (!android::isFormatOpaque(buffer->mGraphicBuffer->getPixelFormat())) {
        components = env->GetObjectField(image, g_imageCache.components);
    }
    isBufferLocked = (components != nullptr);
    if (isBufferLocked) {
        int32_t fenceFd = -1;
        int32_t ret = buffer->mGraphicBuffer->unlockAsync(&fenceFd);
        if (ret != SUCCESS) {
            std::string errMsg = ImageReceiverUtils::GetFmtMsg("async unlock buffer failed:%d", ret);
            ImageReceiverUtils::ThrowException(env, errMsg);
            return android::Fence::NO_FENCE;
        }
        android::sp<android::Fence> releaseFence = new (std::nothrow) android::Fence(fenceFd);
        if (releaseFence == nullptr) {
            HiLog::Error(LABEL, "allocate releaseFence memory error.");
        }
        return releaseFence;
    }
    return android::Fence::NO_FENCE;
}

android::BufferItem *ImageReceiverCommon::GetBufferItem(JNIEnv *env, jobject image)
{
    return reinterpret_cast<android::BufferItem *>(env->GetLongField(image, g_imageCache.nativeBuffer));
}

uint32_t ImageReceiverCommon::GetProcessUniqueId()
{
    static volatile int32_t g_Counter = 0;
    return android_atomic_inc(&g_Counter);
}

// only mapping several formats supported by ImageFormat at present
android::PublicFormat ImageReceiverCommon::ConvertReceiverFormatToAndroidPublic(int32_t receiverFormat)
{
    android::PublicFormat result = android::PublicFormat::UNKNOWN;
    ImageFormat imageFormat = static_cast<ImageFormat>(receiverFormat);
    switch (imageFormat) {
        case ImageFormat::NV21:
            result = android::PublicFormat::NV21;
            break;
        case ImageFormat::YUV420_888:
            result = android::PublicFormat::YUV_420_888;
            break;
        case ImageFormat::JPEG:
            result = android::PublicFormat::JPEG;
            break;
        case ImageFormat::RAW10:
            result = android::PublicFormat::RAW10;
            break;
        case ImageFormat::RAW16:
            result = android::PublicFormat::RAW_SENSOR;
            break;
        case ImageFormat::H264:
            result = android::PublicFormat::H264;
            break;
        case ImageFormat::H265:
            result = android::PublicFormat::H265;
            break;
        default:
            result = android::PublicFormat::UNKNOWN;
            break;
    }
    return result;
}

ImageFormat ImageReceiverCommon::ConvertAndroidPublicToReceiverFormat(android::PublicFormat publicFormat)
{
    ImageFormat result = ImageFormat::UNKNOWN;
    switch (publicFormat) {
        case android::PublicFormat::NV21:
            result = ImageFormat::NV21;
            break;
        case android::PublicFormat::YUV_420_888:
            result = ImageFormat::YUV420_888;
            break;
        case android::PublicFormat::JPEG:
            result = ImageFormat::JPEG;
            break;
        case android::PublicFormat::RAW10:
            result = ImageFormat::RAW10;
            break;
        case android::PublicFormat::RAW_SENSOR:
            result = ImageFormat::RAW16;
            break;
        case android::PublicFormat::H264:
            result = ImageFormat::H264;
            break;
        case android::PublicFormat::H265:
            result = ImageFormat::H265;
            break;
        default:
            result = ImageFormat::UNKNOWN;
            break;
    }
    return result;
}
} // namespace Media
} // namespace OHOS
