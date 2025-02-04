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

#ifdef HEIF_HW_DECODE_ENABLE
#include "ffrt.h"
#include "image_fwk_ext_manager.h"
#include "image_system_properties.h"
#include "image_trace.h"
#include "image_utils.h"
#include "image_log.h"
#include "media_errors.h"

#include "hardware/heif_hw_decoder.h"
#include "heif_impl/hevc_sw_decode_param.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

#include <cmath>
#include <sstream>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "HeifDecoderImpl"

namespace OHOS {
namespace ImagePlugin {
using namespace Media;

const static int LUMA_8_BIT = 8;
const static int LUMA_10_BIT = 10;
const static int DEGREE_360 = 360;
const static int CHUNK_HEAD_OFFSET_1 = 1;
const static int CHUNK_HEAD_OFFSET_2 = 2;
const static int CHUNK_HEAD_OFFSET_3 = 3;
const static int CHUNK_HEAD_SHIFT_8 = 8;
const static int CHUNK_HEAD_SHIFT_16 = 16;
const static int CHUNK_HEAD_SHIFT_24 = 24;
const static int CHUNK_HEAD_SIZE = 4;
const static int PIXEL_OFFSET_0 = 0;
const static int PIXEL_OFFSET_1 = 1;
const static int PIXEL_OFFSET_2 = 2;
const static int PIXEL_OFFSET_3 = 3;
const static int PIXEL_SIZE_4 = 4;
const static int MAX_ALPHA = 255;

const static int GRID_NUM_2 = 2;
const static uint32_t PLANE_COUNT_TWO = 2;
const static uint32_t HEIF_HARDWARE_TILE_MIN_DIM = 128;
const static uint32_t HEIF_HARDWARE_TILE_MAX_DIM = 4096;
const static uint32_t HEIF_HARDWARE_DISPLAY_MIN_DIM = 1280;

const static uint16_t BT2020_PRIMARIES = 9;

struct PixelFormatConvertParam {
    uint8_t *data;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    OH_NativeBuffer_Planes *planesInfo;
    AVPixelFormat format;
};

const std::map<AuxiliaryPictureType, std::string> HEIF_AUXTTYPE_ID_MAP = {
    {AuxiliaryPictureType::GAINMAP, "gainmap"},
    {AuxiliaryPictureType::DEPTH_MAP, "depthmap"},
    {AuxiliaryPictureType::UNREFOCUS_MAP, "unrefocusmap"},
    {AuxiliaryPictureType::LINEAR_MAP, "linearmap"},
    {AuxiliaryPictureType::FRAGMENT_MAP, "fragmentmap"}
};

static bool FillFrameInfoForPixelConvert(AVFrame *frame, PixelFormatConvertParam &param)
{
    if (param.format == AV_PIX_FMT_NV12 || param.format == AV_PIX_FMT_NV21 || param.format == AV_PIX_FMT_P010) {
        if (param.planesInfo == nullptr || param.planesInfo->planeCount < PLANE_COUNT_TWO) {
            IMAGE_LOGE("planesInfo is invalid for yuv buffer");
            return false;
        }
        const OH_NativeBuffer_Plane &planeY = param.planesInfo->planes[0];
        const OH_NativeBuffer_Plane &planeUV = param.planesInfo->planes[param.format == AV_PIX_FMT_NV21 ? 2 : 1];
        IMAGE_LOGI("planeY offset: %{public}llu, columnStride: %{public}u, rowStride: %{public}u,"
                   " planeUV offset: %{public}llu, columnStride: %{public}u, rowStride: %{public}u",
                   planeY.offset, planeY.columnStride, planeY.rowStride,
                   planeUV.offset, planeUV.columnStride, planeUV.rowStride);
        frame->data[0] = param.data + planeY.offset;
        frame->data[1] = param.data + planeUV.offset;
        frame->linesize[0] = static_cast<int>(planeY.columnStride);
        frame->linesize[1] = static_cast<int>(planeUV.columnStride);
    } else {
        IMAGE_LOGI("rgb stride: %{public}d", param.stride);
        frame->data[0] = param.data;
        frame->linesize[0] = static_cast<int>(param.stride);
    }
    return true;
}

static bool ConvertPixelFormat(PixelFormatConvertParam &srcParam, PixelFormatConvertParam &dstParam)
{
    ImageTrace trace("ConvertPixelFormat %d %d", srcParam.format, dstParam.format);
    IMAGE_LOGI("ConvertPixelFormat %{public}d %{public}d", srcParam.format, dstParam.format);
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

static AVPixelFormat PixFmt2AvPixFmtForOutput(PixelFormat pixelFormat)
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
        case PixelFormat::NV12:
            res = AV_PIX_FMT_NV12;
            break;
        case PixelFormat::NV21:
            res = AV_PIX_FMT_NV21;
            break;
        case PixelFormat::RGBA_1010102:
            res = AV_PIX_FMT_X2BGR10;
            break;
        default:
            break;
    }
    return res;
}

// LCOV_EXCL_START
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
        case kHeifColorFormat_NV12:
            res = PixelFormat::NV12;
            break;
        case kHeifColorFormat_NV21:
            res = PixelFormat::NV21;
            break;
        case kHeifColorFormat_RGBA_1010102:
            res = PixelFormat::RGBA_1010102;
            break;
        case kHeifColorFormat_P010_NV12:
            res = PixelFormat::YCBCR_P010;
            break;
        case kHeifColorFormat_P010_NV21:
            res = PixelFormat::YCRCB_P010;
            break;
        default:
            IMAGE_LOGE("Unsupported dst pixel format: %{public}d", format);
            break;
    }
    return res;
}
// LCOV_EXCL_STOP

