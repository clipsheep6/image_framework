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
#include "svg_decoder.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "securec.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "media_errors.h"
namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace MultimediaPlugin;
using namespace Media;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "SvgDecoder" };
constexpr uint32_t SVG_IMAGE_NUM = 1;
} // namespace

SvgDecoder::SvgDecoder()
{
    HiLog::Debug(LABEL, "[Create] IN");

    HiLog::Debug(LABEL, "[Create] OUT");
}

SvgDecoder::~SvgDecoder()
{
    HiLog::Debug(LABEL, "[Release] IN");

    Reset();

    HiLog::Debug(LABEL, "[Release] OUT");
}

void SvgDecoder::SetSource(InputDataStream &sourceStream)
{
    HiLog::Debug(LABEL, "[SetSource] IN");

    Reset();

    inputStreamPtr_ = &sourceStream;
    state_ = SvgDecodingState::SOURCE_INITED;

    HiLog::Debug(LABEL, "[SetSource] OUT");
}

void SvgDecoder::Reset()
{
    HiLog::Debug(LABEL, "[Reset] IN");

    state_ = SvgDecodingState::UNDECIDED;

    if (svgDom_) {
        svgDom_->setContainerSize(svgSize_);
    }

    svgDom_ = nullptr;
    svgStream_ = nullptr;
    inputStreamPtr_ = nullptr;

    svgSize_.setEmpty();

    PixelDecodeOptions opts;
    opts_ = opts;

    HiLog::Debug(LABEL, "[Reset] OUT");
}

uint32_t SvgDecoder::SetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info)
{
    if (index >= SVG_IMAGE_NUM) {
        HiLog::Error(LABEL, "[SetDecodeOptions] decode image index[%{public}u], out of range[%{public}u].",
            index, SVG_IMAGE_NUM);
        return Media::ERR_IMAGE_INVALID_PARAMETER;
    }

    HiLog::Debug(LABEL, "[SetDecodeOptions] IN index=%{public}u, pixelFormat=%{public}d, alphaType=%{public}d, "
        "colorSpace=%{public}d, size=(%{public}u, %{public}u), state=%{public}d", index,
        static_cast<int32_t>(opts.desiredPixelFormat), static_cast<int32_t>(opts.desireAlphaType),
        static_cast<int32_t>(opts.desiredColorSpace), opts.desiredSize.width, opts.desiredSize.height, state_);

    if (state_ < SvgDecodingState::SOURCE_INITED) {
        HiLog::Error(LABEL, "[SetDecodeOptions] set decode options failed for state %{public}d.", state_);
        return Media::ERR_MEDIA_INVALID_OPERATION;
    }

    if (state_ >= SvgDecodingState::IMAGE_DECODING) {
        state_ = SvgDecodingState::SOURCE_INITED;
    }

    if (state_ < SvgDecodingState::BASE_INFO_PARSED) {
        uint32_t ret = DoDecodeHeader();
        if (ret != Media::SUCCESS) {
            HiLog::Error(LABEL, "[SetDecodeOptions] decode header error on set decode options:%{public}u.", ret);
            state_ = SvgDecodingState::BASE_INFO_PARSING;
            return ret;
        }

        state_ = SvgDecodingState::BASE_INFO_PARSED;
    }

    // only state SvgDecodingState::BASE_INFO_PARSED can go here.
    uint32_t ret = DoSetDecodeOptions(index, opts, info);
    if (ret != Media::SUCCESS) {
        HiLog::Error(LABEL, "[SetDecodeOptions] do set decode options:%{public}u.", ret);
        state_ = SvgDecodingState::BASE_INFO_PARSING;
        return ret;
    }

    state_ = SvgDecodingState::IMAGE_DECODING;

    HiLog::Debug(LABEL, "[SetDecodeOptions] OUT");
    return Media::SUCCESS;
}

