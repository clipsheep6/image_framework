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

#include "HeifDecoderImpl.h"
#include "image_trace.h"
#include "image_utils.h"
#include "image_log.h"
#include "media_errors.h"

#ifdef HEIF_HW_DECODE_ENABLE
#include "hardware/heif_hw_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif
#endif

#include <cmath>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "HeifDecoderImpl"

namespace OHOS {
namespace ImagePlugin {
using namespace Media;

const static int LUMA_10_BIT = 10;
const static int DEGREE_360 = 360;
const static int CHUNK_HEAD_OFFSET_1 = 1;
const static int CHUNK_HEAD_OFFSET_2 = 2;
const static int CHUNK_HEAD_OFFSET_3 = 3;
const static int CHUNK_HEAD_SHIFT_8 = 8;
const static int CHUNK_HEAD_SHIFT_16 = 16;
const static int CHUNK_HEAD_SHIFT_24 = 24;
const static int CHUNK_HEAD_SIZE = 4;

#ifdef HEIF_HW_DECODE_ENABLE
const static int GRID_NUM_2 = 2;
const static uint32_t PLANE_COUNT_TWO = 2;

struct PixelFormatConvertParam {
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    OH_NativeBuffer_Planes *planesInfo;
    AVPixelFormat format;
};

static bool FillFrameInfoForPixelConvert(AVFrame *frame, PixelFormatConvertParam &param)
{
    if (param.format == AV_PIX_FMT_NV12 || param.format == AV_PIX_FMT_NV21 || param.format == AV_PIX_FMT_P010) {
        if (param.planesInfo->planeCount < PLANE_COUNT_TWO) {
            return false;
        }
        const OH_NativeBuffer_Plane &planeY = param.planesInfo->planes[0];
        const OH_NativeBuffer_Plane &planeUV = param.planesInfo->planes[1];
        frame->data[0] = param.data + planeY.offset;
        frame->data[1] = param.data + planeUV.offset;
        frame->linesize[0] = static_cast<int>(planeY.columnStride);
        frame->linesize[1] = static_cast<int>(planeUV.columnStride);
    } else {
        frame->data[0] = param.data;
        frame->linesize[0] = static_cast<int>(param.stride);
    }
    return true;
}

static bool ConvertPixelFormat(PixelFormatConvertParam &srcParam, PixelFormatConvertParam &dstParam)
{
    ImageTrace trace("ConvertPixelFormat %d %d", srcParam.format, dstParam.format);
    bool res = false;
    AVFrame *srcFrame = av_frame_alloc();
    AVFrame *dstFrame = av_frame_alloc();
    SwsContext *ctx = sws_getContext(static_cast<int>(srcParam.width), static_cast<int>(srcParam.height),
                                     srcParam.format,
                                     static_cast<int>(dstParam.width), static_cast<int>(dstParam.height),
                                     dstParam.format,
                                     SWS_BICUBIC, nullptr, nullptr, nullptr);
    if (srcFrame != nullptr && dstFrame != nullptr && ctx != nullptr) {
        res = FillFrameInfoForPixelConvert(srcFrame, srcParam)
                && FillFrameInfoForPixelConvert(dstFrame, dstParam)
                && sws_scale(ctx, srcFrame->data, srcFrame->linesize, 0,
                             static_cast<int>(srcParam.height), dstFrame->data, dstFrame->linesize);
    }

    av_frame_free(&srcFrame);
    av_frame_free(&dstFrame);
    sws_freeContext(ctx);
    return res;
}

static AVPixelFormat GraphicPixFmt2AvPixFmtForYuv(GraphicPixelFormat pixelFormat)
{
    AVPixelFormat res = AV_PIX_FMT_NV12;
    switch (pixelFormat) {
        case GRAPHIC_PIXEL_FMT_YCBCR_420_SP:
            res = AV_PIX_FMT_NV12;
            break;
        case GRAPHIC_PIXEL_FMT_YCRCB_420_SP:
            res = AV_PIX_FMT_NV21;
            break;
        case GRAPHIC_PIXEL_FMT_YCBCR_P010:
            res = AV_PIX_FMT_P010;
            break;
        default:
            break;
    }
    return res;
}

static AVPixelFormat PixFmt2AvPixFmtForRgb(PixelFormat pixelFormat)
{
    AVPixelFormat res = AV_PIX_FMT_RGBA;
    switch (pixelFormat) {
        case PixelFormat::RGBA_8888:
            res = AV_PIX_FMT_RGBA;
            break;
        case PixelFormat::BGRA_8888:
            res = AV_PIX_FMT_BGRA;
            break;
        case PixelFormat::RGB_565:
            res = AV_PIX_FMT_RGB565;
            break;
        default:
            break;
    }
    return res;
}
#endif

static PixelFormat SkHeifColorFormat2PixelFormat(SkHeifColorFormat format)
{
    PixelFormat res = PixelFormat::UNKNOWN;
    switch (format) {
        case kHeifColorFormat_RGB565:
            res = PixelFormat::RGB_565;
            break;
        case kHeifColorFormat_RGBA_8888:
            res = PixelFormat::RGBA_8888;
            break;
        case kHeifColorFormat_BGRA_8888:
            res = PixelFormat::BGRA_8888;
            break;
        default:
            IMAGE_LOGE("Unsupported dst pixel format: %{public}d", format);
            break;
    }
    return res;
}

class HeifInputStreamWrapper : public HeifInputStream {
public:
    explicit HeifInputStreamWrapper(HeifStream *stream) : innerStream_(stream)
    {
        if (innerStream_ != nullptr && innerStream_->hasLength()) {
            length_ = stream->getLength();
        }
    }