HeifDecoderImpl::HeifDecoderImpl()
    : outPixelFormat_(PixelFormat::RGBA_8888),
    dstMemory_(nullptr), dstRowStride_(0), dstHwBuffer_(nullptr),
    gainmapDstMemory_(nullptr), gainmapDstRowStride_(0),
    auxiliaryDstMemory_(nullptr), auxiliaryDstRowStride_(0) {}

HeifDecoderImpl::~HeifDecoderImpl()
{
    if (srcMemory_ != nullptr) {
        delete[] srcMemory_;
    }
}

bool HeifDecoderImpl::init(HeifStream *stream, HeifFrameInfo *frameInfo)
{
    ImageTrace trace("HeifDecoderImpl::init");
    if (stream == nullptr) {
        return false;
    }

    size_t fileLength = stream->getLength();
    if (srcMemory_ == nullptr) {
        if (fileLength == 0) {
            IMAGE_LOGE("file size is 0");
            return false;
        }
        srcMemory_ = new uint8_t[fileLength];
        if (srcMemory_ == nullptr) {
            return false;
        }
        stream->read(srcMemory_, fileLength);
    }

    heif_error err = HeifParser::MakeFromMemory(srcMemory_, fileLength, false, &parser_);
    if (parser_ == nullptr || err != heif_error_ok) {
        IMAGE_LOGE("make heif parser failed, err: %{public}d", err);
        return false;
    }
    primaryImage_ = parser_->GetPrimaryImage();
    if (primaryImage_ == nullptr) {
        IMAGE_LOGE("heif primary image is null");
        return false;
    }
    gainmapImage_ = parser_->GetGainmapImage();
    std::shared_ptr<HeifImage> tmapImage = parser_->GetTmapImage();
    if (tmapImage != nullptr) {
        InitFrameInfo(&tmapInfo_, tmapImage);
    }
    return Reinit(frameInfo);
}

bool HeifDecoderImpl::CheckAuxiliaryMap(AuxiliaryPictureType type)
{
    if (parser_ == nullptr) {
        IMAGE_LOGE("Heif parser is nullptr.");
        return false;
    }

    auto iter = HEIF_AUXTTYPE_ID_MAP.find(type);
    switch (type) {
        case AuxiliaryPictureType::GAINMAP:
            auxiliaryImage_ = parser_->GetGainmapImage();
            break;
        case AuxiliaryPictureType::DEPTH_MAP:
        case AuxiliaryPictureType::UNREFOCUS_MAP:
        case AuxiliaryPictureType::LINEAR_MAP:
        case AuxiliaryPictureType::FRAGMENT_MAP:
            auxiliaryImage_ = parser_->GetAuxiliaryMapImage(iter->second);
            break;
        default:
            auxiliaryImage_ = nullptr;
            IMAGE_LOGE("Invalid AuxiliaryPictureType: %{public}d", type);
            break;
    }

    if (auxiliaryImage_ == nullptr) {
        IMAGE_LOGE("Auxiliary map type that does not exist");
        return false;
    }

    return true;
}

bool HeifDecoderImpl::setAuxiliaryMap(AuxiliaryPictureType type)
{
    if (auxiliaryImage_ == nullptr && !CheckAuxiliaryMap(type)) {
        IMAGE_LOGE("make heif parser failed");
        return false;
    }

    InitFrameInfo(&auxiliaryImageInfo_, auxiliaryImage_);
    InitGridInfo(auxiliaryImage_, auxiliaryGridInfo_);
    return true;
}

bool HeifDecoderImpl::Reinit(HeifFrameInfo *frameInfo)
{
    InitFrameInfo(&imageInfo_, primaryImage_);
    InitGridInfo(primaryImage_, gridInfo_);
    if (gainmapImage_ != nullptr) {
        InitFrameInfo(&gainmapImageInfo_, gainmapImage_);
        InitGridInfo(gainmapImage_, gainmapGridInfo_);
    }
    if (frameInfo != nullptr) {
        *frameInfo = imageInfo_;
    }
    return true;
}

void HeifDecoderImpl::InitFrameInfo(HeifFrameInfo *info, const std::shared_ptr<HeifImage> &image)
{
    if (info == nullptr || image == nullptr) {
        IMAGE_LOGE("InitFrameInfo info or image is null");
        return;
    }
    info->mWidth = image->GetOriginalWidth();
    info->mHeight = image->GetOriginalHeight();
    info->mRotationAngle = (DEGREE_360 - image->GetRotateDegrees()) % DEGREE_360;
    info->mBytesPerPixel = static_cast<uint32_t>(ImageUtils::GetPixelBytes(outPixelFormat_));
    info->mDurationUs = 0;
    SetColorSpaceInfo(info, image);
    if (info->mIccData.empty() && !info->hasNclxColor && (parser_->GetItemType(image->GetItemId())== "grid")) {
        std::vector<std::shared_ptr<HeifImage>> tileImages;
        parser_->GetTileImages(image->GetItemId(), tileImages);
        if (!tileImages.empty()) {
            SetColorSpaceInfo(info, tileImages[0]);
        }
    }
}

