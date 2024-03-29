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
#include "image_dfx.h"
#include <unistd.h>
#include <fstream>
#include <chrono>
#include "hisysevent.h"
#include "image_utils.h"


namespace OHOS {
namespace Media {
static constexpr uint64_t MEMORY_THRESHOLD_BYTE = 600 * 1024 * 1024;
static constexpr uint64_t TIME_THRESHOLD_MS = 500;
static constexpr char IMAGE_FWK_UE[] = "IMAGE_FWK_UE";
static constexpr uint32_t SUCCESS = 0;
static const char* ENCODE_TIMEOUT = "ENCODE_TIMEOUT";

ImageEvent::ImageEvent(const std::string &msg, const std::string &eventNameFault)
    : msg_(msg), eventNameFault_(eventNameFault)
{
    startTime_ = ImageUtils::GetNowTimeMilliSeconds();
    errorCode_ = SUCCESS;
}

ImageEvent::~ImageEvent()
{
    if (errorCode_ != SUCCESS) {
        ReportDecodeFailed();
    } else {
        uint64_t endTime = ImageUtils::GetNowTimeMilliSeconds();
        uint64_t consumeTime = endTime - startTime_;
        if (consumeTime > TIME_THRESHOLD_MS) {
            if (eventNameFault_.compare(ENCODE_TIMEOUT)) {
                ReportDecodeIfTimeOverFlow(consumeTime);
            } else {
                ReportEncodeIfTimeOverFlow(consumeTime);
            }
        } else {
            if (eventNameFault_.compare(ENCODE_TIMEOUT)) {
                ReportDecodeInfo(consumeTime);
            } else {
                ReportEncodeInfo(consumeTime);
            }
        }
    }
}

void ImageEvent::SetErrorCode(uint32_t code, std::string message)
{
    errorCode_ = code;
    msg_ += message;
}

void ImageEvent::ReportDecodeFailed()
{
    std::string packageName = ImageUtils::GetCurrentProcessName();
    HiSysEventWrite(IMAGE_FWK_UE,
                    "DECODE_FAULT",
                    OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                    "PNAMEID", packageName,
                    "ENCODE_FORMAT", codecInfo_.encodeFormat,
                    "DST_WIDTH", codecInfo_.dstWidth,
                    "DST_HEIGHT", codecInfo_.dstHeight,
                    "PIXEL_FORMAT", codecInfo_.pixelFormat,
                    "COLOR_SPACE", codecInfo_.colorSpace,
                    "SAMPLE_SIZE", codecInfo_.sampleSize,
                    "ERROR_CODE", errorCode_,
                    "MSG", msg_);
}

void ImageEvent::ReportEncodeIfTimeOverFlow(uint64_t times)
{
    std::string packageName = ImageUtils::GetCurrentProcessName();
    HiSysEventWrite(IMAGE_FWK_UE,
                    ENCODE_TIMEOUT,
                    OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                    "PNAMEID", packageName,
                    "ENCODE_FORMAT", codecInfo_.encodeFormat,
                    "QUALITY", codecInfo_.quality,
                    "DST_WIDTH", codecInfo_.dstWidth,
                    "DST_HEIGHT", codecInfo_.dstHeight,
                    "PIXEL_FORMAT", codecInfo_.pixelFormat,
                    "COLOR_SPACE", codecInfo_.colorSpace,
                    "TAKE_TIME", times,
                    "EXCEEDED_TIME", times - TIME_THRESHOLD_MS,
                    "MSG", msg_);
}

void ImageEvent::ReportDecodeIfTimeOverFlow(uint64_t times)
{
    std::string packageName = ImageUtils::GetCurrentProcessName();
    HiSysEventWrite(IMAGE_FWK_UE,
                    "DECODE_TIMEOUT",
                    OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                    "PNAMEID", packageName,
                    "ENCODE_FORMAT", codecInfo_.encodeFormat,
                    "DST_WIDTH", codecInfo_.dstWidth,
                    "DST_HEIGHT", codecInfo_.dstHeight,
                    "PIXEL_FORMAT", codecInfo_.pixelFormat,
                    "COLOR_SPACE", codecInfo_.colorSpace,
                    "SAMPLE_SIZE", codecInfo_.sampleSize,
                    "TAKE_TIME", times,
                    "EXCEEDED_TIME", times - TIME_THRESHOLD_MS,
                    "MSG", msg_);
}

void ImageEvent::ReportEncodeInfo(uint64_t times)
{
    std::string packageName = ImageUtils::GetCurrentProcessName();
    HiSysEventWrite(IMAGE_FWK_UE,
                    "ENCODE_INFORMATION",
                    OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "PNAMEID", packageName,
                    "ENCODE_FORMAT", codecInfo_.encodeFormat,
                    "QUALITY", codecInfo_.quality,
                    "DST_WIDTH", codecInfo_.dstWidth,
                    "DST_HEIGHT", codecInfo_.dstHeight,
                    "PIXEL_FORMAT", codecInfo_.pixelFormat,
                    "COLOR_SPACE", codecInfo_.colorSpace,
                    "TAKE_TIME", times);
}

void ImageEvent::ReportDecodeInfo(uint64_t times)
{
    std::string packageName = ImageUtils::GetCurrentProcessName();
    HiSysEventWrite(IMAGE_FWK_UE,
                    "DECODE_INFORMATION",
                    OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
                    "PNAMEID", packageName,
                    "ENCODE_FORMAT", codecInfo_.encodeFormat,
                    "DST_WIDTH", codecInfo_.dstWidth,
                    "DST_HEIGHT", codecInfo_.dstHeight,
                    "PIXEL_FORMAT", codecInfo_.pixelFormat,
                    "COLOR_SPACE", codecInfo_.colorSpace,
                    "SAMPLE_SIZE", codecInfo_.sampleSize,
                    "TAKE_TIME", times);
}

void ReportIfMemoryOverflow(std::string msg, uint64_t memory)
{
    if (memory > MEMORY_THRESHOLD_BYTE) {
        std::string packageName = ImageUtils::GetCurrentProcessName();
        HiSysEventWrite(IMAGE_FWK_UE,
                        "MEMORY_OVERFLOW",
                        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                        "PNAMEID", packageName,
                        "OCCUPY_MEMORY", memory,
                        "EXCEEDED_MEMORY", memory - MEMORY_THRESHOLD_BYTE,
                        "MSG", msg);
    }
}

void ReportHardWareDecodeError(int32_t width, int32_t height, int32_t pixelFormat, uint32_t sampleSize, std::string msg)
{
    std::string packageName = ImageUtils::GetCurrentProcessName();
    HiSysEventWrite(IMAGE_FWK_UE,
                    "HARDWARE_DECODE_FAULT",
                    OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
                    "PNAMEID", packageName,
                    "SRC_WIDTH", width,
                    "SRC_HEIGHT", height,
                    "PIXEL_FORMAT", pixelFormat,
                    "SAMPLE_SIZE", sampleSize,
                    "MSG", msg);
}

void CountInterfaceInvokeNums(std::string module, std::string Interface, uint32_t num, uint32_t incocationMode,
    uint64_t addr)
{
    std::string packageName = ImageUtils::GetCurrentProcessName();
    HiSysEventWrite(IMAGE_FWK_UE,
                    "INTERFACE_INVOKE",
                    OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
                    "PNAMEID", packageName,
                    "MODULE", module,
                    "INTERFACE", Interface,
                    "INVOCATION_MODE", incocationMode,
                    "OBJECT_ADDRESS", addr,
                    "CALLED_NUM", num);
}
} // namespace Media
} // namespace OHOS