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

#include "heif_decoder_adapter.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "media_errors.h"

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
using namespace ImagePlugin;

constexpr HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "HeifDecodeAdapter" };

constexpr int32_t PER_BYTES_RGB565 = 2;
constexpr int32_t PER_BYTES_RGBA = 4;
constexpr int32_t PER_BYTES_BGRA = 4;

HeifFrameInfo HeifDecoderAdapter::heifFrameInfo_;
HeifDecoderAdapter::HeifDecoderAdapter(HeifDecoder *heifDecoder) : heifDecoder_(heifDecoder)
{}

std::unique_ptr<HeifDecoderInterface> HeifDecoderAdapter::MakeFromStream(InputDataStream &stream)
{
    std::unique_ptr<HeifDecoder> heifDecoder(createHeifDecoder());
    if (heifDecoder.get() == nullptr) {
        HiLog::Error(LABEL, "heif decoder object is null.");
        return nullptr;
    }

    HeifStreamWrapper *heifStreamWrapper = new (std::nothrow) HeifStreamWrapper(&stream);
    if (heifStreamWrapper == nullptr) {
        HiLog::Error(LABEL, "create heif stream wrapper object failed.");
        return nullptr;
    }

    if (!heifDecoder->init(heifStreamWrapper, &heifFrameInfo_)) {
        HiLog::Error(LABEL, "init heif decoder failed.");
        delete heifStreamWrapper;
        return nullptr;
    }

    HeifDecoderAdapter *heifDecoderAdapter = new (std::nothrow) HeifDecoderAdapter(heifDecoder.release());
    if (heifDecoderAdapter == nullptr) {
        HiLog::Error(LABEL, "create heif decoder adapter object failed.");
        delete heifStreamWrapper;
        return nullptr;
    }
    return std::unique_ptr<HeifDecoderInterface>(heifDecoderAdapter);
}

uint32_t HeifDecoderAdapter::ReadRows(const uint32_t rowBytes, const uint32_t count, DecodeContext &context)
{
    uint8_t *decodeDst = static_cast<uint8_t *>(context.pixelsBuffer.buffer);
    for (uint32_t i = 0; i < count; i++) {
        if (!heifDecoder_->getScanline(decodeDst)) {
            HiLog::Error(LABEL, "scan line failed, current rows:%{public}u.", i);
            return i;
        }
        decodeDst = (decodeDst + rowBytes);
        if (decodeDst == nullptr) {
            HiLog::Error(LABEL, "decodeDst is null, current rows:%{public}u.", i);
            return i;
        }
    }
    return count;
}

uint32_t HeifDecoderAdapter::OnGetPixels(const PlSize &dstSize, const uint32_t dstRowBytes, DecodeContext &context)
{
    if (context.pixelsBuffer.buffer == nullptr) {
        HiLog::Error(LABEL, "buffer is null.");
        return ERR_IMAGE_DECODE_FAILED;
    }

    HeifFrameInfo heifInfo;
    bool success = heifDecoder_->decode(&heifInfo);
    if (!success) {
        HiLog::Error(LABEL, "heif decode failed.");
        return ERR_IMAGE_DECODE_FAILED;
    }

    uint32_t rows = this->ReadRows(dstRowBytes, dstSize.height, context);
    if (rows < dstSize.height && !isAllowPartialImage_) {
        HiLog::Error(LABEL, "current rows:%{public}u less than desire height:%{public}u.", rows, dstSize.height);
        return ERR_IMAGE_DECODE_FAILED;
    }

    return SUCCESS;
}

void HeifDecoderAdapter::GetHeifSize(PlSize &size)
{
    size.width = heifFrameInfo_.mWidth;
    size.height = heifFrameInfo_.mHeight;
}

void HeifDecoderAdapter::SetAllowPartial(const bool isAllowPartialImage)
{
    HiLog::Debug(LABEL, "allow image display partial.");
    isAllowPartialImage_ = isAllowPartialImage;
}

bool HeifDecoderAdapter::ConversionSupported(const PlPixelFormat &format, int32_t &bytesPerPixel)
{
    bool isSetOK = true;
    switch (format) {
        case PlPixelFormat::RGBA_8888:
            isSetOK = heifDecoder_->setOutputColor(kHeifColorFormat_RGBA_8888);
            bytesPerPixel = PER_BYTES_RGBA;
            break;
        case PlPixelFormat::BGRA_8888:
            isSetOK = heifDecoder_->setOutputColor(kHeifColorFormat_BGRA_8888);
            bytesPerPixel = PER_BYTES_BGRA;
            break;
        case PlPixelFormat::RGB_565:
            isSetOK = heifDecoder_->setOutputColor(kHeifColorFormat_RGB565);
            bytesPerPixel = PER_BYTES_RGB565;
            break;
        default:
            isSetOK = heifDecoder_->setOutputColor(kHeifColorFormat_RGBA_8888);
            bytesPerPixel = PER_BYTES_RGBA;
            break;
    }
    return isSetOK;
}
} // namespace Media
} // namespace OHOS