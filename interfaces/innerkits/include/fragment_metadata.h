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

#ifndef INTERFACES_INNERKITS_INCLUDE_FRAGMENT_METADATA_H
#define INTERFACES_INNERKITS_INCLUDE_FRAGMENT_METADATA_H

#include "image_type.h"
#include "metadata.h"
#include <map>

namespace OHOS {
namespace Media {
class FragmentMetadata : public ImageMetadata {
public:
    FragmentMetadata();
    virtual ~FragmentMetadata() {}
    
    virtual int GetValue(const std::string &key, std::string &value) const override;
    virtual bool SetValue(const std::string &key, const std::string &value) override;
    virtual bool RemoveEntry(const std::string &key) override;
    virtual std::vector<std::pair<std::string, std::string>> GetAllProperties() override;
    virtual std::shared_ptr<ImageMetadata> CloneMetadata() override;
    bool Marshalling(Parcel &parcel) const override;
    static FragmentMetadata *Unmarshalling(Parcel &parcel);
    static FragmentMetadata *Unmarshalling(Parcel &parcel, PICTURE_ERR &error);

protected:
    bool IsValidKey(const std::string &key) const;
    std::map<std::string, std::string> properties_ = {
    { "X_IN_ORIGINAL", "" },
    { "Y_IN_ORIGINAL", "" },
    { "WIDTH", "" },
    { "HEIGHT", "" },
};
};
} // namespace Media
} // namespace OHOS

#endif // INTERFACES_INNERKITS_INCLUDE_FRAGMENT_METADATA_H
