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

#include "astc_codec.h"
#include "image_compressor.h"
#include "image_system_properties.h"
#include "securec.h"
#include "media_errors.h"
#include "hilog/log.h"
#include "log_tags.h"

namespace OHOS {
namespace ImagePlugin {
using namespace OHOS::HiviewDFX;
using namespace Media;
namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "AstcCodec"};
}

constexpr uint8_t TEXTURE_HEAD_BYTES = 16;
constexpr uint8_t ASTC_MASK = 0xFF;
constexpr uint8_t ASTC_NUM_3 = 3;
constexpr uint8_t ASTC_NUM_4 = 4;
constexpr uint8_t ASTC_NUM_8 = 8;
constexpr uint8_t ASTC_HEADER_SIZE = 16;
constexpr uint8_t ASTC_NUM_24 = 24;
constexpr uint8_t ASTC_NUM_48 = 48;
constexpr uint8_t ASTC_NUM_52 = 52;
static const uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;

uint32_t AstcCodec::SetAstcEncode(OutputDataStream* outputStream, PlEncodeOptions &option, Media::PixelMap* pixelMap)
{
    if (outputStream == nullptr || pixelMap == nullptr) {
        HiLog::Error(LABEL, "input data is nullptr.");
        return ERROR;
    }
    astcOutput_ = outputStream;
    astcOpts_ = option;
    astcPixelMap_ = pixelMap;
    return SUCCESS;
}

// test ASTCEncoder
uint32_t GenAstcHeader(uint8_t *header, astcenc_image img, TextureEncodeOptions *encodeParams, size_t size)
{
    if ((encodeParams == nullptr) || (header == nullptr) || size < ASTC_HEADER_SIZE) {
        HiLog::Error(LABEL, "header is nullptr or encodeParams is nullptr or header_size is error");
        return ERROR;
    }
    uint8_t *tmp = header;
    *tmp++ = ASTC_MAGIC_ID & ASTC_MASK;
    *tmp++ = (ASTC_MAGIC_ID >> ASTC_NUM_8) & ASTC_MASK;
    *tmp++ = (ASTC_MAGIC_ID >> ASTC_HEADER_SIZE) & ASTC_MASK;
    *tmp++ = (ASTC_MAGIC_ID >> ASTC_NUM_24) & ASTC_MASK;
    *tmp++ = static_cast<uint8_t>(encodeParams->blockX_);
    *tmp++ = static_cast<uint8_t>(encodeParams->blockY_);
    *tmp++ = 1;
    *tmp++ = img.dim_x & ASTC_MASK;
    *tmp++ = (img.dim_x >> ASTC_NUM_8) & ASTC_MASK;
    *tmp++ = (img.dim_x >> ASTC_HEADER_SIZE) & ASTC_MASK;
    *tmp++ = img.dim_y & ASTC_MASK;
    *tmp++ = (img.dim_y >> ASTC_NUM_8) & ASTC_MASK;
    *tmp++ = (img.dim_y >> ASTC_HEADER_SIZE) & ASTC_MASK;
    *tmp++ = img.dim_z & ASTC_MASK;
    *tmp++ = (img.dim_z >> ASTC_NUM_8) & ASTC_MASK;
    *tmp++ = (img.dim_z >> ASTC_HEADER_SIZE) & ASTC_MASK;
    return SUCCESS;
}

