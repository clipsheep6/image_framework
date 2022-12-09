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
#include "heif_encoder.h"
#include "hilog/log.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_convert_adapter.h"
#include <map>
#include "securec.h"
#include "include/core/SkData.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/third_party/skcms/skcms.h"
#include "src/images/SkImageEncoderFns.h"

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace MultimediaPlugin;
using namespace Media;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "HeifEncoder" };
} // namespace
static constexpr uint8_t ICC_MARKER_HEADER[] = {
    0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x00, 0x01, 0x01
};
constexpr uint8_t MAX_QUALITY = 100;
constexpr uint8_t FIRST_IMAGE = 0;
constexpr uint32_t BIT_PRE_BYTE = 8;
constexpr uint32_t YCRCB_PIXEL_BITS = 8;
constexpr uint32_t NUM_0 = 0;
constexpr uint32_t NUM_1 = 1;
constexpr uint32_t NUM_2 = 2;
constexpr uint32_t MAX_ENCODERS = 5;
const std::string HEIF_ENCODER_NAME = "x265";
const std::string HEIF_PROFILE_FOURCC = "prof";

struct HeifEncoderInfos {
    struct heif_context* context = nullptr;
    struct heif_encoder* encoder = nullptr;
    const struct heif_encoder_descriptor* encoderDescriptor;
    struct heif_image* image = nullptr;
    struct heif_encoding_options* options = nullptr;
    struct heif_image_handle* handle = nullptr;
    struct heif_error err;
};
struct HeifEncoderOption {
    int width;
    int height;
    heif_colorspace colorSpace;
    heif_chroma chroma;
    PixelFormat dstFormat;
    int bitDepth;
    std::vector<uint8_t> profileData;
    bool premulAlpha;
    int quality;
    bool lossless;
};
struct HeifPlaneInfo {
    struct heif_image* image;
    enum heif_channel channel;
    int width;
    int height;
    int bitDepth;
    int32_t stride = 0;
    int32_t planeHeight = 0;
    const uint8_t* constPlane;
    uint8_t* plane = nullptr;
};
struct HeifEncoderPixelFormat {
    heif_colorspace colorspace;
    heif_chroma chroma;
    PixelFormat pixelFormat;
};
static const std::map<PixelFormat, HeifEncoderPixelFormat> HEIF_FORMAT_MAP = {
    {PixelFormat::ARGB_8888, {heif_colorspace_RGB, heif_chroma_interleaved_RGBA, PixelFormat::RGBA_8888}},
    {PixelFormat::RGB_565, {heif_colorspace_RGB, heif_chroma_interleaved_RGB, PixelFormat::RGB_888}},
    {PixelFormat::RGBA_8888, {heif_colorspace_RGB, heif_chroma_interleaved_RGBA, PixelFormat::RGBA_8888}},
    {PixelFormat::BGRA_8888, {heif_colorspace_RGB, heif_chroma_interleaved_RGBA, PixelFormat::RGBA_8888}},
    {PixelFormat::RGB_888, {heif_colorspace_RGB, heif_chroma_interleaved_RGB, PixelFormat::RGB_888}},
    {PixelFormat::ALPHA_8, {heif_colorspace_monochrome, heif_chroma_monochrome, PixelFormat::ALPHA_8}},
    {PixelFormat::RGBA_F16, {heif_colorspace_RGB, heif_chroma_interleaved_RRGGBBAA_BE,
    PixelFormat::RGBA_8888}},
    {PixelFormat::NV21, {heif_colorspace_YCbCr, heif_chroma_420, PixelFormat::NV21}},
    {PixelFormat::NV12, {heif_colorspace_YCbCr, heif_chroma_420, PixelFormat::NV12}}
};
HeifEncoder::HeifEncoder()
{
    HiLog::Debug(LABEL, "create IN");

    HiLog::Debug(LABEL, "create OUT");
}

