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

#include "render_context.h"

#include "hilog/log.h"
#include "log_tags.h"

namespace OHOS {
namespace Media {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "PixelMap" };
using namespace OHOS::HiviewDFX;

RenderContext::RenderContext()
{}

RenderContext::~RenderContext() noexcept
{
    Clear();
}
    
bool RenderContext::Init()
{
    if (!InitEGLContext()) {
        return false;
    }

    if (!InitGrContext()) {
        return false;
    }

    return true;
}

bool RenderContext::InitEGLContext()
{
    using GetPlatformDisplayExt = PFNEGLGETPLATFORMDISPLAYEXTPROC;
    GetPlatformDisplayExt eglGetPlatformDisplayExt = reinterpret_cast<GetPlatformDisplayExt>(
        eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (eglGetPlatformDisplayExt != nullptr) {
        eglDisplay_ = eglGetPlatformDisplayExt(EGL_PLATFORM_OHOS_KHR, EGL_DEFAULT_DISPLAY, nullptr);
    } else {
        eglDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    }
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        HiLog::Error(LABEL, "RenderContext::Init: eglGetDisplay error: ");
        return false;
    }

    EGLint major, minor;
    if (eglInitialize(eglDisplay_, &major, &minor) == EGL_FALSE) {
        HiLog::Error(LABEL, "Failed to initialize EGLDisplay");
        return false;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        HiLog::Error(LABEL, "Failed to bind OpenGL ES API");
        return false;
    }

    unsigned int ret;
    EGLint count;
    EGLint configAttribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };

    ret = eglChooseConfig(eglDisplay_, configAttribs, &config_, 1, &count);
    if (!(ret && static_cast<unsigned int>(count) >= 1)) {
        HiLog::Error(LABEL, "Failed to eglChooseConfig");
        return false;
    }

    static const EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    eglContext_ = eglCreateContext(eglDisplay_, config_, EGL_NO_CONTEXT, contextAttribs);
    if (eglContext_ == EGL_NO_CONTEXT) {
        HiLog::Error(LABEL, "Failed to create egl context %{public}x", eglGetError());
        return false;
    }

    return true;
}

bool RenderContext::InitGrContext()
{
    sk_sp<const GrGLInterface> glInterface(GrGLCreateNativeInterface());
    if (glInterface == nullptr) {
        HiLog::Error(LABEL, "SetUpGrContext failed to make native interface");
        return false;
    }

    GrContextOptions options;
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;
    grContext_ = GrDirectContext::MakeGL(std::move(glInterface), options);
    return grContext_ != nullptr;
}

void RenderContext::Clear() noexcept
{
    if (eglDisplay_ == EGL_NO_DISPLAY) {
        return;
    }

    grContext_ = nullptr;
    eglDestroyContext(eglDisplay_, eglContext_);
    eglTerminate(eglDisplay_);
    eglContext_ = EGL_NO_CONTEXT;
    eglDisplay_ = EGL_NO_DISPLAY;
}
} // namespace Media
} // namespace OHOS
