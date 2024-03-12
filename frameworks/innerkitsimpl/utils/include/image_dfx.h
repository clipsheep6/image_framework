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
#include <refbase.h>
#include "nocopyable.h"
#include "hisysevent.h"
namespace OHOS {
namespace Media {
struct ImageDfxInfo{
    std::string srcImgFormat;
    uint32_t srcImgWidth;
    uint32_t srcImgHeight;
    std::string dstImgFormat;
    uint32_t dstImgWidth;
    uint32_t dstImgHeight;
    uint32_t samlingRate;
    std:string pixelFormat;
    std::string imgSource;
    uint32_t sampleSize;
};

void FaultExceededMemory(std::string msg, std::string module, uint64_t memory);
void FaultHardWareDecode(std::string msg, std::string module, uint32_t width, uint32_t height, uint32_t size);
void InfoEncode(ImageDfxInfo imageDfxInfo);
void InfoDecode(ImageDfxInfo imageDfxInfo);
void InfoHardDecode(std::string format, uint32_t width, uint32_t height, bool isSoftwareCodec,
        std::string role, std::string type);
void InfoIncrementalDecode(uint32_t srcwidth, uint32_t srcheight, uint32_t srcsize, uint32_t dstwidth,
    uint32_t dstheight, uint32_t dstsize);
void InfoApplication
void StatisticWithJson(std::string eventName, uint32_t count);


class __attribute__((visibility("default"))) ImageEvent {
public:
    ImageEvent() = delete;
    ImageEvent(const std::string &module, const std::string &msg, const std::string &eventNameFault);
    ~ImageEvent();
private:
    std::string module_;
    std::string eventNameFault_;
    std::string eventNameStatics_;
    std::string function_;
    uint64_t start_time_;
    uint64_t end_time_;
};

} // namespace Media
} // namespace OHOS

#endif // IMAGE_DFX_H 