// LCOV_EXCL_START
void HeifDecoderImpl::SetColorSpaceInfo(HeifFrameInfo* info, const std::shared_ptr<HeifImage>& image)
{
    auto &iccProfile = image->GetRawColorProfile();
    size_t iccSize = iccProfile != nullptr ? iccProfile->GetData().size() : 0;
    if (iccSize > 0) {
        auto iccProfileData = iccProfile->GetData().data();
        info->mIccData.assign(iccProfileData, iccProfileData + iccSize);
    } else {
        info->mIccData.clear();
    }
    auto& nclx = image->GetNclxColorProfile();
    if (nclx != nullptr) {
        info->hasNclxColor = true;
        info->nclxColor.colorPrimaries = nclx->GetColorPrimaries();
        info->nclxColor.transferCharacteristics = nclx->GetTransferCharacteristics();
        info->nclxColor.matrixCoefficients = nclx->GetMatrixCoefficients();
        info->nclxColor.fullRangeFlag = nclx->GetFullRangeFlag();
    } else {
        info->hasNclxColor = false;
    }
}
// LCOV_EXCL_STOP

void HeifDecoderImpl::InitGridInfo(const std::shared_ptr<HeifImage> &image, GridInfo &gridInfo)
{
    if (!image) {
        IMAGE_LOGE("InitGridInfo image is null");
        return;
    }
    gridInfo.displayWidth = image->GetOriginalWidth();
    gridInfo.displayHeight = image->GetOriginalHeight();
    GetTileSize(image, gridInfo);
    GetRowColNum(gridInfo);
}

void HeifDecoderImpl::GetTileSize(const std::shared_ptr<HeifImage> &image, GridInfo &gridInfo)
{
    if (!image) {
        IMAGE_LOGE("GetTileSize image is null");
        return;
    }

    std::string imageType = parser_->GetItemType(image->GetItemId());
    if (imageType == "hvc1") {
        gridInfo.tileWidth = image->GetOriginalWidth();
        gridInfo.tileHeight = image->GetOriginalHeight();
        return;
    }
    if (imageType == "iden") {
        std::shared_ptr<HeifImage> idenImage;
        parser_->GetIdenImage(image->GetItemId(), idenImage);
        if (idenImage != nullptr && idenImage != image) {
            GetTileSize(idenImage, gridInfo);
        }
        return;
    }
    if (imageType != "grid") {
        IMAGE_LOGE("GetTileSize unsupported image type: %{public}s", imageType.c_str());
        return;
    }
    std::vector<std::shared_ptr<HeifImage>> tileImages;
    parser_->GetTileImages(image->GetItemId(), tileImages);
    if (tileImages.empty() || tileImages[0] == nullptr) {
        IMAGE_LOGE("grid image has no tile image");
        return;
    }
    gridInfo.tileWidth = tileImages[0]->GetOriginalWidth();
    gridInfo.tileHeight = tileImages[0]->GetOriginalHeight();
}

void HeifDecoderImpl::GetRowColNum(GridInfo &gridInfo)
{
    if (gridInfo.tileWidth != 0) {
        gridInfo.cols = static_cast<size_t>(ceil((double)gridInfo.displayWidth / (double)gridInfo.tileWidth));
    }
    if (gridInfo.tileHeight != 0) {
        gridInfo.rows = static_cast<size_t>(ceil((double)gridInfo.displayHeight / (double)gridInfo.tileHeight));
    }
}

GraphicPixelFormat HeifDecoderImpl::GetInPixelFormat(const std::shared_ptr<HeifImage> &image)
{
    return (image != nullptr && image->GetLumaBitNum() == LUMA_10_BIT) ?
            GRAPHIC_PIXEL_FMT_YCBCR_P010 : GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
}

bool HeifDecoderImpl::getSequenceInfo(HeifFrameInfo *frameInfo, size_t *frameCount)
{
    // unimplemented
    return false;
}

bool HeifDecoderImpl::setOutputColor(SkHeifColorFormat heifColor)
{
    outPixelFormat_ = SkHeifColorFormat2PixelFormat(heifColor);
    imageInfo_.mBytesPerPixel = static_cast<uint32_t>(ImageUtils::GetPixelBytes(outPixelFormat_));
    return outPixelFormat_ != PixelFormat::UNKNOWN;
}

static bool IsSupportHardwareDecode(const GridInfo &gridInfo)
{
    if (!ImageSystemProperties::GetHeifHardwareDecodeEnabled()) {
        return false;
    }
    return gridInfo.tileWidth >= HEIF_HARDWARE_TILE_MIN_DIM &&
           gridInfo.tileHeight >= HEIF_HARDWARE_TILE_MIN_DIM &&
           gridInfo.tileWidth <= HEIF_HARDWARE_TILE_MAX_DIM &&
           gridInfo.tileHeight <= HEIF_HARDWARE_TILE_MAX_DIM &&
           gridInfo.displayWidth >= HEIF_HARDWARE_DISPLAY_MIN_DIM &&
           gridInfo.displayHeight >= HEIF_HARDWARE_DISPLAY_MIN_DIM;
}

