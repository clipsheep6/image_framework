f/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include "fragment_metadata.h"
#include "image_log.h"
#include "media_errors.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "FragmentMetadata"

namespace OHOS {
namespace Media {

FragmentMetadata::FragmentMetadata() {}

int FragmentMetadata::GetValue(const std::string &key, std::string &value) const
{
    if (!IsValidKey(key)) {
        return ERR_MEDIA_INVALID_VALUE;
    }
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        value = it->second;
        return SUCCESS;
    }
    return ERR_MEDIA_INVALID_VALUE;
}

bool FragmentMetadata::SetValue(const std::string &key, const std::string &value)
{
    if (!IsValidKey(key)) {
        IMAGE_LOGE("Key is not supported.");
        return false;
    }
    properties_[key] = value;
    return true;
}

bool FragmentMetadata::RemoveEntry(const std::string &key)
{
    if (!IsValidKey(key)) {
        IMAGE_LOGE("RemoveEntry failed, can not find entry for key: %{public}s", key.c_str());
        return false;
    }
    properties_[key] = "";
    IMAGE_LOGD("RemoveEntry for key: %{public}s", key.c_str());
    return true;
}

std::vector<std::pair<std::string, std::string>> FragmentMetadata::GetAllProperties()
{
    std::vector<std::pair<std::string, std::string>> result;
    for (const auto &pair : properties_) {
        result.push_back(pair);
    }
    return result;
}

std::shared_ptr<ImageMetadata> FragmentMetadata::CloneMetadata()
{
    std::shared_ptr<FragmentMetadata> newFragmentMetadata = std::make_shared<FragmentMetadata>();
    newFragmentMetadata->properties_ = this->properties_;
    return newFragmentMetadata;
}

bool FragmentMetadata::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(properties_.size())) {
        return false;
    }
    for (const auto &pair : properties_) {
        if (!parcel.WriteString(pair.first)) {
            return false;
        }
        if (!parcel.WriteString(pair.second)) {
            return false;
        }
    }
    return true;
}

FragmentMetadata *FragmentMetadata::Unmarshalling(Parcel &parcel)
{
    PICTURE_ERR error;
    FragmentMetadata* dstFragmentMetadata = FragmentMetadata::Unmarshalling(parcel, error);
    if (dstFragmentMetadata == nullptr || error.errorCode != SUCCESS) {
        IMAGE_LOGE("unmarshalling failed errorCode:%{public}d, errorInfo:%{public}s",
            error.errorCode, error.errorInfo.c_str());
    }
    return dstFragmentMetadata;
}

FragmentMetadata *FragmentMetadata::Unmarshalling(Parcel &parcel, PICTURE_ERR &error)
{
    auto metadata = new (std::nothrow) FragmentMetadata();
    int32_t size;
    if (!parcel.ReadInt32(size)) {
        return nullptr;
    }
    for (int32_t i = 0; i < size; ++i) {
        std::string key;
        std::string value;
        if (!parcel.ReadString(key)) {
            return nullptr;
        }
        if (!parcel.ReadString(value)) {
            return nullptr;
        }
        if (!metadata->IsValidKey(key)) {
            return nullptr;
        }
        FragmentMetadata *fragmentMetadata = new (std::nothrow) FragmentMetadata();
        fragmentMetadata->properties_[key] = value;
    }
    return fragmentMetadata;
}

bool FragmentMetadata::IsValidKey(const std::string &key) const {
    return properties_.find(key) != properties_.end();
}
} // namespace Media
} // namespace OHOS