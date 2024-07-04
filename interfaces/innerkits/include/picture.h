
#ifndef INTERFACES_INNERKITS_INCLUDE_PICTURE_H_
#define INTERFACES_INNERKITS_INCLUDE_PICTURE_H_

#include "pixel_map.h"
#include "surface_buffer.h"
#include "auxiliary_picture.h"
#include <map>

namespace OHOS {
namespace Media {
class Picture: public Parcelable {
public:
	static std::unique_ptr<Picture> Create(std::shared_ptr<PixelMap> &PixelMap);
	static std::unique_ptr<Picture> Create(sptr<SurfaceBuffer> &surfaceBuffer);
	static std::shared_ptr<PixelMap> SurfaceBuffer2PixelMap(sptr<SurfaceBuffer> &surfaceBuffer);
	std::shared_ptr<PixelMap> GetMainPixel();
	std::unique_ptr<PixelMap> GetHdrComposedPixelMap();
	std::shared_ptr<AuxiliaryPicture> GetAuxiliaryPicture(AuxiliaryPictureType type);
	void SetAuxiliaryPicture(AuxiliaryPictureType type, std::shared_ptr<AuxiliaryPicture> &picture);
	bool HasAuxiliaryPicture(AuxiliaryPictureType type);
	virtual bool Marshalling(Parcel &data) const override;
	static Picture *Unmarshalling(Parcel &data);
	static Picture *Unmarshalling(Parcel &data, PIXEL_MAP_ERR &error);
private:
	std::shared_ptr<PixelMap> mainPixelMap_;
	std::map<AuxiliaryPictureType, std::shared_ptr<AuxiliaryPicture>> auxiliaryPictures_;
};
}
}

#endif //INTERFACES_INNERKITS_INCLUDE_PICTURE_H_