bool HeifDecoderImpl::decode(HeifFrameInfo *frameInfo)
{
    ImageTrace trace("HeifDecoderImpl::decode");
    if (!IsSupportHardwareDecode(gridInfo_)) {
        HevcSoftDecodeParam param {
            gridInfo_, outPixelFormat_,
            dstMemory_, 0,
            static_cast<uint32_t>(dstRowStride_), dstHwBuffer_
        };
        bool decodeRes = SwDecodeImage(primaryImage_, param, gridInfo_, true);
        if (!decodeRes) {
            return false;
        }
        SwApplyAlphaImage(primaryImage_, dstMemory_, dstRowStride_);
        return true;
    }
    sptr<SurfaceBuffer> hwBuffer;
    bool decodeRes = HwDecodeImage(nullptr, primaryImage_, gridInfo_, &hwBuffer, true);
    if (!decodeRes) {
        return false;
    }

    bool convertRes = IsDirectYUVDecode() ||
            ConvertHwBufferPixelFormat(hwBuffer, gridInfo_, dstMemory_, dstRowStride_);
    if (!convertRes) {
        return false;
    }
    HwApplyAlphaImage(primaryImage_, dstMemory_, dstRowStride_);
    return true;
}

bool HeifDecoderImpl::decodeGainmap()
{
    ImageTrace trace("HeifDecoderImpl::decodeGainmap");
    sptr<SurfaceBuffer> hwBuffer;
    bool decodeRes = HwDecodeImage(nullptr, gainmapImage_, gainmapGridInfo_, &hwBuffer, false);
    if (!decodeRes) {
        return false;
    }

    bool convertRes = IsDirectYUVDecode() ||
            ConvertHwBufferPixelFormat(hwBuffer, gainmapGridInfo_, gainmapDstMemory_, gainmapDstRowStride_);
    if (!convertRes) {
        return false;
    }
    return true;
}

bool HeifDecoderImpl::decodeAuxiliaryMap()
{
    ImageTrace trace("HeifDecoderImpl::decodeAuxiliaryMap");
    sptr<SurfaceBuffer> hwBuffer;
    bool decodeRes = HwDecodeImage(nullptr, auxiliaryImage_, auxiliaryGridInfo_, &hwBuffer, false);
    if (!decodeRes) {
        return false;
    }

    bool convertRes = IsDirectYUVDecode() ||
            ConvertHwBufferPixelFormat(hwBuffer, auxiliaryGridInfo_, auxiliaryDstMemory_, auxiliaryDstRowStride_);
    if (!convertRes) {
        return false;
    }
    return true;
}

void HeifDecoderImpl::ReleaseHwDecoder(HeifHardwareDecoder *hwDecoder, bool isReuse)
{
    if (isReuse || hwDecoder == nullptr) {
        return;
    }
    ffrt::submit([hwDecoder] {
        ImageTrace trace("delete hwDecoder");
        delete hwDecoder;
        }, {}, {});
}

bool HeifDecoderImpl::HwDecodeImage(HeifHardwareDecoder *hwDecoder,
                                    std::shared_ptr<HeifImage> &image, GridInfo &gridInfo,
                                    sptr<SurfaceBuffer> *outBuffer, bool isPrimary)
{
    if (outPixelFormat_ == PixelFormat::UNKNOWN) {
        IMAGE_LOGE("unknown pixel type: %{public}d", outPixelFormat_);
        return false;
    }

    if (image == nullptr || outBuffer == nullptr) {
        return false;
    }

    bool isReuseHwDecoder = hwDecoder != nullptr;
    if (!isReuseHwDecoder) {
        hwDecoder = new (std::nothrow) HeifHardwareDecoder();
        if (hwDecoder == nullptr) {
            IMAGE_LOGE("make HeifHardwareDecoder failed");
            return false;
        }
    }

    std::string imageType = parser_->GetItemType(image->GetItemId());
    if (imageType == "iden") {
        bool res = HwDecodeIdenImage(hwDecoder, image, gridInfo, outBuffer, isPrimary);
        ReleaseHwDecoder(hwDecoder, isReuseHwDecoder);
        return res;
    }

    GraphicPixelFormat inPixelFormat = GetInPixelFormat(image);
    sptr<SurfaceBuffer> hwBuffer =
            isPrimary && IsDirectYUVDecode() ? sptr<SurfaceBuffer>(dstHwBuffer_) :
            hwDecoder->AllocateOutputBuffer(gridInfo.displayWidth, gridInfo.displayHeight, inPixelFormat);
    if (hwBuffer == nullptr) {
        IMAGE_LOGE("decode AllocateOutputBuffer return null");
        ReleaseHwDecoder(hwDecoder, isReuseHwDecoder);
        return false;
    }
    if (IsDirectYUVDecode()) {
        inPixelFormat = static_cast<GraphicPixelFormat>(hwBuffer->GetFormat());
    }

    bool res = false;
    IMAGE_LOGI("HeifDecoderImpl::DecodeImage width: %{public}d, height: %{public}d, imageType: %{public}s,"
        "inPixelFormat: %{public}d", gridInfo.displayWidth, gridInfo.displayHeight, imageType.c_str(), inPixelFormat);
    if (imageType == "grid") {
        gridInfo.enableGrid = true;
        res = HwDecodeGrids(hwDecoder, image, gridInfo, hwBuffer);
    } else if (imageType == "hvc1") {
        gridInfo.enableGrid = false;
        res = HwDecodeSingleImage(hwDecoder, image, gridInfo, hwBuffer);
    }
    if (res) {
        *outBuffer = hwBuffer;
    }
    ReleaseHwDecoder(hwDecoder, isReuseHwDecoder);
    return res;
}

