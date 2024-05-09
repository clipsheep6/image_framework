/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "image_image_receiver_impl.h"

#include "image_log.h"
#include "securec.h"

namespace OHOS {
namespace Media {

const int32_t CAMERA_APP_INNER_ENCODING_FORMAT = 4;
const int32_t JPEG_ENCODING_FORMAT = 2000;

struct ImageEnum {
    std::string name;
    int32_t numVal;
    std::string strVal;
};

static std::vector<struct ImageEnum> sImageFormatMap = {
    {"CAMERA_APP_INNER", CAMERA_APP_INNER_ENCODING_FORMAT, ""},
    {"JPEG", JPEG_ENCODING_FORMAT, ""},
};

static bool CheckFormat(int32_t format)
{
    for (auto imgEnum : sImageFormatMap) {
        if (imgEnum.numVal == format) {
            return true;
        }
    }
    return false;
}

int64_t ImageReceiverImpl::CreateImageReceiver(int32_t width, int32_t height, int32_t format, int32_t capacity)
{
    IMAGE_LOGI("[ImageReceiver] Create.");
    if (!CheckFormat(format)) {
        IMAGE_LOGE("[ImageReceiverImpl] Invailed param.");
        return IMAGE_FAILED;
    }
    std::shared_ptr imageReceiver = ImageReceiver::CreateImageReceiver(width, height, format, capacity);
    if (imageReceiver == nullptr) {
        IMAGE_LOGE("[ImageReceiverImpl] Failed to create native ImageReceiver.");
        return IMAGE_FAILED;
    }
    auto receiverImpl = FFIData::Create<ImageReceiverImpl>(imageReceiver);
    return receiverImpl->GetID();
}

ImageReceiverImpl::ImageReceiverImpl(std::shared_ptr<ImageReceiver> imageReceiver)
{
    IMAGE_LOGI("[ImageReceiverImpl] constructor.");
    imageReceiver_ = imageReceiver;
    IMAGE_LOGI("[ImageReceiverImpl] success");
}

int64_t ImageReceiverImpl::GetSize(CSize *ret)
{
    if (imageReceiver_ == nullptr || imageReceiver_->iraContext_ == nullptr) {
        IMAGE_LOGE("[ImageReceiverImpl] GetSize : Image receiver context is nullptr");
        return IMAGE_FAILED;
    }

    ret->width = imageReceiver_->iraContext_->GetWidth();
    ret->height = imageReceiver_->iraContext_->GetHeight();
    return IMAGE_SUCCESS;
}

int64_t ImageReceiverImpl::GetCapacity(int32_t *ret)
{
    if (imageReceiver_ == nullptr || imageReceiver_->iraContext_ == nullptr) {
        IMAGE_LOGE("[ImageReceiverImpl] GetCapacity : Image receiver context is nullptr");
        return IMAGE_FAILED;
    }

    *ret = imageReceiver_->iraContext_->GetCapicity();
    return IMAGE_SUCCESS;
}

int64_t ImageReceiverImpl::GetFormat(int32_t *ret)
{
    if (imageReceiver_ == nullptr || imageReceiver_->iraContext_ == nullptr) {
        IMAGE_LOGE("[ImageReceiverImpl] GetFormat : Image receiver context is nullptr");
        return IMAGE_FAILED;
    }

    *ret = imageReceiver_->iraContext_->GetFormat();
    return IMAGE_SUCCESS;
}

char *ImageReceiverImpl::GetReceivingSurfaceId()
{
    if (imageReceiver_ == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ImageReceiverContext> iraContext = imageReceiver_->iraContext_;
    if (iraContext == nullptr) {
        return nullptr;
    }
    
    auto strPtr = iraContext->GetReceiverKey().c_str();
    int len = strlen(strPtr);
    char *newStr = new char[len + 1];
    int ret = memcpy_s(newStr, len + 1, strPtr, len + 1);
    if (ret != 0) {
        IMAGE_LOGE("[ImageReceiverImpl] Failed to get receiver key");
    }
    strPtr = nullptr;
    return newStr;
}

sptr<ImageImpl> ImageReceiverImpl::ReadNextImage()
{
    if (imageReceiver_ == nullptr) {
        return nullptr;
    }
    auto image = imageReceiver_->NextNativeImage();
    if (image == nullptr) {
        IMAGE_LOGE("NextNativeImage is nullptr");
        return nullptr;
    }
    auto imageImpl = FFIData::Create<ImageImpl>(image);
    if (imageImpl == nullptr) {
        IMAGE_LOGE("ImageImpl Create is nullptr");
        return nullptr;
    }
    return imageImpl;
}

sptr<ImageImpl> ImageReceiverImpl::ReadLatestImage()
{
    if (imageReceiver_ == nullptr) {
        return nullptr;
    }
    auto image = imageReceiver_->LastNativeImage();
    if (image == nullptr) {
        IMAGE_LOGE("LastNativeImage is nullptr.");
        return nullptr;
    }
    return FFIData::Create<ImageImpl>(image);
}

void ImageReceiverImpl::Release()
{
    if (imageReceiver_ != nullptr) {
        imageReceiver_ = nullptr;
    }
}
}
}