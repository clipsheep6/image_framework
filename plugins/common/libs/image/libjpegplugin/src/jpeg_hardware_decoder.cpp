/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "jpeg_hardware_decoder.h"
#include "ashmem.h"

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace OHOS::HDI::Display::Buffer::V1_0;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "JpegHardwareDecoder" };
static std::shared_ptr<JpegHardwareDecoder> decoder;
JpegHardwareDecoder::JpegHardwareDecoder()
{
    hdiDecoder_ = ICodecImageJpeg::Get();
    hdiBuffer_ = IDisplayBuffer::Get();
    HiLog::Error(LABEL, "JpegHardwareDecoder start.");
}

JpegHardwareDecoder::~JpegHardwareDecoder()
{
    hdiDecoder_ = nullptr;
    hdiBuffer_ = nullptr;
    callback_ = nullptr;
    HiLog::Error(LABEL, "JpegHardwareDecoder end.");
}

////////////////////////////////////////////////////////////////////////

int32_t JpegHardwareDecoder::JpegAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t fd)
{
    HDF_LOGI("enter");
    int32_t curPos = 0;
    // SOI
    curPos = PutInt16(buffer, curPos, 0xffd8);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOI error");
        return -1;
    }

    // DQT
    curPos = JpegDqtAssemble(decInfo, buffer, curPos);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble DQT error");
        return -1;
    }

    // DHT
    curPos = JpegDhtAssemble(decInfo, buffer, curPos);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble DHT error");
        return -1;
    }
    // DRI
    curPos = JpegDriAssemble(decInfo, buffer, curPos);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble DRI error");
        return -1;
    }

    // SOF
    curPos = JpegSofAssemble(decInfo, buffer, curPos);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOF error");
        return -1;
    }
    // SOS
    curPos = JpegSosAssemble(decInfo, buffer, curPos);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOS error");
        return -1;
    }
    // DATA
    curPos = JpegDataAssemble(buffer, curPos, fd);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble CompressedData error");
        return -1;
    }
    // EOI
    curPos = PutInt16(buffer, curPos, 0xffd9);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble EOI error");
        return -1;
    }
    return curPos;
}

bool JpegHardwareDecoder::DessambleJpeg(int8_t *buffer, size_t bufferLen, struct CodecJpegDecInfo &decInfo,
                                    std::unique_ptr<int8_t[]> &compressBuffer, uint32_t &comBufLen)
{
    HDF_LOGI("enter");
    int8_t *start = buffer;
    const int8_t *end = buffer + bufferLen;
    while (start < end) {
        JpegMarker marker = (JpegMarker)FindMarker(start);
        start += 2;  // 2: marker len
        switch (marker) {
            case SOI:
            case EOI:
                break;
            case SOF0:
                start += DessambleSof(start, decInfo);
                break;
            case DHT:
                start += DessambleDht(start, decInfo);
                break;
            case SOS: {
                start += DessambleSos(start, decInfo);
                // compressed data start
                auto len = DessambleCompressData(start, compressBuffer, comBufLen);
                if (len < 0) {
                    HiLog::Error(LABEL, "copy compressed data error");
                    return false;
                }
                start += len;
                break;
            }

            case DQT:
                start += DessambleDqt(start, decInfo);
                break;
            case DRI: {
                start += 2;  // 2: marker len
                decInfo.restartInterval = GetInt16(start);
                start += 2;  // 2: interval len
                break;
            }
            default: {
                short len = GetInt16(start);
                start += len;
                HDF_LOGW("skip marker[%{public}x], len[%{public}d]", marker, len);
                break;
            }
        }
    }
    return true;
}