bool HeifDecoderImpl::HwDecodeGrids(HeifHardwareDecoder *hwDecoder, std::shared_ptr<HeifImage> &image,
                                    GridInfo &gridInfo, sptr<SurfaceBuffer> &hwBuffer)
{
    if (hwDecoder == nullptr || image == nullptr) {
        IMAGE_LOGE("HeifDecoderImpl::DecodeGrids hwDecoder or image is nullptr");
        return false;
    }
    std::vector<std::shared_ptr<HeifImage>> tileImages;
    parser_->GetTileImages(image->GetItemId(), tileImages);
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

    uint32_t err = hwDecoder->DoDecode(gridInfo, inputs, hwBuffer);
    if (err != SUCCESS) {
        IMAGE_LOGE("heif hw decoder return error: %{public}d, width: %{public}d, height: %{public}d,"
                   " imageType: grid, inPixelFormat: %{public}d, colNum: %{public}d, rowNum: %{public}d,"
                   " tileWidth: %{public}d, tileHeight: %{public}d, hvccLen: %{public}zu",
                   err, gridInfo.displayWidth, gridInfo.displayHeight,
                   hwBuffer->GetFormat(), gridInfo.cols, gridInfo.rows,
                   gridInfo.tileWidth, gridInfo.tileHeight, inputs[0].size());
        SetHardwareDecodeErrMsg(gridInfo.tileWidth, gridInfo.tileHeight);
        return false;
    }
    return true;
}

bool HeifDecoderImpl::HwDecodeIdenImage(HeifHardwareDecoder *hwDecoder,
                                        std::shared_ptr<HeifImage> &image, GridInfo &gridInfo,
                                        sptr<SurfaceBuffer> *outBuffer, bool isPrimary)
{
    if (!image) {
        return false;
    }
    std::shared_ptr<HeifImage> idenImage;
    parser_->GetIdenImage(image->GetItemId(), idenImage);
    if (idenImage == nullptr || idenImage == image) {
        IMAGE_LOGE("invalid iden image");
        return false;
    }
    return HwDecodeImage(hwDecoder, idenImage, gridInfo, outBuffer, isPrimary);
}

bool HeifDecoderImpl::HwDecodeSingleImage(HeifHardwareDecoder *hwDecoder,
                                          std::shared_ptr<HeifImage> &image,
                                          GridInfo &gridInfo, sptr<SurfaceBuffer> &hwBuffer)
{
    if (hwDecoder == nullptr || image == nullptr) {
        IMAGE_LOGE("HeifDecoderImpl::DecodeSingleImage hwDecoder or image is nullptr");
        return false;
    }
    std::vector<std::vector<uint8_t>> inputs(GRID_NUM_2);

    parser_->GetItemData(image->GetItemId(), &inputs[0], heif_only_header);
    ProcessChunkHead(inputs[0].data(), inputs[0].size());

    parser_->GetItemData(image->GetItemId(), &inputs[1], heif_no_header);
    ProcessChunkHead(inputs[1].data(), inputs[1].size());

    uint32_t err = hwDecoder->DoDecode(gridInfo, inputs, hwBuffer);
    if (err != SUCCESS) {
        IMAGE_LOGE("heif hw decoder return error: %{public}d, width: %{public}d, height: %{public}d,"
                   " imageType: hvc1, inPixelFormat: %{public}d, colNum: %{public}d, rowNum: %{public}d,"
                   " tileWidth: %{public}d, tileHeight: %{public}d, hvccLen: %{public}zu, dataLen: %{public}zu",
                   err, gridInfo.displayWidth, gridInfo.displayHeight,
                   hwBuffer->GetFormat(), gridInfo.cols, gridInfo.rows,
                   gridInfo.tileWidth, gridInfo.tileHeight, inputs[0].size(), inputs[1].size());
        SetHardwareDecodeErrMsg(gridInfo.tileWidth, gridInfo.tileHeight);
        return false;
    }
    return true;
}

bool HeifDecoderImpl::SwDecodeImage(std::shared_ptr<HeifImage> &image, HevcSoftDecodeParam &param,
                                    GridInfo &gridInfo, bool isPrimary)
{
    if (outPixelFormat_ == PixelFormat::UNKNOWN) {
        IMAGE_LOGE("unknown pixel type: %{public}d", outPixelFormat_);
        return false;
    }
    if (image == nullptr) {
        return false;
    }

    std::string imageType = parser_->GetItemType(image->GetItemId());
    if (imageType == "iden") {
        return SwDecodeIdenImage(image, param, gridInfo, isPrimary);
    }

    static ImageFwkExtManager imageFwkExtManager;
    bool res = false;
    if (imageType == "grid") {
        param.gridInfo.enableGrid = true;
        gridInfo.enableGrid = true;
        res = SwDecodeGrids(imageFwkExtManager, image, param);
    } else if (imageType == "hvc1") {
        param.gridInfo.enableGrid = false;
        gridInfo.enableGrid = false;
        res = SwDecodeSingleImage(imageFwkExtManager, image, param);
    }
    return res;
}