uint32_t InitAstcencConfig(AstcEncoder* work, TextureEncodeOptions* option)
{
    if ((work == nullptr) || (option == nullptr)) {
        HiLog::Error(LABEL, "astc input work or option is nullptr.");
        return ERROR;
    }
    unsigned int blockX = option->blockX_;
    unsigned int blockY = option->blockY_;
    unsigned int blockZ = 1;

    float quality = ASTCENC_PRE_FAST;
    unsigned int flags = 0;
    astcenc_error status = astcenc_config_init(work->profile, blockX, blockY,
        blockZ, quality, flags, &work->config);
    if (status == ASTCENC_ERR_BAD_BLOCK_SIZE) {
        HiLog::Error(LABEL, "ERROR: block size is invalid");
        return ERROR;
    } else if (status == ASTCENC_ERR_BAD_CPU_FLOAT) {
        HiLog::Error(LABEL, "ERROR: astcenc must not be compiled with fast-math");
        return ERROR;
    } else if (status != ASTCENC_SUCCESS) {
        HiLog::Error(LABEL, "ERROR: config failed");
        return ERROR;
    }
    if (astcenc_context_alloc(&work->config, 1, &work->codec_context) != ASTCENC_SUCCESS) {
        return ERROR;
    }
    return SUCCESS;
}

void extractDimensions(std::string &format, TextureEncodeOptions &param)
{
    std::size_t slashPos = format.rfind('/');
    if (slashPos != std::string::npos) {
        std::string dimensions = format.substr(slashPos + 1);
        std::size_t starPos = dimensions.find('*');
        if (starPos != std::string::npos) {
            std::string widthStr = dimensions.substr(0, starPos);
            std::string heightStr = dimensions.substr(starPos + 1);

            param.blockX_ = static_cast<uint8_t>(std::stoi(widthStr));
            param.blockY_ = static_cast<uint8_t>(std::stoi(heightStr));
        }
    }
}

#if QUALITY_CONTROL
constexpr double MAX_PSNR = 99.9;
constexpr double MAX_VALUE = 255;
constexpr double THRESHOLD_R = 30.0;
constexpr double THRESHOLD_G = 30.0;
constexpr double THRESHOLD_B = 30.0;
constexpr double THRESHOLD_A = 30.0;
constexpr double THRESHOLD_RGB = 30.0;
constexpr double LOG_BASE = 10.0;
bool CheckQuality(int32_t *mseIn[RGBA_COM], int blockNum, int blockXYZ)
{
    double psnr[RGBA_COM + 1];
    double threshold[RGBA_COM + 1] = {THRESHOLD_R, THRESHOLD_G, THRESHOLD_B, THRESHOLD_A, THRESHOLD_RGB};
    uint64_t mseTotal[RGBA_COM + 1] = {0, 0, 0, 0, 0};
    for (int i = R_COM; i < RGBA_COM; i++) {
        int32_t *mse = mseIn[i];
        if (!mse) {
            return false;
        }
        for (int j = 0; j < blockNum; j++) {
            mseTotal[i] += *mse;
            if (i != A_COM) mseTotal[RGBA_COM] += *mse;
            mse++;
        }
    }
    for (int i = R_COM; i < RGBA_COM; i++) {
        if (mseTotal[i] == 0) {
            psnr[i] = MAX_PSNR;
            continue;
        }
        double mseRgb = static_cast<double>(mseTotal[i] / (blockNum * blockXYZ));
        psnr[i] = LOG_BASE * log(static_cast<double>(MAX_VALUE * MAX_VALUE) / mseRgb) / log(LOG_BASE);
    }
    if (mseTotal[RGBA_COM] == 0) {
        psnr[RGBA_COM] = MAX_PSNR;
    } else {
        double mseRgb = static_cast<double>(mseTotal[RGBA_COM] / (blockNum * blockXYZ * (RGBA_COM - 1)));
        psnr[RGBA_COM] = LOG_BASE * log(static_cast<double>(MAX_VALUE * MAX_VALUE) / mseRgb) / log(LOG_BASE);
    }
    HiLog::Debug(LABEL, "astc psnr r%{public}f g%{public}f b%{public}f a%{public}f rgb%{public}f",
        psnr[R_COM], psnr[G_COM], psnr[B_COM], psnr[A_COM],
        psnr[RGBA_COM]);
    return (psnr[R_COM] > threshold[R_COM]) && (psnr[G_COM] > threshold[G_COM])
        && (psnr[B_COM] > threshold[B_COM]) && (psnr[A_COM] > threshold[A_COM])
        && (psnr[RGBA_COM] > threshold[RGBA_COM]);
}
#endif

