/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "image_packer.h"

#include "buffer_packer_stream.h"
#include "file_packer_stream.h"
#include "hilog/log.h"
#include "image/abs_image_encoder.h"
#include "image_trace.h"
#include "image_utils.h"
#include "log_tags.h"
#include "media_errors.h"
#include "ostream_packer_stream.h"
#include "plugin_server.h"
#if defined(A_PLATFORM) || defined(IOS_PLATFORM)
#include "include/jpeg_encoder.h"
#endif

namespace OHOS {
namespace Media {
using namespace OHOS::HiviewDFX;
using namespace ImagePlugin;
using namespace MultimediaPlugin;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "ImagePacker" };
static constexpr uint8_t QUALITY_MAX = 100;
const static std::string EXTENDED_ENCODER = "image/extended";
static constexpr size_t SIZE_ZERO = 0;

PluginServer &ImagePacker::pluginServer_ = ImageUtils::GetPluginServer();

uint32_t ImagePacker::GetSupportedFormats(std::set<std::string> &formats)
{
    formats.clear();
    std::vector<ClassInfo> classInfos;
    uint32_t ret =
        pluginServer_.PluginServerGetClassInfo<AbsImageEncoder>(AbsImageEncoder::SERVICE_DEFAULT, classInfos);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "get class info from plugin server failed, ret:%{public}u.", ret);
        return ret;
    }
    for (auto &info : classInfos) {
        std::map<std::string, AttrData> &capbility = info.capabilities;
        auto iter = capbility.find(IMAGE_ENCODE_FORMAT);
        if (iter == capbility.end()) {
            continue;
        }
        AttrData &attr = iter->second;
        std::string format;
        if (attr.GetValue(format) != SUCCESS) {
            HiLog::Error(LABEL, "attr data get format failed.");
            continue;
        }
        formats.insert(format);
    }
    return SUCCESS;
}

