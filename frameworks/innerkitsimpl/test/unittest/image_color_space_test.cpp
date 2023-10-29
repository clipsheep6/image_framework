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

#include <gtest/gtest.h>
#include <fstream>
#include <fcntl.h>
#ifdef IMAGE_COLORSPACE_FLAG
#include "color_space.h"
#endif
#include "directory_ex.h"
#include "hilog/log.h"
#include "image_packer.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "pixel_map.h"
#include "image_source_util.h"
#include "astcenc.h"

using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::HiviewDFX;
using namespace OHOS::ImageSourceUtil;

namespace OHOS {
namespace Multimedia {
static const std::string IMAGE_INPUT_JPEG_INCLUDE_ICC_PATH = "/data/local/tmp/image/jpeg_include_icc_profile.jpg";
static const std::string IMAGE_OUTPUT_JPEG_INCLUDE_ICC_PATH = "/data/test/test_jpeg_include_icc_profile.jpg";
static const std::string IMAGE_INPUT_JPEG_NOT_INCLUDE_ICC_PATH = "/data/local/tmp/image/test.jpg";
static const std::string IMAGE_OUTPUT_JPEG_NOT_INCLUDE_ICC_PATH = "/data/test/test_jpeg_no_include_icc_profile.jpg";

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL_TEST = {
    LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImageColorSpaceTest"
};

class ImageColorSpaceTest : public testing::Test {
public:
    ImageColorSpaceTest() {};
    ~ImageColorSpaceTest() {};
};

/**
 * @tc.name: JpegColorSpaceDecode001
 * @tc.desc: Decode jpeg icc image from file source stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageColorSpaceTest, JpegColorSpaceDecode001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_INCLUDE_ICC_PATH, opts, errorCode);
    HiLog::Debug(LABEL_TEST, "create image source error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options.
     * @tc.expected: step2. decode image source to pixel map success.
     * @tc.expected: step2. parsing colorspace success.
     */
    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
#ifdef IMAGE_COLORSPACE_FLAG
    OHOS::ColorManager::ColorSpace grColorSpace = pixelMap->InnerGetGrColorSpace();
    EXPECT_NE(grColorSpace.ToSkColorSpace(), nullptr);
    EXPECT_NE(grColorSpace.GetWhitePoint().size(), 0UL);
    EXPECT_NE(grColorSpace.GetXYZToRGB().size(), 0UL);
    EXPECT_NE(grColorSpace.GetRGBToXYZ().size(), 0UL);
#endif
}

/**
 * @tc.name: JpegColorSpaceEncode001
 * @tc.desc: Encode jpeg icc image to file source stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageColorSpaceTest, JpegColorSpaceEncode001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_INCLUDE_ICC_PATH, opts, errorCode);
    HiLog::Debug(LABEL_TEST, "create image source error code==%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options.
     * @tc.expected: step2. decode image source to first pixel map success.
     * @tc.expected: step2. parsing colorspace success.
     */
    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixelMapOne = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMapOne.get(), nullptr);
#ifdef IMAGE_COLORSPACE_FLAG
    OHOS::ColorManager::ColorSpace grColorSpaceOne = pixelMapOne->InnerGetGrColorSpace();
    EXPECT_NE(grColorSpaceOne.ToSkColorSpace(), nullptr);
#endif

    /**
     * @tc.steps: step3. encode image source from pixel map.
     * @tc.expected: step3. encode image source from first pixel map success.
     * @tc.expected: step3. packing colorspace success.
     */
    ImagePacker imagePacker;
    int64_t packSize = OHOS::ImageSourceUtil::PackImage(IMAGE_OUTPUT_JPEG_INCLUDE_ICC_PATH, std::move(pixelMapOne));
    ASSERT_NE(packSize, 0);

    /**
     * @tc.steps: step4. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step4. create second image source success.
     */
    imageSource = ImageSource::CreateImageSource(IMAGE_OUTPUT_JPEG_INCLUDE_ICC_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step5. decode image source to second pixel map by default decode options from packaged image.
     * @tc.expected: step5. decode image source to second pixel map success.
     * @tc.expected: step5. acquire second colorspace success.
     */
    std::unique_ptr<PixelMap> pixelMapTwo = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMapTwo.get(), nullptr);
#ifdef IMAGE_COLORSPACE_FLAG
    OHOS::ColorManager::ColorSpace grColorSpaceTwo = pixelMapTwo->InnerGetGrColorSpace();
    EXPECT_NE(grColorSpaceTwo.ToSkColorSpace(), nullptr);
    EXPECT_EQ(grColorSpaceOne.GetWhitePoint(), grColorSpaceTwo.GetWhitePoint());
    EXPECT_EQ(grColorSpaceOne.GetXYZToRGB(), grColorSpaceTwo.GetXYZToRGB());
    EXPECT_EQ(grColorSpaceOne.GetRGBToXYZ(), grColorSpaceTwo.GetRGBToXYZ());
