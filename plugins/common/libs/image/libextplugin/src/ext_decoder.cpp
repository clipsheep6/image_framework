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

#include "ext_decoder.h"

#include <algorithm>

#include "hilog/log.h"
#include "image_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "securec.h"
#include "string_ex.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "ExtDecoder"};
    constexpr static int32_t ZERO = 0;
    constexpr static size_t SIZE_ZERO = 0;
}

namespace OHOS {
namespace ImagePlugin {
using namespace Media;
using namespace OHOS::HiviewDFX;
using namespace std;
const static string CODEC_INITED_KEY = "CodecInited";
const static string ENCODED_FORMAT_KEY = "EncodedFormat";
const static string EXT_SHAREMEM_NAME = "EXT RawData";
const static string TAG_ORIENTATION_STRING = "Orientation";
const static string TAG_ORIENTATION_INT = "OrientationInt";
struct ColorTypeOutput
{
    PlPixelFormat outFormat;
    SkColorType skFormat;
};
static const map<PlPixelFormat, ColorTypeOutput> COLOR_TYPE_MAP = {
    { PlPixelFormat::UNKNOWN, { PlPixelFormat::RGBA_8888, kRGBA_8888_SkColorType } },
    { PlPixelFormat::RGBA_8888, { PlPixelFormat::RGBA_8888, kRGBA_8888_SkColorType } },
    { PlPixelFormat::BGRA_8888, { PlPixelFormat::BGRA_8888, kBGRA_8888_SkColorType } },
    { PlPixelFormat::ALPHA_8, { PlPixelFormat::ALPHA_8, kAlpha_8_SkColorType } },
    { PlPixelFormat::RGB_565, { PlPixelFormat::RGB_565, kRGB_565_SkColorType } },
};
static const map<PlAlphaType, SkAlphaType> ALPHA_TYPE_MAP = {
    { PlAlphaType::IMAGE_ALPHA_TYPE_OPAQUE, kOpaque_SkAlphaType },
    { PlAlphaType::IMAGE_ALPHA_TYPE_PREMUL, kPremul_SkAlphaType },
    { PlAlphaType::IMAGE_ALPHA_TYPE_UNPREMUL, kUnpremul_SkAlphaType },
};
static const map<SkEncodedImageFormat, string> FORMAT_NAME = {
    {SkEncodedImageFormat::kBMP, "image/bmp"},
    {SkEncodedImageFormat::kGIF, "image/gif"},
    {SkEncodedImageFormat::kICO, "image/png"},
    {SkEncodedImageFormat::kJPEG, "image/jpeg"},
    {SkEncodedImageFormat::kPNG, "image/png"},
    {SkEncodedImageFormat::kWBMP, "image/bmp"},
    {SkEncodedImageFormat::kWEBP, "image/webp"},
    {SkEncodedImageFormat::kPKM, ""},
    {SkEncodedImageFormat::kKTX, ""},
    {SkEncodedImageFormat::kASTC, ""},
    {SkEncodedImageFormat::kDNG, ""},
    {SkEncodedImageFormat::kHEIF, "image/heif"},
};
static void SetDecodeContextBuffer(DecodeContext &context,
    AllocatorType type, uint8_t* ptr, uint64_t count, int32_t* fd)
{
    context.allocatorType = type;
    context.freeFunc = nullptr;
    context.pixelsBuffer.buffer = ptr;
    context.pixelsBuffer.bufferSize = count;
    context.pixelsBuffer.context = fd;
}
static uint32_t ShareMemAlloc(DecodeContext &context, uint64_t count)
{
#if defined(_WIN32) || defined(_APPLE) || defined(A_PLATFORM) || defined(IOS_PLATFORM)
    HiLog::Error(LABEL, "Unsupport share mem alloc");
    return ERR_IMAGE_DATA_UNSUPPORT;
#else
    auto fd = make_unique<int32_t>();
    if (fd == nullptr) {
        HiLog::Error(LABEL, "Alloc Ashmem fd buffer failed");
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    *fd = AshmemCreate(EXT_SHAREMEM_NAME.c_str(), count);
    if (*fd < 0) {
        HiLog::Error(LABEL, "AshmemCreate failed");
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    int result = AshmemSetProt(*fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        ::close(*fd);
        HiLog::Error(LABEL, "AshmemSetProt failed");
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    void* ptr = ::mmap(nullptr, count, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, ZERO);
    if (ptr == MAP_FAILED) {
        ::close(*fd);
        HiLog::Error(LABEL, "::mmap failed");
        return ERR_SHAMEM_DATA_ABNORMAL;
    }
    SetDecodeContextBuffer(context,
        AllocatorType::SHARE_MEM_ALLOC, static_cast<uint8_t*>(ptr), count, fd.release());
    return SUCCESS;
#endif
}
static uint32_t HeapMemAlloc(DecodeContext &context, uint64_t count)
{
    auto out = make_unique<uint8_t[]>(count);
    if (out == nullptr) {
        HiLog::Error(LABEL,
            "Heap memory alloc failed with size %{public}llu", static_cast<unsigned long long>(count));
        return ERR_IMAGE_MALLOC_ABNORMAL;
    }
#ifdef _WIN32
    if (memset_s(out, ZERO, count) != EOK) {
#else
    if (memset_s(out.get(), count, ZERO, count) != EOK) {
#endif
        HiLog::Error(LABEL, "Decode failed, memset buffer failed");
        return ERR_IMAGE_DECODE_FAILED;
    }
    SetDecodeContextBuffer(context, AllocatorType::HEAP_ALLOC, out.release(), count, nullptr);
    return SUCCESS;
}

ExtDecoder::ExtDecoder() : codec_(nullptr), frameCount_(ZERO)
{
}
void ExtDecoder::SetSource(InputDataStream &sourceStream)
{
    stream_ = &sourceStream;
}
void ExtDecoder::Reset()
{
    stream_ = nullptr;
    codec_ = nullptr;
    dstInfo_.reset();
    dstSubset_ = SkIRect::MakeEmpty();
    info_.reset();
}
bool ExtDecoder::HasProperty(string key)
{
    if (CODEC_INITED_KEY.compare(key) == ZERO) {
        return CheckCodec();
    } else if (ENCODED_FORMAT_KEY.compare(key) == ZERO) {
        return true;
    }
    return false;
}

uint32_t ExtDecoder::GetImageSize(uint32_t index, PlSize &size)
{
    if (!CheckIndexVailed(index)) {
        HiLog::Error(LABEL, "Invalid index:%{public}u, range:%{public}d", index, frameCount_);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    // Info has been get in check process, or empty on get failed.
    if (info_.isEmpty()) {
        HiLog::Error(LABEL, "GetImageSize failed, decode header failed.");
        return ERR_IMAGE_DECODE_HEAD_ABNORMAL;
    }
    size.width = info_.width();
    size.height = info_.height();
    return SUCCESS;
}

uint32_t ExtDecoder::SetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info)
{
    if (!CheckIndexVailed(index)) {
        HiLog::Error(LABEL, "Invalid index:%{public}u, range:%{public}d", index, frameCount_);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    auto desireColor = ConvertToColorType(opts.desiredPixelFormat, info.pixelFormat);
    auto desireAlpha = ConvertToAlphaType(opts.desireAlphaType, info.alphaType);
    dstInfo_ = SkImageInfo::Make(opts.desiredSize.width,
        opts.desiredSize.height, desireColor, desireAlpha, info_.refColorSpace());
    if (ImageUtils::CheckMulOverflow(dstInfo_.width(), dstInfo_.height(), dstInfo_.bytesPerPixel())) {
        HiLog::Error(LABEL, "SetDecodeOptions failed, width:%{public}d, height:%{public}d is too large",
                     dstInfo_.width(), dstInfo_.height());
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    dstOptions_.fFrameIndex = index;
    if (opts.CropRect.width != ZERO && opts.CropRect.height != ZERO) {
        dstSubset_ = SkIRect::MakeXYWH(opts.CropRect.left, opts.CropRect.top,
            opts.CropRect.width, opts.CropRect.height);
        dstOptions_.fSubset = &dstSubset_;
    }
    info.size.width = dstInfo_.width();
    info.size.height = dstInfo_.height();
    return SUCCESS;
}

uint32_t ExtDecoder::SetContextPixelsBuffer(uint64_t byteCount, DecodeContext &context)
{
    if (byteCount == ZERO) {
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (context.allocatorType == Media::AllocatorType::SHARE_MEM_ALLOC) {
        return ShareMemAlloc(context, byteCount);
    }
    return HeapMemAlloc(context, byteCount);
}

uint32_t ExtDecoder::Decode(uint32_t index, DecodeContext &context)
{
    if (!CheckIndexVailed(index)) {
        HiLog::Error(LABEL, "Decode failed, invalid index:%{public}u, range:%{public}d", index, frameCount_);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    if (codec_ == nullptr) {
        HiLog::Error(LABEL, "Decode failed, codec is null");
        return ERR_IMAGE_DECODE_FAILED;
    }
    if (dstInfo_.isEmpty()) {
        HiLog::Error(LABEL, "Decode failed, dst info is empty");
        return ERR_IMAGE_DECODE_FAILED;
    }
    if (context.pixelsBuffer.buffer == nullptr) {
        uint64_t byteCount = static_cast<uint64_t>(dstInfo_.computeMinByteSize());
        HiLog::Debug(LABEL, "Decode alloc byte count %{public}llu.", byteCount);
        uint32_t res = SetContextPixelsBuffer(byteCount, context);
        if (res != SUCCESS) {
            return res;
        }
    }
    dstOptions_.fFrameIndex = index;
    uint8_t *dstBuffer = static_cast<uint8_t *>(context.pixelsBuffer.buffer);
    size_t rowBytes = dstInfo_.minRowBytes64();
    SkCodec::Result ret = codec_->getPixels(dstInfo_, dstBuffer, rowBytes, &dstOptions_);
    if (ret != SkCodec::kSuccess) {
        HiLog::Error(LABEL, "Decode failed, get pixels failed, ret=%{public}d", ret);
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    return SUCCESS;
}

uint32_t ExtDecoder::PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context)
{
    // currently not support increment decode
    return ERR_IMAGE_DATA_UNSUPPORT;
}

bool ExtDecoder::CheckCodec()
{
    if (codec_ != nullptr) {
        return true;
    } else if (stream_ == nullptr) {
        HiLog::Error(LABEL, "create codec: input stream is nullptr.");
        return false;
    } else if (stream_->GetStreamSize() == SIZE_ZERO) {
        HiLog::Error(LABEL, "create codec: input stream size is zero.");
        return false;
    }
    codec_ = SkCodec::MakeFromStream(make_unique<ExtStream>(stream_));
    if (codec_ == nullptr) {
        HiLog::Error(LABEL, "create codec from stream failed");
        return false;
    }
    return codec_ != nullptr;
}

bool ExtDecoder::DecodeHeader()
{
    if (!CheckCodec()) {
        HiLog::Error(LABEL, "Check codec failed");
        return false;
    }
    info_ = codec_->getInfo();
    frameCount_ = codec_->getFrameCount();
    HiLog::Debug(LABEL, "DecodeHeader: get frame count %{public}d.", frameCount_);
    return true;
}
bool ExtDecoder::CheckIndexVailed(uint32_t index)
{
    if (frameCount_ == ZERO && !DecodeHeader()) {
        return false;
    }
    return static_cast<int32_t>(index) < frameCount_;
}
static uint32_t GetFormatName(SkEncodedImageFormat format, std::string &name)
{
    auto formatNameIter = FORMAT_NAME.find(format);
    if (formatNameIter != FORMAT_NAME.end() && !formatNameIter->second.empty()) {
        name = formatNameIter->second;
        HiLog::Debug(LABEL, "GetFormatName: get encoded format name (%{public}d)=>[%{public}s].",
            format, name.c_str());
        return SUCCESS;
    }
    HiLog::Error(LABEL, "GetFormatName: get encoded format name failed %{public}d.", format);
    return ERR_IMAGE_DATA_UNSUPPORT;
}
// uint32_t ExtDecoder::GetImagePropertyString(uint32_t index, const std::string &key, std::string &value)
// {
//     if (!CheckIndexVailed(index)) {
//         return ERR_IMAGE_DECODE_HEAD_ABNORMAL;
//     }
//     if (ENCODED_FORMAT_KEY.compare(key) == ZERO) {
//         SkEncodedImageFormat format = codec_->getEncodedFormat();
//         return GetFormatName(format, value);
//     }
//     return ERR_IMAGE_DATA_UNSUPPORT;
// }
bool ExtDecoder::ConvertInfoToAlphaType(SkAlphaType &alphaType, PlAlphaType &outputType)
{
    if (info_.isEmpty()) {
        return false;
    }
    alphaType = info_.alphaType();
    auto findItem = std::find_if(ALPHA_TYPE_MAP.begin(), ALPHA_TYPE_MAP.end(),
        [alphaType](const map<PlAlphaType, SkAlphaType>::value_type item) {
        return item.second == alphaType;
    });
    if (findItem == ALPHA_TYPE_MAP.end()) {
        return false;
    }
    outputType = findItem->first;
    alphaType = findItem->second;
    return true;
}
bool ExtDecoder::ConvertInfoToColorType(SkColorType &format, PlPixelFormat &outputFormat)
{
    if (info_.isEmpty()) {
        return false;
    }
    auto colorType = info_.colorType();
    auto findItem = std::find_if(COLOR_TYPE_MAP.begin(), COLOR_TYPE_MAP.end(),
        [colorType](const map<PlPixelFormat, ColorTypeOutput>::value_type item) {
        return item.second.skFormat == colorType;
    });
    if (findItem == COLOR_TYPE_MAP.end()) {
        return false;
    }
    format = findItem->second.skFormat;
    outputFormat = findItem->second.outFormat;
    return true;
}
SkAlphaType ExtDecoder::ConvertToAlphaType(PlAlphaType desiredType, PlAlphaType &outputType)
{
    if (desiredType != PlAlphaType::IMAGE_ALPHA_TYPE_UNKNOWN) {
        auto alphaType = ALPHA_TYPE_MAP.find(desiredType);
        if (alphaType != ALPHA_TYPE_MAP.end()) {
            outputType = alphaType->first;
            return alphaType->second;
        }
    }
    HiLog::Debug(LABEL, "Unknown alpha type:%{public}d", desiredType);
    SkAlphaType res;
    if (ConvertInfoToAlphaType(res, outputType)) {
        HiLog::Debug(LABEL, "Using alpha type:%{public}d", outputType);
        return res;
    }
    HiLog::Debug(LABEL, "Using default alpha type:%{public}d", PlAlphaType::IMAGE_ALPHA_TYPE_PREMUL);
    outputType = PlAlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    return SkAlphaType::kPremul_SkAlphaType;
}
SkColorType ExtDecoder::ConvertToColorType(PlPixelFormat format, PlPixelFormat &outputFormat)
{
    if (format != PlPixelFormat::UNKNOWN) {
        auto colorType = COLOR_TYPE_MAP.find(format);
        if (colorType != COLOR_TYPE_MAP.end()) {
            outputFormat = colorType->second.outFormat;
            return colorType->second.skFormat;
        }
    }
    HiLog::Debug(LABEL, "Unknown pixel format:%{public}d", format);
    SkColorType res;
    if (ConvertInfoToColorType(res, outputFormat)) {
        HiLog::Debug(LABEL, "Using pixel format:%{public}d", outputFormat);
        return res;
    }
    HiLog::Debug(LABEL, "Using default pixel format:%{public}d", PlPixelFormat::RGBA_8888);
    outputFormat = PlPixelFormat::RGBA_8888;
    return kRGBA_8888_SkColorType;
}
#ifdef IMAGE_COLORSPACE_FLAG
OHOS::ColorManager::ColorSpace ExtDecoder::getGrColorSpace()
{
    return OHOS::ColorManager::ColorSpace(info_.refColorSpace());
}

bool ExtDecoder::IsSupportICCProfile()
{
    if (info_.isEmpty()) {
        return false;
    }
    return info_.refColorSpace() != nullptr;
}
#endif
static uint32_t ProcessWithStreamData(InputDataStream *input,
    std::function<uint32_t(uint8_t*, size_t)> process)
{
    size_t inputSize = input->GetStreamSize();
    if (inputSize <= SIZE_ZERO) {
        return Media::ERR_MEDIA_INVALID_VALUE;
    }
    uint8_t* buffer = nullptr;
    if (input->GetDataPtr() != nullptr) {
        buffer = input->GetDataPtr();
    } else {
        auto tmpBuffer = std::make_unique<uint8_t[]>(inputSize);
        buffer = tmpBuffer.get();
        auto savePos = input->Tell();
        input->Seek(SIZE_ZERO);
        uint32_t readSize = 0;
        input->Read(inputSize, buffer, inputSize, readSize);
        input->Seek(savePos);
    }
    return process(buffer, inputSize);
}
static bool ParseExifData(InputDataStream *input, EXIFInfo &info)
{
    if (info.IsExifDataParsed()) {
        return true;
    }
    HiLog::Debug(LABEL, "ParseExifData enter");
    auto code = ProcessWithStreamData(input, [&info](uint8_t* buffer, size_t size) {
        return info.ParseExifData(buffer, size);
    });
    if (code != SUCCESS) {
        HiLog::Error(LABEL, "Error parsing EXIF: code %{public}d", code);
    }
    return code == SUCCESS;
}
bool ExtDecoder::GetPropertyCheck(uint32_t index, const std::string &key, uint32_t &res)
{
    if (IsSameTextStr(key, ACTUAL_IMAGE_ENCODED_FORMAT)) {
        HiLog::Error(LABEL, "[GetPropertyCheck] this key is used to check the original format of raw image!");
        res = Media::ERR_MEDIA_VALUE_INVALID;
        return false;
    }
    if (!CheckIndexVailed(index)) {
        res = Media::ERR_IMAGE_DECODE_HEAD_ABNORMAL;
        return false;
    }
    SkEncodedImageFormat format = codec_->getEncodedFormat();
    if (format != SkEncodedImageFormat::kJPEG) {
        res = Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
        return true;
    }
    auto result = ParseExifData(stream_, exifInfo_);
    if (!result) {
        res = Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    return result;
}

uint32_t ExtDecoder::GetImagePropertyInt(uint32_t index, const std::string &key, int32_t &value)
{
    HiLog::Debug(LABEL, "[GetImagePropertyInt] enter ExtDecoder plugin, key:%{public}s", key.c_str());
    uint32_t res = Media::ERR_IMAGE_DATA_ABNORMAL;
    if (!GetPropertyCheck(index, key, res)) {
        return res;
    }
    // There can add some not need exif property
    if (res == Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT) {
        return res;
    }
    // Need exif property following
    if (IsSameTextStr(key, TAG_ORIENTATION_STRING)) {
        std::string strValue;
        res = exifInfo_.GetExifData(TAG_ORIENTATION_INT, strValue);
        if (res != SUCCESS) {
            return res;
        }
        value = atoi(strValue.c_str());
        return value;
    }
    HiLog::Error(LABEL, "[GetImagePropertyInt] The key:%{public}s is not supported int32_t", key.c_str());
    return Media::ERR_MEDIA_VALUE_INVALID;
}

uint32_t ExtDecoder::GetImagePropertyString(uint32_t index, const std::string &key, std::string &value)
{
    HiLog::Debug(LABEL, "[GetImagePropertyString] enter jpeg plugin, key:%{public}s", key.c_str());
    uint32_t res = Media::ERR_IMAGE_DATA_ABNORMAL;
    if (!GetPropertyCheck(index, key, res)) {
        return res;
    }
    // There can add some not need exif property
    if (ENCODED_FORMAT_KEY.compare(key) == ZERO) {
        SkEncodedImageFormat format = codec_->getEncodedFormat();
        return GetFormatName(format, value);
    }
    if (res == Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT) {
        return res;
    }
    // Need exif property following
    res = exifInfo_.GetExifData(key, value);
    HiLog::Debug(LABEL, "[GetImagePropertyString] enter jpeg plugin, value:%{public}s", value.c_str());
    return Media::SUCCESS;
}

uint32_t ExtDecoder::ModifyImageProperty(uint32_t index, const std::string &key,
    const std::string &value, const std::string &path)
{
    HiLog::Debug(LABEL, "[ModifyImageProperty] with path:%{public}s, key:%{public}s, value:%{public}s",
        path.c_str(), key.c_str(), value.c_str());
    return exifInfo_.ModifyExifData(key, value, path);
}
uint32_t ExtDecoder::ModifyImageProperty(uint32_t index, const std::string &key,
    const std::string &value, const int fd)
{
    HiLog::Debug(LABEL, "[ModifyImageProperty] with fd:%{public}d, key:%{public}s, value:%{public}s",
        fd, key.c_str(), value.c_str());
    return exifInfo_.ModifyExifData(key, value, fd);
}
uint32_t ExtDecoder::ModifyImageProperty(uint32_t index, const std::string &key,
    const std::string &value, uint8_t *data, uint32_t size)
{
    HiLog::Debug(LABEL, "[ModifyImageProperty] with key:%{public}s, value:%{public}s",
        key.c_str(), value.c_str());
    return exifInfo_.ModifyExifData(key, value, data, size);
}
uint32_t ExtDecoder::GetFilterArea(const int &privacyType, std::vector<std::pair<uint32_t, uint32_t>> &ranges)
{
    HiLog::Debug(LABEL, "[GetFilterArea] with privacyType:%{public}d ", privacyType);
    if (!CheckCodec()) {
        HiLog::Error(LABEL, "Check codec failed");
        return Media::ERR_MEDIA_INVALID_OPERATION;;
    }
    SkEncodedImageFormat format = codec_->getEncodedFormat();
    if (format != SkEncodedImageFormat::kJPEG) {
        return Media::ERR_MEDIA_INVALID_OPERATION;;
    }
    constexpr size_t APP1_SIZE_H_OFF = 4;
    constexpr size_t APP1_SIZE_L_OFF = 5;
        constexpr size_t U8_SHIFT = 8;
    return ProcessWithStreamData(stream_, [this, &privacyType, &ranges](uint8_t* buffer, size_t size){
        size_t appSize = (static_cast<size_t>(buffer[APP1_SIZE_H_OFF]) << U8_SHIFT) | buffer[APP1_SIZE_L_OFF];
        HiLog::Debug(LABEL, "[GetFilterArea]: get app1 area size %{public}d", appSize);
        appSize += APP1_SIZE_H_OFF;
        auto ret = exifInfo_.GetFilterArea(buffer, (appSize < size)?appSize:size, privacyType, ranges);
        if (ret != Media::SUCCESS) {
            HiLog::Error(LABEL, "[GetFilterArea]: failed to get area %{public}d", ret);
        }
        return ret;
    });
}
} // namespace ImagePlugin
} // namespace OHOS