uint32_t SvgDecoder::Decode(uint32_t index, DecodeContext &context)
{
    if (index >= SVG_IMAGE_NUM) {
        HiLog::Error(LABEL, "[Decode] decode image index[%{public}u], out of range[%{public}u].",
            index, SVG_IMAGE_NUM);
        return Media::ERR_IMAGE_INVALID_PARAMETER;
    }

    HiLog::Debug(LABEL, "[Decode] IN index=%{public}u", index);

    if (state_ < SvgDecodingState::IMAGE_DECODING) {
        HiLog::Error(LABEL, "[Decode] decode failed for state %{public}d.", state_);
        return Media::ERR_MEDIA_INVALID_OPERATION;
    }

    uint32_t ret = DoDecode(index, context);
    if (ret == Media::SUCCESS) {
        HiLog::Info(LABEL, "[Decode] success.");
        state_ = SvgDecodingState::IMAGE_DECODED;
    } else {
        HiLog::Error(LABEL, "[Decode] fail, ret=%{public}u", ret);
        state_ = SvgDecodingState::IMAGE_ERROR;
    }

    HiLog::Debug(LABEL, "[Decode] OUT ret=%{public}u", ret);
    return ret;
}

uint32_t SvgDecoder::PromoteIncrementalDecode(uint32_t index, ProgDecodeContext &context)
{
    // currently not support increment decode
    return ERR_IMAGE_DATA_UNSUPPORT;
}

// need decode all frame to get total number.
uint32_t SvgDecoder::GetTopLevelImageNum(uint32_t &num)
{
    num = SVG_IMAGE_NUM;
    return Media::SUCCESS;
}

// return background size but not specific frame size, cause of frame drawing on background.
uint32_t SvgDecoder::GetImageSize(uint32_t index, PlSize &size)
{
    return Media::SUCCESS;
}

bool SvgDecoder::AllocBuffer(DecodeContext &context)
{
    HiLog::Debug(LABEL, "[AllocBuffer] IN");

    if (svgDom_ == nullptr) {
        HiLog::Error(LABEL, "[AllocBuffer] DOM is null.");
        return false;
    }

    bool ret = true;
    HiLog::Debug(LABEL, "[AllocBuffer] OUT ret=%{public}d", ret);
    return ret;
}

bool SvgDecoder::BuildStream()
{
    HiLog::Debug(LABEL, "[BuildStream] IN");
    HiLog::Debug(LABEL, "[BuildStream] OUT");
    return true;
}

bool SvgDecoder::BuildDom()
{
    HiLog::Debug(LABEL, "[BuildDom] IN");
    return true;
}

uint32_t SvgDecoder::DoDecodeHeader()
{
    HiLog::Debug(LABEL, "[DoDecodeHeader] IN");

    if (!BuildStream()) {
        HiLog::Error(LABEL, "[DoDecodeHeader] Build Stream failed");
        return Media::ERR_IMAGE_TOO_LARGE;
    }

    if (!BuildDom()) {
        HiLog::Error(LABEL, "[DoDecodeHeader] Build DOM failed");
        return Media::ERR_IMAGE_DATA_UNSUPPORT;
    }

    HiLog::Debug(LABEL, "[DoDecodeHeader] OUT");
    return Media::SUCCESS;
}

uint32_t SvgDecoder::DoSetDecodeOptions(uint32_t index, const PixelDecodeOptions &opts, PlImageInfo &info)
{
    HiLog::Debug(LABEL, "[DoSetDecodeOptions] IN index=%{public}u", index);
    return Media::SUCCESS;
}

uint32_t SvgDecoder::DoGetImageSize(uint32_t index, PlSize &size)
{
    HiLog::Debug(LABEL, "[DoGetImageSize] IN index=%{public}u", index);
    return Media::SUCCESS;
}

uint32_t SvgDecoder::DoDecode(uint32_t index, DecodeContext &context)
{
    HiLog::Debug(LABEL, "[DoDecode] IN index=%{public}u", index);
    HiLog::Debug(LABEL, "[DoDecode] OUT");
    return Media::SUCCESS;
}
} // namespace ImagePlugin
} // namespace OHOS