bool HeifDecoderImpl::SwDecodeGrids(ImageFwkExtManager &extManager,
                                    std::shared_ptr<HeifImage> &image, HevcSoftDecodeParam &param)
{
    if (extManager.doHardWareEncodeFunc_ == nullptr && !extManager.LoadImageFwkExtNativeSo()) {
        return false;
    }
    if (param.dstBuffer == nullptr || param.dstStride == 0) {
        return false;
    }
    std::vector<std::shared_ptr<HeifImage>> tileImages;
    parser_->GetTileImages(image->GetItemId(), tileImages);
    if (tileImages.empty()) {
        IMAGE_LOGE("grid image has no tile image");
        return false;
    }
    size_t numGrid = tileImages.size();
    std::vector<std::vector<uint8_t>> inputs(numGrid);

    for (size_t index = 0; index < numGrid; ++index) {
        std::shared_ptr<HeifImage> &tileImage = tileImages[index];
        parser_->GetItemData(tileImage->GetItemId(),
                             &inputs[index], index == 0 ? heif_header_data : heif_no_header);
        ProcessChunkHead(inputs[index].data(), inputs[index].size());
    }

    int32_t retCode = extManager.hevcSoftwareDecodeFunc_(inputs, param);
    if (retCode != 0) {
        IMAGE_LOGE("SwDecodeGrids decode failed: %{public}d", retCode);
        return false;
    }
    return true;
}

bool HeifDecoderImpl::SwDecodeIdenImage(std::shared_ptr<HeifImage> &image,
                                        HevcSoftDecodeParam &param, GridInfo &gridInfo, bool isPrimary)
{
    if (!image) {
        return false;
    }
    std::shared_ptr<HeifImage> idenImage;
    parser_->GetIdenImage(image->GetItemId(), idenImage);
    if (idenImage == nullptr || idenImage == image) {
        IMAGE_LOGE("invalid iden image");
        return false;
    }
    return SwDecodeImage(idenImage, param, gridInfo, isPrimary);
}

bool HeifDecoderImpl::SwDecodeSingleImage(ImageFwkExtManager &extManager,
                                          std::shared_ptr<HeifImage> &image, HevcSoftDecodeParam &param)
{
    if (extManager.doHardWareEncodeFunc_ == nullptr && !extManager.LoadImageFwkExtNativeSo()) {
        return false;
    }
    if (param.dstBuffer == nullptr || param.dstStride == 0) {
        return false;
    }
    std::vector<std::vector<uint8_t>> inputs(1);
    parser_->GetItemData(image->GetItemId(), &inputs[0], heif_header_data);
    ProcessChunkHead(inputs[0].data(), inputs[0].size());

    int32_t retCode = extManager.hevcSoftwareDecodeFunc_(inputs, param);
    if (retCode != 0) {
        IMAGE_LOGE("SwDecodeSingleImage decode failed: %{public}d", retCode);
        return false;
    }
    return true;
}

static bool IsEmptyBuffer(uint8_t *buffer, uint32_t width, uint32_t height, uint32_t bpp, uint32_t rowStride)
{
    if (buffer == nullptr) {
        return true;
    }
    uint8_t *bufferRowStart = buffer;
    uint32_t rowBytes = width * bpp;
    for (uint32_t row = 0; row < height; ++row) {
        for (uint32_t col = 0; col < rowBytes; ++col) {
            if (bufferRowStart[col] != 0) {
                return false;
            }
        }
        bufferRowStart += rowStride;
    }
    return true;
}

static bool FillAlphaChannel(std::shared_ptr<HeifImage> &masterImage, uint8_t *alphaMemory,
                             size_t alphaStride, uint8_t *dstMemory, size_t dstRowStride)
{
    // merge alpha channel
    uint8_t *alphaRowStart = alphaMemory;
    uint8_t *dstRowStart = dstMemory;
    uint32_t width = masterImage->GetOriginalWidth();
    uint32_t height = masterImage->GetOriginalHeight();
    if (IsEmptyBuffer(reinterpret_cast<uint8_t*>(alphaMemory), width, height, 1, alphaStride)) {
        return false;
    }

    for (uint32_t row = 0; row < height; ++row) {
        uint8_t *dstPixel = dstRowStart;
        for (uint32_t col = 0; col < width; ++col) {
            uint32_t alphaVal = static_cast<uint32_t>(alphaRowStart[col]);
            dstPixel[PIXEL_OFFSET_0] = static_cast<uint8_t>(alphaVal * dstPixel[PIXEL_OFFSET_0] / MAX_ALPHA);
            dstPixel[PIXEL_OFFSET_1] = static_cast<uint8_t>(alphaVal * dstPixel[PIXEL_OFFSET_1] / MAX_ALPHA);
            dstPixel[PIXEL_OFFSET_2] = static_cast<uint8_t>(alphaVal * dstPixel[PIXEL_OFFSET_2] / MAX_ALPHA);
            dstPixel[PIXEL_OFFSET_3] = static_cast<uint8_t>(alphaVal);
            dstPixel += PIXEL_SIZE_4;
        }
        alphaRowStart += alphaStride;
        dstRowStart += dstRowStride;
    }
    return true;
}

