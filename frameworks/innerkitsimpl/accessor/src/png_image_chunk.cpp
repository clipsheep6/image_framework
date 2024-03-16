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

#include <zlib.h>
#include <libexif/exif-data.h>
#include "data_buf.h"
#include "exif_metadata.h"
#include "image_stream.h"
#include "image_log.h"
#include "media_errors.h"
#include "png_image_chunk.h"
#include "tiff_parser.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "PngImageChunk"

namespace OHOS {
namespace Media {
namespace {
    constexpr auto ASCII_TO_HEX_MAP_SIZE = 103;
    constexpr auto IMAGE_SEG_MAX_SIZE = 65536;
    constexpr auto EXIF_HEADER_SIZE = 6;
    constexpr auto EXIF_BYTEORDER_SIZE = 4;
    constexpr auto PNG_CHUNK_KEYWORD_EXIF_APP1_SIZE = 21;
    constexpr auto HEX_BASE = 16;
    constexpr auto DECIMAL_BASE = 10;
    constexpr auto PNG_PROFILE_EXIF = "Raw profile type exif";
    constexpr auto PNG_PROFILE_APP1 = "Raw profile type APP1";
    constexpr auto CHUNK_COMPRESS_METHOD_VALID = 0;
    constexpr auto CHUNK_FLAG_COMPRESS_NO = 0;
    constexpr auto CHUNK_FLAG_COMPRESS_YES = 1;
    constexpr auto NULL_CHAR_AMOUNT = 2;
    constexpr auto HEX_STRING_UNIT_SIZE = 2;
}

int PngImageChunk::ParseTextChunk(const DataBuf &chunkData, TextChunkType chunkType, DataBuf &tiffData)
{
    DataBuf keyword = GetKeywordFromChunk(chunkData);
    if (keyword.Empty()) {
        IMAGE_LOGE("Failed to read the keyword from chunk.");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }

    DataBuf rawText = GetRawTextFromChunk(chunkData, keyword.Size(), chunkType);
    if (rawText.Empty()) {
        IMAGE_LOGE("Failed to read the raw text from chunk.");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }

    bool foundExifKeyword = FindExifKeyword(keyword.CData());
    if (!foundExifKeyword) {
        IMAGE_LOGI("Ignore the text chunk which without exif keyword");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    return GetTiffDataFromRawText(rawText, tiffData);
}

DataBuf PngImageChunk::GetKeywordFromChunk(const DataBuf &chunkData)
{
    if (chunkData.Size() <= 0) {
        IMAGE_LOGE("Check data size failed: offset bigger than data size.");
        return {};
    }

    auto keyword = std::find(chunkData.CBegin(), chunkData.CEnd(), 0);
    if (keyword == chunkData.CEnd()) {
        IMAGE_LOGE("Lookup key failed: not found key.");
        return {};
    }
    const size_t keywordLength = std::distance(chunkData.CBegin(), keyword);
    return {chunkData.CData(), keywordLength};
}

DataBuf PngImageChunk::GetRawTextFromZtxtChunk(const DataBuf &chunkData, size_t keySize, DataBuf &rawText)
{
    if (*(chunkData.CData(keySize + 1)) != CHUNK_COMPRESS_METHOD_VALID) {
        IMAGE_LOGE("Corrupted Metadata: invalid compression method.");
        return {};
    }

    size_t compressedTextSize = chunkData.Size() - keySize - NULL_CHAR_AMOUNT;
    if (compressedTextSize > 0) {
        const byte* compressedText = chunkData.CData(keySize + NULL_CHAR_AMOUNT);
        int ret = DecompressText(compressedText, static_cast<uint32_t>(compressedTextSize), rawText);
        if (ret != 0) {
            IMAGE_LOGE("Decompress text failed.");
            return {};
        }
    }
    return rawText;
}

DataBuf PngImageChunk::GetRawTextFromTextChunk(const DataBuf &chunkData, size_t keySize, DataBuf &rawText)
{
    size_t rawTextsize = chunkData.Size() - keySize - 1;
    if (rawTextsize) {
        const byte* textPosition = chunkData.CData(keySize + 1);
        rawText = DataBuf(textPosition, rawTextsize);
    }
    return rawText;
}

std::string FetchString(const char* chunkData, size_t data_length)
{
    if (data_length == 0) {
        IMAGE_LOGE("Data_length is 0.");
        return {};
    }
    const size_t stringLength = strnlen(chunkData, data_length);
    return {chunkData, stringLength};
}

DataBuf PngImageChunk::GetRawTextFromItxtChunk(const DataBuf &chunkData, size_t keySize, DataBuf &rawText)
{
    const size_t nullCount = std::count(chunkData.CData(keySize + 3), chunkData.CData(chunkData.Size() - 1), '\0');
    if (nullCount < NULL_CHAR_AMOUNT) {
        IMAGE_LOGE("Corrupted Metadata: the null character after Language tag is less then 2");
        return {};
    }

    const byte compressionFlag = chunkData.ReadUInt8(keySize + 1);
    const byte compressionMethod = chunkData.ReadUInt8(keySize + 2);
    if ((compressionFlag != CHUNK_FLAG_COMPRESS_NO) && (compressionFlag != CHUNK_FLAG_COMPRESS_YES)) {
        IMAGE_LOGE("Corrupted Metadata: the compression flag is invalid");
        return {};
    }

    if ((compressionFlag == CHUNK_FLAG_COMPRESS_YES) && (compressionMethod != CHUNK_COMPRESS_METHOD_VALID)) {
        IMAGE_LOGE("Corrupted Metadata: invalid compression method.");
        return {};
    }

    const size_t languageTextPos = keySize + 3;
    const size_t languageTextMaxLen = chunkData.Size() - keySize - 3;
    std::string languageText = FetchString(reinterpret_cast<const char*>(chunkData.CData(languageTextPos)),
                                           languageTextMaxLen);
    const size_t languageTextLen = languageText.size();

    const size_t translatedKeyPos = languageTextPos + languageTextLen + 1;
    std::string translatedKeyText = FetchString(reinterpret_cast<const char*>(chunkData.CData(translatedKeyPos)),
                                                chunkData.Size() - translatedKeyPos);
    const size_t translatedKeyTextLen = translatedKeyText.size();

    const size_t textLen = chunkData.Size() - (keySize + 3 + languageTextLen + 1 + translatedKeyTextLen + 1);
    if (textLen <= 0) {
        return {};
    }

    const size_t textPosition = translatedKeyPos + translatedKeyTextLen + 1;
    const byte* textPtr = chunkData.CData(textPosition);
    if (compressionFlag == CHUNK_FLAG_COMPRESS_NO) {
        rawText = DataBuf(textPtr, textLen);
    } else {
        int ret = DecompressText(textPtr, textLen, rawText);
        if (ret != 0) {
            IMAGE_LOGE("Decompress text failed.");
            return {};
        }
    }
    return rawText;
}

DataBuf PngImageChunk::GetRawTextFromChunk(const DataBuf &chunkData, size_t keySize, TextChunkType chunkType)
{
    DataBuf rawText;

    if (chunkType == zTXt_Chunk) {
        GetRawTextFromZtxtChunk(chunkData, keySize, rawText);
    } else if (chunkType == tEXt_Chunk) {
        GetRawTextFromTextChunk(chunkData, keySize, rawText);
    } else if (chunkType == iTXt_Chunk) {
        GetRawTextFromItxtChunk(chunkData, keySize, rawText);
    } else {
        IMAGE_LOGE("Unexpected chunk.");
        return {};
    }
    return rawText;
}

bool PngImageChunk::FindExifKeyword(const byte* keyword)
{
    if ((memcmp(PNG_PROFILE_EXIF, keyword, PNG_CHUNK_KEYWORD_EXIF_APP1_SIZE) == 0) ||
        (memcmp(PNG_PROFILE_APP1, keyword, PNG_CHUNK_KEYWORD_EXIF_APP1_SIZE) == 0)) {
        return true;
    }
    return false;
}

size_t PngImageChunk::VerifyExifIdCode(DataBuf &exifInfo, size_t exifInfoLength)
{
    static const std::array<byte, EXIF_HEADER_SIZE> exifIdCode{0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
    size_t exifIdPos = std::numeric_limits<size_t>::max();

    for (size_t i = 0; i < exifInfoLength - exifIdCode.size(); i++) {
        if (exifInfo.CmpBytes(i, exifIdCode.data(), exifIdCode.size()) == 0) {
            exifIdPos = i;
            break;
        }
    }
    return exifIdPos;
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

int PngImageChunk::GetTiffDataFromRawText(const DataBuf &rawText, DataBuf &tiffData)
{
    DataBuf exifInfo = ConvertRawTextToExifInfo(rawText);
    if (exifInfo.Empty()) {
        IMAGE_LOGE("Failed to parse Exif metadata: cannot convert text to hex");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    size_t exifInfoLength = exifInfo.Size();
    if (exifInfoLength < EXIF_HEADER_SIZE) {
        IMAGE_LOGE("Failed to parse Exif metadata: data length is too short");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }

    size_t exifHeadPos = VerifyExifIdCode(exifInfo, exifInfoLength);
    if (exifHeadPos == std::numeric_limits<size_t>::max()) {
        IMAGE_LOGE("Failed to decode parse metadata: cannot found exif header");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }

    size_t byteOrderPos = FindTiffPos(exifInfo, exifInfoLength);
    if (byteOrderPos == std::numeric_limits<size_t>::max()) {
        IMAGE_LOGE("Failed to parse Exif metadata: cannot find tiff byte order");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    tiffData = DataBuf(exifInfo.CData(byteOrderPos), exifInfoLength - byteOrderPos);
    return SUCCESS;
}

int PngImageChunk::DecompressText(const byte* sourceData, unsigned int sourceDataLen, DataBuf &textOut)
{
    if (sourceDataLen > IMAGE_SEG_MAX_SIZE) {
        IMAGE_LOGE("Failed to uncompress: data is too huge.");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    uLongf destDataLen = IMAGE_SEG_MAX_SIZE;

    textOut.Alloc(destDataLen);
    int result = uncompress(textOut.Data(), &destDataLen, sourceData, sourceDataLen);
    if (result != Z_OK) {
        IMAGE_LOGE("Failed to uncompress: the uncompress job abort.");
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    textOut.Resize(destDataLen);
    return SUCCESS;
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
    while ((*sourcePtr == '\0') || (*sourcePtr == ' ') || (*sourcePtr == '\n')) {
        sourcePtr++;
        if (sourcePtr == endPtr) {
            IMAGE_LOGE("Failed to get exif length: only blank content.");
            return NULL;
        }
    }

    size_t exifLength = 0;
    while (('0' <= *sourcePtr) && (*sourcePtr <= '9')) {
        const size_t newlength = (DECIMAL_BASE * exifLength) + (*sourcePtr - '0');
        exifLength = newlength;
        sourcePtr++;
        if (sourcePtr == endPtr) {
            IMAGE_LOGE("Failed to get exif length: no digit content.");
            return NULL;
        }
    }
    sourcePtr++; // ignore the '\n' character
    if (sourcePtr == endPtr) {
        IMAGE_LOGE("Failed to get exif length: exif info don't exist.");
        return NULL;
    }
    *lengthOut = exifLength;
    return sourcePtr;
}

int PngImageChunk::AsciiToInt(const char* sourcePtr, size_t exifInfoLength, unsigned char *destPtr)
{
    static const unsigned char hexAsciiToInt[ASCII_TO_HEX_MAP_SIZE] = {
        0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,    0, 0, 0, 0, 1,    2, 3, 4, 5, 6,    7, 8, 9, 0, 0,
        0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 10, 11, 12,
        13, 14, 15,
    };

    size_t sourceLength = exifInfoLength * 2;
    for (size_t i = 0; i < sourceLength; i++) {
        while ((*sourcePtr < '0') || ((*sourcePtr > '9') && (*sourcePtr < 'a')) || (*sourcePtr > 'f')) {
            if (*sourcePtr == '\0') {
                IMAGE_LOGE("Failed to convert exif ascii string: unexpected null character.");
                return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
            }
            sourcePtr++;
        }

        if ((i % HEX_STRING_UNIT_SIZE) == 0) {
            *destPtr = static_cast<unsigned char>(HEX_BASE * hexAsciiToInt[static_cast<size_t>(*sourcePtr++)]);
        } else {
            (*destPtr++) += hexAsciiToInt[static_cast<size_t>(*sourcePtr++)];
        }
    }
    return SUCCESS;
}

DataBuf PngImageChunk::ConvertRawTextToExifInfo(const DataBuf &rawText)
{
    if (rawText.Size() <= 1) {
        IMAGE_LOGE("Raw profile text size is too small.");
        return {};
    }
    const char* sourcePtr = reinterpret_cast<const char*>(rawText.CData(1));
    const char* endPtr = reinterpret_cast<const char*>(rawText.CData(rawText.Size() - 1));

    if (sourcePtr >= endPtr) {
        IMAGE_LOGE("Source pointer is invalid.");
        return {};
    }
    sourcePtr = StepOverNewLine(sourcePtr, endPtr);
    if (sourcePtr == NULL) {
        IMAGE_LOGE("Failed to copy raw profile text: meet error when step over new line.");
        return {};
    }

    size_t exifInfoLength = 0;
    sourcePtr = GetExifInfoLen(sourcePtr, &exifInfoLength, endPtr);
    if (sourcePtr == NULL) {
        IMAGE_LOGE("Failed to copy raw profile text: meet error when get string len.");
        return {};
    }

    if ((exifInfoLength == 0) || (exifInfoLength > rawText.Size())) {
        IMAGE_LOGE("Failed to copy raw profile text: invalid text length.");
        return {};
    }

    DataBuf exifInfo;
    exifInfo.Alloc(exifInfoLength);
    if (exifInfo.Size() != exifInfoLength) {
        IMAGE_LOGE("Failed to copy raw profile text: cannot allocate memory.");
        return {};
    }
    if (exifInfo.Empty()) {
        return exifInfo;
    }
    unsigned char* destPtr = exifInfo.Data();
    int ret = AsciiToInt(sourcePtr, exifInfoLength, destPtr);
    if (ret != 0) {
        IMAGE_LOGE("Failed to convert exif string ascii to int.");
        return {};
    }

    return exifInfo;
}

} // namespace Media
} // namespace OHOS
