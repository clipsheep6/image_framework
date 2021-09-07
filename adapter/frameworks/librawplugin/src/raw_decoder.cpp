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


#include "raw_decoder.h"
#include "media_errors.h"
#include "securec.h"

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace Media;
using namespace std;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "RawDecoder" };
namespace {
constexpr uint32_t RAW_IMAGE_NUM = 1;
}

void RawDecoder::SetSource(InputDataStream &sourceStream)
{
    stream_ = &sourceStream;
    state_ = RawDecodingState::SOURCE_INITED;
}

void RawDecoder::Reset()
{
    if (stream_ != nullptr) {
        stream_->Seek(0);
    }
    codec_.release();
    info_.reset();
    desireColor_ = kUnknown_SkColorType;
}

uint32_t RawDecoder::GetImageSize(uint32_t index, PlSize &size)
{
    if (index >= RAW_IMAGE_NUM) {
        HiLog::Error(LABEL, "GetImageSize failed, invalid index:%{public}u, range:%{public}u", index, RAW_IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (state_ < RawDecodingState::SOURCE_INITED) {
        HiLog::Error(LABEL, "GetImageSize failed, invalid state:%{public}d", state_);
        return ERR_MEDIA_INVALID_OPERATION;
    }
    if (state_ >= RawDecodingState::BASE_INFO_PARSED) {
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
    state_ = RawDecodingState::BASE_INFO_PARSED;
    return SUCCESS;
}

uint32_t RawDecoder::SetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info)
{
    if (index >= RAW_IMAGE_NUM) {
        HiLog::Error(LABEL, "SetDecodeOptions failed, invalid index:%{public}u, range:%{public}u", index,
                     RAW_IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (state_ < RawDecodingState::SOURCE_INITED) {
        HiLog::Error(LABEL, "SetDecodeOptions failed, invalid state %{public}d", state_);
        return ERR_MEDIA_INVALID_OPERATION;
    }
    if (state_ >= RawDecodingState::IMAGE_DECODING) {
        Reset();
        state_ = RawDecodingState::SOURCE_INITED;
    }
    if (state_ < RawDecodingState::BASE_INFO_PARSED) {
        if (!DecodeHeader()) {
            HiLog::Error(LABEL, "GetImageSize failed, decode header failed, state=%{public}d", state_);
            return ERR_IMAGE_DECODE_HEAD_ABNORMAL;
        }
        state_ = RawDecodingState::BASE_INFO_PARSED;
    }
    PlPixelFormat desiredFormat = opts.desiredPixelFormat;
    desireColor_ = ConvertToColorType(desiredFormat, info.pixelFormat);
    info.size.width = info_.width();
    info.size.height = info_.height();
    info.alphaType = ConvertToAlphaType(info_.alphaType());
    state_ = RawDecodingState::IMAGE_DECODING;
    return SUCCESS;
}

uint32_t RawDecoder::AllocateMemory(SkImageInfo dstInfo, DecodeContext &context)
{
    size_t rowBytes = dstInfo.width() * dstInfo.bytesPerPixel();
    uint64_t byteCount = static_cast<uint64_t>(dstInfo.height()) * rowBytes;
    if (context.allocatorType == Media::AllocatorType::SHARE_MEM_ALLOC) {
#ifndef _WIN32
        int fd = AshmemCreate("Common RawData", byteCount);
        if (fd < 0) {
            return ERR_SHAMEM_DATA_ABNORMAL;
        }
        int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
        if (result < 0) {
            ::close(fd);
            return ERR_SHAMEM_DATA_ABNORMAL;
        }
        void *ptr = ::mmap(nullptr, byteCount, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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
        context.pixelsBuffer.bufferSize = byteCount;
        context.allocatorType = AllocatorType::SHARE_MEM_ALLOC;
        context.freeFunc = nullptr;
#endif
    } else {
        void *outputBuffer = malloc(byteCount);
        if (outputBuffer == nullptr) {
            HiLog::Error(LABEL, "Decode failed, alloc output buffer size:[%{public}llu] error",
                static_cast<unsigned long long>(byteCount));
            return ERR_IMAGE_MALLOC_ABNORMAL;
        }
#ifdef _WIN32
        memset(outputBuffer, 0, byteCount);
#else
        if (memset_s(outputBuffer, byteCount, 0, byteCount) != EOK) {
            HiLog::Error(LABEL, "Decode failed, memset buffer failed");
            free(outputBuffer);
            outputBuffer = nullptr;
            return ERR_IMAGE_MALLOC_ABNORMAL;
        }
#endif
        context.pixelsBuffer.buffer = outputBuffer;
        context.pixelsBuffer.bufferSize = byteCount;
        context.pixelsBuffer.context = nullptr;
        context.allocatorType = AllocatorType::HEAP_ALLOC;
        context.freeFunc = nullptr;
    }
    return SUCCESS;
}

uint32_t RawDecoder::Decode(uint32_t index, DecodeContext &context)
{
    HiLog::Error(LABEL, "Decode begin");
    if (index >= RAW_IMAGE_NUM) {
        HiLog::Error(LABEL, "Decode failed, invalid index:%{public}u, range:%{public}u", index, RAW_IMAGE_NUM);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (codec_ == nullptr) {
        HiLog::Error(LABEL, "Decode failed, codec is null");
        return ERR_IMAGE_DECODE_FAILED;
    }
    if (state_ != RawDecodingState::IMAGE_DECODING) {
        HiLog::Error(LABEL, "Decode failed, invalid state %{public}d", state_);
        return ERR_MEDIA_INVALID_OPERATION;
    }
    SkImageInfo dstInfo = info_.makeColorType(desireColor_);
    size_t rowBytes = dstInfo.width() * dstInfo.bytesPerPixel();
    if (context.pixelsBuffer.buffer == nullptr) {
        uint32_t resultCode = AllocateMemory(dstInfo, context);
        if (resultCode != SUCCESS) {
            return resultCode;
        }
    }
    HiLog::Error(LABEL, "AllocateMemory end");
    uint8_t *dstBuffer = static_cast<uint8_t *>(context.pixelsBuffer.buffer);
    SkCodec::Result ret = codec_->getPixels(dstInfo, dstBuffer, rowBytes);
    if (ret != SkCodec::kSuccess) {
        HiLog::Error(LABEL, "Decode failed, get pixels failed, ret=%{public}d", ret);
        state_ = RawDecodingState::IMAGE_ERROR;
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    state_ = RawDecodingState::IMAGE_DECODED;
    HiLog::Error(LABEL, "Decode end");
    return SUCCESS;
}

uint32_t RawDecoder::PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context)
{
    // currently not support increment decode
    return ERR_IMAGE_DATA_UNSUPPORT;
}

bool RawDecoder::DecodeHeader()
{
    HiLog::Error(LABEL, "DecodeHeader begin");
    codec_ = SkCodec::MakeFromStream(make_unique<RawStream>(stream_));
    if (codec_ == nullptr) {
        HiLog::Error(LABEL, "create codec from stream failed");
        return false;
    }
    info_ = codec_->getInfo();
    HiLog::Error(LABEL, "DecodeHeader end");
    return true;
}

string RawDecoder::ConvertEncodedFormatToString(SkEncodedImageFormat format)
{
    string mimeType;
    switch (format) {
        case SkEncodedImageFormat::kJPEG:
            mimeType = "image/jpeg";
            break;
        case SkEncodedImageFormat::kDNG:
            mimeType = "image/x-adobe-dng";
            break;
        default:
            mimeType = "";
            break;
    }
    return mimeType;
}

uint32_t RawDecoder::GetImagePropertyString(uint32_t index, const std::string &key, std::string &value)
{
    HiLog::Info(LABEL, "[GetImagePropertyString] enter raw plugin, key:%{public}s", key.c_str());
    uint32_t errorCode = SUCCESS;

    if (state_ < RawDecodingState::SOURCE_INITED) {
        HiLog::Error(LABEL, "GetImageSize failed, invalid state:%{public}d", state_);
        return ERR_MEDIA_INVALID_OPERATION;
    }

    if (state_ < RawDecodingState::BASE_INFO_PARSED) {
        if (!DecodeHeader()) {
            HiLog::Error(LABEL, "GetImagePropertyString failed, decode header failed, state=%{public}d", state_);
            return ERR_IMAGE_DECODE_HEAD_ABNORMAL;
        }
        state_ = RawDecodingState::BASE_INFO_PARSED;
    }

    if (key == ACTUAL_IMAGE_ENCODED_FORMAT) {
        SkEncodedImageFormat format = codec_->getEncodedFormat();
        HiLog::Info(LABEL, "getEncodedFormat format %{public}d", format);
        value = ConvertEncodedFormatToString(format);
        if (value == "") {
            HiLog::Error(LABEL, "getEncodedFormat error format %{public}d", format);
            return ERR_IMAGE_DATA_UNSUPPORT;
        }
    } else {
        HiLog::Error(LABEL, "[GetImagePropertyString]key(%{public}s) not supported", key.c_str());
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    return errorCode;
}

PlAlphaType RawDecoder::ConvertToAlphaType(SkAlphaType alphaType)
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

SkColorType RawDecoder::ConvertToColorType(PlPixelFormat format, PlPixelFormat &outputFormat)
{
    switch (format) {
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
    HiLog::Debug(LABEL, "unsupport convert to format:%{public}d, set default RGBA", format);
    outputFormat = PlPixelFormat::RGBA_8888;
    return kRGBA_8888_SkColorType;
}
} // namespace ImagePlugin
} // namespace OHOS
