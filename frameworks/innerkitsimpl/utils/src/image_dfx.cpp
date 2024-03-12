/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include "image_dfx.h"
#include <sys/time.h>

namespace OHOS {
namespace Media {
static constexpr uint32_t INTERVAL = 1000;
static constexpr uint64_t MEMORY_THRESHOLD_BYTE = 314572800;
static constexpr uint64_t TIME_THRESHOLD_MS = 500;
static constexpr std::string ENCODE_TIME_EXCEEDED_THRESHOLD("ENCODE_TIME_EXCEEDED_THRESHOLD");

ImageEvent::ImageEvent(const std::string &module, const std::string &msg, const std::string &eventNameFault) :
    module_(module), msg_(msg), eventNameFault_(eventNameFault)
#if !defined(_WIN32) && !defined(_APPLE)
    struct timeval time;
    gettimeofday(&time, NULL);
    start_time_ = time.tv_sec * INTERVAL + time.tv_usec/INTERVAL;\

    eventNameStatics_("DECODE_TIME");
    if (eventNameFault_.compare(ENCODE_TIME_EXCEEDED_THRESHOLD)) {
        eventNameStatics_("ENCODE_TIME");
    }
#endif
}

ImageEvent::~ImageEvent()
{
#if !defined(_WIN32) && !defined(_APPLE)
    struct timeval time;
    gettimeofday(&time, NULL);
    end_time_ = time.tv_sec * INTERVAL + time.tv_usec/INTERVAL;

    uint64_t timeInterval = end_time_ - start_time_;
    if (timeInterval > TIME_THRESHOLD_MS) {
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
                        eventNameFault_,
                        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                        "TAKE_TIME", timeInterval,
                        "EXCEEDED_TIME", timeInterval - TIME_THRESHOLD_MS,
                        "MODULE", module_,
                        "MSG", msg_);
    } else {
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
                eventNameStatics_,
                OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                "TAKE_TIME", timeInterval,
                "MODULE", module_,
                "MSG", msg_);

    }
#endif
}

void FaultExceededMemory(std::string msg, std::string module, uint64_t memory)
{
    if (memory > MEMORY_THRESHOLD_BYTE) {
        HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
            "MEMORY_EXCEEDED_THRESHOLD",
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
            "OCCUPY_MEMORY", memory,
            "EXCEEDED_MEMORY", memory - MEMORY_THRESHOLD_BYTE
            "MODULE", module,
            "MSG", msg);
    }
}

void FaultHardWareDecode(std::string msg, std::string module, u_int32_t width, u_int32_t height, u_int32_t size)
{
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
        "HARDWARE_DECODE_FAILURE",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "WIDTH", width,
        "HEIGHT", height,
        "SAMPLE_SIZE", size,
        "MODULE", module,
        "MSG", msg_);
}

void BehaviorWithEncodeImageInfo(ImageDfxInfo imageDfxInfo)
{
   HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
        "ENCODE_IMAGE_INFO",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "SRC_IMG_FORMAT", imageDfxInfo.srcImgFormat,
        "SRC_IMG_WIDTH", imageDfxInfo.srcImgWidth,
        "SRC_IMG_HEIGHT", imageDfxInfo.srcImgHeight,
        "DST_IMG_FORMAT",imageDfxInfo.dstImgFormat,
        "DST_IMG_WIDTH",imageDfxInfo.dstImgWidth,
        "DST_IMG_HEIGHT",imageDfxInfo.dstImgHeight,
        "SAMPLING_RATE",imageDfxInfo.samlingRate,
        "PIXEL_FORMAT",imageDfxInfo.pixelFormat,
        "IMG_SOURCE",imageDfxInfo.imgSource,
        "SAMPLE_SIZE", imageDfxInfo.sampleSize);
}

void BehaviorWithDecodeImageInfo(ImageDfxInfo imageDfxInfo)
{
   HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
        "DECODE_IMAGE_INFO",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "SRC_IMG_FORMAT", imageDfxInfo.srcImgFormat,
        "SRC_IMG_WIDTH", imageDfxInfo.srcImgWidth,
        "SRC_IMG_HEIGHT", imageDfxInfo.srcImgHeight,
        "DST_IMG_FORMAT",imageDfxInfo.dstImgFormat,
        "DST_IMG_WIDTH",imageDfxInfo.dstImgWidth,
        "DST_IMG_HEIGHT",imageDfxInfo.dstImgHeight,
        "SAMPLING_RATE",imageDfxInfo.samlingRate,
        "PIXEL_FORMAT",imageDfxInfo.pixelFormat,
        "IMG_SOURCE",imageDfxInfo.imgSource,
        "SAMPLE_SIZE", imageDfxInfo.sampleSize);
}

void BehaviorWithIsHardDecode(std::string format, uint32_t width, uint32_t height, bool isSoftwareCodec,
        std::string role, std::string type)
{
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
        "EXEC_HARDWARE_DECODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "IMG_FORMAT", format,
        "IMG_WIDTH", width,
        "IMG_HEIGHT", height,
        "IS_SOFTWARE_CODEC", isSoftwareCodec,
        "ROLE", role,
        "TYPE", type);
}

void BehaviorWithIncrementalDecoding(uint32_t srcwidth, uint32_t srcheight, uint32_t srcsize, uint32_t dstwidth,
    uint32_t dstheight, uint32_t dstsize)
{
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE,
        "INCREMENTAL_DECODING",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "SRC_WIDTH", srcwidth,
        "SRC_HEIGHT", srcheight,
        "SRC_SIZE", srcsize,
        "DST_WIDTH", dstwidth,
        "DST_HEIGHT", dstheight,
        "DST_SIZE", dstsize);
}

void StatisticWithJson(std::string eventName, uint32_t count)
{
    int32_t appUid = IPCSkeleton::GetCallingUid();
    std::string bundleName = GetClientBundleName(appUid);
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::IMAGE_FWK_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "APP_NAME", bundleName,
        "CALLED_NUM", count);
}

} // namespace Media
} // namespace OHOS