    [[nodiscard]] int64_t Tell() const override
    {
        return (innerStream_ != nullptr && innerStream_->hasPosition())
                ? static_cast<int64_t>(innerStream_->getPosition()) : 0;
    }

    bool CheckSize(size_t size, int64_t end) override
    {
        auto posAfterRead = static_cast<int64_t>(Tell() + size);
        return (end < 0 || posAfterRead <= end) && posAfterRead <= length_;
    }

    bool Read(void *data, size_t size) override
    {
        if (innerStream_ == nullptr) {
            return false;
        }
        if (Tell() + size > length_) {
            return false;
        }
        innerStream_->read(data, size);
        return true;
    }

    bool Seek(int64_t position) override
    {
        if (innerStream_ == nullptr || position > length_) {
            return false;
        }
        return innerStream_->seek(position);
    }

private:
    HeifStream *innerStream_;
    size_t length_ = 0;
};

HeifDecoderImpl::HeifDecoderImpl()
    : inPixelFormat_(GRAPHIC_PIXEL_FMT_YCBCR_420_SP), outPixelFormat_(PixelFormat::RGBA_8888),
    tileWidth_(0), tileHeight_(0), colNum_(0), rowNum_(0), dstMemory_(nullptr), dstRowStride_(0) {}

bool HeifDecoderImpl::init(HeifStream *stream, HeifFrameInfo *frameInfo)
{
    ImageTrace trace("HeifDecoderImpl::init");
    if (stream == nullptr) {
        return false;
    }

    std::shared_ptr<HeifInputStream> streamWrapper = std::make_shared<HeifInputStreamWrapper>(stream);
    heif_error err = HeifParser::MakeFromStream(streamWrapper, &parser_);
    if (parser_ == nullptr || err != heif_error_ok) {
        IMAGE_LOGE("make heif parser failed, err: %{public}d", err);
        return false;
    }
    primaryImage_ = parser_->GetPrimaryImage();
    if (primaryImage_ == nullptr) {
        IMAGE_LOGE("heif primary image is null");
        return false;
    }
    if (primaryImage_->GetLumaBitNum() == LUMA_10_BIT) {
        IMAGE_LOGI("heif image is in 10 bit");
        inPixelFormat_ = GRAPHIC_PIXEL_FMT_YCBCR_P010;
    }

    return Reinit(frameInfo);
}

bool HeifDecoderImpl::Reinit(HeifFrameInfo *frameInfo)
{
    InitFrameInfo(&imageInfo_, primaryImage_);
    GetTileSize(primaryImage_, tileWidth_, tileHeight_);
    SetRowColNum();
    if (frameInfo != nullptr) {
        *frameInfo = imageInfo_;
    }
    return true;
}

void HeifDecoderImpl::InitFrameInfo(HeifFrameInfo *info, const std::shared_ptr<HeifImage> &image)
{
    if (info == nullptr || image == nullptr) {
        IMAGE_LOGI("InitFrameInfo info or image is null");
        return;
    }
    info->mWidth = image->GetOriginalWidth();
    info->mHeight = image->GetOriginalHeight();
    info->mRotationAngle = (DEGREE_360 - image->GetRotateDegrees()) % DEGREE_360;
    info->mBytesPerPixel = ImageUtils::GetPixelBytes(outPixelFormat_);
    info->mDurationUs = 0;
    auto &iccProfile = image->GetRawColorProfile();
    size_t iccSize = iccProfile != nullptr ? iccProfile->GetData().size() : 0;
    if (iccSize > 0) {
        auto iccProfileData = iccProfile->GetData().data();
        info->mIccData.assign(iccProfileData, iccProfileData + iccSize);
    } else {
        info->mIccData.clear();
    }
}

void HeifDecoderImpl::GetTileSize(const std::shared_ptr<HeifImage> &image, uint32_t &tileWidth, uint32_t &tileHeight)
{
    if (!image) {
        IMAGE_LOGI("GetTileSize image is null");
        return;
    }

    std::string imageType = parser_->GetItemType(image->GetItemId());
    if (imageType == "hvc1") {
        tileWidth = image->GetOriginalWidth();
        tileHeight = image->GetOriginalHeight();
        return;
    }
    if (imageType != "grid") {
        IMAGE_LOGE("GetTileSize unsupported image type: %{public}s", imageType.c_str());
        return;
    }
    std::vector<std::shared_ptr<HeifImage>> tileImages;
    parser_->GetTileImages(image->GetItemId(), tileImages);
    if (tileImages.empty()) {
        IMAGE_LOGE("grid image has no tile image");
        return;
    }
    tileWidth = tileImages[0]->GetOriginalWidth();
    tileHeight = tileImages[0]->GetOriginalHeight();
}

void HeifDecoderImpl::SetRowColNum()
{
    if (tileWidth_ != 0) {
        colNum_ = static_cast<size_t>(ceil((double)imageInfo_.mWidth / (double)tileWidth_));
    }
    if (tileHeight_ != 0) {
        rowNum_ = static_cast<size_t>(ceil((double)imageInfo_.mHeight / (double)tileHeight_));
    }
}

bool HeifDecoderImpl::getSequenceInfo(HeifFrameInfo *frameInfo, size_t *frameCount)
{
    // unimplemented
    return false;
}

bool HeifDecoderImpl::setOutputColor(SkHeifColorFormat heifColor)
{
    outPixelFormat_ = SkHeifColorFormat2PixelFormat(heifColor);
    imageInfo_.mBytesPerPixel = ImageUtils::GetPixelBytes(outPixelFormat_);
    return outPixelFormat_ != PixelFormat::UNKNOWN;
}

bool HeifDecoderImpl::decode(HeifFrameInfo *frameInfo)
{
#ifdef HEIF_HW_DECODE_ENABLE
    ImageTrace trace("HeifDecoderImpl::decode");

    if (outPixelFormat_ == PixelFormat::UNKNOWN) {
        IMAGE_LOGE("unknown pixel type: %{public}d", outPixelFormat_);
        return false;
    }

    if (!primaryImage_) {
        return false;
    }

    if (hwDecoder_ == nullptr) {
        hwDecoder_ = std::make_shared<HeifHardwareDecoder>();
        if (hwDecoder_ == nullptr) {
            IMAGE_LOGE("decode make HeifHardwareDecoder failed");
            return false;
        }
    }

    sptr<SurfaceBuffer> hwBuffer
        = hwDecoder_->AllocateOutputBuffer(imageInfo_.mWidth, imageInfo_.mHeight, inPixelFormat_);
    if (hwBuffer == nullptr) {
        IMAGE_LOGE("decode AllocateOutputBuffer return null");
        return false;
    }

    std::string imageType = parser_->GetItemType(primaryImage_->GetItemId());
    bool res = false;
    IMAGE_LOGI("HeifDecoderImpl::decode width: %{public}d, height: %{public}d,"
               " imageType: %{public}s, inPixelFormat: %{public}d",
               imageInfo_.mWidth, imageInfo_.mHeight, imageType.c_str(), inPixelFormat_);
    if (imageType == "grid") {
        res = DecodeGrids(hwBuffer);
    } else if (imageType == "hvc1") {
        res = DecodeSingleImage(primaryImage_, hwBuffer);
    }
    return res;
#else
    return false;
#endif
}

bool HeifDecoderImpl::DecodeGrids(sptr<SurfaceBuffer> &hwBuffer)
{
#ifdef HEIF_HW_DECODE_ENABLE
    std::vector<std::shared_ptr<HeifImage>> tileImages;
    parser_->GetTileImages(primaryImage_->GetItemId(), tileImages);
    if (tileImages.empty()) {
        IMAGE_LOGE("grid image has no tile image");
        return false;
    }
    size_t numGrid = tileImages.size();
    std::vector<std::vector<uint8_t>> inputs(numGrid + 1);

    for (size_t index = 0; index < numGrid; ++index) {
        std::shared_ptr<HeifImage> &tileImage = tileImages[index];
        if (index == 0) {
            // get hvcc header
            parser_->GetItemData(tileImage->GetItemId(), &inputs[index], heif_only_header);
            ProcessChunkHead(inputs[index].data(), inputs[index].size());
        }
        parser_->GetItemData(tileImage->GetItemId(), &inputs[index + 1], heif_no_header);
        ProcessChunkHead(inputs[index + 1].data(), inputs[index + 1].size());
    }

    GridInfo gridInfo = {imageInfo_.mWidth, imageInfo_.mHeight, true, colNum_, rowNum_, tileWidth_, tileHeight_};
    auto err = hwDecoder_->DoDecode(gridInfo, inputs, hwBuffer);
    if (err != SUCCESS) {
        IMAGE_LOGE("heif hw decoder return error: %{public}d, width: %{public}d, height: %{public}d,"
                   " imageType: grid, inPixelFormat: %{public}d, colNum: %{public}d, rowNum: %{public}d,"
                   " tileWidth: %{public}d, tileHeight: %{public}d, hvccLen: %{public}zu",
                   err, imageInfo_.mWidth, imageInfo_.mHeight,
                   inPixelFormat_, colNum_, rowNum_, tileWidth_, tileHeight_, inputs[0].size());
        return false;
    }
    return ConvertHwBufferPixelFormat(hwBuffer);
#else
    return false;
#endif
}

bool HeifDecoderImpl::DecodeSingleImage(std::shared_ptr<HeifImage> &image, sptr<SurfaceBuffer> &hwBuffer)
{
#ifdef HEIF_HW_DECODE_ENABLE
    if (image == nullptr) {
        IMAGE_LOGI("HeifDecoderImpl::DecodeSingleImage image is nullptr");
        return false;
    }
    std::vector<std::vector<uint8_t>> inputs(GRID_NUM_2);

    parser_->GetItemData(image->GetItemId(), &inputs[0], heif_only_header);
    ProcessChunkHead(inputs[0].data(), inputs[0].size());

    parser_->GetItemData(image->GetItemId(), &inputs[1], heif_no_header);
    ProcessChunkHead(inputs[1].data(), inputs[1].size());

    GridInfo gridInfo = {imageInfo_.mWidth, imageInfo_.mHeight, false, colNum_, rowNum_, tileWidth_, tileHeight_};
    auto err = hwDecoder_->DoDecode(gridInfo, inputs, hwBuffer);
    if (err != SUCCESS) {
        IMAGE_LOGE("heif hw decoder return error: %{public}d, width: %{public}d, height: %{public}d,"
                   " imageType: hvc1, inPixelFormat: %{public}d, colNum: %{public}d, rowNum: %{public}d,"
                   " tileWidth: %{public}d, tileHeight: %{public}d, hvccLen: %{public}zu, dataLen: %{public}zu",
                   err, imageInfo_.mWidth, imageInfo_.mHeight,
                   inPixelFormat_, colNum_, rowNum_, tileWidth_, tileHeight_, inputs[0].size(), inputs[1].size());
        return false;
    }
    return ConvertHwBufferPixelFormat(hwBuffer);
#else
    return false;
#endif
}

bool HeifDecoderImpl::ConvertHwBufferPixelFormat(sptr<SurfaceBuffer> &hwBuffer)
{
#ifdef HEIF_HW_DECODE_ENABLE
    OH_NativeBuffer_Planes *hwBufferPlanesInfo = nullptr;
    hwBuffer->GetPlanesInfo((void **)&hwBufferPlanesInfo);
    if (hwBufferPlanesInfo == nullptr) {
        IMAGE_LOGE("find to get hw buffer planes info");
        return false;
    }
    PixelFormatConvertParam srcParam = {static_cast<uint8_t *>(hwBuffer->GetVirAddr()),
                                        imageInfo_.mWidth, imageInfo_.mHeight,
                                        static_cast<uint32_t>(hwBuffer->GetStride()),
                                        hwBufferPlanesInfo,
                                        GraphicPixFmt2AvPixFmtForYuv(inPixelFormat_)};
    PixelFormatConvertParam dstParam = {dstMemory_, imageInfo_.mWidth, imageInfo_.mHeight,
                                        static_cast<uint32_t>(dstRowStride_),
                                        nullptr,
                                        PixFmt2AvPixFmtForRgb(outPixelFormat_)};
    return ConvertPixelFormat(srcParam, dstParam);
#else
    return false;
#endif
}

bool HeifDecoderImpl::ProcessChunkHead(uint8_t *data, size_t len)
{
    if (len < CHUNK_HEAD_SIZE) {
        return false;
    }
    size_t index = 0;
    while (index < len - CHUNK_HEAD_SIZE) {
        size_t chunkLen = (data[index] << CHUNK_HEAD_SHIFT_24)
                | (data[index + CHUNK_HEAD_OFFSET_1] << CHUNK_HEAD_SHIFT_16)
                | (data[index + CHUNK_HEAD_OFFSET_2] << CHUNK_HEAD_SHIFT_8)
                | (data[index + CHUNK_HEAD_OFFSET_3]);
        data[index] = 0;
        data[index + CHUNK_HEAD_OFFSET_1] = 0;
        data[index + CHUNK_HEAD_OFFSET_2] = 0;
        data[index + CHUNK_HEAD_OFFSET_3] = 1;
        index += (chunkLen + CHUNK_HEAD_SIZE);
    }
    return true;
}

bool HeifDecoderImpl::decodeSequence(int frameIndex, HeifFrameInfo *frameInfo)
{
    // unimplemented
    return false;
}

void HeifDecoderImpl::setDstBuffer(uint8_t *dstBuffer, size_t rowStride)
{
    if (dstMemory_ == nullptr) {
        dstMemory_ = dstBuffer;
        dstRowStride_ = rowStride;
    }
}

bool HeifDecoderImpl::getScanline(uint8_t *dst)
{
    // no need to implement
    return true;
}

size_t HeifDecoderImpl::skipScanlines(int count)
{
    // no need to implement
    return true;
}
} // namespace ImagePlugin
} // namespace OHOS

HeifDecoder* CreateHeifDecoderImpl()
{
#ifdef HEIF_HW_DECODE_ENABLE
    return new OHOS::ImagePlugin::HeifDecoderImpl();
#else
    return nullptr;
#endif
}
