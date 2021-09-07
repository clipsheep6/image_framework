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

#include "hw_jpeg_decompressor.h"
#include <sys/mman.h>
#include <unistd.h>
#include "jerror.h"
#include "media_errors.h"
#include "securec.h"

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace MultimediaPlugin;
using namespace Media;

constexpr uint32_t JPEG_SOURCE_SIZE_MAX = 8192;
constexpr uint32_t JPEG_SOURCE_SIZE_MIN = 512;

HwJpegDecompressor::HwJpegDecompressor()
{
    hwDecompressor_ = HwJpegDecompressorWrapper::CreateHwJpegDecompressor();
}

bool HwJpegDecompressor::CheckOriginalImageSize(uint32_t srcWidth, uint32_t srcHeight, uint32_t comps)
{
    // check origal image size.
    if (srcWidth > JPEG_SOURCE_SIZE_MAX || srcWidth < JPEG_SOURCE_SIZE_MIN || srcHeight > JPEG_SOURCE_SIZE_MAX ||
        srcHeight < JPEG_SOURCE_SIZE_MIN) {
        HiLog::Error(LABEL, "image size:[%{public}u, %{public}u] out of hardware range:(%{public}u, %{public}u).",
                     srcWidth, srcHeight, JPEG_SOURCE_SIZE_MIN, JPEG_SOURCE_SIZE_MAX);
        return false;
    }
    // check row bytes is aligned with 128 bits(16 Bytes).
    uint64_t imageRowBytes = static_cast<uint64_t>(srcWidth) * comps;
    if (imageRowBytes & 0xF) {
        HiLog::Error(LABEL, "row bytes is not aligned with 16 bytes, row bytes=%{public}llu.",
                     static_cast<unsigned long long>(imageRowBytes));
        return false;
    }
    return true;
}

bool HwJpegDecompressor::CheckOutputImageSize(uint32_t outWidth, uint32_t comps)
{
    // check output row bytes is aligned with 16 Bytes.
    uint64_t outputRowBytes = static_cast<uint64_t>(outWidth) * comps;
    if (outputRowBytes & 0xF) {
        HiLog::Error(LABEL, "output row bytes is not aligned with 16 bytes, row bytes=%{public}llu.",
                     static_cast<unsigned long long>(outputRowBytes));
        return false;
    }
    return true;
}

uint8_t *HwJpegDecompressor::GetInputBuffer(size_t &bufferSize)
{
    if (stream_ == nullptr || hwDecompressor_ == nullptr) {
        HiLog::Error(LABEL, "get input buffer failed, stream is null.");
        return nullptr;
    }
    bufferSize = stream_->GetStreamSize();
    if (bufferSize == 0) {
        HiLog::Error(LABEL, "get input buffer failed, stream size is zero.");
        return nullptr;
    }
    HiLog::Debug(LABEL, "get stream size=%{public}zu.", bufferSize);
    uint32_t positionRecord = stream_->Tell();
    uint8_t *inBufMapAddr = hwDecompressor_->GetInputBuffer(bufferSize);
    if (inBufMapAddr == nullptr) {
        HiLog::Error(LABEL, "get input buffer failed, alloc buffer error.");
        return nullptr;
    }
    stream_->Seek(0);
    uint32_t readSize = 0;
    stream_->Read(bufferSize, inBufMapAddr, bufferSize, readSize);
    stream_->Seek(positionRecord);
    return inBufMapAddr;
}

uint32_t HwJpegDecompressor::DoDecompress(DecodeContext &context)
{
    if (hwDecompressor_ == nullptr) {
        HiLog::Error(LABEL, "do decompress error, hardware decoder is null.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    // check jpeg_decompress_struct is ok for hardware support.
    if (!hwDecompressor_->CheckHardwareSupport(decodeInfo_)) {
        HiLog::Error(LABEL, "hardware jpeg decoder cannot support.");
        return ERR_IMAGE_HW_DECODE_UNSUPPORT;
    }
    // file input ion buffer.
    size_t bufferSize = 0;
    uint8_t *inputDataAddr = GetInputBuffer(bufferSize);
    if (!hwDecompressor_->HardwareDecode(decodeInfo_, inputDataAddr, bufferSize, context)) {
        HiLog::Error(LABEL, "hardware decoder decode image failed.");
        hwDecompressor_->ReleaseInputBuffer(inputDataAddr);
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    hwDecompressor_->ReleaseInputBuffer(inputDataAddr);
    return SUCCESS;
}

uint32_t HwJpegDecompressor::Decompress(void *decompressInfo, InputDataStream *stream, DecodeContext &context)
{
    if (decompressInfo == nullptr || stream == nullptr) {
        HiLog::Error(LABEL, "jpeg hardware decode error, input parameter is null.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    decodeInfo_ = static_cast<jpeg_decompress_struct *>(decompressInfo);
    stream_ = stream;
    uint32_t srcWidth = decodeInfo_->image_width;
    uint32_t srcHeight = decodeInfo_->image_height;
    uint32_t srcComps = decodeInfo_->num_components;
    if (!CheckOriginalImageSize(srcWidth, srcHeight, srcComps)) {
        HiLog::Error(LABEL, "check original image size failed, hardware jpeg decoder cannot support.");
        return ERR_IMAGE_HW_DECODE_UNSUPPORT;
    }
    uint32_t outWidth = decodeInfo_->output_width;
    uint32_t outComps = decodeInfo_->output_components;
    if (!CheckOutputImageSize(outWidth, outComps)) {
        HiLog::Error(LABEL, "check output image size failed, hardware jpeg decoder cannot support.");
        return ERR_IMAGE_HW_DECODE_UNSUPPORT;
    }
    if (hwDecompressor_ == nullptr) {
        HiLog::Error(LABEL, "hardware jpeg decoder is null.");
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    // get hardware service
    if (!hwDecompressor_->LockDevice()) {
        HiLog::Error(LABEL, "lock hardware device failed!");
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    uint32_t ret = DoDecompress(context);
    hwDecompressor_->UnlockDevice();
    return ret;
}
} // namespace ImagePlugin
} // namespace OHOS