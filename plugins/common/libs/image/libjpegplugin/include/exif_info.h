/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef EXIF_INFO_H
#define EXIF_INFO_H

#include <map>
#include <string>
#include <vector>

#include <libexif/exif-data.h>

#include "exif_maker_note.h"
#include "hilog/log.h"
#include "log_tags.h"

namespace OHOS {
namespace ImagePlugin {
struct DirectoryEntry {
    ExifTag tag;
    ExifFormat format;
    int32_t dataCounts;   // Counts of this format data
    uint32_t valueOffset; // Offset of Directory Entry data area from JPEG File
    uint32_t valueLength; // Length of Directory Entry data area
    ExifIfd ifd;          // IFD of this Entry is in
};

/*
 * Class responsible for storing and parsing EXIF information from a JPEG blob
 */
class EXIFInfo {
public:
    EXIFInfo();
    ~EXIFInfo();
    /*
     * Parsing function for an entire JPEG image buffer.
     * PARAM 'data': A pointer to a JPEG image.
     * PARAM 'length': The length of the JPEG image.
     * RETURN:  PARSE_EXIF_SUCCESS (0) on success with 'result' filled out
     *          error code otherwise, as defined by the PARSE_EXIF_ERROR_* macros
     */
    int ParseExifData(const unsigned char *buf, unsigned len);
    int ParseExifData(const std::string &data);
    uint32_t ModifyExifData(const ExifTag &tag, const std::string &value, const std::string &path);
    uint32_t ModifyExifData(const ExifTag &tag, const std::string &value, const int fd);
    uint32_t ModifyExifData(const ExifTag &tag, const std::string &value, unsigned char *data, uint32_t size);
    uint32_t GetFilterArea(const uint8_t *buf,
                              const uint32_t &bufSize,
                              const int &privacyType,
                              std::vector<std::pair<uint32_t, uint32_t>> &ranges);
    bool IsExifDataParsed();
    uint32_t GetExifData(const std::string name, std::string &value);
    uint32_t ModifyExifData(const std::string name, const std::string &value, const std::string &path);
    uint32_t ModifyExifData(const std::string name, const std::string &value, const int fd);
    uint32_t ModifyExifData(const std::string name, const std::string &value, unsigned char *data, uint32_t size);

public:
    static const std::string DEFAULT_EXIF_VALUE;
    std::string bitsPerSample_; // Number of bits in each pixel of an image.
    std::string orientation_;
    std::string imageLength_;   // Image length.
    std::string imageWidth_;    // mage width.
    std::string gpsLatitude_;
    std::string gpsLongitude_;
    std::string gpsLatitudeRef_;
    std::string gpsLongitudeRef_;
    std::string dateTimeOriginal_;  // Original date and time.
    std::string exposureTime_;
    std::string fNumber_;
    std::string isoSpeedRatings_;
    std::string sceneType_;
    std::string compressedBitsPerPixel_;
    std::string dateTime_;
    std::string gpsTimeStamp_;
    std::string gpsDateStamp_;
    std::string imageDescription_;
    std::string make_;
    std::string model_;
    std::string photoMode_;
    std::string sensitivityType_;
    std::string standardOutputSensitivity_;
    std::string recommendedExposureIndex_;
    std::string apertureValue_;
    std::string exposureBiasValue_;
    std::string meteringMode_;
    std::string lightSource_;
    std::string flash_;
    std::string focalLength_;
    std::string userComment_;
    std::string pixelXDimension_;
    std::string pixelYDimension_;
    std::string whiteBalance_;
    std::string focalLengthIn35mmFilm_;
    std::string hwMnoteCaptureMode_;
    std::string hwMnotePhysicalAperture_;
    std::string hwMnoteRollAngle_;
    std::string hwMnotePitchAngle_;
    std::string hwMnoteSceneFoodConf_;
    std::string hwMnoteSceneStageConf_;
    std::string hwMnoteSceneBlueSkyConf_;
    std::string hwMnoteSceneGreenPlantConf_;
    std::string hwMnoteSceneBeachConf_;
    std::string hwMnoteSceneSnowConf_;
    std::string hwMnoteSceneSunsetConf_;
    std::string hwMnoteSceneFlowersConf_;
    std::string hwMnoteSceneNightConf_;
    std::string hwMnoteSceneTextConf_;
    std::string hwMnoteFaceCount_;
    std::string hwMnoteFocusMode_;
    std::map<std::string, std::string> makerInfoTagValueMap;

private:
    void SetExifTagValues(const ExifTag &tag, const std::string &value);
    void SetExifTagValuesEx(const ExifTag &tag, const std::string &value);
    ExifEntry* InitExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag);
    ExifEntry* CreateExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len, ExifFormat format);
    unsigned long GetFileSize(FILE *fp);
    void ReleaseSource(unsigned char **ptrBuf, FILE **ptrFile);
    bool CreateExifData(unsigned char *buf, unsigned long length, ExifData **data, bool &isNewExifData);
    unsigned int GetOrginExifDataLength(const bool &isNewExifData, unsigned char *buf);
    ExifByteOrder GetExifByteOrder(const bool &isNewExifData, unsigned char *buf);
    bool CreateExifEntry(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool WriteExifDataToFile(ExifData *data, unsigned int orginExifDataLength, unsigned long fileLength,
        unsigned char *buf, FILE *fp);
    void UpdateCacheExifData(FILE *fp);
    bool CheckExifEntryValid(const ExifIfd &ifd, const ExifTag &tag);
    bool CheckExifEntryValidEx(const ExifIfd &ifd, const ExifTag &tag);
    void GetAreaFromExifEntries(const int &privacyType,
                                const std::vector<DirectoryEntry> &entryArray,
                                std::vector<std::pair<uint32_t, uint32_t>> &ranges);
    uint32_t CheckPathValid(const std::string &path);
    uint32_t CheckFdValid(const int fd);
    uint32_t CheckDataValid(const ExifTag &tag, const std::string &value,
                            std::string data, uint32_t size);
    uint32_t ReleaseDataBuffer(std::string tempBuf, ExifData *ptrExifData,
                               ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateBitsPerSampleExifEntry(ExifData *data, const std::string &value,
                               ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateCompressedBitsPerPixelExifEntry(ExifData *data, const std::string &value,
                                ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateDateTimeExifEntry(ExifData *data, const std::string &value,
                                  ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateDateTimeOriginalExifEntry(ExifData *data, const std::string &value,
                                  ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateExposureBiasValueExifEntry(ExifData *data, const std::string &value,
                                   ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateExposureTimeExifEntry(ExifData *data, const std::string &value,
                                  ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateFlashExifEntry(ExifData *data, const std::string &value,
                               ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateFNumberExifEntry(ExifData *data, const std::string &value,
                                 ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateFocalLengthExifEntry(ExifData *data, const std::string &value,
                                 ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateFocalLengthIn35MmExifEntry(ExifData *data, const std::string &value,
                                   ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateGpsDateStampExifEntry(ExifData *data, const std::string &value,
                                      ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateGpsLatitudeExifEntry(ExifData *data, const std::string &value,
                                     ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateGpsLatitudeRefExifEntry(ExifData *data, const std::string &value,
                                        ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateGpsLongitudeExifEntry(ExifData *data, const std::string &value,
                                      ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateGpsLongitudeRefExifEntry(ExifData *data, const std::string &value,
                                         ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateGpsTimeStampExifEntry(ExifData *data, const std::string &value,
                                      ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateImageDescriptionExifEntry(ExifData *data, const std::string &value,
                                      ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateImageLengthExifEntry(ExifData *data, const std::string &value,
                                     ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateImageWidthExifEntry(ExifData *data, const std::string &value,
                                    ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateIsoSpeedExifEntry(ExifData *data, const std::string &value,
                                  ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateIsoSpeedRatingsExifEntry(ExifData *data, const std::string &value,
                                         ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateLightSourceExifEntry(ExifData *data, const std::string &value,
                                     ExifByteOrder order,  ExifEntry **ptrEntry) ;
    bool CreateMakeExifEntry(ExifData *data, const std::string &value,
                              ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateMeteringModeExifEntry(ExifData *data, const std::string &value,
                              ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateModelExifEntry(ExifData *data, const std::string &value,
                               ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateOrientationExifEntry(ExifData *data, const std::string &value,
                                     ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreatePixelXDimensionExifEntry(ExifData *data, const std::string &value,
                                         ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreatePixelYDimensionExifEntry(ExifData *data, const std::string &value,
                                         ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateRecommendedExposureIndexExifEntry(ExifData *data, const std::string &value,
                                                  ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateSceneTypeExifEntry(ExifData *data, const std::string &value,
                                   ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateSensitivityTypeExifEntry(ExifData *data, const std::string &value,
                                         ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateStandardOutputSensitivityExifEntry(ExifData *data, const std::string &value,
                                                   ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateUserCommentExifEntry(ExifData *data, const std::string &value,
                                     ExifByteOrder order, ExifEntry **ptrEntry) ;
    bool CreateWhiteBalanceExifEntry(ExifData *data, const std::string &value,
                                      ExifByteOrder order, ExifEntry **ptrEntry) ;

private:
    ExifIfd imageFileDirectory_;
    ExifData* exifData_;
    bool isExifDataParsed_;
    std::map<ExifTag, std::string> exifTags_;
};

class ByteOrderedBuffer {
public:
    ByteOrderedBuffer(const uint8_t *fileBuf, uint32_t bufferLength);
    ~ByteOrderedBuffer();
    void GenerateDEArray();

public:
    ExifByteOrder byteOrder_ = EXIF_BYTE_ORDER_MOTOROLA;
    const uint8_t *buf_;
    uint32_t bufferLength_ = 0;
    uint32_t curPosition_ = 0;
    std::vector<DirectoryEntry> directoryEntryArray_;
    std::vector<uint32_t> handledIfdOffsets_;

private:
    void GetDataRangeFromIFD(const ExifIfd &ifd);
    void GetDataRangeFromDE(const ExifIfd &ifd, const int16_t &count);
    int32_t ReadInt32();
    uint32_t ReadUnsignedInt32();
    int16_t ReadShort();
    uint16_t ReadUnsignedShort();
    uint32_t Peek();
    bool SetDEDataByteCount(const uint16_t &tagNumber,
                            const uint16_t &dataFormat,
                            const int32_t &numberOfComponents,
                            uint32_t &count);
    bool IsValidTagNumber(const uint16_t &tagNumber);
    bool IsIFDhandled(const uint32_t &position);
    bool IsIFDPointerTag(const uint16_t &tagNumber);
    ExifIfd GetIFDOfIFDPointerTag(const uint16_t &tagNumber);
    ExifIfd GetNextIfdFromLinkList(const ExifIfd &ifd);
    void ParseIFDPointerTag(const ExifIfd &ifd, const uint16_t &dataFormat);
    uint32_t TransformTiffOffsetToFilePos(const uint32_t &offset);
};
} // namespace ImagePlugin
} // namespace OHOS
#endif // EXIF_INFO_H
