/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
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
#ifndef INTERFACES_KITS_JS_MEDIALIBRARY_INCLUDE_DELETE_CALLBACK_H_
#define INTERFACES_KITS_JS_MEDIALIBRARY_INCLUDE_DELETE_CALLBACK_H_

#include <js_native_api.h>
#include "want.h"
#include <string>
#include <uv.h>
#include <napi/native_api.h>
#include <node_api.h>
#include "ui_content.h"

namespace OHOS{
namespace Media{
static const std::string DELETE_UI_PACKAGE_NAME = "com.ohos.photos";
static const std::string DELETE_UI_EXT_ABILITY_NAME = "DeleteUIExtensionAbility";
static const std::string DELETE_UI_EXTENSION_TYPE = "ability.want.params.uiExtensionType";
static const std::string DELETE_UI_REQUEST_TYPE = "sysDialog/common";
static const std::string DELETE_UI_APPNAME = "appName";
static const std::string DELETE_UI_URIS = "uris";

class DeleteCallback{
public:
    explicit DeleteCallback(napi_env env, Ace::UIContent *uiContent);
    void OnRelease(int32_t releaseCode);
    void OnResult(int32_t resultCode, const OHOS::AAFwk::Want &result);
    void OnReceive(const OHOS::AAFwk::WantParams &request);
    void OnError(int32_t code, const std::string &name, const std::string &message);
    void SetSessionId(int32_t sessionId);
    void SetUris(std::vector<std::string> uris);
    void SetFunc(napi_value func);

private:
    int32_t sessionId_  = 0;
    int32_t resultCode_ = -1;
    std::vector<std::string> uris_;
    napi_env env_ = nullptr;
    napi_ref callbackRef = nullptr;
    Ace::UIContent *uiContent = nullptr;
    void SendMessageBack();
    void CloseModalUIExtension();
};
}
}
#endif
