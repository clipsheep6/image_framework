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
#include "hisysevent.h"

namespace OHOS {
namespace Media {
static constexpr uint32_t INTERVAL = 1000;
static constexpr uint64_t MEMORY_THRESHOLD_BYTE = 0;
static constexpr uint64_t TIME_THRESHOLD_MS = 0;
static constexpr char IMAGE_FWK_UE[] = "IMAGE_FWK_UE";

ImageEvent::ImageEvent(const std::string &module, const std::string &msg, const std::string &eventNameFault) :
    module_(module), msg_(msg), eventNameFault_(eventNameFault)
{
#if !defined(_WIN32) && !defined(_APPLE)
    struct timeval time;
    gettimeofday(&time, NULL);
    start_time_ = time.tv_sec * INTERVAL + time.tv_usec/INTERVAL;
    if (eventNameFault_.compare("FAULT_ENCODE_TIME_EXCEEDED")) {
        eventNameStatics_ = "STATISTIC_ENCODE_TIME";
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
    if (timeInterval >= TIME_THRESHOLD_MS) {
        HiSysEventWrite(IMAGE_FWK_UE,
                        eventNameFault_,
                        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                        "TAKE_TIME", timeInterval,
                        "EXCEEDED_TIME", timeInterval - TIME_THRESHOLD_MS,
                        "MODULE", module_,
                        "MSG", msg_);
    } else {
        HiSysEventWrite(IMAGE_FWK_UE,
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
        HiSysEventWrite(IMAGE_FWK_UE,
            "FAULT_MEMORY_EXCEEDED",
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
            "OCCUPY_MEMORY", memory,
            "EXCEEDED_MEMORY", memory - MEMORY_THRESHOLD_BYTE,
            "MODULE", module,
            "MSG", msg);
    }
}

void FaultHardWareDecode(int32_t width, int32_t height, uint32_t samplesize, uint32_t errcode, std::string msg)
{
    HiSysEventWrite(IMAGE_FWK_UE,
        "FAULT_HARDWARE_DECODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "SRC_WIDTH", width,
        "SRC_HEIGHT", height,
        "SAMPLE_SIZE", samplesize,
        "ERROR_CODES", errcode,
        "MSG", msg);
}

void FaultSoftWareDecode(int32_t width, int32_t height, int32_t pixelformat, int32_t colorspace, uint32_t samplesize,
    uint32_t code, std::string msg)
{
    HiSysEventWrite(IMAGE_FWK_UE,
        "FAULT_SOFTWARE_DECODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "DES_WIDTH", width,
        "DES_HEIGHT", height,
        "DES_PIXEL_FORMAT", pixelformat,
        "DES_COLOR_SPACE", colorspace,
        "SAMPLE_SIZE", samplesize,
        "ERROR_CODES", code,
        "MSG", msg);
}

void InfoDecode(int32_t width, int32_t height, int32_t pixelformat, int32_t colorspace, uint32_t samplesize)
{
    HiSysEventWrite(IMAGE_FWK_UE,
        "INFO_DECODE_IMAGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "DES_WIDTH", width,
        "DES_HEIGHT", height,
        "DES_PIXEL_FORMAT", pixelformat,
        "DES_COLOR_SPACE", colorspace,
        "SAMPLE_SIZE", samplesize);
} 



void InfoEncode(std::string format, uint32_t quality, uint32_t width, uint32_t height, int32_t space, int32_t type, int32_t pixelformat)
{
   HiSysEventWrite(IMAGE_FWK_UE,
        "INFO_ENCODE_IMAGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "ENCODE_FORMAT", format,
        "QUALITY", quality,
        "WIDTH", width,
        "HEIGHT", height,
        "COLOR_SPACE", space,
        "COLOR_ALPHA", type,
        "PIXEL_FORMAT", pixelformat);
}

void InfoHardDecode(uint32_t width, uint32_t height, bool supportHardDecode)
{
    HiSysEventWrite(IMAGE_FWK_UE,
        "INFO_HARDWARE_DECODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "WIDTH", width,
        "HEIGHT", height,
        "HARD_DECODE", supportHardDecode);
}

void InfoIncrementalDecode(int32_t width, int32_t height, int32_t pixelFormat, int32_t colorspace, uint32_t sampleSize)
{
    HiSysEventWrite(IMAGE_FWK_UE,
        "INFO_INCREMENTAL_DECODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "WIDTH", width,
        "HEIGHT", height,
        "PIXEL_FORMAT", pixelFormat,
        "COLOR_SPACE", colorspace,
        "SAMPLE_SIZE", sampleSize);
}

void StatisticWithJson(std::string eventName, uint32_t count)
{
    // int32_t appUid = IPCSkeleton::GetCallingUid();
    // std::string bundleName = GetClientBundleName(appUid);
    // HiSysEventWrite(IMAGE_FWK_UE, eventName,
    //     OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
    //     "APP_NAME", bundleName,
    //     "CALLED_NUM", count);
}

} // namespace Media
} // namespace OHOS