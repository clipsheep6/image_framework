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

#include "png_image_chunk.h"
#include "image_stream.h"
#include "image_log.h"
#include "data_buf.h"
#include "tiff_parser.h"
#include "exif_metadata.h"
#include "libexif/exif-data.h"

#include "zlib.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PngImageChunk"

namespace OHOS {
namespace Media {

namespace {
constexpr size_t nullSeparators = 2;
}

int PngImageChunk::ParseTextChunk(const DataBuf &chunkData, TextChunkType type, DataBuf &tiffData)
{
    DataBuf keyword = GetKeyFromChunk(chunkData);
    if (keyword.empty()) {
        IMAGE_LOGE("Error:failed to read the keyword from chunk.");
        return 1;
    }

    DataBuf rawText = GetRawTextFromChunk(chunkData, keyword.size(), type);
    if (!rawText.empty()) {
        int ret = GetTiffDataFromRawText(keyword.C_Data(), keyword.size(), rawText, tiffData);
        return ret;
    }
    IMAGE_LOGE("Error:failed to read the raw text from chunk.");
    return 1;
}

DataBuf PngImageChunk::GetKeyFromChunk(const DataBuf &chunkData)
{
    if (chunkData.size() <= 0) {
        IMAGE_LOGE("Error:check data size failed: offset bigger than data size.");
        return {};
    }

    auto it = std::find(chunkData.cbegin(), chunkData.cend(), 0);
    if (it == chunkData.cend()) {
        IMAGE_LOGE("Error:lookup key failed: not found key.");
        return {};
    }

    return {chunkData.C_Data(), std::distance(chunkData.cbegin(), it)};
}

DataBuf PngImageChunk::GetRawTextFromZtxtChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type, DataBuf &rawText)
{
    if (*(chunkData.C_Data(keysize + 1)) != TEXT_CHUNK_COMPRESS_METHOD_VALID_VALUE) {
        IMAGE_LOGE("Error:corrupted Metadata: invalid compression method.");
        return {};
    }

    size_t compressedTextSize = chunkData.size() - keysize - nullSeparators;
    if (compressedTextSize) {
        const byte* compressedText = chunkData.C_Data(keysize + nullSeparators);
        int ret = DecompressText(compressedText, static_cast<uint32_t>(compressedTextSize), rawText);
        if (ret != 0) {
            IMAGE_LOGE("Error:decompress text failed.");
            return {};
        }
    }
    return rawText;
}

DataBuf PngImageChunk::GetRawTextFromTextChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type, DataBuf &rawText)
{
    size_t rawTextsize = chunkData.size() - keysize - 1;
    if (rawTextsize) {
        const byte* textPosition = chunkData.C_Data(keysize + 1);
        rawText = DataBuf(textPosition, rawTextsize);
    }
    return rawText;
}

std::string FetchStringWithNullCharEnd(const char* chunkData, size_t data_length)
{
    if (data_length == 0) {
        IMAGE_LOGE("Error:data_length is 0.");
        return {};
    }
    const size_t StringLength = strnlen(chunkData, data_length);
    return {chunkData, StringLength};
}

DataBuf PngImageChunk::GetRawTextFromItxtChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type, DataBuf &rawText)
{
    const size_t nullCount = std::count(chunkData.C_Data(keysize + 3), chunkData.C_Data(chunkData.size() - 1), '\0');
    if (nullCount < nullSeparators) {
        IMAGE_LOGE("Error:corrupted Metadata: the null character after Language tag is less then 2");
        return {};
    }

    const byte compressionFlag = chunkData.ReadUInt8(keysize + 1);
    const byte compressionMethod = chunkData.ReadUInt8(keysize + 2);
    if ((compressionFlag != TEXT_CHUNK_COMPRESS_FLAG_UNCOMPRESSED) && (compressionFlag != TEXT_CHUNK_COMPRESS_FLAG_COMPRESSED)) {
        IMAGE_LOGE("Error:corrupted Metadata: the compression flag is invalid");
        return {};
    }

    if ((compressionFlag == TEXT_CHUNK_COMPRESS_FLAG_COMPRESSED) && (compressionMethod != TEXT_CHUNK_COMPRESS_METHOD_VALID_VALUE)){
        IMAGE_LOGE("Error:corrupted Metadata: invalid compression method.");
        return {};
    }

    const size_t languageTextPos = keysize + 3;
    const size_t languageTextMaxLen = chunkData.size() - keysize - 3;
    std::string languageText = FetchStringWithNullCharEnd(reinterpret_cast<const char*>(chunkData.C_Data(languageTextPos)), languageTextMaxLen);
    const size_t languageTextLen = languageText.size();

    const size_t translatedKeyPos = languageTextPos + languageTextLen + 1;
    std::string translatedKeyText = FetchStringWithNullCharEnd(reinterpret_cast<const char*>(chunkData.C_Data(translatedKeyPos)),
                                                             chunkData.size() - translatedKeyPos);
    const size_t translatedKeyTextLen = translatedKeyText.size();

    const size_t textLen = chunkData.size() - (keysize + 3 + languageTextLen + 1 + translatedKeyTextLen + 1);
    if (textLen) {
        const size_t textPosition = translatedKeyPos + translatedKeyTextLen + 1;
        const byte* textPtr = chunkData.C_Data(textPosition);
        if (compressionFlag == TEXT_CHUNK_COMPRESS_FLAG_UNCOMPRESSED) {
            rawText = DataBuf(textPtr, textLen);
        } else {
            int ret = DecompressText(textPtr, textLen, rawText);
            if (ret != 0) {
                IMAGE_LOGE("Error:decompress text failed.");
                return {};
            }
        }
    }
    return rawText;
}

