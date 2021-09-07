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

#include "hw_jpeg_decompressor_adapter.h"

#include <sys/mman.h>
#include <unistd.h>
#include "jerror.h"
#include "media_errors.h"
#include "securec.h"

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
using namespace ImagePlugin;
using namespace MultimediaPlugin;

constexpr int32_t JPEG_INFO_BITS_LENGTH = 17;
constexpr int32_t JPEG_INFO_HUFFVAL_LENGTH = 256;

constexpr uint64_t JPEG_INFO_SAMPLE_111111 = 0x010101010101;
constexpr uint64_t JPEG_INFO_SAMPLE_121111 = 0x010201010101;
constexpr uint64_t JPEG_INFO_SAMPLE_211111 = 0x020101010101;
constexpr uint64_t JPEG_INFO_SAMPLE_221111 = 0x020201010101;
constexpr uint8_t BYTE_COUNT = 8;
constexpr uint8_t JPEG_COMPNENT_NUM_ALPHA = 1;
constexpr uint8_t JPEG_COMPNENT_NUM_RGB = 3;
constexpr uint8_t NEXT_POS_NUM_TWO = 2;

bool HwJpegDecompressorAdapter::HardwareDecode(const jpeg_decompress_struct *jpgDinfo, const uint8_t *inputData,
                                               size_t bufferSize, DecodeContext &context)
{
    if (jpgDinfo == nullptr || inputData == nullptr || inBufHnd_ == nullptr) {
        HiLog::Error(LABEL, "hardware decompress error, input parameter is null.");
        return false;
    }
    // find the compress data start position.
    size_t compressPos = GetCompressPos(inputData, bufferSize);
    if (compressPos == 0) {
        HiLog::Error(LABEL, "get compress data addr abnormal, compressPos:%{public}zu.", compressPos);
        return false;
    }
    // get decode info for HAL from libjpeg struct.
    jpeg_decompress_hidl_t decInfo;
    if (!GetDecodeInfo(jpgDinfo, &decInfo)) {
        HiLog::Error(LABEL, "get HAL decode info failed.");
        return false;
    }
    // do hardware decode.
    hwdecode_region_info regionInfo;
    errno_t ret = memset_s(&regionInfo, sizeof(regionInfo), 0, sizeof(regionInfo));
    if (ret != EOK) {
        HiLog::Error(LABEL, "region info memset failed, error_type: %{public}d.", ret);
        return false;
    }
    // alloc output buffer.
    native_handle_t *outBufHnd = nullptr;
    uint8_t *outputData = GetOutputBuffer(jpgDinfo, outBufHnd);
    if (outputData == nullptr) {
        HiLog::Error(LABEL, "get output buffer failed, alloc buffer error.");
        return false;
    }
    uint32_t sampleSize = 1;  // default sample size.
    if (jpegDecode_->DoDecode(decInfo, android::hardware::hidl_handle(outBufHnd),
        android::hardware::hidl_handle(inBufHnd_), regionInfo, sampleSize,
        compressPos) != JPEG_Error::NONE) {
        HiLog::Error(LABEL, "hardware jpeg decoder decompress image failed.");
        ReleaseBuffer(outputData, outBufHnd, static_cast<JpegOutInfo *>(outBufHnd)->alignSize);
        return false;
    }
    context.pixelsBuffer.buffer = outputData;
    context.pixelsBuffer.context = outBufHnd;
    context.pixelsBuffer.bufferSize = static_cast<JpegOutInfo *>(outBufHnd)->alignSize;
    context.allocatorType = AllocatorType::CUSTOM_ALLOC;
    context.freeFunc = ReleaseBuffer;
    return true;
}

int32_t HwJpegDecompressorAdapter::ComputeDecodeOutColorFormat(uint32_t colorFormat)
{
    // note:the third-party RGBA correspond hisi BGRA
    int32_t outputColorFormat = HISI_JPEG_DECODE_OUT_UNKNOWN;
    switch (colorFormat) {
        case JCS_EXT_RGBA: {
            outputColorFormat = HISI_JPEG_DECODE_OUT_BGRA8888;
            break;
        }
        case JCS_RGB565: {
            outputColorFormat = HISI_JPEG_DECODE_OUT_RGB565;
            break;
        }
        case JCS_EXT_BGRA: {
            outputColorFormat = HISI_JPEG_DECODE_OUT_RGBA8888;
            break;
        }
        default: {
            HiLog::Error(LABEL, "libjpeg decode format:[%{public}d] is unsupported!", colorFormat);
            break;
        }
    }
    return outputColorFormat;
}