int32_t JpegHardwareDecoder::JpegDqtAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos)
{
    HDF_LOGI("enter. curPos = %{public}d", curPos);
    // flag
    curPos = PutInt16(buffer, curPos, 0xffdb);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble DQT flag error");
        return curPos;
    }

    // skip len first
    int32_t lenPos = curPos;
    curPos += 2;  // 2: marker len

    // data
    for (size_t i = 0; i < decInfo.quantTbl.size(); i++) {
        if (!decInfo.quantTbl[i].tableFlag) {
            break;
        }
        uint8_t index = 0;         // precision 1:16bit, 0： 8bit, deault:1
        index = (index << 4) | i;  // precision << 4 | tableid
        curPos = PutInt8(buffer, curPos, index);
        if (curPos < 0) {
            HiLog::Error(LABEL, "assemble precision and tableid error");
            return curPos;
        }

        for (size_t j = 0; j < decInfo.quantTbl[i].quantVal.size(); j++) {
            HDF_LOGI("decInfo.quantTbl[%{public}zu].quantVal[%{public}zu] = %{public}d", i, j,
                decInfo.quantTbl[i].quantVal[j]);
            curPos = PutInt16(buffer, curPos, decInfo.quantTbl[i].quantVal[j]);
        }
    }
    int16_t bufferLen = static_cast<int16_t>(curPos - lenPos);
    auto ret = PutInt16(buffer, lenPos, bufferLen);
    if (ret < 0) {
        HiLog::Error(LABEL, "assemble len error");
        return ret;
    }
    return curPos;
}
int32_t JpegHardwareDecoder::JpegDriAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos)
{
    HDF_LOGI("enter, restartInterval = %{public}d curPos = %{public}d", decInfo.restartInterval, curPos);
    if (decInfo.restartInterval <= 0) {
        return curPos;
    }
    curPos = PutInt16(buffer, curPos, 0xffdd);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble DRI flag error");
        return curPos;
    }

    curPos = PutInt16(buffer, curPos, 4);  // 4: dri data len( marker len included)
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble DRI len error");
        return curPos;
    }

    curPos = PutInt16(buffer, curPos, decInfo.restartInterval);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble dri value error");
        return curPos;
    }
    return curPos;
}

int32_t JpegHardwareDecoder::JpegDhtAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos)
{
    HDF_LOGI("enter. curPos = %{public}d", curPos);
    curPos = JpegDhtAssemble(decInfo.dcHuffTbl, buffer, curPos);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble dc hufman error");
        return curPos;
    }

    curPos = JpegDhtAssemble(decInfo.acHuffTbl, buffer, curPos, false);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble ac hufman error");
    }
    return curPos;
}

int32_t JpegHardwareDecoder::JpegDhtAssemble(const std::vector<CodecJpegHuffTable> &table, int8_t *buffer, int32_t curPos,
                                         bool dc)
{
    HDF_LOGI("enter. curPos = %{public}d", curPos);
    // DC Hufman
    curPos = PutInt16(buffer, curPos, 0xffc4);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble hufman flag error");
        return curPos;
    }
    // skip len
    int32_t lenPos = curPos;
    curPos += 2;  // 2: marker len
    for (size_t i = 0; i < table.size(); i++) {
        if (!table[i].tableFlag) {
            break;
        }
        uint8_t type = 0;  // type  0:DC, 1:AC
        if (!dc) {
            type = 1;
        }
        type = (type << 4) | i;  // type << 4 | tableid
        curPos = PutInt8(buffer, curPos, type);
        if (curPos < 0) {
            HiLog::Error(LABEL, "assemble tableid and dc/ac error");
            return curPos;
        }
        // bits
        auto ret = memcpy_s(buffer + curPos, table[i].bits.size(), table[i].bits.data(), table[i].bits.size());
        if (ret != EOK) {
            char buf[MAX_BUFFER_LEN] = {0};
            strerror_r(errno, buf, sizeof(buf));
            HiLog::Error(LABEL, "assemble bits error ret = %{public}s", buf);
            return ret;
        }
        curPos += table[i].bits.size();
        // val
        ret = memcpy_s(buffer + curPos, table[i].huffVal.size(), table[i].huffVal.data(), table[i].huffVal.size());
        if (ret != EOK) {
            HiLog::Error(LABEL, "assemble huffVal error, ret = %{public}d", ret);
            return ret;
        }
        curPos += table[i].huffVal.size();
    }
    int16_t bufferLen = static_cast<int16_t>(curPos - lenPos);
    auto ret = PutInt16(buffer, lenPos, bufferLen);
    if (ret < 0) {
        HiLog::Error(LABEL, "assemble len error");
        return ret;
    }
    return curPos;
}

