#include "pixelmap_native_impl.h"

#include "pixel_map.h"
#include "pixel_map_from_surface.h"

using namespace OHOS::Media;
#ifdef __cplusplus
extern "C" {
#endif

OH_PixelmapNative::OH_PixelmapNative(std::shared_ptr<PixelMap> pixelmap)
{
    if (pixelmap == nullptr) {
        pixelmap_ = nullptr;
        return;
    }
    pixelmap_ = pixelmap;
}

OH_PixelmapNative::OH_PixelmapNative(const uint32_t *colors, uint32_t colorLength, const InitializationOptions &opts)
{
    auto tmpPixelmap = PixelMap::Create(colors, colorLength, opts);
    pixelmap_ = std::move(tmpPixelmap);
}

OH_PixelmapNative::OH_PixelmapNative(uint64_t surfaceId, OHOS::Media::Rect region)
{
    auto tmpPixelmap = CreatePixelMapFromSurfaceId(surfaceId, region);
    pixelmap_ = std::move(tmpPixelmap);
}

OH_PixelmapNative::OH_PixelmapNative(OH_PixelmapNative *OH_PixelmapNative, const InitializationOptions &opts)
{
    if (OH_PixelmapNative == nullptr) {
        pixelmap_ = nullptr;
        return;
    }
    auto pixelmapPtr = OH_PixelmapNative->GetInnerPixelmap().get();
    auto tmpPixelmap = PixelMap::Create(*pixelmapPtr, opts);
    pixelmap_ = std::move(tmpPixelmap);
}

std::shared_ptr<OHOS::Media::PixelMap> OH_PixelmapNative::GetInnerPixelmap()
{
    return pixelmap_;
}

OH_PixelmapNative::~OH_PixelmapNative()
{
    if (pixelmap_) {
        pixelmap_ = nullptr;
    }
}

#ifdef __cplusplus
};
#endif

// }
// }