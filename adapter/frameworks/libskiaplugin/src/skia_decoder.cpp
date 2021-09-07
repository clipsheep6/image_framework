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

#include "skia_decoder.h"
#include "image_utils.h"
#include "media_errors.h"
#include "securec.h"

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace Media;
using namespace std;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "SkiaDecoder" };
namespace {
constexpr uint32_t IMAGE_NUM = 1;
}

void SkiaDecoder::SetSource(InputDataStream &sourceStream)
{
    stream_ = &sourceStream;
    state_ = SkiaDecodingState::SOURCE_INITED;
}

void SkiaDecoder::Reset()
{
    if (stream_ != nullptr) {
        stream_->Seek(0);
    }
    acodec_.release();
    info_.reset();
    desireColor_ = kUnknown_SkColorType;
}

uint32_t SkiaDecoder::GetImageSize(uint32_t index, PlSize &size)
{
    if (index >= IMAGE_NUM) {
        HiLog::Error(LABEL, "GetImageSize failed, invalid index:%{public}u, range:%{public}u", index, IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (state_ < SkiaDecodingState::SOURCE_INITED) {
        HiLog::Error(LABEL, "GetImageSize failed, invalid state:%{public}d", state_);
        return ERR_MEDIA_INVALID_OPERATION;
    }
    if (state_ >= SkiaDecodingState::BASE_INFO_PARSED) {
        size.width = info_.width();
        size.height = info_.height();
        return SUCCESS;
    }
    if (!DecodeHeader()) {
        HiLog::Error(LABEL, "GetImageSize failed, decode header failed, state=%{public}d", state_);
        return ERR_IMAGE_DECODE_HEAD_ABNORMAL;
    }
    size.width = info_.width();
    size.height = info_.height();
    state_ = SkiaDecodingState::BASE_INFO_PARSED;
    return SUCCESS;
}

uint32_t SkiaDecoder::SetDecodeOptions(uint32_t index, const PlDecodeOptions &opts, PlImageInfo &info)
{
    if (index >= IMAGE_NUM) {
        HiLog::Error(LABEL, "SetDecodeOptions failed, invalid index:%{public}u, range:%{public}u", index,
                     IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (state_ < SkiaDecodingState::SOURCE_INITED) {
        HiLog::Error(LABEL, "SetDecodeOptions failed, invalid state %{public}d", state_);
        return ERR_MEDIA_INVALID_OPERATION;
    }
    if (state_ >= SkiaDecodingState::IMAGE_DECODING) {
        Reset();
        state_ = SkiaDecodingState::SOURCE_INITED;
    }
    if (state_ < SkiaDecodingState::BASE_INFO_PARSED) {
        if (!DecodeHeader()) {
            HiLog::Error(LABEL, "GetImageSize failed, decode header failed, state=%{public}d", state_);
            return ERR_IMAGE_DECODE_HEAD_ABNORMAL;
        }
        state_ = SkiaDecodingState::BASE_INFO_PARSED;
    }
    HiLog::Info(LABEL, "SetDecodeOptions sampleSize:%{public}d, degree:%{public}f",
        opts.sampleSize, opts.rotateDegrees);
    PlPixelFormat desiredFormat = opts.desiredPixelFormat;
    sampleSize_ = opts.sampleSize;
    desireColor_ = ConvertToColorType(desiredFormat, info.pixelFormat);
    auto alphaType = info_.isOpaque() ? kOpaque_SkAlphaType : kPremul_SkAlphaType;
    // we need compute new info, compute sample dimensions
    auto sampledDims = acodec_->getSampledDimensions(sampleSize_);
    HiLog::Info(LABEL, "SetDecodeOptions sampledDims, width:%{public}d, height:%{public}d",
        sampledDims.width(), sampledDims.height());
    info_ = info_.makeColorType(desireColor_)
            .makeWH(sampledDims.width(), sampledDims.height())
            .makeAlphaType(alphaType);
    info.size.width = info_.width();
    info.size.height = info_.height();
    info.alphaType = ConvertToAlphaType(info_.alphaType());
    state_ = SkiaDecodingState::IMAGE_DECODING;
    return SUCCESS;
}

bool SkiaDecoder::HasProperty(std::string key)
{
    if (key == SKIA_DECODER) {
        return true;
    }
    return false;
}

uint32_t SkiaDecoder::AllocShareMemory(DecodeContext &context, uint64_t byteCount)
{
#if !defined(_WIN32) && !defined(_APPLE)
    int fd = AshmemCreate("BMP RawData", byteCount);
    if (fd < 0) {
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        ::close(fd);
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    void* ptr = ::mmap(nullptr, byteCount, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        ::close(fd);
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    context.pixelsBuffer.buffer = ptr;
    void *fdBuffer = new int32_t();
    if (fdBuffer == nullptr) {
        HiLog::Error(LABEL, "new fdBuffer fail");
        ::munmap(ptr, byteCount);
        ::close(fd);
        context.pixelsBuffer.buffer = nullptr;
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    *static_cast<int32_t *>(fdBuffer) = fd;
    context.pixelsBuffer.context = fdBuffer;
    context.allocatorType = AllocatorType::SHARE_MEM_ALLOC;
    context.pixelsBuffer.bufferSize = byteCount;
    context.freeFunc = nullptr;
#endif
    return SUCCESS;
}

uint32_t SkiaDecoder::AllocHeapBuffer(DecodeContext &context, uint64_t byteCount)
{
    void *outputBuffer = malloc(byteCount);
    if (outputBuffer == nullptr) {
        HiLog::Error(LABEL, "Decode failed, alloc failed!");
        return ERR_IMAGE_MALLOC_ABNORMAL;
    }
#ifdef _WIN32
    memset(outputBuffer, 0, byteCount);
#else
    if (memset_s(outputBuffer, byteCount, 0, byteCount) != EOK) {
        free(outputBuffer);
        outputBuffer = nullptr;
        return ERR_IMAGE_DECODE_FAILED;
    }
#endif
    context.pixelsBuffer.buffer = outputBuffer;
    context.pixelsBuffer.context = nullptr;
    context.allocatorType = AllocatorType::HEAP_ALLOC;
    context.pixelsBuffer.bufferSize = byteCount;
    context.freeFunc = nullptr;
    return SUCCESS;
}

uint32_t SkiaDecoder::Decode(uint32_t index, DecodeContext &context)
{
    if (index >= IMAGE_NUM) {
        HiLog::Error(LABEL, "Decode failed, invalid index:%{public}u, range:%{public}u", index, IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (acodec_ == nullptr) {
        HiLog::Error(LABEL, "Decode failed, codec is null");
        return ERR_IMAGE_DECODE_FAILED;
    }
    if (state_ != SkiaDecodingState::IMAGE_DECODING) {
        HiLog::Error(LABEL, "Decode failed, invalid state %{public}d", state_);
        return ERR_MEDIA_INVALID_OPERATION;
    }

    SkImageInfo dstInfo = info_;
    if (ImageUtils::CheckMulOverflow(dstInfo.width(), dstInfo.height(), dstInfo.bytesPerPixel())) {
        HiLog::Error(LABEL, "Decode failed, width:%{public}d, height:%{public}d is too large",
                     dstInfo.width(), dstInfo.height());
        return ERR_IMAGE_DECODE_FAILED;
    }

    if (context.pixelsBuffer.buffer == nullptr) {
        uint64_t byteCount = static_cast<uint64_t>(dstInfo.height()) * dstInfo.width() * dstInfo.bytesPerPixel();
        if (context.allocatorType == Media::AllocatorType::SHARE_MEM_ALLOC) {
            if (AllocShareMemory(context, byteCount) != SUCCESS) {
                return ERR_IMAGE_DECODE_FAILED;
            }
        } else {
            if (AllocHeapBuffer(context, byteCount) != SUCCESS) {
                return ERR_IMAGE_DECODE_FAILED;
            }
        }
    }
    uint8_t *dstBuffer = static_cast<uint8_t *>(context.pixelsBuffer.buffer);
    size_t rowBytes = dstInfo.width() * dstInfo.bytesPerPixel();
    SkAndroidCodec::AndroidOptions options;
    options.fSampleSize = sampleSize_;
    auto result = acodec_->getAndroidPixels(dstInfo, dstBuffer, rowBytes, &options);
    if (result != SkCodec::kSuccess) {
        HiLog::Error(LABEL, "Decode failed, get pixels failed, ret=%{public}d", result);
        state_ = SkiaDecodingState::IMAGE_ERROR;
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    state_ = SkiaDecodingState::IMAGE_DECODED;
    return SUCCESS;
}

uint32_t SkiaDecoder::PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context)
{
    // currently not support increment decode
    return ERR_IMAGE_DATA_UNSUPPORT;
}

bool SkiaDecoder::DecodeHeader()
{
    HiLog::Info(LABEL, "stream_->GetStreamType() %{public}d", stream_->GetStreamType());
    if (stream_->GetStreamType() == ImagePlugin::BUFFER_SOURCE_TYPE) {
        std::unique_ptr<SkStreamRewindable> stream =
                std::make_unique<SkMemoryStream>(stream_->GetDataPtr(), stream_->GetStreamSize(), false);
        acodec_ = SkAndroidCodec::MakeFromCodec(SkCodec::MakeFromStream(std::move(stream)));
    } else {
        acodec_ = SkAndroidCodec::MakeFromCodec(SkCodec::MakeFromStream(make_unique<SkiaStream>(stream_)));
    }
    if (!acodec_) {
        HiLog::Error(LABEL, "Could not create acodec_");
        return false;
    }
    info_ = acodec_->getInfo();
    return true;
}

PlAlphaType SkiaDecoder::ConvertToAlphaType(SkAlphaType alphaType)
{
    switch (alphaType) {
        case kOpaque_SkAlphaType:
            return PlAlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        case kPremul_SkAlphaType:
            return PlAlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        case kUnpremul_SkAlphaType:
            return PlAlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
        default:
            HiLog::Error(LABEL, "known alpha type:%{public}d", alphaType);
            break;
    }
    return PlAlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
}

SkColorType SkiaDecoder::ConvertToColorType(PlPixelFormat format, PlPixelFormat &outputFormat)
{
    switch (format) {
        case PlPixelFormat::UNKNOWN:
        case PlPixelFormat::RGBA_8888: {
            outputFormat = PlPixelFormat::RGBA_8888;
            return kRGBA_8888_SkColorType;
        }
        case PlPixelFormat::BGRA_8888: {
            outputFormat = PlPixelFormat::BGRA_8888;
            return kBGRA_8888_SkColorType;
        }
        case PlPixelFormat::ALPHA_8: {
            SkColorType colorType = info_.colorType();
            if (colorType == kAlpha_8_SkColorType || (colorType == kGray_8_SkColorType && info_.isOpaque())) {
                outputFormat = PlPixelFormat::ALPHA_8;
                return kAlpha_8_SkColorType;
            }
            break;
        }
        case PlPixelFormat::RGB_565: {
            if (info_.isOpaque()) {
                outputFormat = PlPixelFormat::RGB_565;
                return kRGB_565_SkColorType;
            }
            break;
        }
        default: {
            break;
        }
    }
    HiLog::Debug(LABEL, "unsupport conver to format:%{public}d, set default RGBA", format);
    outputFormat = PlPixelFormat::RGBA_8888;
    return kRGBA_8888_SkColorType;
}
}  // namespace ImagePlugin
}  // namespace OHOS