int32_t JpegHardwareDecoder::JpegSofAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos)
{
    HDF_LOGI("enter. curPos = %{public}d", curPos);
    // flag
    curPos = PutInt16(buffer, curPos, 0xffc0);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOF flag error");
        return curPos;
    }

    int16_t len = decInfo.numComponents * 3 + 8; // * rgb channel + other data
    curPos = PutInt16(buffer, curPos, len);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOF len error");
        return curPos;
    }

    int8_t precision = decInfo.dataPrecision & 0xFF;
    curPos = PutInt8(buffer, curPos, precision);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOF precision error");
        return curPos;
    }

    // width
    int16_t width = decInfo.imageHeight & 0xFFFF;
    curPos = PutInt16(buffer, curPos, width);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOF width error");
        return curPos;
    }

    //  height
    int16_t height = decInfo.imageWidth & 0xFFFF;
    curPos = PutInt16(buffer, curPos, height);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOF width error");
        return curPos;
    }
    // components
    int8_t components = decInfo.numComponents & 0xFF;
    curPos = PutInt8(buffer, curPos, components);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOF components error");
        return curPos;
    }
    for (size_t i = 0; i < decInfo.compInfo.size(); i++) {
        int8_t componentId = decInfo.compInfo[i].componentId;
        // byte offset
        int8_t sampFactor = ((decInfo.compInfo[i].hSampFactor & 0xFF) << 4) | (decInfo.compInfo[i].vSampFactor & 0xFF);
        int8_t quantity = decInfo.compInfo[i].quantTableNo;
        int8_t bufferValue[] = {componentId, sampFactor, quantity};
        auto ret = memcpy_s(buffer + curPos, sizeof(bufferValue), bufferValue, sizeof(bufferValue));
        if (ret != EOK) {
            HiLog::Error(LABEL, "assemble component error, ret = %{public}d", ret);
            return ret;
        }
        curPos += sizeof(bufferValue);
    }
    return curPos;
}

int32_t JpegHardwareDecoder::JpegSosAssemble(const struct CodecJpegDecInfo &decInfo, int8_t *buffer, int32_t curPos)
{
    HDF_LOGI("enter. curPos = %{public}d", curPos);
    // flag
    curPos = PutInt16(buffer, curPos, 0xffda);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOS flag error");
        return curPos;
    }

    int16_t len = decInfo.numComponents * 2 + 6; // * rgb table length + other data
    curPos = PutInt16(buffer, curPos, len);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOS len error");
        return curPos;
    }

    int8_t components = decInfo.numComponents & 0xFF;
    curPos = PutInt8(buffer, curPos, components);
    if (curPos < 0) {
        HiLog::Error(LABEL, "assemble SOS components error");
        return curPos;
    }

    for (size_t i = 0; i < decInfo.compInfo.size(); i++) {
        int8_t componentId = decInfo.compInfo[i].componentId;
        int8_t indexNo = ((decInfo.compInfo[i].dcTableNo & 0xFF) << 4) | (decInfo.compInfo[i].acTableNo & 0xFF);
        int16_t value = ((componentId << 8) | indexNo) & 0xffff;
        curPos = PutInt16(buffer, curPos, value);
        if (curPos < 0) {
            HiLog::Error(LABEL, "assemble SOS component value error");
            return curPos;
        }
    }
    int8_t dataStart[] = {0x00, 0x3F, 0x00};
    auto ret = memcpy_s(buffer + curPos, sizeof(dataStart), dataStart, sizeof(dataStart));
    if (ret != EOK) {
        HiLog::Error(LABEL, "assemble SOS data flag error, ret = %{public}d", ret);
        return ret;
    }
    curPos += sizeof(dataStart);
    return curPos;
}

int32_t JpegHardwareDecoder::JpegDataAssemble(int8_t *buffer, int32_t curPos, int32_t fd)
{
    HDF_LOGI("enter. curPos = %{public}d", curPos);
    int32_t size = OHOS::AshmemGetSize(fd);
    HDF_LOGI("get size %{public}d from fd %{public}d", size, fd);
    OHOS::Ashmem mem(fd, size);
    // check ret value
    mem.MapReadOnlyAshmem();
    auto addr = const_cast<void *>(mem.ReadFromAshmem(0, 0));
    auto ret = memcpy_s(buffer + curPos, size, addr, size);
    if (ret != EOK) {
        HiLog::Error(LABEL, "assemble compressed data error, ret = %{public}d", ret);
        mem.UnmapAshmem();
        if (ret > 0) {
            return -ret;
        }
        return ret;
    }
    mem.UnmapAshmem();
    mem.CloseAshmem();
    curPos += size;
    return curPos;
}

