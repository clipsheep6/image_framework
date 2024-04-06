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
#include "gif_encoder.h"
#include "image_log.h"
#include "image_trace.h"
#include "media_errors.h"
#include "securec.h"
#include <iostream>
#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "GifEncoder"

namespace OHOS {
namespace ImagePlugin {
using namespace MultimediaPlugin;
using namespace Media;

static int g_sortRGBAxis = 0;

GifEncoder::GifEncoder()
{
    IMAGE_LOGD("create IN");

    IMAGE_LOGD("create OUT");
}

GifEncoder::~GifEncoder()
{
    IMAGE_LOGD("release IN");

    pixelMaps_.clear();

    IMAGE_LOGD("release OUT");
}

uint32_t GifEncoder::StartEncode(OutputDataStream &outputStream, PlEncodeOptions &option)
{
    ImageTrace imageTrace("GifEncoder::StartEncode");
    IMAGE_LOGD("StartEncode IN, quality=%{public}u, numberHint=%{public}u",
        option.quality, option.numberHint);

    pixelMaps_.clear();
    outputStream_ = &outputStream;
    encodeOpts_ = option;

    IMAGE_LOGD("StartEncode OUT");
    return SUCCESS;
}

uint32_t GifEncoder::AddImage(Media::PixelMap &pixelMap)
{
    ImageTrace imageTrace("GifEncoder::AddImage");
    IMAGE_LOGD("AddImage IN");

    if (pixelMap.GetPixels() == nullptr) {
        IMAGE_LOGE("AddImage failed, invalid pixelMap.");
        return ERROR;
    }

    pixelMaps_.push_back(&pixelMap);
    IMAGE_LOGD("AddImage OUT");
    return SUCCESS;
}

uint32_t GifEncoder::FinalizeEncode()
{
    ImageTrace imageTrace("GifEncoder::FinalizeEncode");
    IMAGE_LOGD("FinalizeEncode IN");

    if (pixelMaps_.empty()) {
        IMAGE_LOGE("FinalizeEncode, no pixel map input.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    uint32_t errorCode = ERROR;
    errorCode = DoEncode();
    if (errorCode != SUCCESS) {
        IMAGE_LOGE("FinalizeEncode, encode failed=%{public}u.", errorCode);
    }

    IMAGE_LOGD("FinalizeEncode OUT");
    return errorCode;
}

uint32_t GifEncoder::DoEncode()
{
    IMAGE_LOGD("DoEncode IN");

    WriteFileInfo();

    for (int index = 0; index < pixelMaps_.size(); index++) {
        InitDictionary();
        WriteFrameInfo(index);
        processFrame(index);
    }

    IMAGE_LOGD("DoEncode OUT");
    return SUCCESS;
}

uint32_t GifEncoder::WriteFileInfo()
{
    if (!Write(GIF89_STAMP, sizeof(GIF89_STAMP))) {
        IMAGE_LOGE("Write to buffer error.");
        return ERROR;
    }

    LogicalScreenDescriptor lsd;
    memset_s(&lsd, sizeof(LogicalScreenDescriptor), 0, sizeof(LogicalScreenDescriptor));
    for (auto pixelMap : pixelMaps_) {
        if (lsd.LogicalScreenWidth < static_cast<uint16_t>(pixelMap->GetWidth())) {
            lsd.LogicalScreenWidth = static_cast<uint16_t>(pixelMap->GetWidth());
        }
        if (lsd.LogicalScreenHeight < static_cast<uint16_t>(pixelMap->GetHeight())) {
            lsd.LogicalScreenHeight = static_cast<uint16_t>(pixelMap->GetHeight());
        }
    }
    if (!Write((const uint8_t*)&lsd, sizeof(LogicalScreenDescriptor))) {
        IMAGE_LOGE("Write to buffer error.");
        return ERROR;
    }

    ApplicationExtension ae;
    memset_s(&ae, sizeof(ApplicationExtension), 0, sizeof(ApplicationExtension));
    ae.ExtensionIntroducer = EXTENSION_INTRODUCER;
    ae.ExtensionLabel = APPLICATION_EXTENSION_LABEL;
    ae.BlockSize = 0x0B;
    memcpy_s(&ae.ApplicationIdentifier, sizeof(ae.ApplicationIdentifier),
             &applicationIdentifier, sizeof(ae.ApplicationIdentifier));
    memcpy_s(&ae.ApplicationAuthenticationCode, sizeof(ae.ApplicationIdentifier),
             &applicationAuthenticationCode, sizeof(ae.ApplicationAuthenticationCode));
    ae.ApplicationDataSize = 0x03;
    ae.ApplicationDataIndex = 0x01;
    ae.LoopTime = encodeOpts_.loop;
    ae.BlockTerminator = 0x00;
    if (!Write((const uint8_t*)&ae, sizeof(ApplicationExtension))) {
        IMAGE_LOGE("Write to buffer error.");
        return ERROR;
    }

    return SUCCESS;
}

uint32_t GifEncoder::WriteFrameInfo(int index)
{
    GraphicControlExtension gce;
    memset_s(&gce, sizeof(GraphicControlExtension), 0, sizeof(GraphicControlExtension));
    gce.ExtensionIntroducer = EXTENSION_INTRODUCER;
    gce.GraphicControlLabel = GRAPHIC_CONTROL_LABEL;
    gce.BlockSize = 0x04;
    gce.PackedFields = 0x08;
    gce.DelayTime = index < encodeOpts_.delayTimes.size() ? encodeOpts_.delayTimes[index] : DEFAULT_DELAY_TIME;
    gce.TransparentColorIndex = 0x00;
    gce.BlockTerminator = 0x00;
    if (!Write((const uint8_t*)&gce, sizeof(GraphicControlExtension))) {
        IMAGE_LOGE("Write to buffer error.");
        return ERROR;
    }

    ImageDescriptor id;
    memset_s(&id, sizeof(ImageDescriptor), 0, sizeof(ImageDescriptor));
    id.ImageSeparator = IMAGE_SEPARATOR;
    id.ImageLeftPosition = 0x0000;
    id.ImageTopPosition = 0x0000;
    id.ImageWidth = static_cast<uint16_t>(pixelMaps_[index]->GetWidth());
    id.ImageHeight = static_cast<uint16_t>(pixelMaps_[index]->GetHeight());
    id.PackedFields = 0x87;
    if (!Write((const uint8_t*)&id, sizeof(ImageDescriptor))) {
        IMAGE_LOGE("Write to buffer error.");
        return ERROR;
    }

    return SUCCESS;
}

uint32_t GifEncoder::processFrame(int index)
{
    ColorType *colorMap = (ColorType *)malloc(sizeof(ColorType) * COLOR_MAP_SIZE);
    if (colorMap == NULL) {
        IMAGE_LOGE("Failed to allocate memory.");
        return ERROR;
    }
    uint16_t width = static_cast<uint16_t>(pixelMaps_[index]->GetWidth());
    uint16_t height = static_cast<uint16_t>(pixelMaps_[index]->GetHeight());
    uint64_t frameSize = width * height;
    uint8_t *colorBuffer = (uint8_t *)malloc(frameSize);
    if (colorBuffer == NULL) {
        IMAGE_LOGE("Failed to allocate memory.");
        free(colorMap);
        return ERROR;
    }
    if (colorQuantize(index, width, height, colorBuffer, colorMap)) {
        IMAGE_LOGE("Failed to quantize color.");
        free(colorBuffer);
        free(colorMap);
        return ERROR;
    }
    for (int j = 0; j < COLOR_MAP_SIZE; j++) {
        Write(&(colorMap[j].Red), 1);
        Write(&(colorMap[j].Green), 1);
        Write(&(colorMap[j].Blue), 1);
    }

    if (LZWEncodeFrame(colorBuffer, width, height)) {
        IMAGE_LOGE("Failed to encode frame.");
        free(colorBuffer);
        free(colorMap);
        return ERROR;
    }

    free(colorBuffer);
    free(colorMap);

    return SUCCESS;
}

uint32_t GifEncoder::colorQuantize(int index, uint16_t width, uint16_t height,
                                   uint8_t *outputBuffer, ColorType *outputColorMap)
{
    uint8_t *redBuffer = NULL;
    uint8_t *greenBuffer = NULL;
    uint8_t *blueBuffer = NULL;
    uint64_t frameSize = width * height;
    if ((redBuffer = (uint8_t *)malloc(frameSize)) == NULL ||
        (greenBuffer = (uint8_t *)malloc(frameSize)) == NULL ||
        (blueBuffer = (uint8_t *)malloc(frameSize)) == NULL) {
        IMAGE_LOGE("Failed to allocate memory.");
        return ERROR;
    }

    if (separateRGB(index, width, height, redBuffer, greenBuffer, blueBuffer)) {
        IMAGE_LOGE("Failed to separate RGB, aborted.");
        free(redBuffer);
        free(greenBuffer);
        free(blueBuffer);
        return ERROR;
    }

    int colorMapSize = COLOR_MAP_SIZE;
    if (GifQuantizeBuffer(width, height, &colorMapSize, redBuffer, greenBuffer, blueBuffer,
                          outputBuffer, outputColorMap)) {
        IMAGE_LOGE("Failed to quantize buffer, aborted.");
        free(redBuffer);
        free(greenBuffer);
        free(blueBuffer);
        return ERROR;
    }

    free(redBuffer);
    free(greenBuffer);
    free(blueBuffer);

    return SUCCESS;
}

uint32_t GifEncoder::separateRGB(int index, uint16_t width, uint16_t height,
                                 uint8_t *redBuffer, uint8_t *greenBuffer, uint8_t *blueBuffer)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t pixelColor;
            if (!pixelMaps_[index]->GetARGB32Color(x, y, pixelColor)) {
                IMAGE_LOGE("Failed to get rgb value.");
                return ERROR;
            }
            redBuffer[y * width + x] = pixelMaps_[index]->GetARGB32ColorR(pixelColor);
            greenBuffer[y * width + x] = pixelMaps_[index]->GetARGB32ColorG(pixelColor);
            blueBuffer[y * width + x] = pixelMaps_[index]->GetARGB32ColorB(pixelColor);
        }
    }