static void FreeMem(AstcEncoder *work)
{
    if (!work) {
        return;
    }
#if QUALITY_CONTROL
    if (work->calQualityEnable) {
        for (int i = R_COM; i < RGBA_COM; i++) {
            if (work->mse[i]) {
                free(work->mse[i]);
                work->mse[i] = nullptr;
            }
        }
    }
#endif
    if (work->image_.data) {
        free(work->image_.data);
        work->image_.data = nullptr;
    }
    if (work->codec_context != nullptr) {
        astcenc_context_free(work->codec_context);
        work->codec_context = nullptr;
    }
}

static bool InitMem(AstcEncoder *work, TextureEncodeOptions param, bool enableQualityCheck, int blockNum)
{
    if (!work) {
        return false;
    }
    work->swizzle_ = {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A};
    work->image_.dim_x = param.width_;
    work->image_.dim_y = param.height_;
    work->image_.dim_z = 1;
    work->image_.data_type = ASTCENC_TYPE_U8;
    work->image_.dim_stride = param.stride_;
    work->codec_context = nullptr;
    work->image_.data = nullptr;
    work->profile = ASTCENC_PRF_LDR_SRGB;
#if QUALITY_CONTROL
    work->mse[R_COM] = work->mse[G_COM] = work->mse[B_COM] = work->mse[RGBA_COM] = nullptr;
#endif
#if QUALITY_CONTROL
    work->calQualityEnable = enableQualityCheck;
    if (work->calQualityEnable) {
        for (int i = R_COM; i < RGBA_COM; i++) {
            work->mse[i] = (int32_t *)calloc(blockNum, sizeof(int32_t));
            if (!work->mse[i]) {
                HiLog::Error(LABEL, "quality control calloc failed");
                return false;
            }
        }
    }
#endif
    work->image_.data = (void **)malloc(sizeof(void*) * work->image_.dim_z);
    if (!work->image_.data) {
        return false;
    }
    return true;
}

uint32_t AstcCodec::SetColorSpace(int32_t outSize)
{
    OHOS::ColorManager::ColorSpace cs = astcPixelMap_->InnerGetGrColorSpace();
    ColorManager::ColorSpaceName colorSpaceName = cs.GetColorSpaceName();
    ColorManager::Matrix3x3 toXYZ = cs.GetRGBToXYZ();
    std::array<float, DIMES_2> whitePoint = cs.GetWhitePoint();
    float transferFuncg = cs.GetGamma();

    uint8_t* outColorSpaceAddr = astcOutput_->GetAddr() + outSize;
    if (outColorSpaceAddr == nullptr) {
        HiLog::Error(LABEL, "ERROR: get outputStream failed");
        return ERROR;
    }
    Uint32ToUint8 csNameData;
    csNameData.csName = colorSpaceName;
    for (int i = 0; i < ASTC_NUM_4; i++) {
        *(outColorSpaceAddr + i) = *(csNameData.u + i);
    }
    FloatToUint8_t FData;
    int32_t num = 0;
    for (int32_t i = 0; i < ASTC_NUM_3; ++i) {
        for (int32_t j = 0; j < ASTC_NUM_3; ++j) {
            FData.f[num++] = toXYZ[i][j];
        }
    }
    FData.f[num++] = whitePoint[0];
    FData.f[num++] = whitePoint[1];
    FData.f[num] = transferFuncg;

    for (int i = 0; i < ASTC_NUM_48; i++) {
        *(outColorSpaceAddr + i + ASTC_NUM_4) = *(FData.u + i);
    }
    return SUCCESS;
}

constexpr uint8_t RGBA_BYTES_PIXEL_LOG2 = 2;

