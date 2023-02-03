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

#include "image_receiver_manager.h"
namespace OHOS {
namespace Media {
using namespace std;

string ImageReceiverManager::getNewId()
{
    std::lock_guard<std::mutex> guard(idMutex_);
    string res = std::to_string(globalId_);
    globalId_ ++;
    return res;
}

bool ImageReceiverManager::isKeyIdExist(string id)
{
    return mapReceiver_.count(id) != 0;
}
string ImageReceiverManager::SaveImageReceiver(shared_ptr<ImageReceiver> imageReceiver)
{
    string id;
    do {
        id = getNewId();
    } while (isKeyIdExist(id));

    mapReceiver_.insert(pair<string, shared_ptr<ImageReceiver>>(id, imageReceiver));
    return id;
}
shared_ptr<ImageReceiver> ImageReceiverManager::getImageReceiverByKeyId(string keyId)
{
    auto iter = mapReceiver_.find(keyId);
    if (iter != mapReceiver_.end()) {
        return iter->second;
    }
    return nullptr;
}
sptr<Surface> ImageReceiverManager::getSurfaceByKeyId(string keyId)
{
    shared_ptr<ImageReceiver> imageReceiver = getImageReceiverByKeyId(keyId);
    if (imageReceiver != nullptr) {
        return imageReceiver->GetReceiverSurface();
    }
    return nullptr;
}
void ImageReceiverManager::releaseReceiverById(string id)
{
    ImageReceiverManager& manager = ImageReceiverManager::getInstance();
    while (manager.isKeyIdExist(id))
    {
        manager.mapReceiver_.erase(id);
    }
}
} // namespace Media
} // namespace OHOS
