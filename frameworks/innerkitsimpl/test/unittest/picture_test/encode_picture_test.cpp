#include <gtest/gtest.h>
#include <fcntl.h>
#include "media_errors.h"
#include "image_packer.h"
#include "picture.h"
#include "auxiliary_picture.h"
#include "image/abs_image_encoder.h"
#include "plugin_server.h"

using namespace OHOS::Media;
using namespace testing::ext;
using namespace OHOS::ImagePlugin;
using namespace OHOS::MultimediaPlugin;

namespace OHOS {
namespace Multimedia {
static const std::string IMAGE_INPUT_JPEG_PATH = "/data/local/tmp/image/test_picture.jpg";

class PictureTest : public testing::Test {
public:
    PictureTest() {}
    ~PictureTest() {}
};

/**
 * @tc.name: StartPacking001
 * @tc.desc: test StartPacking
 * @tc.type: FUNC
 */
HWTEST_F(PictureTest, EncodePicture001, TestSize.Level3)
{
    GTEST_LOG_(INFO) << "PictureTest: EncodePicture001 start";

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(IMAGE_INPUT_JPEG_PATH.c_str(), opts, errorCode);
    ASSERT_EQ(errorCode, OHOS::Media::SUCCESS);
    ASSERT_NE(imageSource, nullptr);
    
    DecodeOptions dstOpts;
    std::unique_ptr<PixelMap> pixelMap = imageSource->CreatePixelMapEx(0, dstOpts, errorCode);
    ASSERT_EQ(errorCode, OHOS::Media::SUCCESS);
    ASSERT_NE(pixelMap, nullptr);

    ASSERT_NE(imageSource->mainPixelMap, nullptr);
    ASSERT_NE(imageSource->gainMapPixelMap, nullptr);
    
    std::unique_ptr<Picture> picture = Picture::Create(imageSource->mainPixelMap);
    ASSERT_NE(picture, nullptr);
    ASSERT_NE(picture->GetMainPixel(), nullptr);
    std::unique_ptr<AuxiliaryPicture> gainMapAux = AuxiliaryPicture::Create(imageSource->gainMapPixelMap,
                                                                            AuxiliaryPictureType::GAIN_MAP);
    std::shared_ptr<AuxiliaryPicture> spGainMapAux = std::move(gainMapAux);
    picture->SetAuxiliaryPicture(AuxiliaryPictureType::GAIN_MAP, spGainMapAux);
    ASSERT_NE(picture->GetAuxiliaryPicture(AuxiliaryPictureType::GAIN_MAP), nullptr);

    std::string outJpeg = "/data/local/tmp/image/encoded_picture.jpg";
    ImagePacker pack;
    const int fd = open(outJpeg.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ASSERT_NE(fd, -1);
    PackOption option;
    option.format = "image/jpeg";
    errorCode = pack.StartPacking(fd, option);
    ASSERT_EQ(errorCode, OHOS::Media::SUCCESS);
    errorCode = pack.AddPicture(*picture);
    ASSERT_EQ(errorCode, OHOS::Media::SUCCESS);
    pack.FinalizePacking();
    
    close(fd);
    ASSERT_EQ(errorCode, OHOS::Media::SUCCESS);
    GTEST_LOG_(INFO) << "PictureTest: EncodePicture001 end";
}
} // namespace Multimedia
} // namespace OHOS