uint8_t *HwJpegDecompressorAdapter::GetOutputBuffer(const jpeg_decompress_struct *cinfo, native_handle_t *&handle)
{
    if (cinfo == nullptr) {
        HiLog::Error(LABEL, "get output buffer failed, jpeg decompress info is null!");
        return nullptr;
    }
    uint32_t width = cinfo->output_width;
    uint32_t height = cinfo->output_height;
    uint32_t colorFormat = cinfo->out_color_space;
    JpegOutInfo *outInfo = nullptr;
    void *outBufMapAddr = nullptr;
    int32_t outputColorFormat = ComputeDecodeOutColorFormat(colorFormat);
    jpegDecode_->Alloc_OutBuffer(width, height, outputColorFormat, [&](const auto &allocrlt, const auto &buffer) {
        if (allocrlt == JPEG_Error::NONE) {
            handle = native_handle_clone(((android::hardware::hidl_handle)buffer).getNativeHandle());
        }
    });
    if (handle == nullptr) {
        HiLog::Error(LABEL, "jpeg hal failed to alloc output buffer.");
        return nullptr;
    }
    outInfo = static_cast<JpegOutInfo *>(handle);
    outBufMapAddr = mmap(nullptr, outInfo->alignSize, PROT_READ | PROT_WRITE, MAP_SHARED, outInfo->data[0], 0);
    if (outBufMapAddr == MAP_FAILED) {
        HiLog::Error(LABEL, "output buffer map abnormal.");
        ReleaseBuffer(nullptr, handle, 0);
        return nullptr;
    }
    return static_cast<uint8_t *>(outBufMapAddr);
}

void HwJpegDecompressorAdapter::ReleaseBuffer(void *addr, void *context, uint32_t size)
{
    native_handle_t *handle = static_cast<native_handle_t *>(context);
    if (addr != nullptr) {
        munmap(addr, size);
    }
    if (handle != nullptr) {
        native_handle_close(handle);
        native_handle_delete(handle);
    }
}

void HwJpegDecompressorAdapter::ReleaseInputBuffer(uint8_t *addr)
{
    uint32_t inBufSize = 0;
    if (inBufHnd_ != nullptr) {
        inBufSize = static_cast<JpegInInfo *>(inBufHnd_)->alignSize;
    }
    ReleaseBuffer(addr, inBufHnd_, inBufSize);
    inBufHnd_ = nullptr;
}

bool HwJpegDecompressorAdapter::LockDevice()
{
    jpegDecode_ = IJpegDecode::tryGetService();
    if (jpegDecode_.get() == nullptr) {
        HiLog::Error(LABEL, "get hardware jpeg service failed.");
        return false;
    }
    if (jpegDecode_->LockDevice() != JPEG_Error::NONE) {
        HiLog::Error(LABEL, "lock hardware device failed.");
        return false;
    }
    return true;
}

void HwJpegDecompressorAdapter::UnlockDevice()
{
    if (jpegDecode_.get() == nullptr) {
        return;
    }
    jpegDecode_->UnLockDevice();
}

bool HwJpegDecompressorAdapter::FillHuffTable(const JHUFF_TBL *inHuffTable, jpeg_huff_hidl_tbl &outHuffTable)
{
    errno_t ret = EOK;
    if (inHuffTable != nullptr) {
        outHuffTable.table_flag = true;
        ret = memcpy_s(&outHuffTable.bits[0], JPEG_INFO_BITS_LENGTH, &inHuffTable->bits[0], JPEG_INFO_BITS_LENGTH);
        if (ret != EOK) {
            HiLog::Error(LABEL, "huff table memcpy failed, error_type: %{public}d.", ret);
            return false;
        }
        ret = memcpy_s(&outHuffTable.huffval[0], JPEG_INFO_HUFFVAL_LENGTH, &inHuffTable->huffval[0],
                       JPEG_INFO_HUFFVAL_LENGTH);
        if (ret != EOK) {
            HiLog::Error(LABEL, "huff value memcpy failed, error_type: %{public}d.", ret);
            return false;
        }
    } else {
        ret = memset_s(&outHuffTable, sizeof(outHuffTable), 0, sizeof(outHuffTable));
        if (ret != EOK) {
            HiLog::Error(LABEL, "huff table memset failed, error_type: %{public}d.", ret);
            return false;
        }
    }
    return true;
}

