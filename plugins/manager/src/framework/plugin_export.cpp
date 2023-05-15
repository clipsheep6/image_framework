/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "plugin_export.h"
#include "bmp_decoder.h"
#include "bmp_format_agent.h"
#include "gif_decoder.h"
#include "gif_format_agent.h"
#include "hilog/log.h"
#include "hilog/log_c.h"
#include "hilog/log_cpp.h"
#include "iosfwd"
#include "jpeg_decoder.h"
#include "jpeg_encoder.h"
#include "jpeg_format_agent.h"
#include "log_tags.h"
#include "map"
#include "plugin_class_base.h"
#include "plugin_utils.h"
#include "png_decoder.h"
#include "png_format_agent.h"
#include "raw_decoder.h"
#include "raw_format_agent.h"
#include "string"
#include "svg_decoder.h"
#include "svg_format_agent.h"
#include "utility"
#include "wbmp_format_agent.h"
#include "webp_decoder.h"
#include "webp_encoder.h"
#include "webp_format_agent.h"

namespace {
    const std::string PACKAGE_NAME = ("LibImagePluginsExport");
}

using std::string;
using namespace OHOS::HiviewDFX;

static constexpr HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_PLUGIN, "LibImagePluginsExport" };

#define PLUGIN_LOG_D(...) HiLog::Debug(LABEL, __VA_ARGS__)
#define PLUGIN_LOG_E(...) HiLog::Error(LABEL, __VA_ARGS__)

// define the external interface of this plugin.
PLUGIN_EXPORT_DEFAULT_EXTERNAL_START()
PLUGIN_EXPORT_DEFAULT_EXTERNAL_STOP()

#if defined(IOS_PLATFORM) || defined(A_PLATFORM)
template<class ImplClassType>
OHOS::MultimediaPlugin::PluginClassBase *InnerCreatePluginObject()
{
    return static_cast<OHOS::MultimediaPlugin::PluginClassBase *>(new (std::nothrow) ImplClassType());
}

static bool CompareClass(const string target, const string &className) {
  return target.compare(className) == 0;
}
#endif

OHOS::MultimediaPlugin::PluginClassBase *PluginExternalCreate(const string &className)
{
    HiLog::Debug(LABEL, "LibImagePluginsExport: create object for package: %{public}s, class: %{public}s.",
                PACKAGE_NAME.c_str(), className.c_str());
#if !defined(IOS_PLATFORM) && !defined(A_PLATFORM)
    auto iter = implClassMap.find(className);
    if (iter == implClassMap.end()) {
        HiLog::Error(LABEL, "LibImagePluginsExport: failed to find class: %{public}s, in package: %{public}s.",
                     className.c_str(), PACKAGE_NAME.c_str());
        return nullptr;
    }

    auto creator = iter->second;
    if (creator == nullptr) {
        HiLog::Error(LABEL, "LibImagePluginsExport: null creator for class: %{public}s, in package: %{public}s.",
                     className.c_str(), PACKAGE_NAME.c_str());
        return nullptr;
    }

    return creator();
#else
    if (CompareClass("OHOS::ImagePlugin::JpegDecoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::JpegDecoder>();
    } else if (CompareClass("OHOS::ImagePlugin::JpegEncoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::JpegEncoder>();
    } else if (CompareClass("OHOS::ImagePlugin::BmpDecoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::BmpDecoder>();
    } else if (CompareClass("OHOS::ImagePlugin::BmpFormatAgent", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::BmpFormatAgent>();
    } else if (CompareClass("OHOS::ImagePlugin::JpegFormatAgent", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::JpegFormatAgent>();
    } else if (CompareClass("OHOS::ImagePlugin::PngDecoder", className)) {
      HiLog::Debug(LABEL, "LibImagePluginsExport: create PngDecoder");
      return InnerCreatePluginObject<OHOS::ImagePlugin::PngDecoder>();
    } else if (CompareClass("OHOS::ImagePlugin::PngFormatAgent", className)) {
      HiLog::Debug(LABEL, "LibImagePluginsExport: create PngFormatAgent");
      return InnerCreatePluginObject<OHOS::ImagePlugin::PngFormatAgent>();
    } else if (CompareClass("OHOS::ImagePlugin::GifDecoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::GifDecoder>();
    } else if (CompareClass("OHOS::ImagePlugin::GifFormatAgent", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::GifFormatAgent>();
    } else if (CompareClass("OHOS::ImagePlugin::WebpDecoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::WebpDecoder>();
    } else if (CompareClass("OHOS::ImagePlugin::WebpEncoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::WebpEncoder>();
    } else if (CompareClass("OHOS::ImagePlugin::WebpFormatAgent", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::WebpFormatAgent>();
    } else if (CompareClass("OHOS::ImagePlugin::WbmpFormatAgent", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::WbmpFormatAgent>();
    } else if (CompareClass("OHOS::ImagePlugin::RawDecoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::RawDecoder>();
    } else if (CompareClass("OHOS::ImagePlugin::RawFormatAgent", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::RawFormatAgent>();
    } else if (CompareClass("OHOS::ImagePlugin::SvgDecoder", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::SvgDecoder>();
    } else if (CompareClass("OHOS::ImagePlugin::SvgFormatAgent", className)) {
      return InnerCreatePluginObject<OHOS::ImagePlugin::SvgFormatAgent>();
    }
    HiLog::Error(LABEL, "LibImagePluginsExport: failed to find class: %{public}s, in package: %{public}s.",
        className.c_str(), PACKAGE_NAME.c_str());
    return nullptr;
#endif
}