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

#include <chrono>
#include "securec.h"
#include "image_log.h"
#include "image_data_statistics.h"

namespace OHOS {
namespace Media {
static constexpr int32_t FORMAT_BUF_SIZE = 128;
static constexpr uint64_t MEMORY_THRESHOLD_BYTE = 314572800;
static constexpr uint64_t TIME_THRESHOLD_MS = 500;

ImageDataStatistics::ImageDataStatistics(const std::string &title) : title_(title),
    memorythreshold_(MEMORY_THRESHOLD_BYTE), timethreshold_(TIME_THRESHOLD_MS), memorysize_(0)
{
    start_time_ = GetNowTimeMillSeconds();
}

ImageDataStatistics::ImageDataStatistics(const char *fmt, ...)
{
#if !defined(_WIN32) && !defined(_APPLE)
    if (fmt == nullptr) {
        title_ = "ImageDataTraceFmt Param invalid";
    } else {
        char buf[FORMAT_BUF_SIZE] = { 0 };
        va_list args;
        va_start(args, fmt);
        int32_t ret = vsprintf_s(buf, FORMAT_BUF_SIZE, fmt, args);
        va_end(args);
        if (ret != -1) {
            title_ = buf;
        } else {
            title_ = "ImageDataTraceFmt Format Error";
        }
    }
    start_time_ = GetNowTimeMillSeconds();
#endif
}

ImageDataStatistics::~ImageDataStatistics()
{
#if !defined(_WIN32) && !defined(_APPLE)
    if (memorysize_ >= memorythreshold_) {
        IMAGE_LOGW("%{public}s The requested memory [%{public}llu] bytes exceeded the threshold [%{public}llu]bytes\n",
            title_.c_str(), memorysize_, memorythreshold_);
    }
    end_time_ = GetNowTimeMillSeconds();
    uint64_t timeInterval = end_time_ - start_time_;
    if (timeInterval > timethreshold_) {
        IMAGE_LOGW("%{public}s Runtime [%{public}llu/%{public}llu],start time: %{public}llu, end time: %{public}llu\n",
            title_.c_str(), timeInterval, timethreshold_, start_time_, end_time_);
    }
#endif
}

uint64_t ImageDataStatistics::GetNowTimeMillSeconds()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

void ImageDataStatistics::SetRequestMemory(uint64_t size)
{
    memorysize_ = size;
}

void ImageDataStatistics::SetTimeThreshold(uint64_t size)
{
    timethreshold_ = size;
}

void ImageDataStatistics::SetMemoryThreshold(uint64_t size)
{
    memorythreshold_ = size;
}

void ImageDataStatistics::AddTitle(const std::string title)
{
    title_ += title;
}

void ImageDataStatistics::AddTitle(const char *fmt, ...)
{
#if !defined(_WIN32) && !defined(_APPLE)
    if (fmt == nullptr) {
        title_ += "AddTitle Param invalid";
    } else {
        char buf[FORMAT_BUF_SIZE] = { 0 };
        va_list args;
        va_start(args, fmt);
        int32_t ret = vsprintf_s(buf, FORMAT_BUF_SIZE, fmt, args);
        va_end(args);
        if (ret != -1) {
            title_ += buf;
        } else {
            title_ += "AddTitle Format Error";
        }
    }
#endif
}

std::string ImageDataStatistics::GetTitle()
{
    return title_;
}

} // namespace Media
} // namespace OHOS