DataBuf PngImageChunk::GetRawTextFromChunk(const DataBuf &chunkData, size_t keysize, TextChunkType type)
{
    DataBuf rawText;

    if (type == zTXt_Chunk) {
        GetRawTextFromZtxtChunk(chunkData, keysize, type, rawText);
    } else if (type == tEXt_Chunk) {
        GetRawTextFromTextChunk(chunkData, keysize, type, rawText);
    } else if (type == iTXt_Chunk) {
        GetRawTextFromItxtChunk(chunkData, keysize, type, rawText);
    } else {
        IMAGE_LOGE("Error:unexpected chunk.");
        return {};
    }
    return rawText;
}

size_t PngImageChunk::VerifyExifIdCode(DataBuf &exifInfo, size_t exifInfoLength)
{
    static const std::array<byte, EXIF_HEADER_SIZE> exifHeader{0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
    size_t exifHeadPos = std::numeric_limits<size_t>::max();

    for (size_t i = 0; i < exifInfoLength - exifHeader.size(); i++) {
        if (exifInfo.CmpBytes(i, exifHeader.data(), exifHeader.size()) == 0) {
            exifHeadPos = i;
            break;
        }
    }
    return exifHeadPos;
}

size_t PngImageChunk::FindTiffPos(DataBuf &exifInfo, size_t exifInfoLength)
{
    static const std::array<byte, EXIF_BYTEORDER_SIZE> tiffByteOrderII{0x49, 0x49, 0x2a, 0x00};
    static const std::array<byte, EXIF_BYTEORDER_SIZE> tiffByteOrderMM{0x4d, 0x4d, 0x00, 0x2a};
    size_t byteOrderPos = std::numeric_limits<size_t>::max();

    for (size_t i = 0; i < exifInfoLength - tiffByteOrderII.size(); i++) {
        if (exifInfo.CmpBytes(i, tiffByteOrderII.data(), tiffByteOrderII.size()) == 0) {
            byteOrderPos = i;
            break;
        }
    }
    if (byteOrderPos == std::numeric_limits<size_t>::max()) {
        for (size_t i = 0; i < exifInfoLength - tiffByteOrderMM.size(); i++) {
            if (exifInfo.CmpBytes(i, tiffByteOrderMM.data(), tiffByteOrderMM.size()) == 0) {
                byteOrderPos = i;
                break;
            }
        }
    }
    return byteOrderPos;
}

int PngImageChunk::GetTiffDataFromRawText(const byte* key, size_t keySize, const DataBuf &rawText, DataBuf &tiffData)
{
    if ((memcmp(PNG_PROFILE_EXIF, key, PNG_CHUNK_KEYWORD_EXIF_APP1_SIZE) == 0) ||
        (memcmp(PNG_PROFILE_APP1, key, PNG_CHUNK_KEYWORD_EXIF_APP1_SIZE) == 0)) {
        DataBuf exifInfo = ConvertRawTextToExifInfo(rawText);
        if (exifInfo.empty()) {
            IMAGE_LOGE("Error:failed to parse Exif metadata: cannot convert text to hex");
            return 1;
        }
        size_t exifInfoLength = exifInfo.size();

        if (exifInfoLength < EXIF_HEADER_SIZE) {
            IMAGE_LOGE("Error:failed to parse Exif metadata: data length is too short");
            return 1;
        }

        size_t exifHeadPos = VerifyExifIdCode(exifInfo, exifInfoLength);
        if (exifHeadPos != std::numeric_limits<size_t>::max()) {
            size_t byteOrderPos = FindTiffPos(exifInfo, exifInfoLength);
            if (byteOrderPos == std::numeric_limits<size_t>::max()) {
                IMAGE_LOGE("Error:failed to parse Exif metadata: cannot find tiff byte order");
                return 1;
            }
            tiffData = DataBuf(exifInfo.C_Data(byteOrderPos), exifInfoLength - byteOrderPos);
            return 0;
        } else {
            IMAGE_LOGE("Error:failed to decode parse metadata: cannot found exif header");
            return 1;
        }
    }
    return 1;
}

int PngImageChunk::DecompressText(const byte* sourceData, unsigned int sourceDataLen, DataBuf &textOut)
{
    if (sourceDataLen > IMAGE_SEG_MAX_SIZE) {
        IMAGE_LOGE("Error:failed to uncompress: data is too huge.");
        return 1;
    }
    uLongf destDataLen = IMAGE_SEG_MAX_SIZE;

    textOut.Alloc(destDataLen);
    int result = uncompress(textOut.Data(), &destDataLen, sourceData, sourceDataLen);
    if (result != Z_OK) {
        IMAGE_LOGE("Error:failed to uncompress: the uncompress job abort.");
        return 1;
    }
    textOut.Resize(destDataLen);
    return 0;
}

const char* PngImageChunk::StepOverNewLine(const char* sourcePtr, const char* endPtr)
{
    while (*sourcePtr != '\n') {
        sourcePtr++;
        if (sourcePtr == endPtr) {
            return NULL;
        }
    }
    sourcePtr++;
    if (sourcePtr == endPtr) {
        return NULL;
    }
    return sourcePtr;
}

const char* PngImageChunk::GetExifInfoLen(const char* sourcePtr, size_t *lengthOut, const char* endPtr)
{
      while (*sourcePtr == '\0' || *sourcePtr == ' ' || *sourcePtr == '\n') {
          sourcePtr++;
          if (sourcePtr == endPtr) {
              IMAGE_LOGE("Error:exif info don't exist.");
              return NULL;
          }
      }

      size_t exifLength = 0;
      while (('0' <= *sourcePtr) && (*sourcePtr <= '9')) {
          const size_t newlength = (DECIMAL_BASE * exifLength) + (*sourcePtr - '0');
          exifLength = newlength;
          sourcePtr++;
          if (sourcePtr == endPtr) {
              IMAGE_LOGE("Error:exif info don't exist.");
              return NULL;
          }
      }
      sourcePtr++;  // ignore '\n'
      if (sourcePtr == endPtr) {
          IMAGE_LOGE("Error:exif info don't exist.");
          return NULL;
      }
      *lengthOut = exifLength;
      return sourcePtr;
}

int PngImageChunk::AsciiToInt(const char* sourcePtr, size_t exifInfoLength, unsigned char *destPtr)
{
    static const unsigned char asciiToInt[ASCII_TO_HEX_MAP_SIZE] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15,
    };

    size_t sourceLength = exifInfoLength * 2;
    for (size_t i = 0; i < sourceLength; i++) {
        while ((*sourcePtr < ASCII_0) || ((*sourcePtr > ASCII_9) && (*sourcePtr < ASCII_a)) || (*sourcePtr > ASCII_f)) {
            if (*sourcePtr == '\0') {
                IMAGE_LOGE("Error:failed to convert exif ascii string: unexpected null character.");
                return 1;
            }
            sourcePtr++;
        }

        if ((i % 2) == 0) {
            *destPtr = static_cast<unsigned char>(HEX_BASE * asciiToInt[static_cast<size_t>(*sourcePtr++)]);
        } else {
            (*destPtr++) += asciiToInt[static_cast<size_t>(*sourcePtr++)];
        }
    }
    return 0;
}

