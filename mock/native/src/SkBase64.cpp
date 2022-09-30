/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "include/utils/SkBase64.h"

#define DecodePad (-2)
#define EncodePad 64

SkBase64::SkBase64() : fLength((size_t) -1), fData(nullptr) {
}

#if defined _WIN32  // disable 'two', etc. may be used without having been initialized
#pragma warning (push)
#pragma warning (disable : 4701)
#endif

SkBase64::Error SkBase64::decode(const void* srcPtr, size_t size, bool writeDestination)
{
    return kNoError;
}

#if defined _WIN32
#pragma warning (pop)
#endif

size_t SkBase64::Encode(const void* srcPtr, size_t length, void* dstPtr, const char* encodeMap)
{
    return 0;
}

SkBase64::Error SkBase64::decode(const char* src, size_t len)
{
    return kNoError;
}
