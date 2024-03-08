/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "image_receiver.h"

#include "image_log.h"
#include "image_packer.h"
#include "image_source.h"
#include "image_utils.h"
#include "image_receiver_buffer_processor.h"
#include "image_receiver_manager.h"
#include "v1_1/buffer_handle_meta_key_type.h"

using namespace OHOS::HDI::Display::Graphic::Common::V1_1;

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "imageReceiver"

namespace OHOS {
namespace Media {

ImageReceiver::~ImageReceiver()
{
    std::lock_guard<std::mutex> guard(imageReceiverMutex_);
    if (receiverConsumerSurface_ != nullptr) {
        receiverConsumerSurface_->UnregisterConsumerListener();
    }
    receiverConsumerSurface_ = nullptr;
    receiverProducerSurface_ = nullptr;
    iraContext_ = nullptr;
    surfaceBufferAvaliableListener_ = nullptr;
    bufferProcessor_ = nullptr;
}

enum class Mode {
    MODE_PREVIEW = 0,
    MODE_PHOTO
};

int64_t PackImage(int &fd, std::unique_ptr<PixelMap> pixelMap)
{
    IMAGE_LOGD("PackImage");
    ImagePacker imagePacker;
    PackOption option;
    option.format = ImageReceiver::OPTION_FORMAT;
    option.quality = ImageReceiver::OPTION_QUALITY;
    option.numberHint = ImageReceiver::OPTION_NUMBERHINT;
    std::set<std::string> formats;
    if (pixelMap == nullptr) {
        IMAGE_LOGE("pixelMap is nullptr");
        return 0;
    }
    uint32_t ret = imagePacker.GetSupportedFormats(formats);
    if (ret != SUCCESS) {
        IMAGE_LOGE("image packer get supported format failed, ret=%{public}u.", ret);
        return 0;
    } else {
        IMAGE_LOGD("SUCCESS");
    }
    imagePacker.StartPacking(fd, option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize = 0;
    imagePacker.FinalizePacking(packedSize);
    IMAGE_LOGD("packedSize=%{public}lld.", static_cast<long long>(packedSize));
    IMAGE_LOGD("packedSize=%{public}lld.", static_cast<long long>(packedSize));
    return packedSize;
}

std::unique_ptr<PixelMap> ImageReceiver::getSurfacePixelMap(InitializationOptions initializationOpts)
{
    uint32_t *addr = reinterpret_cast<uint32_t *>(iraContext_->currentBuffer_->GetVirAddr());
    int32_t size = iraContext_->currentBuffer_->GetSize();
    return PixelMap::Create(addr, (uint32_t)size, initializationOpts);
}

static int32_t SaveSTP(uint32_t *buffer,
                       uint32_t bufferSize,
                       int &fd,
                       InitializationOptions initializationOpts)
{
    int64_t errorCode = -1;
    std::unique_ptr<PixelMap> pixelMap = PixelMap::Create(buffer, bufferSize, initializationOpts);
    if (pixelMap.get() != nullptr) {
        ImageInfo imageInfo;
        pixelMap->GetImageInfo(imageInfo);
        IMAGE_LOGD("create pixel map imageInfo.size.width=%{public}u.", imageInfo.size.width);
    } else {
        IMAGE_LOGE("pixelMap.get() == nullptr");
        return ERR_MEDIA_INVALID_VALUE;
    }
    ImagePacker imagePacker;
    errorCode = PackImage(fd, std::move(pixelMap));
    if (errorCode > 0) {
        errorCode = SUCCESS;
    } else {
        errorCode = ERR_MEDIA_INVALID_VALUE;
    }
    return errorCode;
}

int32_t ImageReceiver::SaveBufferAsImage(int &fd,
                                         OHOS::sptr<OHOS::SurfaceBuffer> buffer,
                                         InitializationOptions initializationOpts)
{
    int32_t errorcode = 0;
    if (buffer != nullptr) {
        uint32_t *addr = reinterpret_cast<uint32_t *>(buffer->GetVirAddr());
        int32_t size = buffer->GetSize();
        errorcode = SaveSTP(addr, static_cast<uint32_t>(size), fd, initializationOpts);
        if ((iraContext_->GetReceiverBufferConsumer()) != nullptr) {
            (iraContext_->GetReceiverBufferConsumer())->ReleaseBuffer(buffer, -1);
        } else {
            IMAGE_LOGD("iraContext_->GetReceiverBufferConsumer() == nullptr");
        }
    } else {
        IMAGE_LOGD("SaveBufferAsImage buffer == nullptr");
    }
    return errorcode;
}

int32_t ImageReceiver::SaveBufferAsImage(int &fd,
                                         InitializationOptions initializationOpts)
{
    if (iraContext_->currentBuffer_ != nullptr) {
        return SaveBufferAsImage(fd, iraContext_->currentBuffer_, initializationOpts);
    }
    IMAGE_LOGD("iraContext_->GetCurrentBuffer() == nullptr");
    return 0;
}

void ImageReceiver::ReleaseBuffer(OHOS::sptr<OHOS::SurfaceBuffer> &buffer) __attribute__((no_sanitize("cfi")))
{
    std::lock_guard<std::mutex> guard(imageReceiverMutex_);
    if (buffer != nullptr) {
        if (iraContext_ != nullptr) {
            auto listenerConsumerSurface = iraContext_->GetReceiverBufferConsumer();
            if (listenerConsumerSurface != nullptr) {
                listenerConsumerSurface->ReleaseBuffer(buffer, -1);
            } else {
                IMAGE_LOGD("listenerConsumerSurface == nullptr");
            }
        } else {
            IMAGE_LOGD("iraContext_ == nullptr");
        }
        buffer = nullptr;
    }
}

void ImageReceiverSurfaceListener ::OnBufferAvailable()
{
    IMAGE_LOGD("OnBufferAvailable");
    auto ir = ir_.lock();
    if (ir && ir->surfaceBufferAvaliableListener_ != nullptr) {
        ir->surfaceBufferAvaliableListener_->OnSurfaceBufferAvaliable();
    }
}

std::shared_ptr<ImageReceiverContext> ImageReceiverContext ::CreateImageReceiverContext()
{
    std::shared_ptr<ImageReceiverContext> irc = std::make_shared<ImageReceiverContext>();
    return irc;
}

sptr<Surface> ImageReceiver::getSurfaceById(std::string id)
{
    ImageReceiverManager& imageReceiverManager = ImageReceiverManager::getInstance();
    sptr<Surface> surface = imageReceiverManager.getSurfaceByKeyId(id);
    IMAGE_LOGD("getSurfaceById");
    return surface;
}

std::shared_ptr<ImageReceiver> ImageReceiver::CreateImageReceiver(int32_t width,
                                                                  int32_t height,
                                                                  int32_t format,
                                                                  int32_t capicity)
{
    std::shared_ptr<ImageReceiver> iva = std::make_shared<ImageReceiver>();
    iva->iraContext_ = ImageReceiverContext::CreateImageReceiverContext();
    iva->receiverConsumerSurface_ = IConsumerSurface::Create();
    if (iva->receiverConsumerSurface_ == nullptr) {
        IMAGE_LOGD("SurfaceAsConsumer == nullptr");
        return iva;
    }

    iva->receiverConsumerSurface_->SetDefaultWidthAndHeight(width, height);
    iva->receiverConsumerSurface_->SetQueueSize(capicity);

    iva->receiverConsumerSurface_->SetDefaultUsage(BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE |
            BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_CPU_HW_BOTH);
    auto p = iva->receiverConsumerSurface_->GetProducer();
    iva->receiverProducerSurface_ = Surface::CreateSurfaceAsProducer(p);
    if (iva->receiverProducerSurface_ == nullptr) {
        IMAGE_LOGD("SurfaceAsProducer == nullptr");
        return iva;
    }

    iva->iraContext_->SetReceiverBufferConsumer(iva->receiverConsumerSurface_);
    iva->iraContext_->SetReceiverBufferProducer(iva->receiverProducerSurface_);
    iva->iraContext_->SetWidth(width);
    iva->iraContext_->SetHeight(height);
    iva->iraContext_->SetFormat(format);
    iva->iraContext_->SetCapicity(capicity);
    ImageReceiverManager& imageReceiverManager = ImageReceiverManager::getInstance();
    std::string receiverKey = imageReceiverManager.SaveImageReceiver(iva);
    iva->iraContext_->SetReceiverKey(receiverKey);
    sptr<ImageReceiverSurfaceListener> listener = new ImageReceiverSurfaceListener();
    listener->ir_ = iva;
    iva->receiverConsumerSurface_->
    RegisterConsumerListener((sptr<IBufferConsumerListener> &)listener);
    return iva;
}

uint32_t ImageReceiver::RequestCpuAccess(bool isCpuAccess)
{
    receiverConsumerSurface_->ConsumerRequestCpuAccess(isCpuAccess);
    IMAGE_LOGD("ImageReceiver::RequestCpuAccess %{public}d", isCpuAccess);
    return SUCCESS;
}

OHOS::sptr<OHOS::SurfaceBuffer> ImageReceiver::ReadNextImage()
{
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage = {};
    OHOS::sptr<OHOS::SurfaceBuffer> buffer;
    sptr<IConsumerSurface> listenerConsumerSurface = iraContext_->GetReceiverBufferConsumer();
    SurfaceError surfaceError = listenerConsumerSurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    if (surfaceError == SURFACE_ERROR_OK) {
        iraContext_->currentBuffer_ = buffer;
    } else {
        IMAGE_LOGD("buffer is null");
    }
    return iraContext_->GetCurrentBuffer();
}

OHOS::sptr<OHOS::SurfaceBuffer> ImageReceiver::ReadLastImage()
{
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage = {};
    OHOS::sptr<OHOS::SurfaceBuffer> buffer;
    OHOS::sptr<OHOS::SurfaceBuffer> bufferBefore;
    sptr<IConsumerSurface> listenerConsumerSurface = iraContext_->GetReceiverBufferConsumer();
    SurfaceError surfaceError = listenerConsumerSurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    while (surfaceError == SURFACE_ERROR_OK) {
        bufferBefore = buffer;
        surfaceError = listenerConsumerSurface->AcquireBuffer(buffer, flushFence, timestamp, damage);
    }
    iraContext_->currentBuffer_ = bufferBefore;
    return iraContext_->GetCurrentBuffer();
}

sptr<Surface> ImageReceiver::GetReceiverSurface()
{
    return iraContext_->GetReceiverBufferProducer();
}

void ImageReceiver::ReleaseReceiver()
{
    ImageReceiver::~ImageReceiver();
}

std::shared_ptr<IBufferProcessor> ImageReceiver::GetBufferProcessor()
{
    if (bufferProcessor_ == nullptr) {
        bufferProcessor_ = std::make_shared<ImageReceiverBufferProcessor>(this);
    }
    return bufferProcessor_;
}

std::shared_ptr<NativeImage> ImageReceiver::NextNativeImage()
{
    if (GetBufferProcessor() == nullptr) {
        return nullptr;
    }

    auto surfaceBuffer = ReadNextImage();
    if (surfaceBuffer == nullptr) {
        return nullptr;
    }
    std::vector<uint8_t> values;
    surfaceBuffer->GetMetadata(ATTRKEY_REQUEST_ACCESS_TYPE, values);
    if (values.size() == 1) {
        if (values[0] == HEBC_ACCESS_CPU_ACCESS) {
            IMAGE_LOGD("ImageReceiver::NextNativeImage GetMetadata ATTRKEY_REQUEST_ACCESS_TYPE HEBC_ACCESS_CPU_ACCESS");
            return std::make_shared<NativeImage>(surfaceBuffer, GetBufferProcessor(), HEBC_ACCESS_CPU_ACCESS);
        } else {
            IMAGE_LOGD("ImageReceiver::NextNativeImage GetMetadata ATTRKEY_REQUEST_ACCESS_TYPE HW_ONLY");
        }
    } else {
        IMAGE_LOGD("ImageReceiver::NextNativeImage GetMetadata ATTRKEY_REQUEST_ACCESS_TYPE not found");
    }
    return std::make_shared<NativeImage>(surfaceBuffer, GetBufferProcessor());
}

std::shared_ptr<NativeImage> ImageReceiver::LastNativeImage()
{
    if (GetBufferProcessor() == nullptr) {
        return nullptr;
    }

    auto surfaceBuffer = ReadLastImage();
    if (surfaceBuffer == nullptr) {
        return nullptr;
    }
    
    std::vector<uint8_t> values;
    surfaceBuffer->GetMetadata(ATTRKEY_REQUEST_ACCESS_TYPE, values);
    if (values.size() == 1) {
        if (values[0] ==HEBC_ACCESS_CPU_ACCESS) {
            IMAGE_LOGD("ImageReceiver::LastNativeImage GetMetadata ATTRKEY_REQUEST_ACCESS_TYPE HEBC_ACCESS_CPU_ACCESS");
            return std::make_shared<NativeImage>(surfaceBuffer, GetBufferProcessor(), HEBC_ACCESS_CPU_ACCESS);
        } else {
            IMAGE_LOGD("ImageReceiver::LastNativeImage GetMetadata ATTRKEY_REQUEST_ACCESS_TYPE HW_ONLY");
        }
    } else {
        IMAGE_LOGD("ImageReceiver::LastNativeImage GetMetadata ATTRKEY_REQUEST_ACCESS_TYPE not found");
    }   
    return std::make_shared<NativeImage>(surfaceBuffer, GetBufferProcessor());
}
} // namespace Media
} // namespace OHOS