bool HwJpegDecompressorAdapter::FillQuantTable(const JQUANT_TBL *inQuantTable, jpeg_quant_hidl_tbl &outQuantTable)
{
    errno_t ret = EOK;
    if (inQuantTable != nullptr) {
        outQuantTable.table_flag = true;
        ret = memcpy_s(&outQuantTable.quantval[0], DCTSIZE2 * sizeof(uint16_t), &inQuantTable->quantval[0],
                       DCTSIZE2 * sizeof(uint16_t));
        if (ret != EOK) {
            HiLog::Error(LABEL, "quant table memcpy failed, error_type: %{public}d.", ret);
            return false;
        }
    } else {
        ret = memset_s(&outQuantTable, sizeof(outQuantTable), 0, sizeof(outQuantTable));
        if (ret != EOK) {
            HiLog::Error(LABEL, "quant table memset failed, error_type: %{public}d.", ret);
            return false;
        }
    }
    return true;
}

bool HwJpegDecompressorAdapter::FillComponentInfo(const jpeg_decompress_struct *jpgDinfo,
                                                  jpeg_decompress_hidl_t *decInfo)
{
    if (decInfo == nullptr || jpgDinfo == nullptr) {
        HiLog::Error(LABEL, "fill component info error, input parameter is null.");
        return false;
    }
    decInfo->comp_info.resize(jpgDinfo->num_components);
    for (int32_t i = 0; i < jpgDinfo->num_components; i++) {
        jpeg_comp_hidl_info compInfo;
        compInfo.h_samp_factor = jpgDinfo->comp_info[i].h_samp_factor;
        compInfo.v_samp_factor = jpgDinfo->comp_info[i].v_samp_factor;
        compInfo.quant_tbl_no = jpgDinfo->comp_info[i].quant_tbl_no;
        compInfo.component_index = jpgDinfo->comp_info[i].component_index;
        compInfo.component_id = jpgDinfo->comp_info[i].component_id;
        compInfo.dc_tbl_no = jpgDinfo->comp_info[i].dc_tbl_no;
        compInfo.ac_tbl_no = jpgDinfo->comp_info[i].ac_tbl_no;
        compInfo.info_flag = true;
        decInfo->comp_info[i] = compInfo;
    }
    return true;
}

bool HwJpegDecompressorAdapter::GetDecodeInfo(const jpeg_decompress_struct *jpgDinfo, jpeg_decompress_hidl_t *decInfo)
{
    if (decInfo == nullptr || jpgDinfo == nullptr) {
        HiLog::Error(LABEL, "get decode info input parameter is null.");
        return false;
    }
    decInfo->num_components = jpgDinfo->num_components;
    decInfo->arith_code = jpgDinfo->arith_code;
    decInfo->data_precision = jpgDinfo->data_precision;
    decInfo->image_height = jpgDinfo->image_height;
    decInfo->image_width = jpgDinfo->image_width;
    decInfo->progressive_mode = jpgDinfo->progressive_mode;
    decInfo->restart_interval = jpgDinfo->restart_interval;
    if (!FillComponentInfo(jpgDinfo, decInfo)) {
        HiLog::Error(LABEL, "fill component info failed.");
        return false;
    }
    decInfo->dc_huff_tbl_ptrs.resize(NUM_HUFF_TBLS);
    decInfo->ac_huff_tbl_ptrs.resize(NUM_HUFF_TBLS);
    for (int32_t i = 0; i < NUM_HUFF_TBLS; i++) {
        jpeg_huff_hidl_tbl huffTable;
        if (!FillHuffTable(jpgDinfo->dc_huff_tbl_ptrs[i], huffTable)) {
            HiLog::Error(LABEL, "fill dc huff table failed.");
            return false;
        }
        decInfo->dc_huff_tbl_ptrs[i] = huffTable;
        if (!FillHuffTable(jpgDinfo->ac_huff_tbl_ptrs[i], huffTable)) {
            HiLog::Error(LABEL, "fill ac huff table failed.");
            return false;
        }
        decInfo->ac_huff_tbl_ptrs[i] = huffTable;
    }
    decInfo->quant_tbl_ptrs.resize(NUM_QUANT_TBLS);
    for (int32_t i = 0; i < NUM_QUANT_TBLS; i++) {
        jpeg_quant_hidl_tbl quantTable;
        if (!FillQuantTable(jpgDinfo->quant_tbl_ptrs[i], quantTable)) {
            HiLog::Error(LABEL, "fill quant table failed.");
            return false;
        }
        decInfo->quant_tbl_ptrs[i] = quantTable;
    }
    return true;
}

