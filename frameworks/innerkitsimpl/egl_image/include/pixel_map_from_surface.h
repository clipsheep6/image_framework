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

#ifndef FRAMEWORKS_INNERKITSIMPL_EGL_IMAGE_INCLUDE_PIXELMAP_FROM_SURFACE_H_
#define FRAMEWORKS_INNERKITSIMPL_EGL_IMAGE_INCLUDE_PIXELMAP_FROM_SURFACE_H_

#include "render_context.h"

#include "pixel_map.h"
#include "surface_buffer.h"
#include "surface_utils.h"
#include "window.h"

namespace OHOS {
namespace Media {
// we use this helper to make pixelmap from a surface, and will construct this object
// in every PixelMap::CreateFromSurface() call.
class CreateFromSurfaceHelper {
public:
    CreateFromSurfaceHelper();
    ~CreateFromSurfaceHelper() noexcept;

    std::unique_ptr<PixelMap> Create(uint64_t surfaceId, const Rect &srcRect);

    // disallow copy and move
    CreateFromSurfaceHelper(const CreateFromSurfaceHelper &) = delete;
    void operator=(const CreateFromSurfaceHelper &) = delete;
    CreateFromSurfaceHelper(const CreateFromSurfaceHelper &&) = delete;
    void operator=(const CreateFromSurfaceHelper &&) = delete;

private:
    void Clear() noexcept;

    OHNativeWindow *nativeWindow_ = nullptr;
    OHNativeWindowBuffer *nativeWindowBuffer_ = nullptr;
    std::unique_ptr<RenderContext> renderContext_;
    GLuint texId_ = 0U;
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
};

std::unique_ptr<PixelMap> CreatePixelMapFromSurface(uint64_t surfaceId, const Rect &srcRect);
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_EGL_IMAGE_INCLUDE_PIXELMAP_FROM_SURFACE_H_