int32_t JpegHardwareDecoder::DessambleSof(int8_t *buffer, struct CodecJpegDecInfo &decInfo)
{
    HDF_LOGI("dessamble SOI");
    // len
    int32_t len = GetInt16(buffer);
    buffer += 2;  // 2: marker len
    // precision
    decInfo.dataPrecision = GetInt8(buffer);
    buffer++;
    // height
    decInfo.imageHeight = GetInt16(buffer);
    buffer += 2;  // 2: height len
    // width
    decInfo.imageWidth = GetInt16(buffer);
    buffer += 2;  // 2: width len

    decInfo.numComponents = GetInt8(buffer);
    buffer++;

    HDF_LOGI("image width[%{public}d],height[%{public}d],components[%{public}d]", decInfo.imageWidth,
        decInfo.imageHeight, decInfo.numComponents);
    for (size_t i = 0; i < decInfo.numComponents; i++) {
        CodecJpegCompInfo comInfo;

        comInfo.infoFlag = true;
        comInfo.componentId = GetInt8(buffer);
        buffer++;

        int8_t sampFactor = GetInt8(buffer);
        buffer++;
        comInfo.hSampFactor = (sampFactor >> 4) & 0xFF;  // 4: hsampfactor offset
        comInfo.vSampFactor = sampFactor & 0x0F;

        comInfo.quantTableNo = GetInt8(buffer);
        buffer++;
        decInfo.compInfo.push_back(std::move(comInfo));
        HDF_LOGI("componentId[%{public}d],hSampFactor[%{public}d],vSampFactor[%{public}d],quantTableNo[%{public}d]",
            comInfo.componentId, comInfo.hSampFactor, comInfo.vSampFactor, comInfo.quantTableNo);
    }
    return len;
}

int32_t JpegHardwareDecoder::DessambleSos(int8_t *buffer, struct CodecJpegDecInfo &decInfo)
{
    HDF_LOGI("dessamble SOS");
    int32_t len = GetInt16(buffer);
    buffer += 2;  // 2:marker len

    int32_t components = GetInt8(buffer);
    buffer++;

    for (int32_t i = 0; i < components; i++) {
        decInfo.compInfo[i].infoFlag = true;

        int32_t componentId = GetInt8(buffer);
        (void)componentId;
        buffer++;
        // index not used
        auto data = GetInt8(buffer);
        buffer++;
        decInfo.compInfo[i].dcTableNo = (data >> 4) & 0x0F;  // 4: dctable offset
        decInfo.compInfo[i].acTableNo = data & 0x0F;
        HDF_LOGI("componentId[%{public}d],dcTableNo[%{public}d],acTableNo[%{public}d]", componentId,
            decInfo.compInfo[i].dcTableNo, decInfo.compInfo[i].acTableNo);
    }
    buffer += 3;  // skip 0x003F00
    return len;
}

int32_t JpegHardwareDecoder::DessambleCompressData(int8_t *buffer, std::unique_ptr<int8_t[]> &compressBuffer,
                                               uint32_t &comBufLen)
{
    int8_t *dataStart = buffer;
    do {
        int32_t v = GetInt8(buffer);
        buffer++;
        if (v != 0xff) {
            continue;
        }
        v = GetInt8(buffer);
        buffer++;
        if (v != 0xd9) {
            continue;
        }
        buffer -= 2;  // 2: marker len
        break;
    } while (1);
    comBufLen = (int32_t)(buffer - dataStart) + 1;
    compressBuffer = std::make_unique<int8_t[]>(comBufLen);
    auto ret = memcpy_s(compressBuffer.get(), comBufLen, dataStart, comBufLen);
    if (ret != EOK) {
        HiLog::Error(LABEL, "copy compressed data error, dataLen %{public}d, ret %{public}d", comBufLen, ret);
        compressBuffer = nullptr;
        return ret;
    }
    return comBufLen;
}