HeifEncoder::~HeifEncoder()
{
    HiLog::Debug(LABEL, "release IN");

    pixelMaps_.clear();

    HiLog::Debug(LABEL, "release OUT");
}

uint32_t HeifEncoder::StartEncode(OutputDataStream &outputStream, PlEncodeOptions &option)
{
    HiLog::Debug(LABEL, "StartEncode IN");

    pixelMaps_.clear();

    outputStream_ = &outputStream;
    encodeOpts_ = option;

    HiLog::Debug(LABEL, "StartEncode OUT");
    return SUCCESS;
}

uint32_t HeifEncoder::AddImage(Media::PixelMap &pixelMap)
{
    HiLog::Debug(LABEL, "AddImage IN");

    if (pixelMaps_.empty()) {
        pixelMaps_.push_back(&pixelMap);
    } else {
        HiLog::Error(LABEL, "AddImage, add pixel map out of range=%{public}zu.", pixelMaps_.size());
        return ERR_IMAGE_ADD_PIXEL_MAP_FAILED;
    }

    HiLog::Debug(LABEL, "AddImage OUT");
    return SUCCESS;
}

uint32_t HeifEncoder::FinalizeEncode()
{
    HiLog::Debug(LABEL, "FinalizeEncode IN");

    if (pixelMaps_.empty()) {
        HiLog::Error(LABEL, "FinalizeEncode, no pixel map input.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    HiLog::Debug(LABEL, "FinalizeEncode, quality=%{public}" PRIu32 ", numberHint=%{public}" PRIu32,
        encodeOpts_.quality, encodeOpts_.numberHint);

    uint32_t errorCode = DoEncode();

    HiLog::Debug(LABEL, "FinalizeEncode OUT");
    return errorCode;
}

static void ReleaseHeifEncoderInfo(HeifEncoderInfos* c)
{
    if (c->handle != nullptr) {
        heif_image_handle_release(c->handle);
        c->handle = nullptr;
    }
    if (c->options != nullptr) {
        heif_encoding_options_free(c->options);
        c->options = nullptr;
    }
    if (c->encoder != nullptr) {
        heif_encoder_release(c->encoder);
        c->encoder = nullptr;
    }
    if (c->image != nullptr) {
        heif_image_release(c->image);
        c->image = nullptr;
    }
}
static inline uint32_t ShowHeifInnerErr(struct heif_error* err, std::string errMsg, uint32_t error)
{
    if (err->code != heif_error_Ok) {
        if (err->message != nullptr) {
            HiLog::Error(LABEL, "%{public}s: %{public}s.", errMsg.c_str(), err->message);
        }
        return error;
    }
    return SUCCESS;
}

static inline struct heif_error BuildError(enum heif_error_code iCode, enum heif_suberror_code sub,
    const char* msg)
{
    heif_error err = { .code = iCode, .subcode = sub, .message = msg };
    return err;
}

static struct heif_error HeifWriteToStream(struct heif_context* ctx,
    const uint8_t* data, size_t size, OutputDataStream* outStream)
{
    if (data == nullptr) {
        return BuildError(heif_error_Invalid_input, heif_suberror_Cannot_write_output_data,
            "Write input buffer is nullptr");
    }
    if (size == 0) {
        return BuildError(heif_error_Invalid_input, heif_suberror_Cannot_write_output_data,
            "Write input size is 0");
    }
    if(outStream->Write(data, size)) {
        outStream->Flush();
    } else {
        return BuildError(heif_error_Encoding_error, heif_suberror_Cannot_write_output_data,
            "Output failed");
    }
    return BuildError(heif_error_Ok, heif_suberror_Unspecified, "Success");
}

using HeifWrite = struct heif_error (*)(struct heif_context* ctx, const void* data, size_t size, void* user);
static uint32_t DoOutput(std::shared_ptr<HeifEncoderInfos> &info, OutputDataStream *outStream)
{
    heif_writer writer = {
        .writer_api_version = 1,
        .write = reinterpret_cast<HeifWrite>(HeifWriteToStream)
    };
    info->err = heif_context_write(info->context, &writer, static_cast<void*>(outStream));
    return ShowHeifInnerErr(&(info->err), "Heif write error", ERR_IMAGE_ENCODE_FAILED);
}
static inline void ProfileFromPixelmap(Media::PixelMap* pixelmap, std::vector<uint8_t> &profile) {
    if (!pixelmap->HasGrColorSpace()) {
        return;
    }
    auto grColorSpace = pixelmap->InnerGetGrColorSpace();
    auto skColorSpace = grColorSpace.ToSkColorSpace();
    if (skColorSpace == nullptr) {
        HiLog::Error(LABEL, "Sk Color Space get error.");
        return;
    }
    skcms_TransferFunction fn;
    if (!skColorSpace->isNumericalTransferFn(&fn)) {
        HiLog::Error(LABEL, "Sk Color Space isNumericalTransferFn error.");
        return;
    }
    skcms_Matrix3x3 toXYZD50;
    if (!skColorSpace->toXYZD50(&toXYZD50)) {
        HiLog::Error(LABEL, "Sk Color Space toXYZD50 error.");
        return;
    }
    sk_sp<SkData> icc = SkWriteICCProfile(fn, toXYZD50);
    if (icc == nullptr) {
        HiLog::Error(LABEL, "SkWriteICCProfile error.");
        return;
    }

    profile.resize(sizeof(ICC_MARKER_HEADER) + icc->size());
    errno_t ret = memcpy_s(profile.data(), profile.size(), ICC_MARKER_HEADER, sizeof(ICC_MARKER_HEADER));
    if (ret != 0) {
        HiLog::Error(LABEL, "ICC header memcpy failed,ret=%{public}" PRId32 ".", ret);
        profile.clear();
        profile.resize(0);
        return;
    }

    ret = memcpy_s(profile.data() + sizeof(ICC_MARKER_HEADER), profile.size() - sizeof(ICC_MARKER_HEADER),
        icc->data(), icc->size());
    if (ret != 0) {
        HiLog::Error(LABEL, "ICC data memcpy failed,ret=%{public}" PRId32 ".", ret);
        profile.clear();
        profile.resize(0);
        return;
    }
    return;
}
static uint32_t SetHeifEncoderOption(std::shared_ptr<HeifEncoderOption> option,
    Media::PixelMap* pixelmap, PlEncodeOptions plOption)
{
    if (pixelmap == nullptr) {
        HiLog::Error(LABEL, "Input pixelmap is nullptr");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    auto pixelFormat = pixelmap->GetPixelFormat();
    if (pixelFormat == PixelFormat::UNKNOWN || pixelFormat == PixelFormat::CMYK) {
        return ERR_IMAGE_DATA_UNSUPPORT;
    }
    auto iter = HEIF_FORMAT_MAP.find(pixelFormat);
    if (iter == HEIF_FORMAT_MAP.end()) {
        HiLog::Error(LABEL, "Unsupport format error : %{public}" PRId32 ".", pixelFormat);
        return ERR_IMAGE_DATA_UNSUPPORT;
    }
    auto info = iter->second;
    option->colorSpace = info.colorspace;
    option->chroma = info.chroma;
    option->dstFormat = info.pixelFormat;
    option->width = pixelmap->GetWidth();
    option->height = pixelmap->GetHeight();
    option->bitDepth = pixelmap->GetPixelBytes() * BIT_PRE_BYTE;
    option->premulAlpha = pixelmap->GetAlphaType() == AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    option->quality = plOption.quality;
    option->lossless = plOption.quality == MAX_QUALITY;
    ProfileFromPixelmap(pixelmap, option->profileData);
    return SUCCESS;
}
static inline uint32_t CheckAddPlaneArgs(std::shared_ptr<HeifEncoderInfos> info, Media::PixelMap* pixelmap)
{
    if (pixelmap == nullptr) {
        HiLog::Error(LABEL, "Input pixelmap is nullptr");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    if (pixelmap->GetPixels() == nullptr) {
        HiLog::Error(LABEL, "Input pixelmap data is nullptr");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    if (info->image == nullptr) {
        HiLog::Error(LABEL, "Input heif image is nullptr");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    return SUCCESS;
}
static inline uint32_t AddOnePlane(HeifPlaneInfo &info)
{
    struct heif_error err = heif_image_add_plane(info.image, info.channel, info.width,
        info.height, info.bitDepth);

    if (ShowHeifInnerErr(&err, "Heif add plane error", ERR_IMAGE_ENCODE_FAILED) != SUCCESS) {
        return ERR_IMAGE_ENCODE_FAILED;
    }

    info.planeHeight = heif_image_get_height(info.image, info.channel);
    if (info.height <= 0) {
        HiLog::Error(LABEL, "Heif image plane height is invalid");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    info.plane = heif_image_get_plane(info.image, info.channel, &(info.stride));
    if (info.plane == nullptr) {
        HiLog::Error(LABEL, "Heif image plane is nullptr");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    return SUCCESS;
}
static uint32_t AddPlane(std::shared_ptr<HeifEncoderOption> opt, enum heif_channel channel,
    std::shared_ptr<HeifEncoderInfos> info, Media::PixelMap* pixelmap)
{
    uint32_t ret = CheckAddPlaneArgs(info, pixelmap);
    if (ret != SUCCESS) {
        return ret;
    }
    HeifPlaneInfo src = {
        .stride = pixelmap->GetRowBytes(),
        .planeHeight = pixelmap->GetHeight(),
        .constPlane = pixelmap->GetPixels(),
    };

    HeifPlaneInfo dst = {
        .image = info->image,
        .channel = channel,
        .width = opt->width,
        .height = opt->height,
        .bitDepth = opt->bitDepth,
    };

    ret = AddOnePlane(dst);
    if (ret != SUCCESS) {
        return ret;
    }

    if (src.stride > dst.stride || src.planeHeight > dst.planeHeight) {
        HiLog::Error(LABEL,
            "src.stride %{public}" PRId32 " src.height %{public}" PRId32
            " dst.stride %{public}" PRId32 " dst.planeHeight %{public}" PRId32 ".",
            src.stride, src.planeHeight, dst.stride, dst.planeHeight);
        return ERR_IMAGE_ENCODE_FAILED;
    }
    errno_t res = 0;
    for (int32_t y = 0; y < src.planeHeight; y++) {
        res = memcpy_s(dst.plane + (dst.stride * y), dst.stride, src.constPlane + (src.stride * y), src.stride);
        if (res != 0) {
            HiLog::Error(LABEL, "AddPlane memcpy failed,ret=%{public}" PRId32 ".", res);
            return ERR_IMAGE_ENCODE_FAILED;
        }
    }
    return SUCCESS;
}
static uint32_t CopyYCbCrPlaneData(HeifPlaneInfo &src, bool uPlaneFirst, HeifPlaneInfo &yDst,
    HeifPlaneInfo &cbDst, HeifPlaneInfo &crDst)
{
    errno_t ret = 0;
    for (int32_t y = 0; y < src.planeHeight; y++) {
        ret = memcpy_s(yDst.plane + (yDst.stride * y), yDst.stride, src.constPlane + (src.width * y), src.width);
        if (ret != 0) {
            HiLog::Error(LABEL, "AddRGBPlane memcpy failed,ret=%{public}" PRId32 ".", ret);
            return ERR_IMAGE_ENCODE_FAILED;
        }
    }
    const uint8_t* uvPlane = src.constPlane + (src.width * src.planeHeight);
    int32_t lastCount = src.stride * src.planeHeight - src.width * src.planeHeight;

    for (int32_t i = 0; (i * NUM_2 + NUM_1) < lastCount; i++) {
        if (uPlaneFirst) {
            cbDst.plane[i] = uvPlane[i * NUM_2];
            crDst.plane[i] = uvPlane[i * NUM_2 + NUM_1];
        } else {
            crDst.plane[i] = uvPlane[i * NUM_2];
            cbDst.plane[i] = uvPlane[i * NUM_2 + NUM_1];
        }
    }
    return SUCCESS;
}
static uint32_t AddYCbCrPlane(std::shared_ptr<HeifEncoderOption> opt, std::shared_ptr<HeifEncoderInfos> info,
    Media::PixelMap* pixelmap)
{
    uint32_t ret = CheckAddPlaneArgs(info, pixelmap);
    if (ret != SUCCESS) {
        return ret;
    }
    HeifPlaneInfo src = {
        .stride = pixelmap->GetRowBytes(),
        .width = pixelmap->GetWidth(),
        .planeHeight = pixelmap->GetHeight(),
        .constPlane = pixelmap->GetPixels(),
    };

    HeifPlaneInfo yDst = {
        .image = info->image,
        .channel = heif_channel_Y,
        .width = opt->width,
        .height = opt->height,
        .bitDepth = YCRCB_PIXEL_BITS,
    };

    ret = AddOnePlane(yDst);
    if (ret != SUCCESS) {
        return ret;
    }

    HeifPlaneInfo cbDst = {
        .image = info->image,
        .channel = heif_channel_Cb,
        .width = (opt->width + NUM_1) / NUM_2,
        .height = (opt->height + NUM_1) / NUM_2,
        .bitDepth = YCRCB_PIXEL_BITS,
    };
    ret = AddOnePlane(cbDst);
    if (ret != SUCCESS) {
        return ret;
    }

    HeifPlaneInfo crDst = {
        .image = info->image,
        .channel = heif_channel_Cr,
        .width = (opt->width + NUM_1) / NUM_2,
        .height = (opt->height + NUM_1) / NUM_2,
        .bitDepth = YCRCB_PIXEL_BITS,
    };
    ret = AddOnePlane(crDst);
    if (ret != SUCCESS) {
        return ret;
    }
    return CopyYCbCrPlaneData(src, pixelmap->GetPixelFormat() == PixelFormat::NV12, yDst, cbDst, crDst);
}
static uint32_t AllocHeifEncoder(std::shared_ptr<HeifEncoderInfos> &info)
{
    if (info == nullptr) {
        HiLog::Error(LABEL, "Input heif encoder infos is nullptr.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }

    info->context = heif_context_alloc();
    if (info->context == nullptr) {
        HiLog::Error(LABEL, "Alloc heif context error.");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    std::vector<const struct heif_encoder_descriptor*> encoderDescriptors(MAX_ENCODERS);
    int count = heif_context_get_encoder_descriptors(info->context, heif_compression_HEVC, nullptr,
        encoderDescriptors.data(), encoderDescriptors.size());
    if (count == NUM_0) {
        HiLog::Error(LABEL, "No encoder supported.");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    for (int i = 0; i < count; i++) {
        std::string encoderName(heif_encoder_descriptor_get_id_name(encoderDescriptors[i]));
        if (encoderName.size() == HEIF_ENCODER_NAME.size() &&
            (encoderName.compare(0, HEIF_ENCODER_NAME.size(), HEIF_ENCODER_NAME) == 0)) {
                info->encoderDescriptor = encoderDescriptors[i];
                break;
        }
    }
    if (info->encoderDescriptor == nullptr) {
        HiLog::Error(LABEL, "No encoder matched.");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    info->err = heif_context_get_encoder(info->context, info->encoderDescriptor, &(info->encoder));
    if (ShowHeifInnerErr(&(info->err), "Heif get encoder error", ERR_IMAGE_ENCODE_FAILED) != SUCCESS) {
        return ERR_IMAGE_ENCODE_FAILED;
    }
    info->options = heif_encoding_options_alloc();
    if (info->options == nullptr) {
        HiLog::Error(LABEL, "Alloc heif encoding options error.");
        return ERR_IMAGE_ENCODE_FAILED;
    }
    return SUCCESS;
}
static inline uint32_t SetHeifEncoderParameters(std::shared_ptr<HeifEncoderOption> &opt,
    std::shared_ptr<HeifEncoderInfos> &info)
{
    if(opt == nullptr || info == nullptr) {
        HiLog::Error(LABEL, "Input opt or info is nullptr");
        return ERR_IMAGE_ENCODE_FAILED;
    }

    info->err = heif_image_set_raw_color_profile(info->image, HEIF_PROFILE_FOURCC.c_str(),
        opt->profileData.data(), opt->profileData.size());
    (void) ShowHeifInnerErr(&(info->err), "Heif set color profile error", ERR_IMAGE_ENCODE_FAILED);
    if (opt->lossless) {
        if (heif_encoder_descriptor_supportes_lossless_compression(info->encoderDescriptor)) {
            info->err = heif_encoder_set_lossless(info->encoder, true);
            (void) ShowHeifInnerErr(&(info->err),
                "Heif encoder set lossless error", ERR_IMAGE_ENCODE_FAILED);
        } else {
            HiLog::Info(LABEL, "Heif encoder not support lossless.");
        }
    }
    info->err = heif_encoder_set_lossy_quality(info->encoder, opt->quality);
    if (ShowHeifInnerErr(&(info->err),
        "Heif set lossy quality error", ERR_IMAGE_ENCODE_FAILED) != SUCCESS) {
        return ERR_IMAGE_ENCODE_FAILED;
    }
    // Some parameter may by heif_encoder_set_parameter()?
    if (opt->premulAlpha) {
        heif_image_set_premultiplied_alpha(info->image, opt->premulAlpha);
    }
    return SUCCESS;
}
uint32_t HeifEncoder::DoEncode()
{
    if (pixelMaps_.empty()) {
        HiLog::Error(LABEL, "No Encode data.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    std::shared_ptr<HeifEncoderOption> opt = std::make_shared<HeifEncoderOption>();
    uint32_t ret = SetHeifEncoderOption(opt, pixelMaps_[FIRST_IMAGE], encodeOpts_);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "Set heif encoder option error %{public}" PRIu32 ".", ret);
        return ret;
    }
    std::shared_ptr<HeifEncoderInfos> info(new HeifEncoderInfos, ReleaseHeifEncoderInfo);
    ret = AllocHeifEncoder(info);
    if (ret != SUCCESS) {
        return ret;
    }
    info->err = heif_image_create(opt->width, opt->height, opt->colorSpace, opt->chroma, &(info->image));
    if (ShowHeifInnerErr(&(info->err), "Heif image create error", ERR_IMAGE_ENCODE_FAILED) != SUCCESS) {
        return ERR_IMAGE_ENCODE_FAILED;
    }
    if (opt->colorSpace == heif_colorspace_RGB) {
        ret = AddPlane(opt, heif_channel_interleaved, info, pixelMaps_[FIRST_IMAGE]);
    } else if (opt->colorSpace == heif_colorspace_YCbCr) {
        ret = AddYCbCrPlane(opt, info, pixelMaps_[FIRST_IMAGE]);
    } else if (opt->colorSpace == heif_colorspace_monochrome) {
        ret = AddPlane(opt, heif_channel_Y, info, pixelMaps_[FIRST_IMAGE]);
    } else {
        HiLog::Error(LABEL, "Heif color space error %{public}" PRId32 ".", opt->colorSpace);
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    ret = SetHeifEncoderParameters(opt, info);
    if (ret != SUCCESS) {
        return ret;
    }
    info->err = heif_context_encode_image(info->context, info->image, info->encoder,
        info->options, &(info->handle));
    if (ShowHeifInnerErr(&(info->err),
        "Heif endcode error", ERR_IMAGE_ENCODE_FAILED) != SUCCESS) {
        return ERR_IMAGE_ENCODE_FAILED;
    }
    return DoOutput(info, outputStream_);
}
} // namespace ImagePlugin
} // namespace OHOS