    return SUCCESS;
}

uint32_t GifEncoder::GifQuantizeBuffer(uint16_t Width, uint16_t Height, int *ColorMapSize,
                                       const uint8_t *RedInput, const uint8_t *GreenInput, const uint8_t *BlueInput,
                                       uint8_t *OutputBuffer, ColorType *OutputColorMap)
{
    int i;
    int j;
    int MaxRGBError[NUM_OF_RGB];
    long Red;
    long Green;
    long Blue;
    uint32_t Index;
    uint32_t NumOfEntries;
    uint32_t NewColorMapSize;
    NewColorMapType NewColorSubdiv[COLOR_OF_GIF];
    QuantizedColorType *ColorArrayEntries;
    QuantizedColorType *QuantizedColor;

    ColorArrayEntries = (QuantizedColorType *)malloc(sizeof(QuantizedColorType) * COLOR_ARRAY_SIZE);
    if (ColorArrayEntries == NULL) {
        return ERROR;
    }

    for (i = 0; i < COLOR_ARRAY_SIZE; i++) {
        ColorArrayEntries[i].RGB[R_IN_RGB] = i >> (BITS_PER_PRIM_COLOR << 1);
        ColorArrayEntries[i].RGB[G_IN_RGB] = (i >> BITS_PER_PRIM_COLOR) & 0x1F;
        ColorArrayEntries[i].RGB[B_IN_RGB] = i & 0x1F;
        ColorArrayEntries[i].Count = 0;
    }

    for (i = 0; i < (int)(Width * Height); i++) {
        Index = ((RedInput[i] >> (BITS_IN_BYTE - BITS_PER_PRIM_COLOR)) << (BITS_PER_PRIM_COLOR << 1)) +
                 ((GreenInput[i] >> (BITS_IN_BYTE - BITS_PER_PRIM_COLOR)) << BITS_PER_PRIM_COLOR) +
                 (BlueInput[i] >> (BITS_IN_BYTE - BITS_PER_PRIM_COLOR));
        ColorArrayEntries[Index].Count++;
    }

    for (i = 0; i < COLOR_OF_GIF; i++) {
        NewColorSubdiv[i].QuantizedColors = NULL;
        NewColorSubdiv[i].Count = NewColorSubdiv[i].NumEntries = 0;
        for (j = 0; j < NUM_OF_RGB; j++) {
            NewColorSubdiv[i].RGBMin[j] = 0;
            NewColorSubdiv[i].RGBWidth[j] = 0xFF;
        }
    }

    for (i = 0; i < COLOR_ARRAY_SIZE; i++) {
        if (ColorArrayEntries[i].Count > 0) {
            break;
        }
    }
    QuantizedColor = NewColorSubdiv[0].QuantizedColors = &ColorArrayEntries[i];
    NumOfEntries = 1;
    while (++i < COLOR_ARRAY_SIZE) {
        if (ColorArrayEntries[i].Count > 0) {
            QuantizedColor->Pnext = &ColorArrayEntries[i];
            QuantizedColor = &ColorArrayEntries[i];
            NumOfEntries++;
        }
    }
    QuantizedColor->Pnext = NULL;

    NewColorSubdiv[0].NumEntries = NumOfEntries;
    NewColorSubdiv[0].Count = ((long)Width) * Height;
    NewColorMapSize = 1;
    if (SubdivColorMap(NewColorSubdiv, *ColorMapSize, &NewColorMapSize)) {
        free((char *)ColorArrayEntries);
        return ERROR;
    }
    if (NewColorMapSize < *ColorMapSize) {
        for (i = NewColorMapSize; i < *ColorMapSize; i++) {
            OutputColorMap[i].Red = OutputColorMap[i].Green = OutputColorMap[i].Blue = 0;
        }
    }

    for (i = 0; i < NewColorMapSize; i++) {
        if ((j = NewColorSubdiv[i].NumEntries) > 0) {
            QuantizedColor = NewColorSubdiv[i].QuantizedColors;
            Red = Green = Blue = 0;
            while (QuantizedColor) {
                QuantizedColor->NewColorIndex = i;
                Red += QuantizedColor->RGB[R_IN_RGB];
                Green += QuantizedColor->RGB[G_IN_RGB];
                Blue += QuantizedColor->RGB[B_IN_RGB];
                QuantizedColor = QuantizedColor->Pnext;
            }
            OutputColorMap[i].Red = (Red << (BITS_IN_BYTE - BITS_PER_PRIM_COLOR)) / j;
            OutputColorMap[i].Green = (Green << (BITS_IN_BYTE - BITS_PER_PRIM_COLOR)) / j;
            OutputColorMap[i].Blue = (Blue << (BITS_IN_BYTE - BITS_PER_PRIM_COLOR)) / j;
        }
    }

    MaxRGBError[R_IN_RGB] = MaxRGBError[G_IN_RGB] = MaxRGBError[B_IN_RGB] = 0;
    for (i = 0; i < (int)(Width * Height); i++) {
        Index = ((RedInput[i] >> (BITS_IN_BYTE - BITS_PER_PRIM_COLOR)) << (BITS_PER_PRIM_COLOR << 1)) +
                 ((GreenInput[i] >> (BITS_IN_BYTE - BITS_PER_PRIM_COLOR)) << BITS_PER_PRIM_COLOR) +
                 (BlueInput[i] >> (BITS_IN_BYTE - BITS_PER_PRIM_COLOR));
        Index = ColorArrayEntries[Index].NewColorIndex;
        OutputBuffer[i] = Index;
        if (MaxRGBError[R_IN_RGB] < abs(OutputColorMap[Index].Red - RedInput[i])) {
            MaxRGBError[R_IN_RGB] = abs(OutputColorMap[Index].Red - RedInput[i]);
        }
        if (MaxRGBError[G_IN_RGB] < abs(OutputColorMap[Index].Green - GreenInput[i])) {
            MaxRGBError[G_IN_RGB] = abs(OutputColorMap[Index].Green - GreenInput[i]);
        }
        if (MaxRGBError[B_IN_RGB] < abs(OutputColorMap[Index].Blue - BlueInput[i])) {
            MaxRGBError[B_IN_RGB] = abs(OutputColorMap[Index].Blue - BlueInput[i]);
        }
    }

    free((char *)ColorArrayEntries);

    *ColorMapSize = NewColorMapSize;

    return SUCCESS;
}

uint32_t GifEncoder::SubdivColorMap(NewColorMapType *NewColorSubdiv,
                                    uint32_t ColorMapSize,
                                    uint32_t *NewColorMapSize)
{
    uint32_t i;
    uint32_t j;
    uint32_t Index = 0;
    QuantizedColorType *QuantizedColor;
    QuantizedColorType **SortArray;

    while (ColorMapSize > *NewColorMapSize) {
        long Sum;
        long Count;
        int MaxSize = -1;
        uint32_t NumEntries;
        uint32_t MinColor;
        uint32_t MaxColor;
        for (i = 0; i < *NewColorMapSize; i++) {
            for (j = 0; j < NUM_OF_RGB; j++) {
                if (((int)NewColorSubdiv[i].RGBWidth[j] > MaxSize) && (NewColorSubdiv[i].NumEntries > 1)) {
                    MaxSize = NewColorSubdiv[i].RGBWidth[j];
                    Index = i;
                    g_sortRGBAxis = j;
                }
            }
        }
        if (MaxSize == -1) {
            return SUCCESS;
        }

        SortArray = (QuantizedColorType **)malloc(sizeof(QuantizedColorType *) * NewColorSubdiv[Index].NumEntries);
        if (SortArray == NULL) {
            return ERROR;
        }
        for (j = 0, QuantizedColor = NewColorSubdiv[Index].QuantizedColors;
             j < NewColorSubdiv[Index].NumEntries && QuantizedColor != NULL;
             j++, QuantizedColor = QuantizedColor->Pnext) {
            SortArray[j] = QuantizedColor;
        }
        qsort(SortArray, NewColorSubdiv[Index].NumEntries, sizeof(QuantizedColorType *), SortCmpRtn);
        for (j = 0; j < NewColorSubdiv[Index].NumEntries - 1; j++) {
            SortArray[j]->Pnext = SortArray[j + 1];
        }
        SortArray[NewColorSubdiv[Index].NumEntries - 1]->Pnext = NULL;
        NewColorSubdiv[Index].QuantizedColors = QuantizedColor = SortArray[0];
        free((char *)SortArray);
        Sum = (NewColorSubdiv[Index].Count >> 1) - QuantizedColor->Count;
        NumEntries = 1;
        Count = QuantizedColor->Count;
        while (QuantizedColor->Pnext != NULL &&
               (Sum -= QuantizedColor->Pnext->Count) >= 0 &&
               QuantizedColor->Pnext->Pnext != NULL) {
            QuantizedColor = QuantizedColor->Pnext;
            NumEntries++;
            Count += QuantizedColor->Count;
        }
        MaxColor = QuantizedColor->RGB[g_sortRGBAxis];
        MinColor = QuantizedColor->Pnext->RGB[g_sortRGBAxis];
        MaxColor <<= (BITS_IN_BYTE - BITS_PER_PRIM_COLOR);
        MinColor <<= (BITS_IN_BYTE - BITS_PER_PRIM_COLOR);

        NewColorSubdiv[*NewColorMapSize].QuantizedColors = QuantizedColor->Pnext;
        QuantizedColor->Pnext = NULL;
        NewColorSubdiv[*NewColorMapSize].Count = Count;
        NewColorSubdiv[Index].Count -= Count;
        NewColorSubdiv[*NewColorMapSize].NumEntries = NewColorSubdiv[Index].NumEntries - NumEntries;
        NewColorSubdiv[Index].NumEntries = NumEntries;
        for (j = 0; j < NUM_OF_RGB; j++) {
            NewColorSubdiv[*NewColorMapSize].RGBMin[j] = NewColorSubdiv[Index].RGBMin[j];
            NewColorSubdiv[*NewColorMapSize].RGBWidth[j] = NewColorSubdiv[Index].RGBWidth[j];
        }
        NewColorSubdiv[*NewColorMapSize].RGBWidth[g_sortRGBAxis] =
            NewColorSubdiv[*NewColorMapSize].RGBMin[g_sortRGBAxis] +
            NewColorSubdiv[*NewColorMapSize].RGBWidth[g_sortRGBAxis] - MinColor;
        NewColorSubdiv[*NewColorMapSize].RGBMin[g_sortRGBAxis] = MinColor;

        NewColorSubdiv[Index].RGBWidth[g_sortRGBAxis] = MaxColor - NewColorSubdiv[Index].RGBMin[g_sortRGBAxis];
        (*NewColorMapSize)++;
    }

    return SUCCESS;
}

int GifEncoder::SortCmpRtn(const void *Entry1, const void *Entry2)
{
    QuantizedColorType *entry1 = (*((QuantizedColorType **)Entry1));
    QuantizedColorType *entry2 = (*((QuantizedColorType **)Entry2));

    int hash1 = entry1->RGB[(g_sortRGBAxis + R_IN_RGB)] * COLOR_OF_GIF * COLOR_OF_GIF +
                entry1->RGB[(g_sortRGBAxis + G_IN_RGB) % NUM_OF_RGB] * COLOR_OF_GIF +
                entry1->RGB[(g_sortRGBAxis + B_IN_RGB) % NUM_OF_RGB];
    int hash2 = entry2->RGB[(g_sortRGBAxis + R_IN_RGB)] * COLOR_OF_GIF * COLOR_OF_GIF +
                entry2->RGB[(g_sortRGBAxis + G_IN_RGB) % NUM_OF_RGB] * COLOR_OF_GIF +
                entry2->RGB[(g_sortRGBAxis + B_IN_RGB) % NUM_OF_RGB];

    return (hash1 - hash2);
}

void GifEncoder::InitDictionary()
{
    lastCode = FIRST_CODE;
    ClearCode = CLEAR_CODE;
    EOFCode = ClearCode + 1;
    RunningCode = EOFCode + 1;
    RunningBits = BITS_IN_BYTE + 1;
    MaxCode1 = 1 << RunningBits;
    CrntShiftState = 0;
    CrntShiftDWord = 0;
    memset_s(dictionary, sizeof(uint32_t) * DICTIONARY_SIZE, 0xFF, sizeof(uint32_t) * DICTIONARY_SIZE);
}

int GifEncoder::IsInDictionary(uint32_t Key)
{
    int key = ((Key >> LZ_BITS) ^ Key) & 0x1FFF;
    uint32_t DKey;
    while ((DKey = (dictionary[key] >> LZ_BITS)) != 0xFFFFFL) {
        if (Key == DKey) {
            return (dictionary[key] & 0x0FFF);
        }
        key = (key + 1) & 0x1FFF;
    }
    return -1;
}
 
void GifEncoder::AddToDictionary(uint32_t Key, int Code)
{
    int key = ((Key >> LZ_BITS) ^ Key) & 0x1FFF;
    while ((dictionary[key] >> LZ_BITS) != 0xFFFFFL) {
        key = (key + 1) & 0x1FFF;
    }
    dictionary[key] = (Key << LZ_BITS) | (Code & 0x0FFF);
}

uint32_t GifEncoder::LZWEncodeFrame(uint8_t *outputBuffer, uint16_t width, uint16_t height)
{
    uint8_t *pTmp = outputBuffer;
    uint8_t BitsPerPixel = BITS_IN_BYTE;
    InitDictionary();
    Write((const uint8_t*)&BitsPerPixel, 1);
    LZWWriteOut(ClearCode);
    for (int j = 0; j < height; j++) {
        if (LZWEncode(pTmp, width)) {
            IMAGE_LOGE("Failed to encode, aborted.");
            return ERROR;
        }
        pTmp += width;
    }
    if (LZWWriteOut(lastCode)) {
        IMAGE_LOGE("Failed to write lastCode, aborted.");
        return ERROR;
    }
    if (LZWWriteOut(EOFCode)) {
        IMAGE_LOGE("Failed to write EOFCode, aborted.");
        return ERROR;
    }
    if (LZWWriteOut(FLUSH_OUTPUT)) {
        IMAGE_LOGE("Failed to write flushCode, aborted.");
        return ERROR;
    }
    return SUCCESS;
}

uint32_t GifEncoder::LZWEncode(uint8_t *buffer, int length)
{
    int i = 0;
    int CrntCode;
    if (lastCode == FIRST_CODE) {
        CrntCode = buffer[i++];
    } else {
        CrntCode = lastCode;
    }
    while (i < length) {
        uint8_t Pixel = buffer[i++];
        int NewCode;
        uint32_t NewKey = (((uint32_t)CrntCode) << BITS_IN_BYTE) + Pixel;
        if ((NewCode = IsInDictionary(NewKey)) >= 0) {
            CrntCode = NewCode;
        } else {
            if (LZWWriteOut(CrntCode)) {
                IMAGE_LOGE("Failed to write.");
                return ERROR;
            }
            CrntCode = Pixel;
            if (RunningCode >= LZ_MAX_CODE) {
                if (LZWWriteOut(ClearCode)) {
                    IMAGE_LOGE("Failed to write.");
                    return ERROR;
                }
                RunningCode = EOFCode + 1;
                RunningBits = BITS_IN_BYTE + 1;
                MaxCode1 = 1 << RunningBits;
                memset_s(dictionary, sizeof(uint32_t) * DICTIONARY_SIZE, 0xFF, sizeof(uint32_t) * DICTIONARY_SIZE);
            } else {
                AddToDictionary(NewKey, RunningCode++);
            }
        }
    }
    lastCode = CrntCode;
    return SUCCESS;
}

uint32_t GifEncoder::LZWWriteOut(int Code)
{
    uint32_t ret = SUCCESS;
    if (Code == FLUSH_OUTPUT) {
        while (CrntShiftState > 0) {
            if (LZWBufferOutput(CrntShiftDWord & 0xFF)) {
                ret = ERROR;
            }
            CrntShiftDWord >>= BITS_IN_BYTE;
            CrntShiftState -= BITS_IN_BYTE;
        }
        CrntShiftState = 0;
        if (LZWBufferOutput(FLUSH_OUTPUT)) {
            ret = ERROR;
        }
    } else {
        CrntShiftDWord |= ((long)Code) << CrntShiftState;
        CrntShiftState += RunningBits;
        while (CrntShiftState >= BITS_IN_BYTE) {
            if (LZWBufferOutput(CrntShiftDWord & 0xFF)) {
                ret = ERROR;
            }
            CrntShiftDWord >>= BITS_IN_BYTE;
            CrntShiftState -= BITS_IN_BYTE;
        }
    }
    if (RunningCode >= MaxCode1 && Code <= LZ_MAX_CODE) {
        MaxCode1 = 1 << ++RunningBits;
    }
    return ret;
}

uint32_t GifEncoder::LZWBufferOutput(int c)
{
    if (c == FLUSH_OUTPUT) {
        if (outputLZWBuffer[0] != 0 && !Write(outputLZWBuffer, outputLZWBuffer[0] + 1)) {
            return ERROR;
        }
        outputLZWBuffer[0] = 0;
        if (!Write(outputLZWBuffer, 1)) {
            return ERROR;
        }
    } else {
        if (outputLZWBuffer[0] == 0xFF) {
            if (!Write(outputLZWBuffer, outputLZWBuffer[0] + 1)) {
                return ERROR;
            }
            outputLZWBuffer[0] = 0;
        }
        outputLZWBuffer[++outputLZWBuffer[0]] = c;
    }
    return SUCCESS;
}

bool GifEncoder::Write(const uint8_t* data, size_t data_size)
{
    return outputStream_->Write(data, data_size);
}

} // namespace ImagePlugin
} // namespace OHOS