uint32_t AstcCodec::AstcSoftwareEncode(TextureEncodeOptions &param, bool enableQualityCheck,
                                       int32_t blocksNum, int32_t outSize)
{
    AstcEncoder work;
    if (!InitMem(&work, param, enableQualityCheck, blocksNum)) {
        FreeMem(&work);
        return ERROR;
    }
    if (InitAstcencConfig(&work, &param) != SUCCESS) {
        HiLog::Error(LABEL, "astc InitAstcencConfig failed");
        FreeMem(&work);
        return ERROR;
    }
    work.image_.data[0] = static_cast<uint8_t *>(astcPixelMap_->GetWritablePixels());
    work.data_out_ = astcOutput_->GetAddr();
    size_t size;
    astcOutput_->GetCapicity(size);
    if (GenAstcHeader(work.data_out_, work.image_, &param, size) != SUCCESS) {
        HiLog::Error(LABEL, "astc GenAstcHeader failed");
        FreeMem(&work);
        return ERROR;
    }
    work.error_ = astcenc_compress_image(work.codec_context, &work.image_, &work.swizzle_,
        work.data_out_ + TEXTURE_HEAD_BYTES, outSize - TEXTURE_HEAD_BYTES,
#if QUALITY_CONTROL
        work.calQualityEnable, work.mse,
#endif
        0);
#if QUALITY_CONTROL
    if ((ASTCENC_SUCCESS != work.error_) ||
        (work.calQualityEnable && !CheckQuality(work.mse, blocksNum, param.blockX_ * param.blockY_))) {
#else
    if (ASTCENC_SUCCESS != work.error_) {
#endif
        HiLog::Error(LABEL, "astc compress failed");
        FreeMem(&work);
        return ERROR;
    }
    FreeMem(&work);
    return SUCCESS;
}

uint32_t AstcCodec::ASTCEncode()
{
    ImageInfo imageInfo;
    astcPixelMap_->GetImageInfo(imageInfo);
    TextureEncodeOptions param;
    param.width_ = imageInfo.size.width;
    param.height_ = imageInfo.size.height;
    param.stride_ = astcPixelMap_->GetRowStride() >> RGBA_BYTES_PIXEL_LOG2;
    bool enableQualityCheck = false; // astcOpts_.enableQualityCheck
    bool hardwareFlag = false;
    extractDimensions(astcOpts_.format, param);
    int32_t blocksNum = ((param.width_ + param.blockX_ - 1) / param.blockX_) *
        ((param.height_ + param.blockY_ - 1) / param.blockY_);
    int32_t outSize = blocksNum * TEXTURE_HEAD_BYTES + TEXTURE_HEAD_BYTES;

    if (ImageSystemProperties::GetAstcHardWareEncodeEnabled()) {
        HiLog::Info(LABEL, "astc hardware encode begin");
        std::shared_ptr<ImageCompressor> instance = ImageCompressor::GetInstance();
        if (!(instance -> CreateKernel())) {
            HiLog::Error(LABEL, "Create kernel error !");
        } else {
            if (instance->TextureEncodeCL(static_cast<uint8_t *>(astcPixelMap_->GetWritablePixels()),
                astcPixelMap_->GetRowStride(), param.width_, param.height_, astcOutput_->GetAddr())) {
                hardwareFlag = true;
            }
        }
        instance->ReleaseResource();
    }
    if (!hardwareFlag) {
        uint32_t res = AstcSoftwareEncode(param, enableQualityCheck, blocksNum, outSize);
        if (res != SUCCESS) {
            HiLog::Error(LABEL, "AstcSoftwareEncode failed");
            return ERROR;
        }
    }

    if (SetColorSpace(outSize) != SUCCESS) {
        HiLog::Error(LABEL, "astc set colorSpace failed");
        return ERROR;
    }
    
    astcOutput_->SetOffset(outSize + ASTC_NUM_52);
    return SUCCESS;
}
} // namespace ImagePlugin
} // namespace OHOS