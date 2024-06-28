/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "heifimpl_fuzzer.h"
#define private public
#define protected public

#include <cstdint>
#include <string>
#include <unistd.h>
#include <fcntl.h>

#include "include/core/SkStream.h"
#include "image_source.h"
#include "buffer_source_stream.h"
#include "ext_decoder.h"
#include "HeifDecoderImpl.h"
#include "heif_image.h"
#include "heif_parser.h"
#include "heif_stream.h"
#include "heif_utils.h"
#include "box/basic_box.h"
#include "box/heif_box.h"
#include "box/item_data_box.h"
#include "box/item_info_box.h"
#include "box/item_property_basic_box.h"
#include "box/item_property_aux_box.h"
#include "box/item_property_transform_box.h"
#include "box/item_property_box.h"
#include "box/item_property_color_box.h"
#include "box/item_property_display_box.h"
#include "box/item_property_hvcc_box.h"
#include "box/item_ref_box.h"

namespace OHOS {
namespace Media {
    struct HeifStreamMock : public HeifStream {
    explicit HeifStreamMock(SkStream* stream) : fStream(stream) {}
    
    ~HeifStreamMock() override{}
    
    size_t read(void* buffer, size_t size) override
    {
        return fStream->read(buffer, size);
    }

    bool rewind() override
    {
        return fStream->rewind();
    }

    bool seek(size_t position) override
    {
        return fStream->seek(position);
    }

    bool hasLength() const override
    {
        return fStream->hasLength();
    }

    size_t getLength() const override
    {
        return fStream->getLength();
    }

    bool hasPosition() const override
    {
        return fStream->hasPosition();
    }

    size_t getPosition() const override
    {
        return fStream->getPosition();
    }

