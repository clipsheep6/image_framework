/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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


#ifndef MOCK_NATIVE_INCLUDE_MESSAGE_PARCEL_H
#define MOCK_NATIVE_INCLUDE_MESSAGE_PARCEL_H
#include <string>
#include "parcel.h"

namespace OHOS {
class IRemoteObject;
class MessageParcel : public Parcel {
public:
    MessageParcel();
    ~MessageParcel();
    bool WriteFileDescriptor(int fd);
    int  ReadFileDescriptor();
};
} // namespace OHOS
#endif // MOCK_NATIVE_INCLUDE_MESSAGE_PARCEL_H
