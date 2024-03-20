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
//#include <refbase.h>
//#include "nocopyable.h"
//#include "hisysevent.h"
namespace OHOS {
namespace Media {
void FaultExceededMemory(std::string msg, std::string module, uint64_t memory);
void FaultHardWareDecode(int32_t width, int32_t height, uint32_t samplesize, uint32_t errcode, std::string msg);
void FaultSoftWareDecode(int32_t width, int32_t height, int32_t pixelformat, int32_t colorspace, uint32_t samplesize,
    uint32_t code, std::string msg);
void InfoDecode(int32_t width, int32_t height, int32_t pixelformat, int32_t colorspace, uint32_t samplesize);
void InfoEncode(std::string format, uint32_t quality, uint32_t width, uint32_t height, int32_t space, int32_t type, int32_t pixelformat);
void InfoHardDecode(uint32_t width, uint32_t height, bool supportHardDecode);
void InfoIncrementalDecode(int32_t width, int32_t height, int32_t pixelFormat, int32_t colorspace, uint32_t sampleSize);


class ImageEvent {
public:
    ImageEvent() = delete;
    ImageEvent(const std::string &module, const std::string &msg, const std::string &eventNameFault);
    ~ImageEvent();
private:
    std::string module_;
    std::string msg_;
    std::string eventNameFault_;
    std::string eventNameStatics_ = "STATISTIC_DECODE_TIME";
    std::string function_;
    uint64_t start_time_;
    uint64_t end_time_;
};

} // namespace Media
} // namespace OHOS

#endif // IMAGE_DFX_H 