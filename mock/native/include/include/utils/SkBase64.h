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

#ifndef MOCK_NATIVE_INCLUDE_INCLUDE_UIILS_SkBase64_
#define MOCK_NATIVE_INCLUDE_INCLUDE_UIILS_SkBase64_

#include <stddef.h>
#include <stdint.h>

struct SkBase64 {
public:
    enum Error {
        kNoError,
        kPadError,
        kBadCharError
    };

    SkBase64();
    Error decode(const char* src, size_t length);
    char* getData() { return fData; }
    size_t getDataSize() { return fLength; }
    /**
       Base64 encodes src into dst. encode is a pointer to at least 65 chars.
       encode[64] will be used as the pad character. Encodings other than the
       default encoding cannot be decoded.
    */
    static size_t Encode(const void* src, size_t length, void* dest, const char* encode = nullptr);

private:
    Error decode(const void* srcPtr, size_t length, bool writeDestination);

    size_t fLength;
    char* fData;
    friend class SkImageBaseBitmap;
};

#endif // MOCK_NATIVE_INCLUDE_INCLUDE_UIILS_SkBase64_
