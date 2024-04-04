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

#ifndef IMAGE_DFX_H
#define IMAGE_DFX_H

#include <string>
namespace OHOS {
namespace Media {
enum class InvocationMode : uint32_t {
    INTERNAL_CALL = 0,
    TS_CALL,
    Native_Development_Kit_CALL
};

struct ReportImageoptions {
    uint32_t srcWidth;
    uint32_t srcHeight;
    uint32_t dstWidth;
    uint32_t dstHeight;
    int32_t pixelFormat;
    int32_t colorSpace;
    uint32_t sampleSize;
    std::string encodeFormat;
    uint8_t quality;
};

class ImageEvent {
public:
    ImageEvent() = delete;
    ImageEvent(const std::string &module, const std::string &eventNameFault);
    ~ImageEvent();
    void SetImageInfo(const ReportImageoptions &opts)
    {
        codecInfo_ = opts;
    }
    void ReportEncodeIfTimeOverFlow(uint64_t times);
    void ReportDecodeIfTimeOverFlow(uint64_t times);
    void ReportEncodeInfo(uint64_t times);
    void ReportDecodeInfo(uint64_t times);
    void ReportDecodeFailed();
    void SetErrorCode(uint32_t code, std::string message);

private:
    std::string msg_;
    std::string eventNameFault_;
    uint64_t startTime_;
    ReportImageoptions codecInfo_;
    uint32_t errorCode_;
};

void ReportIfMemoryOverflow(std::string msg, uint64_t memory);
void ReportHardWareDecodeError(int32_t width, int32_t height, int32_t pixelFormat, uint32_t sampleSize,
    std::string msg);
void CountInterfaceInvokeNums(std::string module, std::string Interface, uint32_t num, uint32_t incocationMode,
    uint64_t addr);
} // namespace Media
} // namespace OHOS
#endif // IMAGE_DFX_H
