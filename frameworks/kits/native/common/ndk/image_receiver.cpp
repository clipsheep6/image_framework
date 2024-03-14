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

#include <memory>
#include <string>
#include "common_utils.h"
#include "image/image_receiver.h"
#include "image_kits.h"
#include "receiver/include/image_receiver.h"

#ifdef __cplusplus
extern "C" {
#endif

struct OH_ImageReceiver {
    std::shared_ptr<OHOS::Media::ImageReceiver> ptrImgRcv;
};

struct OH_ImageReceiverOptions {
    /* Default width of the image received by the consumer, in pixels. */
    int32_t width = 0;
    /* Default height of the image received by the consumer, in pixels. */
    int32_t height = 0;
    /* Image format {@link OHOS_IMAGE_FORMAT_JPEG} created by using the receiver. */
    int32_t format = 0;
    /* Maximum number of images that can be cached. */
    int32_t capacity = 0;
};

namespace OHOS {
    namespace Media {
        class ImageReceiverListener : public SurfaceBufferAvaliableListener {
        public:
            explicit ImageReceiverListener(OH_ImageReceiver* receiver) : receiver_(receiver), callback_(nullptr) {}

            ~ImageReceiverListener() override
            {
                callback_ = nullptr;
            }

            void OnSurfaceBufferAvaliable() __attribute__((no_sanitize("cfi"))) override
            {
                if (nullptr != callback_) {
                    callback_(receiver_);
                }
            }

            OH_ImageReceiver* receiver_;
            OH_Image_Receiver_On_Callback callback_;
        };
    } // namespace Media
} // namespace OHOS

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_CreateImageReceiverOptions(OH_ImageReceiverOptions** options)
{
    if (nullptr == options) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    auto rst = new OH_ImageReceiverOptions;
    if (nullptr == rst) {
        return IMAGE_ERRORCODE_MALLOC_ABNORMAL;
    }
    *options = rst;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ImageReceiverOptionsGetSize(OH_ImageReceiverOptions* options, Image_Size* size)
{
    if (nullptr == options) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    size->width = options->width;
    size->height = options->height;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ImageReceiverOptionsSetSize(OH_ImageReceiverOptions* options, Image_Size size)
{
    if (nullptr == options) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    options->width = size.width;
    options->height = size.height;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ImageReceiverOptionsGetFormat(OH_ImageReceiverOptions* options, int32_t* format)
{
    if (nullptr == options) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    *format = options->format;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ImageReceiverOptionsSetFormat(OH_ImageReceiverOptions* options, int32_t format)
{
    if (nullptr == options) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    options->format = format;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ImageReceiverOptionsGetCapacity(OH_ImageReceiverOptions* options, int32_t* capacity)
{
    if (nullptr == options) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    *capacity = options->capacity;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ImageReceiverOptionsSetCapacity(OH_ImageReceiverOptions* options, int32_t capacity)
{
    if (nullptr == options) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    options->capacity = capacity;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ReleaseImageReceiverOptions(OH_ImageReceiverOptions* options)
{
    if (nullptr != options) {
        delete options;
    }
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_Create(OH_ImageReceiverOptions* options, OH_ImageReceiver** receiver)
{
    if (nullptr == options || nullptr == receiver) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }

    auto rst = new OH_ImageReceiver;
    if (nullptr == rst) {
        return IMAGE_ERRORCODE_MALLOC_ABNORMAL;
    }

    rst->ptrImgRcv = OHOS::Media::ImageReceiver::CreateImageReceiver(
        options->width, options->height, options->format, options->capacity);
    if (!(rst->ptrImgRcv)) {
        delete rst;
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    *receiver = rst;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_GetReceivingSurfaceId(OH_ImageReceiver* receiver, uint64_t* surfaceId)
{
    if (nullptr == receiver) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv || nullptr == receiver->ptrImgRcv->iraContext_) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    std::string strKey = receiver->ptrImgRcv->iraContext_->GetReceiverKey();
    if (strKey.empty() || nullptr == strKey.c_str()) {
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    *surfaceId = std::stoul(strKey);
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ReadLatestImage(OH_ImageReceiver* receiver, OH_Image** image)
{
    if (nullptr == receiver) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    auto bufferProcessor = receiver->ptrImgRcv->GetBufferProcessor();
    if (nullptr == bufferProcessor) {
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    auto surfaceBuffer = receiver->ptrImgRcv->ReadLastImage();
    if (nullptr == surfaceBuffer) {
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    auto rst = new OH_Image;
    if (nullptr == rst) {
        return IMAGE_ERRORCODE_MALLOC_ABNORMAL;
    }

    rst->imgNative = new OHOS::Media::NativeImage(surfaceBuffer, bufferProcessor);
    if (!(rst->imgNative)) {
        delete rst;
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    *image = rst;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_ReadNextImage(OH_ImageReceiver* receiver, OH_Image** image)
{
    if (nullptr == receiver) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    auto bufferProcessor = receiver->ptrImgRcv->GetBufferProcessor();
    if (nullptr == bufferProcessor) {
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    auto surfaceBuffer = receiver->ptrImgRcv->ReadNextImage();
    if (nullptr == surfaceBuffer) {
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    auto rst = new OH_Image;
    if (nullptr == rst) {
        return IMAGE_ERRORCODE_MALLOC_ABNORMAL;
    }

    rst->imgNative = new OHOS::Media::NativeImage(surfaceBuffer, bufferProcessor);
    if (!(rst->imgNative)) {
        delete rst;
        return IMAGE_ERRORCODE_GET_DATA_ABNORMAL;
    }

    *image = rst;
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_On(OH_ImageReceiver* receiver, OH_Image_Receiver_On_Callback callback)
{
    if (nullptr == receiver || nullptr == callback) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    auto listener = std::make_shared<OHOS::Media::ImageReceiverListener>(receiver);
    listener->callback_ = callback;
    receiver->ptrImgRcv->RegisterBufferAvaliableListener(listener);
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_Off(OH_ImageReceiver* receiver)
{
    if (nullptr == receiver) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    receiver->ptrImgRcv->UnRegisterBufferAvaliableListener();
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_GetSize(OH_ImageReceiver* receiver, Image_Size* size)
{
    if (nullptr == receiver || nullptr == size) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv || nullptr == receiver->ptrImgRcv->iraContext_) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    size->width = receiver->ptrImgRcv->iraContext_->GetWidth();
    size->height = receiver->ptrImgRcv->iraContext_->GetHeight();
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_GetCapacity(OH_ImageReceiver* receiver, int32_t* capacity)
{
    if (nullptr == receiver || nullptr == capacity) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv || nullptr == receiver->ptrImgRcv->iraContext_) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    *capacity = receiver->ptrImgRcv->iraContext_->GetCapicity();
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_GetFormat(OH_ImageReceiver* receiver, int32_t* format)
{
    if (nullptr == receiver || nullptr == format) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    if (nullptr == receiver->ptrImgRcv || nullptr == receiver->ptrImgRcv->iraContext_) {
        return IMAGE_ERRORCODE_BAD_PARAMETER;
    }

    *format = receiver->ptrImgRcv->iraContext_->GetFormat();
    return IMAGE_ERRORCODE_SUCCESS;
}

MIDK_EXPORT
Image_ErrorCode OH_ImageReceiver2_Release(OH_ImageReceiver* receiver)
{
    if (nullptr == receiver) {
        return IMAGE_ERRORCODE_INVALID_PARAMETER;
    }
    receiver->ptrImgRcv.reset();
    delete receiver;
    return IMAGE_ERRORCODE_SUCCESS;
}

#ifdef __cplusplus
};
#endif