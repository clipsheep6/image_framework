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
 
#include "hardware/jpeg_hw_encoder.h"
 
#include <cstdlib>
#include <memory>
#include <fstream>
#include <securec.h>
 
#include "SkStream.h"
#include "surface_buffer.h"
#include "image/abs_image_encoder.h"
#include "image_func_timer.h"
#include "image_log.h"
#include "image_system_properties.h"
#include "image_trace.h"
#include "image_type_converter.h"
#include "image_utils.h"
#include "media_errors.h"
 
#include "src/images/SkImageEncoderFns.h"
 
#include "drivers/interface/camera/metadata/include/metadata_utils.h"
 
#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN
 
#undef LOG_TAG
#define LOG_TAG "JpegHardwareEncode"
 
using namespace OHOS::HDI::Camera;
using namespace OHOS::HDI::Base;
using namespace OHOS::Media;
using namespace OHOS::Camera;
 
namespace OHOS::ImagePlugin {
struct OfflineRequestSetting {
    int32_t orientation = OHOS_CAMERA_JPEG_ROTATION_0;
    uint8_t quality = OHOS_CAMERA_JPEG_LEVEL_HIGH;
    uint32_t outJpegSize = 0;
    std::vector<uint32_t> bufferTypes;
};

static constexpr int32_t ITEM_CAPACITY = 10;
static constexpr int32_t DATA_CAPACITY = 50;
static constexpr int32_t MAX_OUT_BUFFER_SIZE = 64 * 1024 * 1024;
constexpr int32_t INVALID_ICC_DATA = 1;
constexpr int32_t INVALID_MEDIA_OFFLINE_SERVER = 2;
constexpr int32_t INPUT_BUFFER_NULL = 3;
constexpr int32_t INVALID_OUTPUT_JPEG_SIZE = 4;
constexpr int32_t INVALID_OUTPUT_BUFFER_DATA = 5;
constexpr int32_t PIXEL_FORMAT_NOT_SUPPORTED = 6;
constexpr int32_t INVALID_INPUT = 7;
constexpr int32_t WRITE_FILE_ERROR = 8;
constexpr int32_t DUMP_DISABLED = 9;
static constexpr uint8_t ICC_MARKER[] = {
    0xff, 0xe2,
};
constexpr size_t MARKER_LENGTH = 2;
static constexpr uint8_t ICC_SIGNATURE[] = {
    'I', 'C', 'C', '_', 'P', 'R', 'O', 'F', 'I', 'L', 'E', '\0', 1, 1,
}; // Corresponding hexadecimal: { 0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x00 }
 
std::vector<uint8_t> WriteSettings(const OfflineRequestSetting& inputSeetings, unsigned int tagId)
{
    const static auto OFFLINE_MODE = 1;
    std::shared_ptr<CameraMetadata> metaData = std::make_shared<CameraMetadata>(ITEM_CAPACITY, DATA_CAPACITY);
    metaData->addEntry(OHOS_JPEG_ORIENTATION, &inputSeetings.orientation, 1);
    metaData->addEntry(OHOS_JPEG_QUALITY, &inputSeetings.quality, 1);
    metaData->addEntry(OHOS_JPEG_SIZE, &inputSeetings.outJpegSize, 1);
    metaData->addEntry(tagId, &OFFLINE_MODE, 1);
    metaData->addEntry("tag_bufferTypeArr", inputSeetings.bufferTypes.data(), inputSeetings.bufferTypes.size());
    std::vector<uint8_t> metaUp;
    MetadataUtils::ConvertMetadataToVec(metaData, metaUp);
 
    return metaUp;
}
 
void ReadSettings(const std::vector<uint8_t>& settings, OfflineRequestSetting& outputSettings)
{
    if (settings.empty()) {
        IMAGE_LOGE("settings is empty");
        return ;
    }
    std::shared_ptr<CameraMetadata> resultMeta = std::make_shared<CameraMetadata>(ITEM_CAPACITY, DATA_CAPACITY);
    MetadataUtils::ConvertVecToMetadata(settings, resultMeta);
    common_metadata_header_t *head = resultMeta->get();
    camera_metadata_item_t item;
    int ret = CameraMetadata::FindCameraMetadataItem(head, OHOS_JPEG_SIZE, &item);
    if (ret) {
        IMAGE_LOGE("something error ret is %{public}d", ret);
        return ;
    }
    outputSettings.outJpegSize = *item.data.ui32;
}

int32_t OHMediaOfflineCallback::OnProcessDone(const MediaOffline::V1_0::OfflineResult& result)
{
    IMAGE_LOGI("OHMediaOfflineCallback::OnProcessDone IN");
    return SUCCESS;
}
 
int32_t OHMediaOfflineCallback::OnError(int32_t requestIndex, int32_t errorCode)
{
    IMAGE_LOGE("OHMediaOfflineCallback::OnError errCode is %{public}d", errorCode);
    return SUCCESS;
}
 
JpegHardwareEncoder::JpegHardwareEncoder()
{
    mediaOfflineService_ = MediaOffline::V1_0::IMediaOfflineService::Get("camera_offline_algo_service", false);
    sessionId_ = 0;
    if (mediaOfflineService_ == nullptr) {
        IMAGE_LOGE("JpegHardwareEncoder::JpegHardwareEncoder() failed, mediaOfflineService is nullptr");
    } else {
        mediaOfflineCallback_ = new OHMediaOfflineCallback();
        mediaOfflineService_->CreateOfflineSession(mediaOfflineCallback_, sessionId_, mediaOfflineSession_);
    }
}
 
JpegHardwareEncoder::~JpegHardwareEncoder()
{
}
 
int32_t JpegHardwareEncoder::AllocDmaBuffer(PlImageBuffer& dmaBuffer, int32_t width, int32_t height,
    const GraphicPixelFormat& pixelFormat)
{
    ImageFuncTimer imageFuncTimer("%s width*height:(%d, %d)", __func__, width, height);
    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = width,
        .height = height,
        .format = pixelFormat,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_MMZ_CACHE,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = sb->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        IMAGE_LOGE("SurfaceBuffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return ERR_DMA_NOT_EXIST;
    }
    void* nativeBuffer = sb.GetRefPtr();
    int32_t err = ImageUtils::SurfaceBuffer_Reference(nativeBuffer);
    if (err != OHOS::GSERROR_OK) {
        IMAGE_LOGE("NativeBufferReference failed");
        return ERR_DMA_DATA_ABNORMAL;
    }

     dmaBuffer.buffer = sb->GetVirAddr();
    dmaBuffer.bufferSize = sb->GetSize();
    dmaBuffer.context = nativeBuffer;
    return SUCCESS;
}
 
int32_t JpegHardwareEncoder::DumpInputSurfaceBuffer(const PlImageBuffer& inputBuffer,
    const Media::YUVDataInfo& yuvDataInfo)
{
    if (!ImageSystemProperties::GetDumpImageEnabled()) {
        return DUMP_DISABLED;
    }
    SurfaceBuffer* buffer = static_cast<SurfaceBuffer*>(inputBuffer.context);
    std::string fileName = "/data/storage/el2/base/files/";
    fileName += "stride_" + std::to_string(buffer->GetStride()) +
                "_height_" + std::to_string(buffer->GetHeight()) +
                "_y_stride_" + std::to_string(yuvDataInfo.y_stride) +
                "_y_height_" + std::to_string(yuvDataInfo.y_height) +
                "_uv_stride_" + std::to_string(yuvDataInfo.uv_stride) +
                "_uv_height_" + std::to_string(yuvDataInfo.uv_height);
    std::ofstream outFile(fileName, std::ofstream::out);
    IMAGE_LOGI("DumpInputSurfaceBuffer start, path=%{public}s", fileName.c_str());
    if (!outFile.is_open()) {
        IMAGE_LOGE("DumpInputSurfaceBuffer failed because open file error, path=%{public}s", fileName.c_str());
        return WRITE_FILE_ERROR;
    }
    IMAGE_LOGI("DumpInputSurfaceBuffer success, path=%{public}s", fileName.c_str());
    outFile.write(static_cast<char *>(inputBuffer.buffer), inputBuffer.bufferSize);
    return SUCCESS;
}
 
int32_t JpegHardwareEncoder::CopyInputBufferFromPixelMap(PlImageBuffer& inputBuffer, Media::PixelMap* pixelMap)
{
    ImageFuncTimer imageFuncTimer("%s", __func__);
    if (pixelMap == nullptr) {
        IMAGE_LOGE("CopyInputBufferFromPixelMap pixel is nullptr");
        return PIXEL_FORMAT_NOT_SUPPORTED;
    }
    if (pixelMap->GetPixelFormat() != PixelFormat::NV21 && pixelMap->GetPixelFormat() != PixelFormat::NV12) {
        return INVALID_INPUT;
    }
    if (pixelMap->GetAllocatorType() == AllocatorType::DMA_ALLOC) {
        auto surfaceBuf = static_cast<SurfaceBuffer*>(pixelMap->GetFd());
        void* nativeBuffer = pixelMap->GetFd();
        int32_t err = ImageUtils::SurfaceBuffer_Reference(nativeBuffer);
        if (err != OHOS::GSERROR_OK) {
            IMAGE_LOGE("NativeBufferReference failed");
            return ERR_DMA_DATA_ABNORMAL;
        }
        inputBuffer.buffer = (void *)(pixelMap->GetPixels());
        inputBuffer.bufferSize = surfaceBuf->GetSize();
        inputBuffer.context = nativeBuffer;
        return SUCCESS;
    }
 
    int32_t retCode = AllocDmaBuffer(inputBuffer, pixelMap->GetWidth(), pixelMap->GetHeight(),
        pixelMap->GetPixelFormat() == PixelFormat::NV21 ? GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCRCB_420_SP :
        GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP);
    if (retCode != SUCCESS) {
        return retCode;
    }

     // copy yuv data from pixelMap to DmaBuffer
    YUVDataInfo yuvDataInfo{};
    pixelMap->GetImageYUVInfo(yuvDataInfo);
 
    uint8_t* src = const_cast<uint8_t *>(pixelMap->GetPixels());
    uint8_t* dst = static_cast<uint8_t*>(inputBuffer.buffer);
    size_t dstSize = inputBuffer.bufferSize;
    SurfaceBuffer* buffer = static_cast<SurfaceBuffer*>(inputBuffer.context);
    for (int32_t i = 0; i < pixelMap->GetHeight(); ++i) {
        memcpy_s(dst, dstSize, src, pixelMap->GetWidth());
        dst += buffer->GetStride();
        dstSize -= buffer->GetStride();
        src += pixelMap->GetWidth();
    }
    const int32_t NUM_2 = 2;
    for (int32_t i = 0; i < pixelMap->GetHeight() / NUM_2; ++i) {
        memcpy_s(dst, dstSize, src, pixelMap->GetWidth());
        dst += buffer->GetStride();
        dstSize -= buffer->GetStride();
        src += pixelMap->GetWidth();
    }
    DumpInputSurfaceBuffer(inputBuffer, yuvDataInfo);
    return SUCCESS;
}
 
static SkImageInfo ToSkInfo(Media::PixelMap *pixelMap)
{
    ImageInfo info;
    pixelMap->GetImageInfo(info);
    SkColorType colorType = ImageTypeConverter::ToSkColorType(info.pixelFormat);
    SkAlphaType alphaType = ImageTypeConverter::ToSkAlphaType(info.alphaType);
    sk_sp<SkColorSpace> colorSpace = nullptr;
#ifdef IMAGE_COLORSPACE_FLAG
    if (pixelMap->InnerGetGrColorSpacePtr() != nullptr) {
        colorSpace = pixelMap->InnerGetGrColorSpacePtr()->ToSkColorSpace();
    } else {
        IMAGE_LOGI("ToSkInfo colorSpace is nullptr");
    }
#endif
    return SkImageInfo::Make(info.size.width, info.size.height, colorType, alphaType, colorSpace);
}

int32_t JpegHardwareEncoder::GetIccData(Media::PixelMap* pixelMap, uint8_t*& outIcc, size_t& outIccLength)
{
    ImageFuncTimer imageFuncTimer("%s", __func__);
    // write colorspace to SKData
    sk_sp<SkData> icc = icc_from_color_space(ToSkInfo(pixelMap));
    if (icc) {
        if ((icc->size() > UINT16_MAX - sizeof(ICC_MARKER))) {
            IMAGE_LOGE("JpegEncodeImage::GetIccData failed, icc size is too large!");
            return INVALID_ICC_DATA;
        }
        outIccLength = sizeof(ICC_MARKER) + MARKER_LENGTH + sizeof(ICC_SIGNATURE) + icc->size();
        IMAGE_LOGD("JpegEncodeImage::GetIccData outIccLength is %{public}zu", outIccLength);
        outIcc = (uint8_t*)malloc(outIccLength);
 
        uint8_t* ptr = outIcc;
        memcpy_s(ptr, outIccLength, ICC_MARKER, sizeof(ICC_MARKER));
        ptr += sizeof(ICC_MARKER);
 
        uint16_t iccSize = outIccLength - sizeof(ICC_MARKER);
        uint8_t iccMarkerLength[MARKER_LENGTH];
        static constexpr int bitsPerByte = 8;
        iccMarkerLength[0] = (iccSize >> bitsPerByte) & 0xff;
        iccMarkerLength[1] = iccSize & 0xff;
        memcpy_s(ptr, outIccLength, iccMarkerLength, MARKER_LENGTH);
        ptr += MARKER_LENGTH;
 
        memcpy_s(ptr, outIccLength, ICC_SIGNATURE, sizeof(ICC_SIGNATURE));
        ptr += sizeof(ICC_SIGNATURE);
 
        memcpy_s(ptr, outIccLength, static_cast<const uint8_t *>(icc->data()), icc->size());
        ptr += icc->size();
    } else {
        IMAGE_LOGE("JpegEncodeImage::GetIccData failed, icc profile is null!");
        return INVALID_ICC_DATA;
    }
    return SUCCESS;
}

int32_t JpegHardwareEncoder::WriteJpegData(SkWStream* output, const PlImageBuffer& outputBuffer,
    Media::PixelMap* pixelMap, uint32_t compressedJpegSize)
{
    ImageFuncTimer imageFuncTimer("%s, compressedJpegSize:%d, bufferSize:%d", __func__, compressedJpegSize,
        outputBuffer.bufferSize);
    if (compressedJpegSize == 0 || compressedJpegSize > outputBuffer.bufferSize) {
        IMAGE_LOGE("JpegEncodeImage::WriteJpegData failed, compressedJpegSize: %{public}d, bufferSize: %{public}d",
            compressedJpegSize, outputBuffer.bufferSize);
        return INVALID_OUTPUT_JPEG_SIZE;
    }
    auto* ptr = static_cast<uint8_t *>(outputBuffer.buffer);
    size_t dropDataSize = 0;
    while (dropDataSize < compressedJpegSize && (*ptr) != 0xff) {
        ++dropDataSize;
        ++ptr;
    }
    if (dropDataSize >= compressedJpegSize) {
        IMAGE_LOGE("WriteJpegData failed, invalid data, dropDataSize: %{public}lu, compressedJpegSize: %{public}u",
            dropDataSize, compressedJpegSize);
        return INVALID_OUTPUT_BUFFER_DATA;
    }
    compressedJpegSize -= dropDataSize;
    uint8_t* iccData = nullptr;
    size_t iccLength = 0;
    int32_t retCode = GetIccData(pixelMap, iccData, iccLength);
    if (retCode != SUCCESS) {
        IMAGE_LOGI("pack icc data failed, retCode is %{public}d", retCode);
        output->write(ptr, compressedJpegSize);
        return SUCCESS;
    }
 
    auto* head = ptr;
    size_t writedJpegDataSize = 0;
    size_t lastValidIndex = compressedJpegSize - 3;
    while (writedJpegDataSize < lastValidIndex) {
        if (*ptr == 0xff && *(ptr + 1) == 0xe0) {
            // match app0 marker
            uint16_t app0Length = ((*(ptr + 2)) << 8) + (*(ptr + 3));
            writedJpegDataSize += MARKER_LENGTH + app0Length;
            ptr += MARKER_LENGTH + app0Length;
            break;
        } else {
            ++writedJpegDataSize;
            ++ptr;
        }
    }
    output->write(head, writedJpegDataSize);
    output->write(iccData, iccLength);
    output->write(ptr, compressedJpegSize - writedJpegDataSize);
    free(iccData);
    return SUCCESS;
}

int32_t JpegHardwareEncoder::WriteJpegData(SkWStream* output, const PlImageBuffer& outputBuffer,
    uint32_t compressedJpegSize)
{
    ImageFuncTimer imageFuncTimer("%s, compressedJpegSize:%d, bufferSize:%d", __func__, compressedJpegSize,
        outputBuffer.bufferSize);
    if (compressedJpegSize == 0 || compressedJpegSize > outputBuffer.bufferSize) {
        IMAGE_LOGE("JpegEncodeImage::WriteJpegData failed, compressedJpegSize: %{public}d, bufferSize: %{public}d",
            compressedJpegSize, outputBuffer.bufferSize);
        return INVALID_OUTPUT_JPEG_SIZE;
    }
    auto* ptr = static_cast<uint8_t *>(outputBuffer.buffer);
    size_t dropDataSize = 0;
    while (dropDataSize < compressedJpegSize && (*ptr) != 0xff) {
        ++dropDataSize;
        ++ptr;
    }
    if (dropDataSize >= compressedJpegSize) {
        IMAGE_LOGE("WriteJpegData failed, invalid data, dropDataSize: %{public}lu, compressedJpegSize: %{public}u",
            dropDataSize, compressedJpegSize);
        return INVALID_OUTPUT_BUFFER_DATA;
    }
    compressedJpegSize -= dropDataSize;

    output->write(ptr, compressedJpegSize);
    free(iccData);
    return SUCCESS;
}

void JpegHardwareEncoder::ReleaseDmaBuffer(PlImageBuffer& dmaBuffer)
{
    dmaBuffer.buffer = nullptr;
    ImageUtils::SurfaceBuffer_Unreference(static_cast<SurfaceBuffer*>(dmaBuffer.context));
}
 
bool JpegHardwareEncoder::GetOfflineModeTagId(unsigned int& tagId)
{
    std::vector<MediaOffline::V1_0::OfflineVendorTag> vendorTags;
    mediaOfflineSession_->GetVendorTags(vendorTags);
    std::string targetTagName = "offlineMode";
    auto it = std::find_if(vendorTags.begin(), vendorTags.end(), [&targetTagName]
        (const MediaOffline::V1_0::OfflineVendorTag& vendorTag) {return vendorTag.tagName == targetTagName;});
    if (it != vendorTags.end()) {
        tagId = it->tagId;
        return true;
    } else {
        IMAGE_LOGE("%{public}s failed, %{public}s not exist", __func__, targetTagName.c_str());
        return false;
    }
}

int32_t JpegHardwareEncoder::JpegEncodeImage(SkWStream* output, const PlEncodeOptions& opts, Media::PixelMap* pixelMap)
{
    ImageFuncTimer imageFuncTimer("%s width*height:(%d, %d)", __func__, pixelMap->GetWidth(), pixelMap->GetHeight());
    if (mediaOfflineService_ == nullptr || mediaOfflineSession_ == nullptr) {
        IMAGE_LOGE("JpegEncodeImage mediaOfflineService or mediaOfflineSession nullptr");
        return INVALID_MEDIA_OFFLINE_SERVER;
    }
 
    unsigned int tagId;
    if (!GetOfflineModeTagId(tagId)) {
        IMAGE_LOGE("%{public}s failed because of offline mode tag id not exist", __func__);
        return INVALID_MEDIA_OFFLINE_SERVER;
    }
 
    PlImageBuffer inputBuffer = {};
    int32_t retCode = CopyInputBufferFromPixelMap(inputBuffer, pixelMap);
    if (retCode != SUCCESS) {
        IMAGE_LOGE("JpegEncodeImage CopyInputBufferFromPixelMap failed, inputSurfaceBuffer is null");
        return INPUT_BUFFER_NULL;
    }
    auto inputSurfaceBuffer = static_cast<SurfaceBuffer*>(inputBuffer.context);
    OHOS::sptr<NativeBuffer> inNativeBuffer = new NativeBuffer(inputSurfaceBuffer->GetBufferHandle());
 
    PlImageBuffer outputBuffer = {};
    int32_t outBufferSize = static_cast<int32_t>(inputSurfaceBuffer->GetSize());
    retCode = AllocDmaBuffer(outputBuffer, outBufferSize > MAX_OUT_BUFFER_SIZE ? MAX_OUT_BUFFER_SIZE : outBufferSize, 1,
        GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BLOB);
    if (retCode != SUCCESS) {
        IMAGE_LOGE("JpegEncodeImage AllocOutputBuffer failed");
        ReleaseDmaBuffer(inputBuffer);
        return retCode;
    }
    BufferHandle *outputHandle = (static_cast<SurfaceBuffer*>(outputBuffer.context))->GetBufferHandle();
    OHOS::sptr<NativeBuffer> outNativeBuffer = new NativeBuffer(outputHandle);
 
    OfflineRequestSetting inputSettings;
    MediaOffline::V1_0::OfflineRequest req {
        .requestIndex = 0,
        .settings = WriteSettings(inputSettings, tagId),
        .inputBuffers = {inNativeBuffer},
        .outputBuffers = {outNativeBuffer},
    };
    std::vector<uint8_t> outSettings;
    retCode = mediaOfflineSession_->ProcessSyncRequest(req, outSettings);
    if (retCode != SUCCESS) {
        IMAGE_LOGE("JpegEncodeImage ProcessRequest failed, retCode is %{public}d", retCode);
    } else {
        OfflineRequestSetting outputSettings;
        ReadSettings(outSettings, outputSettings);
        retCode = WriteJpegData(output, outputBuffer, pixelMap, outputSettings.outJpegSize);
    }
 
    ReleaseDmaBuffer(inputBuffer);
    ReleaseDmaBuffer(outputBuffer);
    return retCode;
}

int32_t JpegHardwareEncoder::ConvertHardwareType(AuxiliaryPictureType type)
{
    int32_t ret= 0;
    switch (type)
    {
    case AuxiliaryPictureType::GAIN_MAP:
        ret = EXTENDED_STREAM_INFO_GAINMAP;
        break;
    case AuxiliaryPictureType::DEPTH_MAP:
        ret = EXTENDED_STREAM_INFO_DEPTH ;
        break;
    case AuxiliaryPictureType::UNREFOCUS_MAP:
        ;
        break;
    case AuxiliaryPictureType::LINEAR_MAP:
        ;
        break;
    case AuxiliaryPictureType::MARK_CUT_MAP:
        ;
        break;
    default:
        break;
    }
    return ret
}

int32_t JpegHardwareEncoder::JpegEncodeImage(SkWStream* output, const PlEncodeOptions& opts, Media::Picture* picture)
{
    ImageFuncTimer imageFuncTimer("%s width*height:(%d, %d)", __func__,
        picture->GetMainPixel()->GetWidth(), picture->GetMainPixel()->GetHeight());
    if (mediaOfflineService_ == nullptr || mediaOfflineSession_ == nullptr) {
        IMAGE_LOGE("JpegEncodeImage mediaOfflineService or mediaOfflineSession nullptr");
        return INVALID_MEDIA_OFFLINE_SERVER;
    }
 
    unsigned int tagId;
    if (!GetOfflineModeTagId(tagId)) {
        IMAGE_LOGE("%{public}s failed because of offline mode tag id not exist", __func__);
        return INVALID_MEDIA_OFFLINE_SERVER;
    }
 
    PlImageBuffer inputBuffer = {};
    int32_t retCode = CopyInputBufferFromPixelMap(inputBuffer, picture->GetMainPixel().Get());
    if (retCode != SUCCESS) {
        IMAGE_LOGE("JpegEncodeImage CopyInputBufferFromPixelMap failed, inputSurfaceBuffer is null");
        return INPUT_BUFFER_NULL;
    }
    auto inputSurfaceBuffer = static_cast<SurfaceBuffer*>(inputBuffer.context);
    OHOS::sptr<NativeBuffer> inNativeBuffer = new NativeBuffer(inputSurfaceBuffer->GetBufferHandle());

    std::vector<OHOS::sptr<NativeBuffer>> inputBuffers;
    inputBuffers.push_back(inNativeBuffer);
    int32_t outBufferSize = static_cast<int32_t>(inputSurfaceBuffer->GetSize());

    std::vector<PlImageBuffer> imageBuffers;
    OfflineRequestSetting inputSettings;
    std::vector<AuxiliaryPictureType> auxiliaryTypes = {AuxiliaryPictureType::GAIN_MAP, AuxiliaryPictureType::DEPTH_MAP
        AuxiliaryPictureType::UNREFOCUS_MAP, AuxiliaryPictureType::LINEAR_MAP, AuxiliaryPictureType::MARK_CUT_MAP};
    for (auto type : auxiliaryTypes) {
        if (picture->HasAuxiliaryPicture(type)) {
            auto auxiliaryMap = picture->GetAuxiliaryPicture(type);
            auto contentPixel = auxiliaryMap->GetContentPixel();
            PlImageBuffer inputAuxiliaryBuffer = {};
            retCode = CopyInputBufferFromPixelMap(inputAuxiliaryBuffer, contentPixel.Get());
            if (retCode != SUCCESS) {
                IMAGE_LOGE("JpegEncodeImage CopyInputBufferFromPixelMap failed, "
                    "inputAuxiliaryBuffer is null  %{public}d", type);
                return INPUT_BUFFER_NULL;
            }
            imageBuffers.push_back(inputAuxiliaryBuffer);
            auto inputAuxiliarySurfaceBuffer = static_cast<SurfaceBuffer*>(inputAuxiliaryBuffer.context);
            OHOS::sptr<NativeBuffer> nativeBuffer = new NativeBuffer(inputAuxiliarySurfaceBuffer->GetBufferHandle());
            outBufferSize += static_cast<int32_t>(nativeBuffer->GetSize());
            inputBuffers.push_back(nativeBuffer);
            auto settingType = ConvertHardwareType(type)
            inputSettings.bufferTypes.push_back(settingType);
        }
    }

    PlImageBuffer outputBuffer = {};
    retCode = AllocDmaBuffer(outputBuffer, outBufferSize > MAX_OUT_BUFFER_SIZE ? MAX_OUT_BUFFER_SIZE : outBufferSize, 1,
        GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BLOB);
    if (retCode != SUCCESS) {
        IMAGE_LOGE("JpegEncodeImage AllocOutputBuffer failed");
        ReleaseDmaBuffer(inputBuffer);
        return retCode;
    }
    BufferHandle *outputHandle = (static_cast<SurfaceBuffer*>(outputBuffer.context))->GetBufferHandle();
    OHOS::sptr<NativeBuffer> outNativeBuffer = new NativeBuffer(outputHandle);
 
    MediaOffline::V1_0::OfflineRequest req {
        .requestIndex = 0,
        .settings = WriteSettings(inputSettings, tagId),
        .inputBuffers = inputBuffers,
        .outputBuffers = {outNativeBuffer},
    };
    std::vector<uint8_t> outSettings;
    retCode = mediaOfflineSession_->ProcessSyncRequest(req, outSettings);
    if (retCode != SUCCESS) {
        IMAGE_LOGE("JpegEncodeImage ProcessRequest failed, retCode is %{public}d", retCode);
    } else {
        OfflineRequestSetting outputSettings;
        ReadSettings(outSettings, outputSettings);
        retCode = WriteJpegData(output, outputBuffer, outputSettings.outJpegSize);
    }
    for (const auto &buffer: imageBuffers) {
        ReleaseDmaBuffer(buffer);
    }
    ReleaseDmaBuffer(outputBuffer);
    return retCode;
}
} // namespace OHOS::ImagePlugin
