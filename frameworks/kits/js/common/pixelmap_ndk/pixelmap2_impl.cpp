#include "pixelmap2_impl.h"

#include "pixel_map.h"
#include "pixel_map_from_surface.h"

using namespace OHOS::Media;
#ifdef __cplusplus
extern "C" {
#endif

OH_Pixelmap::OH_Pixelmap(std::shared_ptr<PixelMap> pixelMap)
{
    if (pixelMap == nullptr) {
        pixelMap_ = nullptr;
        return;
    }
    pixelMap_ = pixelMap;
}

OH_Pixelmap::OH_Pixelmap(const uint32_t *colors, uint32_t colorLength, const InitializationOptions &opts)
{
    auto tmpPixelmap = PixelMap::Create(colors, colorLength, opts);
    pixelMap_ = std::move(tmpPixelmap);
}

OH_Pixelmap::OH_Pixelmap(uint64_t surfaceId, OHOS::Media::Rect region)
{
    auto tmpPixelmap = CreatePixelMapFromSurfaceId(surfaceId, region);
    pixelMap_ = std::move(tmpPixelmap);
}

OH_Pixelmap::OH_Pixelmap(OH_Pixelmap *OH_Pixelmap, const InitializationOptions &opts)
{
    if (OH_Pixelmap == nullptr) {
        pixelMap_ = nullptr;
        return;
    }
    auto pixelMapPtr = OH_Pixelmap->GetInnerPixelMap().get();
    auto tmpPixelmap = PixelMap::Create(*pixelMapPtr, opts);
    pixelMap_ = std::move(tmpPixelmap);
}

std::shared_ptr<OHOS::Media::PixelMap> OH_Pixelmap::GetInnerPixelMap()
{
    return pixelMap_;
}

bool OH_Pixelmap::IsLockPixelMap()
{
    return (lockCount > 0);
}

bool OH_Pixelmap::LockPixelMap()
{
    lockCount++;
    return true;
}

void OH_Pixelmap::UnlockPixelMap()
{
    if (lockCount > 0) {
        lockCount--;
    }
}

OH_Pixelmap::~OH_Pixelmap(){
    if (pixelMap_) {
        pixelMap_ = nullptr;
    }
}

#ifdef __cplusplus
};
#endif

// }
// }