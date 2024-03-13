/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef PNG_IMAGE_CHUNK_H_
#define PNG_IMAGE_CHUNK_H_

#include <libexif/exif-data.h>
#include <algorithm>
#include <map>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdint.h>
#include <vector>

#include "data_buf.h"

namespace OHOS {
namespace Media {

#define PNG_IMAGE_CHUNK_DEBUG            0
#define ASCII_TO_HEX_MAP_SIZE            103
#define IMAGE_SEG_MAX_SIZE               65536
#define EXIF_HEADER_SIZE                 6
#define EXIF_BYTEORDER_SIZE              4
#define PNG_CHUNK_KEYWORD_EXIF_APP1_SIZE 21
#define HEX_BASE                         16
#define DECIMAL_BASE                     10
#define ASCII_0                          '0'
#define ASCII_9                          '9'
#define ASCII_a                          'a'
#define ASCII_f                          'f'
#define PNG_PROFILE_EXIF                 "Raw profile type exif"
#define PNG_PROFILE_APP1                 "Raw profile type APP1"

#define TEXT_CHUNK_COMPRESS_METHOD_VALID_VALUE 0
#define TEXT_CHUNK_COMPRESS_FLAG_UNCOMPRESSED  0
#define TEXT_CHUNK_COMPRESS_FLAG_COMPRESSED    1

class PngImageChunk {
public:
    // type of png text chunk
    enum TextChunkType { tEXt_Chunk = 0, zTXt_Chunk = 1, iTXt_Chunk = 2 };

    // parse exif meta data from text type chunk (tEXt/zTXt/iTXt)
    static int ParseTextChunk(const DataBuf &chunkData, TextChunkType type, DataBuf &tiffData);

    // lookup tiff marker
    static size_t FindTiffPos(DataBuf &exifInfo, size_t exifInfoLength);

private:
    // get keyword from png txt chunk
    static DataBuf GetKeyFromChunk(const DataBuf &chunkData);

    // get data from zTxt chunk
    static DataBuf GetRawTextFromZtxtChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type, DataBuf &rawText);

    // get data from tExt chunk
    static DataBuf GetRawTextFromTextChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type, DataBuf &rawText);

    // get data from iTxt chunk
    static DataBuf GetRawTextFromItxtChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type, DataBuf &rawText);

    // get text field from png txt chunk
    static DataBuf GetRawTextFromChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type);

    // lookup exif marker
    static size_t VerifyExifIdCode(DataBuf &exifInfo, size_t exifInfoLength);

    // parse Exif Tiff metadata from data in png txt chunk
    static int GetTiffDataFromRawText(const byte* key, size_t keySize, const DataBuf &rawText, DataBuf &tiffData);

    // decompress with zlib
    static int DecompressText(const byte* sourceData, unsigned int sourceDataLen, DataBuf &textOut);

    // step over newline character
    static const char* StepOverNewLine(const char* sourcePtr, const char* endPtr);

    // get exif length
    static const char* GetExifInfoLen(const char* sourcePtr, size_t *lengthOut, const char* endPtr);

    // convert sting to digit
    static int AsciiToInt(const char* sourcePtr, size_t exifInfoLength, unsigned char *destPtr);

    // convert Exif metadata from Ascii char to hex
    static DataBuf ConvertRawTextToExifInfo(const DataBuf &rawText);
};
} // namespace Media
} // namespace OHOS

#endif  // PNG_IMAGE_CHUNK_H_
