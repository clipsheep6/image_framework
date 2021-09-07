/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2009 The Android Open Source Project
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

#ifndef HW_JPEG_DECOMPRESSOR_ADAPTER_H
#define HW_JPEG_DECOMPRESSOR_ADAPTER_H

#include <cstdint>
#include <string>
#include "abs_image_decoder.h"
#include "cutils/native_handle.h"
#include "hilog/log.h"
#include "hw_jpeg_decompressor_interface.h"
#include "jerror.h"
#include "jpeglib.h"
#include "log_tags.h"
#include "vendor/huawei/hardware/jpegdec/1.0/IJpegDecode.h"

namespace OHOS {
namespace Media {
constexpr int32_t HISI_JPEG_DECODE_OUT_UNKNOWN = -1;
constexpr int32_t HISI_JPEG_DECODE_OUT_RGB565 = 7;
constexpr int32_t HISI_JPEG_DECODE_OUT_RGBA8888 = 9;
constexpr int32_t HISI_JPEG_DECODE_OUT_BGRA8888 = 10;

using ::android::hardware::hidl_vec;
using vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info;
using vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode;
using vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info;
using vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t;
using vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error;
using vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl;
using vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl;

// use for message to HAL.
struct JpegOutInfo : public native_handle {
    int32_t sharedFd;
    int32_t outWidth;
    int32_t outHeight;
    int32_t byteStride;
    int32_t size;
    int32_t alignSize;
    int32_t outHnd;
    int32_t colorFormat;
    uint64_t outBufferAddr;
};

struct JpegInInfo : public native_handle {
    int32_t sharedFd;
    int32_t dataSize;
    int32_t alignSize;
    int32_t ionHnd;
    uint64_t inBufferAddr;
};

class HwJpegDecompressorAdapter : public ImagePlugin::HwJpegDecompressorInterface {
public:
    HwJpegDecompressorAdapter() = default;
    ~HwJpegDecompressorAdapter() = default;
    bool LockDevice() override;
    void UnlockDevice() override;
    bool CheckHardwareSupport(const jpeg_decompress_struct *cinfo) override;
    uint8_t *GetInputBuffer(size_t bufferSize) override;
    bool HardwareDecode(const jpeg_decompress_struct *jpgDinfo, const uint8_t *inputData, size_t bufferSize,
                        ImagePlugin::DecodeContext &context) override;
    void ReleaseInputBuffer(uint8_t *addr) override;

private:
    DISALLOW_COPY_AND_MOVE(HwJpegDecompressorAdapter);
    size_t GetCompressPos(const uint8_t *inBufMapAddr, size_t buffersize);
    bool GetDecodeInfo(const jpeg_decompress_struct *jpgDinfo, jpeg_decompress_hidl_t *decInfo);
    bool FillHuffTable(const JHUFF_TBL *inHuffTable, jpeg_huff_hidl_tbl &outHuffTable);
    bool FillQuantTable(const JQUANT_TBL *inQuantTable, jpeg_quant_hidl_tbl &outQuantTable);
    bool FillComponentInfo(const jpeg_decompress_struct *jpgDinfo, jpeg_decompress_hidl_t *decInfo);
    uint8_t *GetOutputBuffer(const jpeg_decompress_struct *cinfo, native_handle_t *&handle);
    static void ReleaseBuffer(void *addr, void *context, uint32_t size);
    int32_t ComputeDecodeOutColorFormat(uint32_t colorFormat);
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
        LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "HwJpegDecompressorAdapter"
    };
    android::sp<IJpegDecode> jpegDecode_ = nullptr;
    native_handle_t *inBufHnd_ = nullptr;
};
} // namespace Media
} // namespace OHOS

#endif // HW_JPEG_DECOMPRESSOR_ADAPTER_H