DataBuf PngImageChunk::ConvertRawTextToExifInfo(const DataBuf &rawText)
{
    if (rawText.size() <= 1) {
        IMAGE_LOGE("Error:raw profile text size is too small.");
        return {};
    }
    const char* sourcePtr = reinterpret_cast<const char*>(rawText.C_Data(1));
    const char* endPtr = reinterpret_cast<const char*>(rawText.C_Data(rawText.size() - 1));

    if (sourcePtr >= endPtr) {
        IMAGE_LOGE("Error:source pointer is invalid.");
        return {};
    }
    sourcePtr = StepOverNewLine(sourcePtr, endPtr);
    if (sourcePtr == NULL) {
        IMAGE_LOGE("Error:failed to copy raw profile text: meet error when step over new line.");
        return {};
    }

    size_t exifInfoLength = 0;
    sourcePtr = GetExifInfoLen(sourcePtr, &exifInfoLength, endPtr);
    if (sourcePtr == NULL) {
        IMAGE_LOGE("Error:failed to copy raw profile text: meet error when get string len.");
        return {};
    }

    if ((exifInfoLength == 0) || (exifInfoLength > rawText.size())) {
        IMAGE_LOGE("Error:failed to copy raw profile text: invalid text length.");
        return {};
    }

    DataBuf exifInfo;
    exifInfo.Alloc(exifInfoLength);
    if (exifInfo.size() != exifInfoLength) {
        IMAGE_LOGE("Error:failed to copy raw profile text: cannot allocate memory.");
        return {};
    }
    if (exifInfo.empty()) {
        return exifInfo;
    }
    unsigned char* destPtr = exifInfo.Data();
    int ret = AsciiToInt(sourcePtr, exifInfoLength, destPtr);
    if (ret != 0) {
        IMAGE_LOGE("Error:failed to convert exif string ascii to int.");
        return {};
    }

    return exifInfo;
}

} // namespace Media
} // namespace OHOS