#endif
}

/**
 * @tc.name: JpegColorSpaceDecode002
 * @tc.desc: Decode jpeg image, which don't contain ICC from file source stream
 * @tc.type: FUNC
 */
HWTEST_F(ImageColorSpaceTest, JpegColorSpaceDecode002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_NOT_INCLUDE_ICC_PATH, opts, errorCode);
    HiLog::Debug(LABEL_TEST, "create image source error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options.
     * @tc.expected: step2. decode image source to pixel map success.
     * @tc.expected: step2. parsing colorspace failed.
     */
    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMap.get(), nullptr);
#ifdef IMAGE_COLORSPACE_FLAG
    OHOS::ColorManager::ColorSpace grColorSpace = pixelMap->InnerGetGrColorSpace();
    EXPECT_EQ(grColorSpace.ToSkColorSpace(), nullptr);
#endif
}

/**
 * @tc.name: JpegColorSpaceEncode002
 * @tc.desc: Encode jpeg image, which don't contain ICC to file source stream.
 * @tc.type: FUNC
 */
HWTEST_F(ImageColorSpaceTest, JpegColorSpaceEncode002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_NOT_INCLUDE_ICC_PATH, opts, errorCode);
    HiLog::Debug(LABEL_TEST, "create image source error code==%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options.
     * @tc.expected: step2. decode image source to first pixel map success.
     * @tc.expected: step2. parsing colorspace failed.
     */
    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixelMapOne = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMapOne.get(), nullptr);
#ifdef IMAGE_COLORSPACE_FLAG
    OHOS::ColorManager::ColorSpace grColorSpaceOne = pixelMapOne->InnerGetGrColorSpace();
    EXPECT_EQ(grColorSpaceOne.ToSkColorSpace(), nullptr);
#endif

    /**
     * @tc.steps: step3. encode image source from pixel map.
     * @tc.expected: step3. encode image source from first pixel map success.
     * @tc.expected: step3. packing colorspace success.
     */
    ImagePacker imagePacker;
    int64_t packSize = OHOS::ImageSourceUtil::PackImage(IMAGE_OUTPUT_JPEG_NOT_INCLUDE_ICC_PATH, std::move(pixelMapOne));
    ASSERT_NE(packSize, 0);

    /**
     * @tc.steps: step4. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step4. create second image source success.
     */
    imageSource = ImageSource::CreateImageSource(IMAGE_OUTPUT_JPEG_NOT_INCLUDE_ICC_PATH, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step5. decode image source to second pixel map by default decode options from packaged image.
     * @tc.expected: step5. decode image source to second pixel map success.
     * @tc.expected: step5. parsing second colorspace failed.
     */
    std::unique_ptr<PixelMap> pixelMapTwo = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixelMapTwo.get(), nullptr);
#ifdef IMAGE_COLORSPACE_FLAG
    OHOS::ColorManager::ColorSpace grColorSpaceTwo = pixelMapTwo->InnerGetGrColorSpace();
    EXPECT_EQ(grColorSpaceTwo.ToSkColorSpace(), nullptr);
#endif
}

/**
 * @tc.name: astc_test_001
 * @tc.desc: Encode jpeg image, which don't contain ICC to file source stream.
 * @tc.type: FUNC
 */
#define TEXTURE_HEAD_BYTES (16)
static const uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;

typedef struct TextureEncodeOptionsType {
    int32_t width_;
    int32_t height_;
    uint8_t blockX_;
    uint8_t blockY_;
} TextureEncodeOptions;

struct astc_header
{
    uint8_t magic[4];
    uint8_t block_x;
    uint8_t block_y;
    uint8_t block_z;
    uint8_t dim_x[3];
    uint8_t dim_y[3];
    uint8_t dim_z[3];
};

typedef struct AstcEncoderInfo {
    astc_header head;
    astcenc_config config;
    astcenc_profile profile;
    astcenc_context* codec_context;
    astcenc_image image_;
    astcenc_swizzle swizzle_;
    uint8_t* data_out_;
    size_t data_len_;
    astcenc_error error_;
} AstcEncoder;