int32_t JpegHardwareDecoder::DessambleDqt(int8_t *buffer, struct CodecJpegDecInfo &decInfo)
{
    HDF_LOGI("dessamble DQT");
    int8_t *bufferOri = buffer;
    int32_t len = GetInt16(buffer);
    buffer += 2;  // 2: marker len
    // maybe has more dqt table
    while ((buffer - bufferOri) < len) {
        auto data = GetInt8(buffer);
        buffer++;
        int32_t tableId = data & 0x000f;
        (void)tableId;
        int32_t size = 32;     // size
        if (((data >> 4) & 0x0f) == 1) { // 4: low 4 bits, 1: for 16 bits
            size *= 2;  // 2: 16bits has double size
        }
        CodecJpegQuantTable table;
        table.tableFlag = true;
        HDF_LOGI("tableid[%{public}d]", tableId);
        for (int32_t i = 0; i < size; i++) { // 2: 16bits has double size
            table.quantVal.push_back(static_cast<int16_t>(GetInt16(buffer)));
            buffer += 2;  // 2: data offset
        }
        decInfo.quantTbl.push_back(std::move(table));
    }
    return len;
}

int32_t JpegHardwareDecoder::DessambleDht(int8_t *buffer, struct CodecJpegDecInfo &decInfo)
{
    HDF_LOGI("dessamble DHT");
    int8_t *bufferOri = buffer;
    int32_t len = GetInt16(buffer);
    buffer += 2;  // 2: marker len
    // 可能存在多个表在同一个dht marker 中
    while ((buffer - bufferOri) < len) {
        auto data = GetInt8(buffer);
        buffer++;
        int32_t tableId = data & 0x000f;
        (void)tableId;
        int32_t acOrDc = (data >> 4) & 0x0f;  // 0:DC, 1:AC, 4: ac/dc data offset
        CodecJpegHuffTable table;
        table.tableFlag = true;
        int32_t num = 0;
        for (size_t i = 0; i < 16; i++) {  // 16: Data size
            auto data = GetInt8(buffer);
            buffer++;
            table.bits.push_back(data);
            num += data & 0x00ff;
        }
        HDF_LOGI("tableid[%{public}d], acOrDc[%{public}d], num[%{public}d]", tableId, acOrDc, num);
        // val
        for (int32_t i = 0; i < num; i++) {
            table.huffVal.push_back(*buffer++);
        }
        if (acOrDc == 1) {
            decInfo.acHuffTbl.push_back(std::move(table));
        } else {
            decInfo.dcHuffTbl.push_back(std::move(table));
        }
    }
    return len;
}

int32_t JpegHardwareDecoder::FindMarker(int8_t *start)
{
    int32_t marker = GetInt16(start);
    return marker;
}

int32_t JpegHardwareDecoder::PutInt16(int8_t *buffer, int32_t curPos, int16_t value)
{
    int8_t data[] = {value >> 8, value & 0xFF};
    auto ret = memcpy_s(buffer + curPos, sizeof(data), data, sizeof(data));
    if (ret != EOK) {
        HiLog::Error(LABEL, "memcpy ret err %{public}d", ret);
        return -1;
    }
    return curPos + sizeof(data);
}

int32_t JpegHardwareDecoder::PutInt8(int8_t *buffer, int32_t curPos, int8_t value)
{
    auto ret = memcpy_s(buffer + curPos, sizeof(value), &value, sizeof(value));
    if (ret != EOK) {
        HiLog::Error(LABEL, "memcpy ret err %{public}d", ret);
        return -1;
    }
    return curPos + sizeof(value);
}

int32_t JpegHardwareDecoder::GetInt8(int8_t *buffer)
{
    return buffer[0] & 0x00ff;
}

int32_t JpegHardwareDecoder::GetInt16(int8_t *buffer)
{
    return ((buffer[0] << 8) & 0x00ff00) | (buffer[1] & 0x00ff);  // 8:data offset
}
/////////////////////////////////////////////////////////////////////////

int32_t JpegHardwareDecoder::Init()
{
    if (hdiDecoder_ == nullptr || hdiBuffer_ == nullptr) {
        HiLog::Error(LABEL, "hdiDecoder_ or hdiBuffer_ is null !");
        return HDF_FAILURE;
    }
    callback_ = new CodecJpegCallbackService(decoder);
    return hdiDecoder_->JpegInit();
}

int32_t JpegHardwareDecoder::DeInit()
{
    return hdiDecoder_->JpegDeInit();
}