static bool IsValidAlphaImage(std::shared_ptr<HeifImage> &masterImage, std::shared_ptr<HeifImage> &alphaImage,
                              PixelFormat dstPixFmt, bool isHardware)
{
    return alphaImage != nullptr && alphaImage != masterImage &&
        alphaImage->GetOriginalWidth() == masterImage->GetOriginalWidth() &&
        alphaImage->GetOriginalHeight() == masterImage->GetOriginalHeight() &&
        ((isHardware && alphaImage->GetDefaultPixelFormat() == HeifPixelFormat::YUV420) ||
        (!isHardware && (alphaImage->GetDefaultPixelFormat() == HeifPixelFormat::YUV420 ||
        alphaImage->GetDefaultPixelFormat() == HeifPixelFormat::MONOCHROME))) &&
        alphaImage->GetLumaBitNum() == LUMA_8_BIT &&
        (dstPixFmt == PixelFormat::RGBA_8888 || dstPixFmt == PixelFormat::BGRA_8888);
}

bool HeifDecoderImpl::HwApplyAlphaImage(std::shared_ptr<HeifImage> &masterImage,
                                        uint8_t *dstMemory, size_t dstRowStride)
{
    // check alpha image is available
    if (masterImage == nullptr || IsDirectYUVDecode()) {
        return false;
    }
    std::shared_ptr<HeifImage> alphaImage = masterImage->GetAlphaImage();
    if (!IsValidAlphaImage(masterImage, alphaImage, outPixelFormat_, true)) {
        return false;
    }

    // decode alpha image
    GridInfo alphaGridInfo;
    sptr<SurfaceBuffer> hwBuffer;
    InitGridInfo(alphaImage, alphaGridInfo);
    bool decodeRes = HwDecodeImage(nullptr, alphaImage, alphaGridInfo, &hwBuffer, false);
    if (!decodeRes) {
        IMAGE_LOGE("hw decode alpha image failed");
        return false;
    }

    // merge alpha channel
    return FillAlphaChannel(masterImage, reinterpret_cast<uint8_t *>(hwBuffer->GetVirAddr()),
                            hwBuffer->GetStride(), dstMemory, dstRowStride);
}

bool HeifDecoderImpl::SwApplyAlphaImage(std::shared_ptr<HeifImage> &masterImage,
                                        uint8_t *dstMemory, size_t dstRowStride)
{
    // check alpha image is available
    if (masterImage == nullptr || IsDirectYUVDecode()) {
        return false;
    }
    std::shared_ptr<HeifImage> alphaImage = masterImage->GetAlphaImage();
    if (!IsValidAlphaImage(masterImage, alphaImage, outPixelFormat_, false)) {
        return false;
    }

    GridInfo alphaGridInfo;
    InitGridInfo(alphaImage, alphaGridInfo);
    uint32_t alphaStride = alphaImage->GetOriginalWidth();
    uint32_t alphaMemorySize = alphaStride * alphaImage->GetOriginalHeight();
    PixelFormat alphaDstFmt = PixelFormat::ALPHA_8;
    std::unique_ptr<uint8_t[]> alphaMemory = std::make_unique<uint8_t[]>(alphaMemorySize);
    HevcSoftDecodeParam param {
        alphaGridInfo, alphaDstFmt, alphaMemory.get(), alphaMemorySize, alphaStride, nullptr
    };
    bool decodeRes = SwDecodeImage(alphaImage, param, alphaGridInfo, false);
    if (!decodeRes) {
        IMAGE_LOGE("sw decode alpha image failed");
        return false;
    }

    // merge alpha channel
    return FillAlphaChannel(masterImage, alphaMemory.get(), alphaStride, dstMemory, dstRowStride);
}

// LCOV_EXCL_START
bool HeifDecoderImpl::ConvertHwBufferPixelFormat(sptr<SurfaceBuffer> &hwBuffer, GridInfo &gridInfo,
                                                 uint8_t *dstMemory, size_t dstRowStride)
{
    OH_NativeBuffer_Planes *srcBufferPlanesInfo = nullptr;
    hwBuffer->GetPlanesInfo((void **)&srcBufferPlanesInfo);
    if (srcBufferPlanesInfo == nullptr) {
        IMAGE_LOGE("find to get src buffer planes info");
        return false;
    }

    OH_NativeBuffer_Planes *dstBufferPlanesInfo = nullptr;
    if (dstHwBuffer_ != nullptr && dstHwBuffer_->GetFormat() != GRAPHIC_PIXEL_FMT_RGBA_1010102) {
        dstHwBuffer_->GetPlanesInfo((void **)&dstBufferPlanesInfo);
        if (dstBufferPlanesInfo == nullptr) {
            IMAGE_LOGE("fail to get dst buffer planes info");
            return false;
        }
    }

    PixelFormatConvertParam srcParam = {static_cast<uint8_t *>(hwBuffer->GetVirAddr()),
                                        gridInfo.displayWidth, gridInfo.displayHeight,
                                        static_cast<uint32_t>(hwBuffer->GetStride()),
                                        srcBufferPlanesInfo,
                                        GraphicPixFmt2AvPixFmtForYuv(
                                            static_cast<GraphicPixelFormat>(hwBuffer->GetFormat()))};
    PixelFormatConvertParam dstParam = {dstMemory, gridInfo.displayWidth, gridInfo.displayHeight,
                                        static_cast<uint32_t>(dstRowStride),
                                        dstBufferPlanesInfo,
                                        PixFmt2AvPixFmtForOutput(outPixelFormat_)};
    return ConvertPixelFormat(srcParam, dstParam);
}
// LCOV_EXCL_STOP

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