bool HwJpegDecompressorAdapter::CheckHardwareSupport(const jpeg_decompress_struct *cinfo)
{
    if (cinfo == nullptr) {
        HiLog::Error(LABEL, "check hardware support failed, input parameter error!");
        return false;
    }
    if (((cinfo->num_components != JPEG_COMPNENT_NUM_ALPHA) && (cinfo->num_components != JPEG_COMPNENT_NUM_RGB)) ||
        (cinfo->arith_code) || (cinfo->progressive_mode) || (cinfo->data_precision != BYTE_COUNT)) {
        HiLog::Error(LABEL,
                     "hardware decoder cannot handle num_components:%{public}d, progressive_mode:%{public}d, "
                     "data_precision:%{public}d.",
                     cinfo->num_components, cinfo->progressive_mode, cinfo->data_precision);
        return false;
    }
    if (cinfo->comp_info == nullptr) {
        HiLog::Error(LABEL, "jpeg decompress info factor is null.");
        return false;
    }
    // JPEG hardware decode only support 111111 121111 211111 221111 and xx0000
    // h_samp_factor/v_samp_factor in range (0...255)
    uint64_t sampleValue = 0;
    for (int32_t i = 0; i < cinfo->num_components; i++) {
        sampleValue = sampleValue << BYTE_COUNT;
        sampleValue |= (static_cast<uint32_t>(cinfo->comp_info[i].h_samp_factor) & 0xFF);
        sampleValue = sampleValue << BYTE_COUNT;
        sampleValue |= (static_cast<uint32_t>(cinfo->comp_info[i].v_samp_factor) & 0xFF);
    }
    switch (sampleValue) {
        case JPEG_INFO_SAMPLE_111111:
        case JPEG_INFO_SAMPLE_121111:
        case JPEG_INFO_SAMPLE_211111:
        case JPEG_INFO_SAMPLE_221111:
            return true;
        default: {
            if ((sampleValue & 0xFFFFFFFF) == 0) {
                return true;
            }
            break;
        }
    }
    return false;
}

size_t HwJpegDecompressorAdapter::GetCompressPos(const uint8_t *inBufMapAddr, size_t bufferSize)
{
    const uint8_t *curr = inBufMapAddr;
    size_t lh = 0;
    size_t ll = 0;
    size_t pos = 0;
    size_t ret = 0;
    while (bufferSize > (pos + NEXT_POS_NUM_TWO) && *curr++ == 0xff) {
        uint8_t value = *curr++;
        pos += NEXT_POS_NUM_TWO;
        if (value == 0xD8 || value == 0xD9) {
            continue;
        }
        if ((pos + NEXT_POS_NUM_TWO) > bufferSize) {
            HiLog::Warn(LABEL, "next pos %{public}zu may be large than bufferSize %{public}zu", pos, bufferSize);
            break;
        }
        lh = *curr;
        ll = *(curr + 1);
        curr += (lh << BYTE_COUNT) + ll;
        pos = curr - inBufMapAddr;
        HiLog::Debug(LABEL, "pos %{public}zu, lh = %{public}zx, ll = %{public}zx ", pos, lh, ll);
        if (value == 0xDA) {
            ret = pos;
            break;
        }
    }
    return ret;
}

uint8_t *HwJpegDecompressorAdapter::GetInputBuffer(size_t bufferSize)
{
    void *inBufMapAddr = nullptr;
    jpegDecode_->Alloc_InBuffer(bufferSize, [&](const auto &tmpError, const auto &tmpbuffer) {
        if (tmpError == JPEG_Error::NONE) {
            inBufHnd_ = native_handle_clone(((android::hardware::hidl_handle)tmpbuffer).getNativeHandle());
        }
    });
    if (inBufHnd_ == nullptr) {
        HiLog::Error(LABEL, "alloc input buffer failed.");
        return nullptr;
    }
    JpegInInfo *inInfo = static_cast<JpegInInfo *>(inBufHnd_);
    inBufMapAddr = mmap(nullptr, inInfo->alignSize, PROT_READ | PROT_WRITE, MAP_SHARED, inInfo->data[0], 0);
    if (inBufMapAddr == MAP_FAILED) {
        ReleaseInputBuffer(nullptr);
        return nullptr;
    }
    return static_cast<uint8_t *>(inBufMapAddr);
}
} // namespace Media
} // namespace OHOS