void GenAstcHeader(astc_header &hdr, astcenc_image img, TextureEncodeOptions *encodeParams)
{
    hdr.magic[0] = ASTC_MAGIC_ID & 0xFF;
    hdr.magic[1] = (ASTC_MAGIC_ID >> 8) & 0xFF;
    hdr.magic[2] = (ASTC_MAGIC_ID >> 16) & 0xFF;
    hdr.magic[3] = (ASTC_MAGIC_ID >> 24) & 0xFF;

    hdr.block_x = static_cast<uint8_t>(encodeParams->blockX_);
    hdr.block_y = static_cast<uint8_t>(encodeParams->blockY_);
    hdr.block_z = 1;

    hdr.dim_x[0] = img.dim_x & 0xFF;
    hdr.dim_x[1] = (img.dim_x >> 8) & 0xFF;
    hdr.dim_x[2] = (img.dim_x >> 16) & 0xFF;

    hdr.dim_y[0] = img.dim_x & 0xFF;
    hdr.dim_y[1] = (img.dim_x >> 8) & 0xFF;
    hdr.dim_y[2] = (img.dim_x >> 16) & 0xFF;

    hdr.dim_z[0] = img.dim_x & 0xFF;
    hdr.dim_z[1] = (img.dim_x >> 8) & 0xFF;
    hdr.dim_z[2] = (img.dim_x >> 16) & 0xFF;
}

int InitAstcencConfig(astcenc_profile profile, TextureEncodeOptions *option, astcenc_config& config)
{
    unsigned int block_x = option->blockX_;
    unsigned int block_y = option->blockY_;
    unsigned int block_z = 1;

    float quality = ASTCENC_PRE_FAST;
    unsigned int flags = 0;
    astcenc_error status = astcenc_config_init(profile, block_x, block_y,
        block_z, quality, flags, &config);
    if (status == ASTCENC_ERR_BAD_BLOCK_SIZE)
    {
        HiLog::Debug(LABEL_TEST, "ERROR: block size is invalid");
        return 1;
    }
    else if (status == ASTCENC_ERR_BAD_CPU_FLOAT)
    {
        HiLog::Debug(LABEL_TEST, "ERROR: astcenc must not be compiled with fast-math");
        return 1;
    }
    else if (status != ASTCENC_SUCCESS)
    {
        HiLog::Debug(LABEL_TEST, "ERROR: config failed");
        return 1;
    }
    return 0;
}

HWTEST_F(ImageColorSpaceTest, astc_test_001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step1. create image source success.
     */
    static char rgbaFile[512] = "/data/local/256x256.rgba";
    static char astcFile[512] = "/data/local/256x256.astc";
    int w = 256;
    int h = 256;
    int fileSize = w * h * 4;
    uint8_t *inRGBA = (uint8_t*)malloc(fileSize);
    FILE *inFp = fopen(rgbaFile, "rb");
    if (!inFp) {
        HiLog::Error(LABEL_TEST, "astc uint test error");
    }
    fread(inRGBA, 1, fileSize, inFp);
    fclose(inFp);
    AstcEncoder work;
    TextureEncodeOptions encodeParams;
    encodeParams.width_ = w;
    encodeParams.height_ = h;
    encodeParams.blockX_ = 4;
    encodeParams.blockY_ = 4;
    InitAstcencConfig(work.profile, &encodeParams, work.config);
    astcenc_context_alloc(&work.config, 1, &work.codec_context);
    work.swizzle_ = {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A};
    work.image_.dim_x = encodeParams.width_;
    work.image_.dim_y = encodeParams.height_;
    work.image_.dim_z = 1;
    work.image_.data_type = ASTCENC_TYPE_U8;
    work.image_.data = (void **)malloc(sizeof(void*) * work.image_.dim_z);
    GenAstcHeader(work.head, work.image_, &encodeParams);

    work.image_.data[0] = inRGBA;
    int outSize = ((encodeParams.width_ + encodeParams.blockX_ - 1) / encodeParams.blockX_) *
        ((encodeParams.height_ + encodeParams.blockY_ -1) / encodeParams.blockY_) * TEXTURE_HEAD_BYTES + TEXTURE_HEAD_BYTES;
    uint8_t *buffer = (uint8_t*)malloc(outSize);
    memcpy(buffer, &work.head, sizeof(astc_header));
    work.data_out_ = buffer + TEXTURE_HEAD_BYTES;
    work.data_len_ = outSize - TEXTURE_HEAD_BYTES;
    work.error_ = astcenc_compress_image(work.codec_context, &work.image_, &work.swizzle_, work.data_out_, work.data_len_, 0);
    if (ASTCENC_SUCCESS != work.error_) {
        HiLog::Error(LABEL_TEST, "compress failed");
    }
    int encBytes = work.data_len_ + TEXTURE_HEAD_BYTES;

    EXPECT_EQ(ASTCENC_SUCCESS, work.error_);

    FILE *outFp = fopen(astcFile, "wb");
    if (!inFp) {
        HiLog::Error(LABEL_TEST, "fopen failed");
    }
    fwrite(buffer, 1, encBytes, outFp);
    fclose(outFp);
    if (inRGBA) free(inRGBA);
    if (buffer) free(buffer);
    if (work.codec_context != nullptr) {
        astcenc_context_free(work.codec_context);
        work.codec_context = nullptr;
    }
    free(work.image_.data);
}
} // namespace Multimedia
} // namespace OHOS