bool HeifDecoderImpl::IsDirectYUVDecode()
{
    if (dstHwBuffer_ == nullptr) {
        return false;
    }
    if (primaryImage_->GetLumaBitNum() == LUMA_10_BIT) {
        return outPixelFormat_ == Media::PixelFormat::YCRCB_P010 || outPixelFormat_ == Media::PixelFormat::YCBCR_P010;
    }
    return outPixelFormat_ == Media::PixelFormat::NV21 || outPixelFormat_ == Media::PixelFormat::NV12;
}

bool HeifDecoderImpl::decodeSequence(int frameIndex, HeifFrameInfo *frameInfo)
{
    // unimplemented
    return false;
}

// LCOV_EXCL_START
void HeifDecoderImpl::setDstBuffer(uint8_t *dstBuffer, size_t rowStride, void *context)
{
    if (dstMemory_ == nullptr) {
        dstMemory_ = dstBuffer;
        dstRowStride_ = rowStride;
    }
    dstHwBuffer_ = reinterpret_cast<SurfaceBuffer*>(context);
}
// LCOV_EXCL_STOP

void HeifDecoderImpl::setGainmapDstBuffer(uint8_t* dstBuffer, size_t rowStride)
{
    if (gainmapDstMemory_ == nullptr) {
        gainmapDstMemory_ = dstBuffer;
        gainmapDstRowStride_ = rowStride;
    }
}

void HeifDecoderImpl::setAuxiliaryDstBuffer(uint8_t* dstBuffer, size_t rowStride)
{
    if (auxiliaryDstMemory_ == nullptr) {
        auxiliaryDstMemory_ = dstBuffer;
        auxiliaryDstRowStride_ = rowStride;
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

bool HeifDecoderImpl::getImageInfo(HeifFrameInfo *frameInfo)
{
    if (frameInfo != nullptr) {
        *frameInfo = imageInfo_;
    }
    return true;
}

bool HeifDecoderImpl::getGainmapInfo(HeifFrameInfo* frameInfo)
{
    if (frameInfo != nullptr) {
        *frameInfo = gainmapImageInfo_;
    }
    return true;
}

bool HeifDecoderImpl::getAuxiliaryMapInfo(HeifFrameInfo* frameInfo)
{
    if (frameInfo != nullptr) {
        *frameInfo = auxiliaryImageInfo_;
    }
    return true;
}

bool HeifDecoderImpl::getTmapInfo(HeifFrameInfo* frameInfo)
{
    if (frameInfo != nullptr) {
        *frameInfo = tmapInfo_;
    }
    return true;
}

HeifImageHdrType HeifDecoderImpl::getHdrType()
{
    std::vector<uint8_t> uwaInfo = primaryImage_->GetUWAInfo();
    if (primaryImage_->GetLumaBitNum() == LUMA_10_BIT && imageInfo_.hasNclxColor &&
        imageInfo_.nclxColor.colorPrimaries == BT2020_PRIMARIES) {
        return uwaInfo.empty() ? HeifImageHdrType::ISO_SINGLE : HeifImageHdrType::VIVID_SINGLE;
    }
    if (gainmapImage_ != nullptr) {
        return uwaInfo.empty() ? HeifImageHdrType::ISO_DUAL : HeifImageHdrType::VIVID_DUAL;
    }
    return HeifImageHdrType::UNKNOWN;
}

void HeifDecoderImpl::getVividMetadata(std::vector<uint8_t>& uwaInfo, std::vector<uint8_t>& displayInfo,
    std::vector<uint8_t>& lightInfo)
{
    uwaInfo = primaryImage_->GetUWAInfo();
    displayInfo = primaryImage_->GetDisplayInfo();
    lightInfo = primaryImage_->GetLightInfo();
}

void HeifDecoderImpl::getISOMetadata(std::vector<uint8_t>& isoMetadata)
{
    isoMetadata = primaryImage_->GetISOMetadata();
}

void HeifDecoderImpl::getErrMsg(std::string& errMsg)
{
    errMsg = errMsg_;
}

void HeifDecoderImpl::SetHardwareDecodeErrMsg(const uint32_t width, const uint32_t height)
{
    std::stringstream sstream;
    sstream << "HEIF Hardware Decode Failed, Width: ";
    sstream << width;
    sstream << ", Height: ";
    sstream << height;
    errMsg_ = sstream.str();
}
} // namespace ImagePlugin
} // namespace OHOS
#endif

HeifDecoder* CreateHeifDecoderImpl(void)
{
#ifdef HEIF_HW_DECODE_ENABLE
    return new OHOS::ImagePlugin::HeifDecoderImpl();
#else
    return nullptr;
#endif
}