int32_t JpegHardwareDecoder::OnEvent(int32_t error)
{
    HiLog::Error(LABEL, "enter callback , ret [%{public}d] !", error);
    if (error != HDF_SUCCESS) {
        HiLog::Error(LABEL, "hardware decode error, should to decode by software !");
    }
    // write decode result
    if (error == HDF_SUCCESS) {
        // BufferHandle *outHandle = outBuffer_.buffer->GetBufferHandle();
        // hdiBuffer_->Mmap(*outHandle);
        uint8_t* str =  const_cast<uint8_t*>(jpegBuffer.inputStreamBuffer);
        context.pixelsBuffer.buffer = reinterpret_cast<void *>(str);
        context.pixelsBuffer.bufferSize = bufferLen_;
        context.pixelsBuffer.dataSize = compDataLen_;
        context.pixelsBuffer.context = outBuffer_.buffer;
        // ioOut_.write(reinterpret_cast<char *>(outHandle->virAddr), outHandle->size);
        // ioOut_.flush();
        // hdiBuffer_->Unmap(*outHandle);
        // hdiBuffer_->FreeMem(*outHandle);
    }
    HiLog::Error(LABEL, "decode and write output buffer succ !");

    // freeInBuffer
    auto ret = hdiDecoder_->FreeInBuffer(inBuffer_);
    if (ret != HDF_SUCCESS) {
        HiLog::Error(LABEL, "FreeInBuffer failed, err [%{public}d] !", ret);
    }
    return ret;
}

uint32_t JpegHardwareDecoder::GetJpegBuffer(InputDataStream *sourceStream)
{
    sourceStream->Seek(streamPosition_);
    bufferLen_ = sourceStream->Tell();
    streamPosition_ = bufferLen_;
    bool tag = sourceStream->Read(bufferLen_, jpegBuffer);
    if (tag == false) {
        HiLog::Error(LABEL, "JpegHardwareDecoder read buffer error.");
        return ERR_IMAGE_GET_DATA_ABNORMAL;
    }
    uint8_t* str =  const_cast<uint8_t*>(jpegBuffer.inputStreamBuffer);
    bool err = DessambleJpeg(reinterpret_cast<int8_t *>(str), bufferLen_,
                             decInfo_, compressBuffer_, compDataLen_);
    if (!err) {
        HiLog::Error(LABEL,"DecodeJpegHeader failed !");
        return HDF_FAILURE;
    }
    return SUCCESS;
}

uint32_t JpegHardwareDecoder::AllocBuffer(uint32_t width, uint32_t height)
{
    // alloc inBuffer
    auto ret = hdiDecoder_->AllocateInBuffer(inBuffer_, compDataLen_);
    if (ret != HDF_SUCCESS) {
        HiLog::Error(LABEL,"AllocateInBuffer failed, err [%{public}d] !", ret);
        return ERROR;
    }
    // alloc outBuffer
    AllocInfo alloc = {.width = AlignUp(width),
                       .height = height,
                       .usage =  HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
                       .format = PIXEL_FMT_YCBCR_420_SP};

    BufferHandle *handle = nullptr;
    ret = hdiBuffer_->AllocMem(alloc, handle);
    if (ret != HDF_SUCCESS) {
        HiLog::Error(LABEL, "AllocMem failed, err [%{public}d] !", ret);
        return HDF_FAILURE;
    }

    outBuffer_.buffer = new NativeBuffer(handle);
    outBuffer_.fenceFd = -1;
    HiLog::Error(LABEL, "JpegHardwareDecoder AllocBuffer success.");
    return SUCCESS;
}

uint32_t JpegHardwareDecoder::Decode(JpegHardwareDecodeOpt &opt)
{
    uint32_t ret = GetJpegBuffer(opt.inputStream);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "JpegHardwareDecoder prepare data failed.");
        return ret;
    }

    ret = AllocBuffer(opt.width, opt.height);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "JpegHardwareDecoder alloc buffer failed.");
        return ret;
    }

    BufferHandle *bufferHandle = inBuffer_.buffer->GetBufferHandle();
    hdiBuffer_->Mmap(*bufferHandle);
    ret = memcpy_s(bufferHandle->virAddr, compDataLen_, compressBuffer_.get(), compDataLen_);
    if (ret != 0) {
        // HiLog::Error("memcpy_s failed, err [%{public}d] !", ret);
        return ERROR;
    }
    hdiBuffer_->Unmap(*bufferHandle);

    ret = hdiDecoder_->DoJpegDecode(inBuffer_, outBuffer_, callback_, decInfo_);
    if (ret != HDF_SUCCESS) {
        // HiLog::Error("DoJpegDecode failed, err [%{public}d] !", err);
    }
    opt.context = context;
    return ret;
}
}// namespace ImagePlugin
} // namespace OHOS