uint32_t ImagePacker::StartPackingImpl(const PackOption &option)
{
    if (packerStream_ == nullptr || packerStream_.get() == nullptr) {
        HiLog::Error(LABEL, "make buffer packer stream failed.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    if (!GetEncoderPlugin(option)) {
        HiLog::Error(LABEL, "StartPackingImpl get encoder plugin failed.");
        return ERR_IMAGE_MISMATCHED_FORMAT;
    }
    PlEncodeOptions plOpts;
    CopyOptionsToPlugin(option, plOpts);
    return DoEncodingFunc([this, &plOpts](ImagePlugin::AbsImageEncoder* encoder) {
        return encoder->StartEncode(*packerStream_.get(), plOpts);
    });
}

uint32_t ImagePacker::StartPacking(uint8_t *outputData, uint32_t maxSize, const PackOption &option)
{
    ImageTrace imageTrace("ImagePacker::StartPacking by outputData");
    if (!IsPackOptionValid(option)) {
        HiLog::Error(LABEL, "array startPacking option invalid %{public}s, %{public}u.", option.format.c_str(),
                     option.quality);
        return ERR_IMAGE_INVALID_PARAMETER;
    }

    if (outputData == nullptr) {
        HiLog::Error(LABEL, "output buffer is null.");
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    BufferPackerStream *stream = new (std::nothrow) BufferPackerStream(outputData, maxSize);
    if (stream == nullptr) {
        HiLog::Error(LABEL, "make buffer packer stream failed.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    FreeOldPackerStream();
    packerStream_ = std::unique_ptr<BufferPackerStream>(stream);
    return StartPackingImpl(option);
}

uint32_t ImagePacker::StartPacking(const std::string &filePath, const PackOption &option)
{
    ImageTrace imageTrace("ImagePacker::StartPacking by filePath");
    if (!IsPackOptionValid(option)) {
        HiLog::Error(LABEL, "filepath startPacking option invalid %{public}s, %{public}u.", option.format.c_str(),
                     option.quality);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    FilePackerStream *stream = new (std::nothrow) FilePackerStream(filePath);
    if (stream == nullptr) {
        HiLog::Error(LABEL, "make file packer stream failed.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    FreeOldPackerStream();
    packerStream_ = std::unique_ptr<FilePackerStream>(stream);
    return StartPackingImpl(option);
}

uint32_t ImagePacker::StartPacking(const int &fd, const PackOption &option)
{
    ImageTrace imageTrace("ImagePacker::StartPacking by fd");
    if (!IsPackOptionValid(option)) {
        HiLog::Error(LABEL, "fd startPacking option invalid %{public}s, %{public}u.", option.format.c_str(),
                     option.quality);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    FilePackerStream *stream = new (std::nothrow) FilePackerStream(fd);
    if (stream == nullptr) {
        HiLog::Error(LABEL, "make file packer stream failed.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    FreeOldPackerStream();
    packerStream_ = std::unique_ptr<FilePackerStream>(stream);
    return StartPackingImpl(option);
}

uint32_t ImagePacker::StartPacking(std::ostream &outputStream, const PackOption &option)
{
    ImageTrace imageTrace("ImagePacker::StartPacking by outputStream");
    if (!IsPackOptionValid(option)) {
        HiLog::Error(LABEL, "outputStream startPacking option invalid %{public}s, %{public}u.", option.format.c_str(),
                     option.quality);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    OstreamPackerStream *stream = new (std::nothrow) OstreamPackerStream(outputStream);
    if (stream == nullptr) {
        HiLog::Error(LABEL, "make ostream packer stream failed.");
        return ERR_IMAGE_DATA_ABNORMAL;
    }
    FreeOldPackerStream();
    packerStream_ = std::unique_ptr<OstreamPackerStream>(stream);
    return StartPackingImpl(option);
}

// JNI adapter method, this method be called by jni and the outputStream be created by jni, here we manage the lifecycle
// of the outputStream
uint32_t ImagePacker::StartPackingAdapter(PackerStream &outputStream, const PackOption &option)
{
    FreeOldPackerStream();
    packerStream_ = std::unique_ptr<PackerStream>(&outputStream);

    if (!IsPackOptionValid(option)) {
        HiLog::Error(LABEL, "packer stream option invalid %{public}s, %{public}u.", option.format.c_str(),
                     option.quality);
        return ERR_IMAGE_INVALID_PARAMETER;
    }
    return StartPackingImpl(option);
}

uint32_t ImagePacker::AddImage(PixelMap &pixelMap)
{
    ImageTrace imageTrace("ImagePacker::AddImage by pixelMap");
    return DoEncodingFunc([this, &pixelMap](ImagePlugin::AbsImageEncoder* encoder) {
        return encoder->AddImage(pixelMap);
    });
}

uint32_t ImagePacker::AddImage(ImageSource &source)
{
    ImageTrace imageTrace("ImagePacker::AddImage by imageSource");
    DecodeOptions opts;
    uint32_t ret = SUCCESS;
    if (pixelMap_ != nullptr) {
        pixelMap_.reset();  // release old inner pixelmap
    }
    pixelMap_ = source.CreatePixelMap(opts, ret);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "image source create pixel map failed.");
        return ret;
    }
    if (pixelMap_ == nullptr || pixelMap_.get() == nullptr) {
        HiLog::Error(LABEL, "create the pixel map unique_ptr fail.");
        return ERR_IMAGE_MALLOC_ABNORMAL;
    }
    return AddImage(*pixelMap_.get());
}

uint32_t ImagePacker::AddImage(ImageSource &source, uint32_t index)
{
    ImageTrace imageTrace("ImagePacker::AddImage by imageSource and index:%{public}u", index);
    DecodeOptions opts;
    uint32_t ret = SUCCESS;
    if (pixelMap_ != nullptr) {
        pixelMap_.reset();  // release old inner pixelmap
    }
    pixelMap_ = source.CreatePixelMap(index, opts, ret);
    if (ret != SUCCESS) {
        HiLog::Error(LABEL, "image source create pixel map failed.");
        return ret;
    }
    if (pixelMap_ == nullptr || pixelMap_.get() == nullptr) {
        HiLog::Error(LABEL, "create the pixel map unique_ptr fail.");
        return ERR_IMAGE_MALLOC_ABNORMAL;
    }
    return AddImage(*pixelMap_.get());
}

uint32_t ImagePacker::FinalizePacking()
{
    ImageTrace imageTrace("ImagePacker::FinalizePacking");
    return DoEncodingFunc([](ImagePlugin::AbsImageEncoder* encoder) {
        auto res = encoder->FinalizeEncode();
        if (res != SUCCESS) {
            HiLog::Error(LABEL, "FinalizePacking failed %{public}d.", res);
        }
        return res;
        }, false);
}

uint32_t ImagePacker::FinalizePacking(int64_t &packedSize)
{
    uint32_t ret = FinalizePacking();
    if (packerStream_ != nullptr) {
        packerStream_ ->Flush();
    }
    packedSize = (packerStream_ != nullptr) ? packerStream_->BytesWritten() : 0;
    return ret;
}

static ImagePlugin::AbsImageEncoder* GetEncoder(PluginServer &pluginServer, std::string format)
{
    std::map<std::string, AttrData> capabilities;
    capabilities.insert(std::map<std::string, AttrData>::value_type(IMAGE_ENCODE_FORMAT, AttrData(format)));
    return pluginServer.CreateObject<AbsImageEncoder>(AbsImageEncoder::SERVICE_DEFAULT, capabilities);
}

bool ImagePacker::GetEncoderPlugin(const PackOption &option)
{
    encoders_.clear();
    HiLog::Debug(LABEL, "GetEncoderPlugin current encoder plugin size %{public}zu.", encoders_.size());
    auto encoder = GetEncoder(pluginServer_, EXTENDED_ENCODER);
    if (encoder != nullptr) {
        encoders_.emplace_back(std::unique_ptr<ImagePlugin::AbsImageEncoder>(encoder));
    } else {
        HiLog::Error(LABEL, "GetEncoderPlugin get extencoder plugin failed.");
    }
    encoder = GetEncoder(pluginServer_, option.format);
    if (encoder != nullptr) {
        encoders_.emplace_back(std::unique_ptr<ImagePlugin::AbsImageEncoder>(encoder));
    } else {
        HiLog::Error(LABEL, "GetEncoderPlugin get %{public}s plugin failed.", option.format.c_str());
    }
    return encoders_.size() != SIZE_ZERO;
}

void ImagePacker::CopyOptionsToPlugin(const PackOption &opts, PlEncodeOptions &plOpts)
{
    plOpts.numberHint = opts.numberHint;
    plOpts.quality = opts.quality;
    plOpts.format = opts.format;
}

void ImagePacker::FreeOldPackerStream()
{
    if (packerStream_ != nullptr) {
        packerStream_.reset();
    }
}

bool ImagePacker::IsPackOptionValid(const PackOption &option)
{
    return !(option.quality > QUALITY_MAX || option.format.empty());
}

uint32_t ImagePacker::DoEncodingFunc(std::function<uint32_t(ImagePlugin::AbsImageEncoder*)> func, bool forAll)
{
    if (encoders_.size() == SIZE_ZERO) {
        HiLog::Error(LABEL, "DoEncodingFunc encoders is empty.");
        return ERR_IMAGE_DECODE_ABNORMAL;
    }
    std::vector<uint32_t> rets;
    rets.resize(SIZE_ZERO);
    bool isSuccessOnce = false;
    for (size_t i = SIZE_ZERO; i < encoders_.size(); i++) {
        if (!forAll && isSuccessOnce) {
            HiLog::Debug(LABEL, "DoEncodingFunc encoding successed, reset other encoder.");
            encoders_.at(i).reset();
            continue;
        }
        auto iterRes = func(encoders_.at(i).get());
        rets.emplace_back(iterRes);
        if (iterRes == SUCCESS) {
            isSuccessOnce = true;
        }
        if (!forAll && !isSuccessOnce) {
            HiLog::Debug(LABEL, "DoEncodingFunc failed.");
        }
    }
    if (isSuccessOnce) {
        return SUCCESS;
    }
    return (rets.size() == SIZE_ZERO)?ERR_IMAGE_DECODE_ABNORMAL:rets.front();
}

// class reference need explicit constructor and destructor, otherwise unique_ptr<T> use unnormal
ImagePacker::ImagePacker()
{}

ImagePacker::~ImagePacker()
{}
} // namespace Media
} // namespace OHOS
