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
#ifndef GIF_ENCODER_H
#define GIF_ENCODER_H
#include <vector>
#include "abs_image_encoder.h"
#include "plugin_class_base.h"
namespace OHOS {
namespace ImagePlugin {

#define BITS_IN_BYTE 8
#define BITS_PER_PRIM_COLOR 5
#define R_IN_RGB 0
#define G_IN_RGB 1
#define B_IN_RGB 2
#define NUM_OF_RGB 3
#define COLOR_OF_GIF 256
#define COLOR_MAP_SIZE 256
#define COLOR_ARRAY_SIZE 32768
#define EXTENSION_INTRODUCER 0x21
#define APPLICATION_EXTENSION_LABEL 0xFF
#define GRAPHIC_CONTROL_LABEL 0xF9
#define IMAGE_SEPARATOR 0x2C
#define DICTIONARY_SIZE 8192
#define LZ_BITS 12
#define CLEAR_CODE 256
#define LZ_MAX_CODE 4095
#define FLUSH_OUTPUT 4096
#define FIRST_CODE 4097
#define DEFAULT_DELAY_TIME 100

const uint8_t GIF89_STAMP[] = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61};
const uint8_t applicationIdentifier[] = {0x4E, 0x45, 0x54, 0x53, 0x43, 0x41, 0x50, 0x45};
const uint8_t applicationAuthenticationCode[] = {0x32, 0x2E, 0x30};

#pragma pack(1)
typedef struct LogicalScreenDescriptor {
    uint16_t LogicalScreenWidth;
    uint16_t LogicalScreenHeight;
    uint8_t PackedFields;
    uint8_t BackgroundColorIndex;
    uint8_t PixelAspectRatio;
} LogicalScreenDescriptor;

typedef struct ApplicationExtension {
    uint8_t ExtensionIntroducer;
    uint8_t ExtensionLabel;
    uint8_t BlockSize;
    uint8_t ApplicationIdentifier[8];
    uint8_t ApplicationAuthenticationCode[3];
    uint8_t ApplicationDataSize;
    uint8_t ApplicationDataIndex;
    uint16_t LoopTime;
    uint8_t BlockTerminator;
} ApplicationExtension;

typedef struct GraphicControlExtension {
    uint8_t ExtensionIntroducer;
    uint8_t GraphicControlLabel;
    uint8_t BlockSize;
    uint8_t PackedFields;
    uint16_t DelayTime;
    uint8_t TransparentColorIndex;
    uint8_t BlockTerminator;
} GraphicControlExtension;

typedef struct ImageDescriptor {
    uint8_t ImageSeparator;
    uint16_t ImageLeftPosition;
    uint16_t ImageTopPosition;
    uint16_t ImageWidth;
    uint16_t ImageHeight;
    uint8_t PackedFields;
} ImageDescriptor;

typedef struct ColorType {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
} ColorType;

typedef struct QuantizedColorType {
    uint8_t RGB[3];
    uint8_t NewColorIndex;
    long Count;
    struct QuantizedColorType *Pnext;
} QuantizedColorType;

typedef struct NewColorMapType {
    uint8_t RGBMin[3], RGBWidth[3];
    uint32_t NumEntries;
    unsigned long Count;
    QuantizedColorType *QuantizedColors;
} NewColorMapType;
#pragma pack()

class GifEncoder : public AbsImageEncoder, public OHOS::MultimediaPlugin::PluginClassBase {
public:
    GifEncoder();
    ~GifEncoder() override;
    uint32_t StartEncode(OutputDataStream &outputStream, PlEncodeOptions &option) override;
    uint32_t AddImage(Media::PixelMap &pixelMap) override;
    uint32_t FinalizeEncode() override;
    bool Write(const uint8_t* data, size_t data_size);

private:
    DISALLOW_COPY_AND_MOVE(GifEncoder);
    uint32_t DoEncode();
    uint32_t WriteFileInfo();
    uint32_t WriteFrameInfo(int index);
    uint32_t processFrame(int index);
    uint32_t colorQuantize(int index, uint16_t width, uint16_t height,
                           uint8_t *outputBuffer, ColorType *outputColorMap);
    uint32_t separateRGB(int index, uint16_t width, uint16_t height,
                         uint8_t *redBuffer, uint8_t *greenBuffer, uint8_t *blueBuffer);
    static uint32_t GifQuantizeBuffer(uint16_t Width, uint16_t Height, int *ColorMapSize,
                                      const uint8_t *RedInput, const uint8_t *GreenInput, const uint8_t *BlueInput,
                                      uint8_t *outputBuffer, ColorType *OutputColorMap);
    static uint32_t SubdivColorMap(NewColorMapType *NewColorSubdiv,
                                   uint32_t ColorMapSize,
                                   uint32_t *NewColorMapSize);
    static int SortCmpRtn(const void *Entry1, const void *Entry2);
    void InitDictionary();
    int IsInDictionary(uint32_t Key);
    void AddToDictionary(uint32_t Key, int Code);
    uint32_t LZWEncodeFrame(uint8_t *outputBuffer, uint16_t width, uint16_t height);
    uint32_t LZWEncode(uint8_t *buffer, int length);
    uint32_t LZWWriteOut(int Code);
    uint32_t LZWBufferOutput(int c);

private:
    OutputDataStream *outputStream_ {nullptr};
    std::vector<Media::PixelMap *> pixelMaps_;
    PlEncodeOptions encodeOpts_;
    int lastCode;
    int EOFCode;
    int RunningCode;
    int ClearCode;
    int RunningBits;
    int MaxCode1;
    int CrntShiftState;
    uint32_t CrntShiftDWord;
    uint32_t dictionary[DICTIONARY_SIZE];
    uint8_t outputLZWBuffer[256];
};

} // namespace ImagePlugin
} // namespace OHOS

#endif // GIF_ENCODER_H