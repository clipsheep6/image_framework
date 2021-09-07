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

#ifndef IMAGE_RECEIVER_PRIVATE_H
#define IMAGE_RECEIVER_PRIVATE_H

#include <android_media_Utils.h>
#include <cutils/atomic.h>
#include <gui/BufferItemConsumer.h>
#include <gui/Surface.h>
#include <ui/PublicFormat.h>
#include <utils/List.h>
#include "agp_view/agp_ui_surface.h"
#include "hilog/log.h"
#include "image_format.h"
#include "jkit_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "nocopyable.h"

namespace OHOS {
namespace Media {
class ImageReceiverContext : public android::ConsumerBase::FrameAvailableListener {
public:
    ImageReceiverContext(JNIEnv *env, jobject weakThiz, jclass clazz, int32_t capacity);
    virtual ~ImageReceiverContext();
    virtual void onFrameAvailable(const android::BufferItem &item);  // android virtual method
    android::BufferItem *PopBufferItem();
    void PushBufferItem(android::BufferItem *buffer);
    void SetBufferConsumer(const android::sp<android::BufferItemConsumer> &consumer)
    {
        bufferConsumer_ = consumer;
    }
    android::BufferItemConsumer *GetBufferConsumer()
    {
        return bufferConsumer_.get();
    }
    void SetProducer(const android::sp<android::IGraphicBufferProducer> &producer)
    {
        bufferProducer_ = producer;
    }
    android::IGraphicBufferProducer *GetProducer()
    {
        return bufferProducer_.get();
    }
    void SetBufferFormat(int32_t format)
    {
        format_ = format;
    }
    int32_t GetBufferFormat() const
    {
        return format_;
    }
    void SetBufferDataspace(android_dataspace dataSpace)
    {
        dataSpace_ = dataSpace;
    }
    android_dataspace GetBufferDataspace() const
    {
        return dataSpace_;
    }
    void SetBufferWidth(int32_t width)
    {
        width_ = width;
    }
    int32_t GetBufferWidth() const
    {
        return width_;
    }
    void SetBufferHeight(int32_t height)
    {
        height_ = height;
    }
    int32_t GetBufferHeight() const
    {
        return height_;
    }
    DISALLOW_COPY_AND_MOVE(ImageReceiverContext);

private:
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
        LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageReceiverContext"
    };
    android::List<android::BufferItem *> buffers_;
    android::sp<android::BufferItemConsumer> bufferConsumer_;
    android::sp<android::IGraphicBufferProducer> bufferProducer_;
    jobject weakThiz_;
    jclass clazz_;
    int32_t width_;
    int32_t height_;
    android_dataspace dataSpace_;
    int32_t format_;
};

class ImageReceiverCommon {
public:
    ImageReceiverCommon() = default;
    ~ImageReceiverCommon() = default;
    static bool InitBufferConsumer(JNIEnv *env, android::sp<ImageReceiverContext> context,
                                   android::sp<android::BufferItemConsumer> bufferConsumer,
                                   android::String8 consumerName, jint width, jint height, int nativeFormat,
                                   android_dataspace nativeDataspace);
    static bool CheckNoOpaqueFormat(JNIEnv *env, android::sp<ImageReceiverContext> context, android::BufferItem *buffer,
                                    android::BufferItemConsumer *bufferConsumer);
    static void SetNativeContext(JNIEnv *env, jobject thiz, android::sp<ImageReceiverContext> context);
    static ImageReceiverContext *GetImageReceiverContext(JNIEnv *env, jobject thiz);
    static android::BufferItem *GetBufferItem(JNIEnv *env, jobject image);
    static bool GetLockedImage(JNIEnv *env, jobject thiz, android::LockedImage *image);
    static android::sp<android::Fence> UnlockImageIfLocked(JNIEnv *env, jobject image);
    static uint32_t GetProcessUniqueId();
    // format transform
    static android::PublicFormat ConvertReceiverFormatToAndroidPublic(int32_t receiverFormat);
    static ImageFormat ConvertAndroidPublicToReceiverFormat(android::PublicFormat publicFormat);

private:
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
        LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageReceiverCommon"
    };
};
} // namespace Media
} // namespace OHOS

#endif // IMAGE_RECEIVER_PRIVATE_H