    private:
    std::unique_ptr<SkStream> fStream;
    };

void HeifDecodeImplTest001(ImagePlugin::HeifDecoderImpl *heifdecoderimpl, HeifFrameInfo *frameInfo)
{
    size_t frameCount = 0;
    enum SkHeifColorFormat heifColor = kHeifColorFormat_RGB565;
    int frameIndex = 0;
    uint8_t dstBuffer = 0;
    void *context = nullptr;
    std::string errMsg = "error";
    std::vector<uint8_t> v1(1, 1);
    const std::shared_ptr<ImagePlugin::HeifImage> const_image = nullptr;
    uint32_t uint32data = 0;
    sptr<OHOS::SurfaceBuffer> hwBuffer = nullptr;
    bool flag = false;
    const uint32_t const_uint32data = 0;
    std::shared_ptr<ImagePlugin::HeifImage> image = nullptr;

    heifdecoderimpl->getSequenceInfo(frameInfo, frameCount);
    heifdecoderimpl->setOutputColor(heifColor);
    heifdecoderimpl->decode(frameInfo);
    heifdecoderimpl->decodeSequence(frameIndex, frameInfo);
    heifdecoderimpl->setDstBuffer(&dstBuffer, frameCount, context);
    heifdecoderimpl->getScanline(&dstBuffer);
    heifdecoderimpl->skipScanlines(frameIndex);
    heifdecoderimpl->getImageInfo(frameInfo);
    heifdecoderimpl->decodeGainmap();
    heifdecoderimpl->setGainmapDstBuffer(&dstBuffer, frameCount);
    heifdecoderimpl->getGainmapInfo(frameInfo);
    heifdecoderimpl->getTmapInfo(frameInfo);
    heifdecoderimpl->getHdrType();
    heifdecoderimpl->getVividMetadata(v1, v1, v1);
    heifdecoderimpl->getISOMetadata(v1);
    heifdecoderimpl->getErrMsg(errMsg);
    heifdecoderimpl->Reinit(frameInfo);
    heifdecoderimpl->InitFrameInfo(frameInfo, const_image);
    heifdecoderimpl->GetTileSize(const_image, uint32data, uint32data);
    heifdecoderimpl->SetRowColNum();
    heifdecoderimpl->ProcessChunkHead(&dstBuffer, frameCount);
    heifdecoderimpl->DecodeGrids(hwBuffer, flag);
    heifdecoderimpl->DecodeIdenImage(hwBuffer, flag);
    heifdecoderimpl->DecodeSingleImage(image, hwBuffer, flag);
    heifdecoderimpl->ConvertHwBufferPixelFormat(hwBuffer, flag);
    heifdecoderimpl->IsDirectYUVDecode();
    heifdecoderimpl->SetColorSpaceInfo(frameInfo, const_image);
    heifdecoderimpl->SetHardwareDecodeErrMsg(const_uint32data, const_uint32data);
}

void HeifParserTest001(ImagePlugin::HeifParser &heifparse, const std::shared_ptr<ImagePlugin::HeifInputStream> &stream)
{
    std::shared_ptr<ImagePlugin::HeifParser> *out = nullptr;
    const void *data = nullptr;
    size_t size = 0;
    bool flag = false;
    ImagePlugin::HeifStreamWriter writer;
    ImagePlugin::heif_item_id itemId = 0xffff;
    std::vector<uint8_t> v1(1, 1);
    enum ImagePlugin::heif_header_option option = ImagePlugin::heif_no_header;
    std::vector<std::shared_ptr<ImagePlugin::HeifImage>> v2(1, nullptr);
    std::vector<ImagePlugin::heif_item_id> itemIdList(1, 0xffff);
    const uint8_t intdata8 = 0;
    uint32_t uint32data = 0;
    const std::shared_ptr<ImagePlugin::HeifImage> master_image = nullptr;
    int64_t int64data = 0;
    ImagePlugin::HeifStreamReader reader(stream, int64data, size);
    std::vector<std::shared_ptr<ImagePlugin::HeifBox>> v3(1, nullptr);
    std::shared_ptr<ImagePlugin::HeifImage> image = nullptr;
    const ImagePlugin::HeifIrefBox::Reference ref;

    heifparse.MakeFromStream(stream, out);
    heifparse.MakeFromMemory(data, size, flag, out);
    heifparse.Write(writer);
    heifparse.GetImage(itemId);
    heifparse.GetPrimaryImage();
    heifparse.GetGainmapImage();
    heifparse.GetTmapImage();
    heifparse.GetItemType(itemId);
    heifparse.GetItemData(itemId, &v1, option);
    heifparse.GetTileImages(itemId, v2);
    heifparse.GetIdenImage(itemId, v2);
    heifparse.GetAllItemId(itemIdList);
    heifparse.SetExifMetadata(master_image, &intdata8, uint32data);
    heifparse.UpdateExifMetadata(master_image, &intdata8, uint32data, itemId);
    heifparse.AssembleBoxes(reader);
    heifparse.GetPrimaryItemId();
    heifparse.HasItemId(itemId);
    heifparse.GetItemContentType(itemId);
    heifparse.GetItemUriType(itemId);
    heifparse.GetInfeBox(itemId);
    heifparse.GetAllProperties(itemId, v3);
    heifparse.ExtractImageProperties(image);
    heifparse.ExtractDerivedImageProperties();
    heifparse.ExtractThumbnailImage(image, ref);
    heifparse.ExtractAuxImage(image, ref);
}

void HeifParserTest002(ImagePlugin::HeifParser &heifparse, const std::shared_ptr<ImagePlugin::HeifInputStream> &stream)
{
    std::shared_ptr<ImagePlugin::HeifImage> image = nullptr;
    const std::shared_ptr<ImagePlugin::HeifImage> master_image;
    uint8_t intdata8 = 0;
    uint32_t uint32data = 0;
    bool flag = false;
    ImagePlugin::heif_item_id itemId = 0xffff;
    const char *itemType = nullptr;
    const std::vector<uint8_t> v1(1, 1);
    const ImagePlugin::HvccConfig config = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const std::shared_ptr<ImagePlugin::HeifBox> heifbox = nullptr;
    const std::vector<ImagePlugin::heif_item_id> v2(1, 0xffff);
    const std::string type1 = "abc";
    const std::shared_ptr<const ImagePlugin::HeifColorProfile> Colorptr = nullptr;
    const ImagePlugin::heif_item_id id = 0xffff;
    const uint8_t constdata = 0;

    heifparse.ExtractNonMasterImages();
    heifparse.ExtractMetadata(const std::vector<heif_item_id> &allItemIds);
    heifparse.GetNextItemId();
    heifparse.AddItem(itemType, flag);
    heifparse.AddHvccProperty(itemId);
    heifparse.AppendHvccNalData(itemId, v1);
    heifparse.SetHvccConfig(itemId, config);
    heifparse.AddIspeProperty(itemId, uint32data, uint32data);
    heifparse.AddPixiProperty(itemId, intdata8, intdata8, intdata8);
    heifparse.AddProperty(itemId, heifbox, flag);
    heifparse.AppendIlocData(itemId, v1, intdata8);
    heifparse.SetPrimaryItemId(itemId);
    heifparse.AddReference(itemId, uint32data, v2);
    heifparse.SetAuxcProperty(itemId, type1);
    heifparse.SetColorProfile(itemId, Colorptr);
    heifparse.CheckExtentData();
    heifparse.SetPrimaryImage(master_image);
    heifparse.GetExifHeaderOffset(&constdata, uint32data);
    heifparse.SetMetadata(master_image, v1, itemType, itemType);
    heifparse.GetConstructMethod(id);
    heifparse.ExtractGainmap(v2);
    heifparse.ExtractDisplayData(image, itemId);
    heifparse.ExtractIT35Metadata(id);
    heifparse.ExtractISOMetadata(id);
    heifparse.ExtractGainmapImage(id);
}

void HeifImageTest001(std::shared_ptr<ImagePlugin::HeifImage> &heifimage)
{
    bool flag = false;
    uint32_t int32data = 0;
    int degrees = 0;
    enum ImagePlugin::HeifTransformMirrorDirection direction = ImagePlugin::HeifTransformMirrorDirection::VERTICAL;
    ImagePlugin::HeifColorFormat defaultColorFormat_ = ImagePlugin::HeifColorFormat::UNDEDEFINED;
    ImagePlugin::HeifPixelFormat defaultPixelFormat_ = ImagePlugin::HeifPixelFormat::UNDEDEFINED;
    ImagePlugin::heif_item_id itemId = 0xffff;
    const std::shared_ptr<ImagePlugin::HeifImage> const_img = nullptr;

    heifimage->GetItemId();
    heifimage->IsPrimaryImage();
    heifimage->SetPrimaryImage(flag);
    heifimage->GetOriginalWidth();
    heifimage->GetOriginalHeight();
    heifimage->SetOriginalSize(int32data, int32data);
    heifimage->GetRotateDegrees();
    heifimage->SetRotateDegrees(degrees);
    heifimage->GetMirrorDirection();
    heifimage->SetMirrorDirection(direction);
    heifimage->IsResolutionReverse();
    heifimage->GetWidth();
    heifimage->GetHeight();
    heifimage->GetLumaBitNum();
    heifimage->SetLumaBitNum(degrees);
    heifimage->GetChromaBitNum();
    heifimage->SetChromaBitNum(degrees);
    heifimage->GetDefaultColorFormat();
    heifimage->SetDefaultColorFormat(defaultColorFormat_);
    heifimage->GetDefaultPixelFormat();
    heifimage->SetDefaultPixelFormat(defaultPixelFormat_);
    heifimage->SetThumbnailImage(itemId);
    heifimage->AddThumbnailImage(const_img);
    heifimage->IsThumbnailImage();
    heifimage->GetThumbnailImages();
}

void HeifImageTest002(std::shared_ptr<ImagePlugin::HeifImage> &heifimage)
{
    ImagePlugin::heif_item_id itemId = 0xffff;
    std::shared_ptr<ImagePlugin::HeifImage> image = nullptr;
    std::string aux_type = "abc";
    std::shared_ptr<ImagePlugin::HeifMetadata> metadata = nullptr;
    const std::shared_ptr<const ImagePlugin::HeifColorProfile> const_profile = nullptr;
    std::vector<uint8_t> v1(1, 1);

    heifimage->IsAuxImage();
    heifimage->GetAuxImageType();
    heifimage->GetAuxImages();
    heifimage->SetAuxImage(itemId, aux_type);
    heifimage->AddAuxImage(image);
    heifimage->GetAllMetadata();
    heifimage->AddMetadata(metadata);
    heifimage->GetNclxColorProfile();
    heifimage->GetRawColorProfile();
    heifimage->SetColorProfile(const_profile);
    heifimage->SetGainmapMasterImage(itemId);
    heifimage->AddGainmapImage(image);
    heifimage->GetGainmapImage();
    heifimage->SetTmapBoxId(itemId);
    heifimage->SetStaticMetadata(v1, v1);
    heifimage->SetUWAInfo(v1);
    heifimage->SetISOMetadata(v1);
    heifimage->GetDisplayInfo();
    heifimage->GetLightInfo();
    heifimage->GetUWAInfo();
    heifimage->GetISOMetadata();
}

void HeifStreamTest001(std::shared_ptr<ImagePlugin::HeifBufferInPutStream> &heifbuffstream,
                       ImagePlugin::HeifStreamReader &heifstreamreader,
                       ImagePlugin::HeifStreamWriter &heifstreamwriter)
{
    size_t size = 0;
    int64_t int64data = 0;
    void *data = nullptr;
    uint8_t uint8data = 0;
    bool flag = false;
    uint16_t uint16data = 0;
    uint32_t uint32data = 0;
    uint64_t uint64data = 0;
    int sizenum = 0;
    const std::string const_ptr = "abcd";
    const std::vector<uint8_t> v1(1, 1);
    enum ImagePlugin::heif_error errMsg = heif_error_ok;

    int64data = heifbuffstream->Tell();
    heifbuffstream->CheckSize(size, int64data);
    heifbuffstream->Read(data, size);
    heifbuffstream->Seek(int64data);

    heifstreamreader.Read8();
    heifstreamreader.Read16();
    heifstreamreader.Read32();
    heifstreamreader.Read64();
    heifstreamreader.ReadData(&uint8data, size);
    heifstreamreader.ReadString();
    flag = heifstreamreader.CheckSize(size);
    heifstreamreader.SkipEnd();
    flag = heifstreamreader.IsAtEnd();
    flag = heifstreamreader.HasError();
    heifstreamreader.SetError(flag);
    errMsg = heifstreamreader.GetError();
    heifstreamreader.GetStream();
    size = heifstreamreader.GetRemainSize();

    heifstreamwriter.CheckSize(size);
    heifstreamwriter.Write8(uint8data);
    heifstreamwriter.Write16(uint16data);
    heifstreamwriter.Write32(uint32data);
    heifstreamwriter.Write64(uint64data);
    heifstreamwriter.Write(sizenum, uint64data);
    heifstreamwriter.Write(const_ptr);
    heifstreamwriter.Write(v1);
    heifstreamwriter.Skip(size);
    heifstreamwriter.Insert(size);
    heifstreamwriter.GetDataSize();
    heifstreamwriter.GetPos();
    heifstreamwriter.SetPos(size);
    heifstreamwriter.SetPositionToEnd();
    heifstreamwriter.GetData();
}

void itemInfoBoxTest001(ImagePlugin::HeifIinfBox &heifIinfbox,
                        std::shared_ptr<ImagePlugin::HeifInfeBox> &heifinfebox,
                        ImagePlugin::HeifPtimBox &heifptimbox, ImagePlugin::HeifStreamReader&reader,
                        ImagePlugin::HeifStreamWriter &writer)
{
    ImagePlugin::heif_item_id itemId = 0xffff;
    bool flag = false;
    cosnt std::string const_type = "abc";
    
    heifIinfbox.InferFullBoxVersion();
    heifIinfbox.Write(writer);
    heifIinfbox.ParseContent(reader);

    heifinfebox->IsHidden();
    heifinfebox->SetHidden(flag);
    heifinfebox->GetItemId();
    heifinfebox->SetItemId(itemId);
    heifinfebox->GetItemType();

    heifinfebox->SetItemType(const_type);
    heifinfebox->SetItemName(const_type);
    heifinfebox->GetContentType();
    heifinfebox->GetContentEncoding();
    heifinfebox->SetContentType(const_type);
    heifinfebox->SetContentEncoding(const_type);
    heifinfebox->InferFullBoxVersion();
    heifinfebox->Write(writer);
    heifinfebox->GetItemUriType();
    heifinfebox->ParseContent(reader);
    
    heifptimbox.GetItemId();
    heifptimbox.SetItemId(itemId);
    heifptimbox.InferFullBoxVersion();
    heifptimbox.Write(writer);
    heifptimbox.ParseContent(reader);
}

void HeifBoxTest001(std::shared_ptr<ImagePlugin::HeifBox> &heifbox,
                    std::shared_ptr<ImagePlugin::HeifFullBox> &heiffullbox,
                    ImagePlugin::HeifStreamReader &reader, ImagePlugin::HeifStreamWriter &writer)
{
    std::shared_ptr<ImagePlugin::HeifBox> *result = nullptr;
    uint32_t type = 0;
    const ImagePlugin::HeifBox box;
    const std::shared_ptr<ImagePlugin::HeifBox> const_box = nullptr;
    size_t size = 0;
    uint8_t version = 0;

    heifbox->MakeFromReader(reader, result);
    heifbox->heif_error Write(writer);
    heifbox->GetBoxSize();
    heifbox->GetHeaderSize();
    heifbox->GetBoxType();
    heifbox->SetBoxType(type);
    heifbox->GetBoxUuidType();
    heifbox->ParseHeader(reader);
    heifbox->SetHeaderInfo(box);
    heifbox->InferHeaderSize();
    heifbox->InferFullBoxVersion();
    heifbox->InferAllFullBoxVersion();
    heifbox->GetChildren();
    heifbox->AddChild(const_box);
    heifbox->ParseContent(reader);
    heifbox->ReadChildren(reader);
    heifbox->WriteChildren(writer);
    heifbox->ReserveHeader(writer);
    heifbox->WriteCalculatedHeader(writer, size);
    heifbox->heif_error WriteHeader(writer, size);

    heiffullbox->InferFullBoxVersion();
    heiffullbox->ParseFullHeader(reader);
    heiffullbox->GetVersion();
    heiffullbox->SetVersion(version);
    heiffullbox->GetFlags();
    heiffullbox->SetFlags(type);
    heiffullbox->ReserveHeader(writer);
    heiffullbox->WriteHeader(writer, size);
}

void BasicBoxTest001(ImagePlugin::HeifFtypBox *heifftypbox, ImagePlugin::HeifMetaBox *heifmetabox,
                     ImagePlugin::HeifHdlrBox *heifhdlrbox, ImagePlugin::HeifStreamReader &reader,
                     ImagePlugin::HeifStreamWriter &writer)
{
    heifftypbox->Write(writer);
    heifftypbox->ParseContent(reader);

    heifmetabox->ParseContent(reader);

    heifhdlrbox->GetHandlerType();
    heifhdlrbox->Write(writer);
    heifhdlrbox->ParseContent(reader);
}

void ItemDataBoxTest001(ImagePlugin::HeifIlocBox *heifilocbox, ImagePlugin::HeifIdatBox *heifidatbox,
                        ImagePlugin::HeifStreamReader &reader, ImagePlugin::HeifStreamWriter &writer)
{
    const ImagePlugin::HeifIlocBox::Item const_item;
    const std::shared_ptr<ImagePlugin::HeifInputStream> const_stream = nullptr;
    const std::shared_ptr<ImagePlugin::HeifIdatBox> const_idat = nullptr;
    std::vector<uint8_t> dest(1, 1);
    ImagePlugin::heif_item_id itemId = 0xffff;
    const std::vector<uint8_t> const_data(1, 1);
    uint8_t uint8data = 0;
    ImagePlugin::HeifIlocBox::Item item;
    int indexSize = 0;
    uint64_t uint64data = 0;
    
    heifilocbox->GetItems();
    heifilocbox->ReadData(const_item, const_stream, const_idat, &dest);
    heifilocbox->AppendData(itemId, const_data, uint8data);
    heifilocbox->UpdateData(itemId, const_data, uint8data);
    heifilocbox->InferFullBoxVersion();
    heifilocbox->Write(writer);
    heifilocbox->WriteMdatBox(writer);
    heifilocbox->ReadToExtentData(item, const_stream, const_idat);
    heifilocbox->ParseContent(reader);
    heifilocbox->ParseExtents(item, reader, indexSize, indexSize, indexSize);
    heifilocbox->PackIlocHeader(writer);

    heifidatbox->ReadData(const_stream, uint64data, uint64data, dest);
    heifidatbox->AppendData(const_data);
    heifidatbox->Write(writer);
    heifidatbox->ParseContent(reader);
}

void ItemRefBoxTest001(ImagePlugin::HeifIrefBox *heifirefbox, ImagePlugin::HeifStreamReader &reader,
                       ImagePlugin::HeifStreamWriter &writer)
{
    ImagePlugin::heif_item_id itemId = 0xffff;
    uint32_t uint32data = 0;
    const std::vector<ImagePlugin::heif_item_id> v1(1, 0xffff);
    ImagePlugin::HeifIrefBox::Reference ref;
    
    heifirefbox->HasReferences(itemId);
    heifirefbox->GetReferences(itemId, uint32data);
    heifirefbox->GetReferencesFrom(itemId);
    heifirefbox->AddReferences(itemId, uint32data, v1);
    heifirefbox->ParseContent(reader);
    heifirefbox->Write(writer);
    heifirefbox->InferFullBoxVersion();
    heifirefbox->ParseItemRef(reader, ref);
}

void ItemPropertyBoxTest001(ImagePlugin::HeifIprpBox *heifiprpbox, ImagePlugin::HeifIpcoBox *heifipcobox,
                            ImagePlugin::HeifIpmaBox *heifipmabox, ImagePlugin::HeifStreamReader &reader,
                            ImagePlugin::HeifStreamWriter &writer)
{
    ImagePlugin::heif_item_id itemId = 0xffff;
    const std::shared_ptr<ImagePlugin::HeifIpmaBox> const_box_ptr = nullptr;
    std::vector<std::shared_ptr<ImagePlugin::HeifBox>> v1(1, nullptr);
    uint32_t uint32data = 0;
    ImagePlugin::PropertyAssociation assoc;
    const ImagePlugin::HeifIpmaBox const_box;

    heifiprpbox->ParseContent(reader);
    
    heifipcobox->GetProperties(itemId, const_box_ptr, v1);
    heifipcobox->GetProperty(itemId, const_box_ptr, uint32data);
    heifipcobox->ParseContent(reader);

    heifipmabox->GetProperties(itemId);
    heifipmabox->AddProperty(itemId, assoc);
    heifipmabox->InferFullBoxVersion();
    heifipmabox->Write(writer);
    heifipmabox->MergeImpaBoxes(const_box);
    heifipmabox->ParseContent(reader);
}

void ItemPropertyBasicBoxTest001(ImagePlugin::HeifIspeBox *heifispebox, ImagePlugin::HeifPixiBox *heifpixibox,
                                 ImagePlugin::HeifStreamReader &reader, ImagePlugin::HeifStreamWriter &writer)
{
    uint32_t uint32data = 0;
    int channel = 0;
    uint8_t uint8data = 0;
    
    heifispebox->GetWidth();
    heifispebox->GetHeight();
    heifispebox->SetDimension(uint32data, uint32data);
    heifispebox->Write(writer);
    heifispebox->ParseContent(reader);

    heifpixibox->GetChannelNum();
    heifpixibox->GetBitNum(channel);
    heifpixibox->AddBitNum(uint8data);
    heifpixibox->Write(writer);
    heifpixibox->ParseContent(reader);
}

void ItemPropertyAuxBoxTest001(ImagePlugin::HeifAuxcBox *heifauxcbox, ImagePlugin::HeifStreamReader &reader,
                               ImagePlugin::HeifStreamWriter &writer)
{
    const std::string const_type = "abc";

    heifauxcbox->GetAuxType();
    heifauxcbox->SetAuxType(const_type);
    heifauxcbox->GetAuxSubtypes();
    heifauxcbox->ParseContent(reader);
    heifauxcbox->Write(writer);
}

void ItemPropertyColorBoxTest001(const std::shared_ptr<const ImagePlugin::HeifRawColorProfile> &heifrawcolorprofile,
                                 const std::shared_ptr<const ImagePlugin::HeifNclxColorProfile> &heifnclxcolorprofile,
                                 ImagePlugin::HeifColrBox *heifcolrbox, ImagePlugin::HeifStreamReader &reader,
                                 ImagePlugin::HeifStreamWriter &writer)
{
    const std::shared_ptr<const HeifColorProfile> const_prof = nullptr;
    
    heifrawcolorprofile->GetProfileType();
    heifrawcolorprofile->GetData();
    heifrawcolorprofile->Write(writer);
    
    heifnclxcolorprofile->GetProfileType();
    heifnclxcolorprofile->Write(writer);
    heifnclxcolorprofile->GetColorPrimaries();
    heifnclxcolorprofile->GetTransferCharacteristics();
    heifnclxcolorprofile->GetMatrixCoefficients();
    heifnclxcolorprofile->GetFullRangeFlag();

    HeifColrBox->GetColorProfileType();
    HeifColrBox->GetColorProfile();
    HeifColrBox->SetColorProfile(const_prof);
    HeifColrBox->Write(writer);
    HeifColrBox->ParseContent(reader);
}

void ItemPropertyDisplayBoxTest001(ImagePlugin::HeifMdcvBox *heifmdcvbox, ImagePlugin::HeifClliBox *heifcllibox,
                                   ImagePlugin::HeifStreamReader &reader,  ImagePlugin::HeifStreamWriter &writer)
{
    ImagePlugin::DisplayColourVolume colourVolume;
    ImagePlugin::ContentLightLevelInfo lightLevel;

    heifmdcvbox->GetColourVolume();
    heifmdcvbox->SetColourVolume(colourVolume);
    heifmdcvbox->ParseContent(reader);
    heifmdcvbox->Write(writer);

    heifcllibox->GetLightLevel();
    heifcllibox->SetLightLevel(lightLevel);
    heifcllibox->ParseContent(reader);
    heifcllibox->Write(writer);
}

void ItemPropertyHvccBoxTest001(ImagePlugin::HeifHvccBox *heifhvccbox, ImagePlugin::HeifStreamReader &reader,
                                ImagePlugin::HeifStreamWriter &writer)
{
    std::vector<uint8_t> v1(1, 1);
    const ImagePlugin::HvccConfig config = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    const std::vector<uint8_t> v2(1, 1);
    std::vector<std::vector<uint8_t>> v3(1, v1);
    
    heifhvccbox->GetHeaders(v1);
    heifhvccbox->SetConfig(config);
    heifhvccbox->GetConfig();
    heifhvccbox->AppendNalData(v2);
    heifhvccbox->Write(writer);
    heifhvccbox->ParseContent(reader);
    heifhvccbox->ParseNalUnitArray(reader, v3);
}

void ItemPropertyTransformBoxTest001(ImagePlugin::HeifIrotBox *heifirotbox,
                                     ImagePlugin::HeifImirBox *heifimirbox,
                                     ImagePlugin::HeifStreamReader &reader, ImagePlugin::HeifStreamWriter &writer)
{
    int rot = 0;
    ImagePlugin::HeifTransformMirrorDirection dir = ImagePlugin::HeifTransformMirrorDirection::VERTICAL;

    heifirotbox->GetRotDegree();
    heifirotbox->SetRotDegree(rot);
    heifirotbox->ParseContent(reader);
    heifirotbox->Write(writer);

    heifimirbox->GetDirection();
    heifimirbox->SetDirection();
    heifimirbox->ParseContent(reader);
    heifimirbox->Write(writer);
}

void HeifImplFuzzTest002(const uint8_t *data, size_t size)
{
    bool flag;
    const char *itemType = "abc";
    int64_t start = 0;
    auto heifbuffstream = std::make_shared<ImagePlugin::HeifBufferInPutStream>(data, size, flag);
    auto heifparse = ImagePlugin::HeifParser(heifbuffstream);
    std::shared_ptr<ImagePlugin::HeifInfeBox> heifinfebox = heifparse.AddItem(itemType, flag);
    std::shared_ptr<ImagePlugin::HeifFullBox> heiffullbox = heifinfebox;
    std::shared_ptr<ImagePlugin::HeifBox> heifbox = heiffullbox;
    ImagePlugin::HeifBox *temp_heifbox = heifbox.get();
    void *obj_heifbox = dynamic_cast<void *>(temp_heifbox);
    ImagePlugin::HeifFullBox *temp_heiffullbox = heiffullbox.get();
    void *obj_heiffullbox = dynamic_cast<void *>(temp_heiffullbox);
    auto heifstreamreader = ImagePlugin::HeifStreamReader(heifbuffstream, start, size);
    auto heifstreamwriter = ImagePlugin::HeifStreamWriter();

    //item_property_box.cpp create/init/fuzztest
    auto heifiprpbox = static_cast<ImagePlugin::HeifIprpBox *>(obj_heifbox);
    auto heifipcobox = static_cast<ImagePlugin::HeifIpcoBox *>(obj_heifbox);
    auto heifipmabox = static_cast<ImagePlugin::HeifIpmaBox *>(obj_heiffullbox);
    ItemPropertyBoxTest001(heifiprpbox, heifipcobox, heifipmabox, heifstreamreader, heifstreamwriter);

    //item_property_basic_box.cpp create/init/fuzztest
    auto heifispebox = static_cast<ImagePlugin::HeifIspeBox *>(obj_heiffullbox);
    auto heifpixibox = static_cast<ImagePlugin::HeifPixiBox *>(obj_heiffullbox);
    ItemPropertyBasicBoxTest001(heifispebox, heifpixibox, heifstreamreader, heifstreamwriter);

    //item_property_aux_box.cpp create/init/fuzztest
    auto heifauxcbox = static_cast<ImagePlugin::HeifAuxcBox *>(obj_heiffullbox);
    ItemPropertyAuxBoxTest001(heifauxcbox, heifstreamreader, heifstreamwriter);

    //item_property_color_box.cpp create/init/fuzztest
    std::shared_ptr<ImagePlugin::HeifImage> heifimage = heifparse.GetGainmapImage();
    const std::shared_ptr<const ImagePlugin::HeifRawColorProfile> heifrawcolorprofile = heifimage->GetRawColorProfile();
    const std::shared_ptr<const ImagePlugin::HeifNclxColorProfile> heifnclxcolorprofile =
        heifimage->GetNclxColorProfile();
    auto heifcolrbox = static_cast<ImagePlugin::HeifColrBox *>(obj_heifbox);
    ItemPropertyColorBoxTest001(heifrawcolorprofile, heifnclxcolorprofile, heifcolrbox,
                                heifstreamreader, heifstreamwriter);

    //item_property_display_box.cpp create/init/fuzztest
    auto heifmdcvbox = static_cast<ImagePlugin::HeifMdcvBox *>(obj_heifbox);
    auto heifcllibox = static_cast<ImagePlugin::HeifClliBox *>(obj_heifbox);
    ItemPropertyDisplayBoxTest001(heifmdcvbox, heifcllibox, heifstreamreader, heifstreamwriter);

    //item_property_hvcc_box.cpp create/init/fuzztest
    auto heifhvccbox = static_cast<ImagePlugin::HeifHvccBox *>(obj_heifbox);
    ItemPropertyHvccBoxTest001(heifhvccbox, heifstreamreader, heifstreamwriter);

    //item_property_transform_box.cpp create/init/fuzztest
    auto heifirotbox = static_cast<ImagePlugin::HeifIrotBox *>(obj_heifbox);
    auto heifimirbox = static_cast<ImagePlugin::HeifImirBox *>(obj_heifbox);
    ItemPropertyTransformBoxTest001(heifirotbox, heifimirbox, heifstreamreader, heifstreamwriter);
}

void HeifImplFuzzTest001(const uint8_t* data, size_t size)
{
    //HeifDecodeImpl.cpp create/init/fuzztest
    auto heifdecoder = CreateHeifDecoderImpl();
    void *obj = dynamic_cast<void*>(heifdecoder);
    ImagePlugin::HeifDecoderImpl *heifdecoderimpl = static_cast<ImagePlugin::HeifDecoderImpl*>(obj);
    std::unique_ptr<ImagePlugin::InputDataStream> stream_ = BufferSourceStream::CreateSourceStream(data, size);
    auto skStream = std::make_shared<ImagePlugin::ExtStream>(stream_.release());
    HeifFrameInfo heifInfo;
    heifdecoderimpl->init(new HeifStreamMock(skStream.release()), &heifInfo);
    HeifDecodeImplTest001(heifdecoderimpl, &heifInfo);

    //heif_parse.cpp create/init/fuzztest
    bool needCopy;
    auto heifbuffstream = std::make_shared<ImagePlugin::HeifBufferInPutStream>(data, size, needCopy);
    auto heifparse = ImagePlugin::HeifParser(heifbuffstream);
    HeifParserTest001(heifparse, heifbuffstream);
    HeifParserTest001(heifparse, heifbuffstream);

    //heif_image.cpp create/init/fuzztest
    std::shared_ptr<ImagePlugin::HeifImage> heifimage = heifparse.GetGainmapImage();
    HeifImageTest001(heifimage);
    HeifImageTest002(heifimage);

    //heif_stream.cpp create/init/fuzztest
    int64_t start = 0;
    auto heifstreamreader = ImagePlugin::HeifStreamReader(heifbuffstream, start, size);
    auto heifstreamwriter = ImagePlugin::HeifStreamWriter();
    HeifStreamTest001(heifbuffstream, heifstreamreader, heifstreamwriter);

    //heif_utils.cpp create/init/fuzztest
    uint32_t uint32data = 0;
    ImagePlugin::code_to_fourcc(uint32data);

    //item_info_box.cpp create/init/fuzztest
    const char* itemType = "abc";
    bool hidden = false;
    std::shared_ptr<ImagePlugin::HeifInfeBox> heifinfebox = heifparse.AddItem(itemType, hidden);
    auto heifIinfbox = ImagePlugin::HeifIinfBox();
    auto heifptimbox = ImagePlugin::HeifPtimBox();
    itemInfoBoxTest001(heifIinfbox, heifinfebox, heifptimbox, heifstreamreader, heifstreamwriter);

    //heif_box.cpp create/init/fuzztest
    std::shared_ptr<ImagePlugin::HeifFullBox> heiffullbox = heifinfebox;
    std::shared_ptr<ImagePlugin::HeifBox> heifbox = heiffullbox;
    HeifBoxTest001(heifbox, heiffullbox, heifstreamreader, heifstreamwriter);

    //basic_box.cpp create/init/fuzztest
    ImagePlugin::HeifBox *temp_heifbox = heifbox.get();
    void *obj_heifbox = dynamic_cast<void *>(temp_heifbox);
    auto heifftypbox = static_cast<ImagePlugin::HeifFtypBox *>(obj_heifbox);
    ImagePlugin::HeifFullBox *temp_heiffullbox = heiffullbox.get();
    void *obj_heiffullbox = dynamic_cast<void *>(temp_heiffullbox);
    auto heifmetabox = static_cast<ImagePlugin::HeifMetaBox *>(obj_heiffullbox);
    auto heifhdlrbox = static_cast<ImagePlugin::HeifHdlrBox *>(obj_heiffullbox);
    BasicBoxTest001(heifftypbox, heifmetabox, heifhdlrbox, heifstreamreader, heifstreamwriter);

    //item_data_box.cpp create/init/fuzztest
    auto heifilocbox = static_cast<ImagePlugin::HeifIlocBox *>(obj_heiffullbox);
    auto heifidatbox = static_cast<ImagePlugin::HeifIdatBox *>(obj_heifbox);
    ItemDataBoxTest001(heifilocbox, heifidatbox, heifstreamreader, heifstreamwriter);

    //item_ref_box.cpp create/init/fuzztest
    auto heifirefbox = static_cast<ImagePlugin::HeifIrefBox *>(obj_heiffullbox);
    ItemRefBoxTest001(heifirefbox, heifstreamreader, heifstreamwriter);
}
} //namespace media
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Media::HeifImplFuzzTest001(data, size);
    OHOS::Media::HeifImplFuzzTest002(data, size);
    return 0;
}