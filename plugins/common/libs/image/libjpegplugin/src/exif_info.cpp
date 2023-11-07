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

#include "exif_info.h"
#include <algorithm>
#include <cstdio>
#include <memory>
#include <unistd.h>
#include "media_errors.h"
#include "string_ex.h"
#include "securec.h"
#include "exif_maker_note.h"

namespace OHOS {
namespace ImagePlugin {
namespace {
    using namespace OHOS::HiviewDFX;
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "Exif" };
    static constexpr int PARSE_EXIF_SUCCESS = 0;
    static constexpr int PARSE_EXIF_DATA_ERROR = 10001;
    static constexpr int PARSE_EXIF_IFD_ERROR = 10002;
    static constexpr int BUFFER_POSITION_4 = 4;
    static constexpr int BUFFER_POSITION_5 = 5;
    static constexpr int BUFFER_POSITION_6 = 6;
    static constexpr int BUFFER_POSITION_7 = 7;
    static constexpr int BUFFER_POSITION_8 = 8;
    static constexpr int BUFFER_POSITION_9 = 9;
    static constexpr int BUFFER_POSITION_12 = 12;
    static constexpr int BUFFER_POSITION_13 = 13;
    static constexpr int LENGTH_OFFSET_2 = 2;
    static constexpr int BYTE_COUNTS_12 = 12;
    static constexpr int MOVE_OFFSET_8 = 8;
    static constexpr int MOVE_OFFSET_16 = 16;
    static constexpr int MOVE_OFFSET_24 = 24;
    static constexpr int CONSTANT_0 = 0;
    static constexpr int CONSTANT_1 = 1;
    static constexpr int CONSTANT_2 = 2;
    static constexpr int CONSTANT_3 = 3;
    static constexpr int CONSTANT_4 = 4;
    static constexpr unsigned long MAX_FILE_SIZE = 1000 * 1000 * 1000;
    static constexpr uint32_t ERROR_PARSE_EXIF_FAILED = 1;
    static constexpr uint32_t ERROR_NO_EXIF_TAGS = 2;
    static constexpr ExifTag TAG_SENSITIVITY_TYPE = static_cast<ExifTag>(0x8830);
    static constexpr ExifTag TAG_STANDARD_OUTPUT_SENSITIVITY = static_cast<ExifTag>(0x8831);
    static constexpr ExifTag TAG_RECOMMENDED_EXPOSURE_INDEX = static_cast<ExifTag>(0x8832);
    static constexpr size_t SIZE_ONE = 1;

    /* raw EXIF header data */
    static const unsigned char exifHeader[] = {
        0xff, 0xd8, 0xff, 0xe1
    };
    /* Offset of tiff begin from jpeg file begin */
    static constexpr uint32_t TIFF_OFFSET_FROM_FILE_BEGIN = 12;
    static constexpr int PERMISSION_GPS_TYPE = 1;

    static const struct TagEntry {
        /*! Tag ID. There may be duplicate tags when the same number is used for
         * different meanings in different IFDs. */
        ExifTag tag;
        const std::string name;
        const uint16_t number;
    } ExifTagTable[] = {
        {EXIF_TAG_GPS_VERSION_ID, "GPSVersionID", 0x0000},
        {EXIF_TAG_INTEROPERABILITY_INDEX, "InteroperabilityIndex", 0x0001},
        {EXIF_TAG_GPS_LATITUDE_REF, "GPSLatitudeRef", 0x0001},
        {EXIF_TAG_INTEROPERABILITY_VERSION, "InteroperabilityVersion", 0x0002},
        {EXIF_TAG_GPS_LATITUDE, "GPSLatitude", 0x0002},
        {EXIF_TAG_GPS_LONGITUDE_REF, "GPSLongitudeRef", 0x0003},
        {EXIF_TAG_GPS_LONGITUDE, "GPSLongitude", 0x0004},
        {EXIF_TAG_GPS_ALTITUDE_REF, "GPSAltitudeRef", 0x0005},
        {EXIF_TAG_GPS_ALTITUDE, "GPSAltitude", 0x0006},
        {EXIF_TAG_GPS_TIME_STAMP, "GPSTimeStamp", 0x0007},
        {EXIF_TAG_GPS_SATELLITES, "GPSSatellites", 0x0008},
        {EXIF_TAG_GPS_STATUS, "GPSStatus", 0x0009},
        {EXIF_TAG_GPS_MEASURE_MODE, "GPSMeasureMode", 0x000a},
        {EXIF_TAG_GPS_DOP, "GPSDOP", 0x000b},
        {EXIF_TAG_GPS_SPEED_REF, "GPSSpeedRef", 0x000c},
        {EXIF_TAG_GPS_SPEED, "GPSSpeed", 0x000d},
        {EXIF_TAG_GPS_TRACK_REF, "GPSTrackRef", 0x000e},
        {EXIF_TAG_GPS_TRACK, "GPSTrack", 0x000f},
        {EXIF_TAG_GPS_IMG_DIRECTION_REF, "GPSImgDirectionRef", 0x0010},
        {EXIF_TAG_GPS_IMG_DIRECTION, "GPSImgDirection", 0x0011},
        {EXIF_TAG_GPS_MAP_DATUM, "GPSMapDatum", 0x0012},
        {EXIF_TAG_GPS_DEST_LATITUDE_REF, "GPSDestLatitudeRef", 0x0013},
        {EXIF_TAG_GPS_DEST_LATITUDE, "GPSDestLatitude", 0x0014},
        {EXIF_TAG_GPS_DEST_LONGITUDE_REF, "GPSDestLongitudeRef", 0x0015},
        {EXIF_TAG_GPS_DEST_LONGITUDE, "GPSDestLongitude", 0x0016},
        {EXIF_TAG_GPS_DEST_BEARING_REF, "GPSDestBearingRef", 0x0017},
        {EXIF_TAG_GPS_DEST_BEARING, "GPSDestBearing", 0x0018},
        {EXIF_TAG_GPS_DEST_DISTANCE_REF, "GPSDestDistanceRef", 0x0019},
        {EXIF_TAG_GPS_DEST_DISTANCE, "GPSDestDistance", 0x001a},
        {EXIF_TAG_GPS_PROCESSING_METHOD, "GPSProcessingMethod", 0x001b},
        {EXIF_TAG_GPS_AREA_INFORMATION, "GPSAreaInformation", 0x001c},
        {EXIF_TAG_GPS_DATE_STAMP, "GPSDateStamp", 0x001d},
        {EXIF_TAG_GPS_DIFFERENTIAL, "GPSDifferential", 0x001e},
        {EXIF_TAG_GPS_H_POSITIONING_ERROR, "GPSHPositioningError", 0x001f},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_NEW_SUBFILE_TYPE, "NewSubfileType", 0x00fe},
        {EXIF_TAG_IMAGE_WIDTH, "ImageWidth", 0x0100},
        {EXIF_TAG_IMAGE_LENGTH, "ImageLength", 0x0101},
        {EXIF_TAG_BITS_PER_SAMPLE, "BitsPerSample", 0x0102},
        {EXIF_TAG_COMPRESSION, "Compression", 0x0103},
        {EXIF_TAG_PHOTOMETRIC_INTERPRETATION, "PhotometricInterpretation", 0x0106},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_FILL_ORDER, "FillOrder", 0x010a},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_DOCUMENT_NAME, "DocumentName", 0x010d},
        {EXIF_TAG_IMAGE_DESCRIPTION, "ImageDescription", 0x010e},
        {EXIF_TAG_MAKE, "Make", 0x010f},
        {EXIF_TAG_MODEL, "Model", 0x0110},
        {EXIF_TAG_STRIP_OFFSETS, "StripOffsets", 0x0111},
        {EXIF_TAG_ORIENTATION, "Orientation", 0x0112},
        {EXIF_TAG_SAMPLES_PER_PIXEL, "SamplesPerPixel", 0x0115},
        {EXIF_TAG_ROWS_PER_STRIP, "RowsPerStrip", 0x0116},
        {EXIF_TAG_STRIP_BYTE_COUNTS, "StripByteCounts", 0x0117},
        {EXIF_TAG_X_RESOLUTION, "XResolution", 0x011a},
        {EXIF_TAG_Y_RESOLUTION, "YResolution", 0x011b},
        {EXIF_TAG_PLANAR_CONFIGURATION, "PlanarConfiguration", 0x011c},
        {EXIF_TAG_RESOLUTION_UNIT, "ResolutionUnit", 0x0128},
        {EXIF_TAG_TRANSFER_FUNCTION, "TransferFunction", 0x012d},
        {EXIF_TAG_SOFTWARE, "Software", 0x0131},
        {EXIF_TAG_DATE_TIME, "DateTime", 0x0132},
        {EXIF_TAG_ARTIST, "Artist", 0x013b},
        {EXIF_TAG_WHITE_POINT, "WhitePoint", 0x013e},
        {EXIF_TAG_PRIMARY_CHROMATICITIES, "PrimaryChromaticities", 0x013f},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_SUB_IFDS, "SubIFDs", 0x014a},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_TRANSFER_RANGE, "TransferRange", 0x0156},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_JPEG_INTERCHANGE_FORMAT, "JPEGInterchangeFormat", 0x0201},
        {EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH, "JPEGInterchangeFormatLength", 0x0202},
        {EXIF_TAG_YCBCR_COEFFICIENTS, "YCbCrCoefficients", 0x0211},
        {EXIF_TAG_YCBCR_SUB_SAMPLING, "YCbCrSubSampling", 0x0212},
        {EXIF_TAG_YCBCR_POSITIONING, "YCbCrPositioning", 0x0213},
        {EXIF_TAG_REFERENCE_BLACK_WHITE, "ReferenceBlackWhite", 0x0214},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_XML_PACKET, "XMLPacket", 0x02bc},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_RELATED_IMAGE_FILE_FORMAT, "RelatedImageFileFormat", 0x1000},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_RELATED_IMAGE_WIDTH, "RelatedImageWidth", 0x1001},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_RELATED_IMAGE_LENGTH, "RelatedImageLength", 0x1002},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_CFA_REPEAT_PATTERN_DIM, "CFARepeatPatternDim", 0x828d},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_CFA_PATTERN, "CFAPattern", 0x828e},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_BATTERY_LEVEL, "BatteryLevel", 0x828f},
        {EXIF_TAG_COPYRIGHT, "Copyright", 0x8298},
        {EXIF_TAG_EXPOSURE_TIME, "ExposureTime", 0x829a},
        {EXIF_TAG_FNUMBER, "FNumber", 0x829d},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_IPTC_NAA, "IPTC/NAA", 0x83bb},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_IMAGE_RESOURCES, "ImageResources", 0x8649},
        {EXIF_TAG_EXIF_IFD_POINTER, "ExifIfdPointer", 0x8769},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_INTER_COLOR_PROFILE, "InterColorProfile", 0x8773},
        {EXIF_TAG_EXPOSURE_PROGRAM, "ExposureProgram", 0x8822},
        {EXIF_TAG_SPECTRAL_SENSITIVITY, "SpectralSensitivity", 0x8824},
        {EXIF_TAG_GPS_INFO_IFD_POINTER, "GPSInfoIFDPointer", 0x8825},
        {EXIF_TAG_ISO_SPEED_RATINGS, "ISOSpeedRatings", 0x8827},
        {EXIF_TAG_OECF, "OECF", 0x8828},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_TIME_ZONE_OFFSET, "TimeZoneOffset", 0x882a},
        {TAG_SENSITIVITY_TYPE, "SensitivityType", 0x8830},
        {TAG_STANDARD_OUTPUT_SENSITIVITY, "StandardOutputSensitivity", 0x8831},
        {TAG_RECOMMENDED_EXPOSURE_INDEX, "RecommendedExposureIndex", 0x8832},
        {EXIF_TAG_EXIF_VERSION, "ExifVersion", 0x9000},
        {EXIF_TAG_DATE_TIME_ORIGINAL, "DateTimeOriginal", 0x9003},
        {EXIF_TAG_DATE_TIME_DIGITIZED, "DateTimeDigitized", 0x9004},
        {EXIF_TAG_COMPONENTS_CONFIGURATION, "ComponentsConfiguration", 0x9101},
        {EXIF_TAG_COMPRESSED_BITS_PER_PIXEL, "CompressedBitsPerPixel", 0x9102},
        {EXIF_TAG_SHUTTER_SPEED_VALUE, "ShutterSpeedValue", 0x9201},
        {EXIF_TAG_APERTURE_VALUE, "ApertureValue", 0x9202},
        {EXIF_TAG_BRIGHTNESS_VALUE, "BrightnessValue", 0x9203},
        {EXIF_TAG_EXPOSURE_BIAS_VALUE, "ExposureBiasValue", 0x9204},
        {EXIF_TAG_MAX_APERTURE_VALUE, "MaxApertureValue", 0x9205},
        {EXIF_TAG_SUBJECT_DISTANCE, "SubjectDistance", 0x9206},
        {EXIF_TAG_METERING_MODE, "MeteringMode", 0x9207},
        {EXIF_TAG_LIGHT_SOURCE, "LightSource", 0x9208},
        {EXIF_TAG_FLASH, "Flash", 0x9209},
        {EXIF_TAG_FOCAL_LENGTH, "FocalLength", 0x920a},
        {EXIF_TAG_SUBJECT_AREA, "SubjectArea", 0x9214},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_TIFF_EP_STANDARD_ID, "TIFF/EPStandardID", 0x9216},
        {EXIF_TAG_MAKER_NOTE, "MakerNote", 0x927c},
        {EXIF_TAG_USER_COMMENT, "UserComment", 0x9286},
        {EXIF_TAG_SUB_SEC_TIME, "SubsecTime", 0x9290},
        {EXIF_TAG_SUB_SEC_TIME_ORIGINAL, "SubSecTimeOriginal", 0x9291},
        {EXIF_TAG_SUB_SEC_TIME_DIGITIZED, "SubSecTimeDigitized", 0x9292},
        /* Not in EXIF 2.2 (Microsoft extension) */
        {EXIF_TAG_XP_TITLE, "XPTitle", 0x9c9b},
        /* Not in EXIF 2.2 (Microsoft extension) */
        {EXIF_TAG_XP_COMMENT, "XPComment", 0x9c9c},
        /* Not in EXIF 2.2 (Microsoft extension) */
        {EXIF_TAG_XP_AUTHOR, "XPAuthor", 0x9c9d},
        /* Not in EXIF 2.2 (Microsoft extension) */
        {EXIF_TAG_XP_KEYWORDS, "XPKeywords", 0x9c9e},
        /* Not in EXIF 2.2 (Microsoft extension) */
        {EXIF_TAG_XP_SUBJECT, "XPSubject", 0x9c9f},
        {EXIF_TAG_FLASH_PIX_VERSION, "FlashpixVersion", 0xa000},
        {EXIF_TAG_COLOR_SPACE, "ColorSpace", 0xa001},
        {EXIF_TAG_PIXEL_X_DIMENSION, "PixelXDimension", 0xa002},
        {EXIF_TAG_PIXEL_Y_DIMENSION, "PixelYDimension", 0xa003},
        {EXIF_TAG_RELATED_SOUND_FILE, "RelatedSoundFile", 0xa004},
        {EXIF_TAG_INTEROPERABILITY_IFD_POINTER, "InteroperabilityIFDPointer", 0xa005},
        {EXIF_TAG_FLASH_ENERGY, "FlashEnergy", 0xa20b},
        {EXIF_TAG_SPATIAL_FREQUENCY_RESPONSE, "SpatialFrequencyResponse", 0xa20c},
        {EXIF_TAG_FOCAL_PLANE_X_RESOLUTION, "FocalPlaneXResolution", 0xa20e},
        {EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION, "FocalPlaneYResolution", 0xa20f},
        {EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT, "FocalPlaneResolutionUnit", 0xa210},
        {EXIF_TAG_SUBJECT_LOCATION, "SubjectLocation", 0xa214},
        {EXIF_TAG_EXPOSURE_INDEX, "ExposureIndex", 0xa215},
        {EXIF_TAG_SENSING_METHOD, "SensingMethod", 0xa217},
        {EXIF_TAG_FILE_SOURCE, "FileSource", 0xa300},
        {EXIF_TAG_SCENE_TYPE, "SceneType", 0xa301},
        {EXIF_TAG_NEW_CFA_PATTERN, "CFAPattern", 0xa302},
        {EXIF_TAG_CUSTOM_RENDERED, "CustomRendered", 0xa401},
        {EXIF_TAG_EXPOSURE_MODE, "ExposureMode", 0xa402},
        {EXIF_TAG_WHITE_BALANCE, "WhiteBalance", 0xa403},
        {EXIF_TAG_DIGITAL_ZOOM_RATIO, "DigitalZoomRatio", 0xa404},
        {EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, "FocalLengthIn35mmFilm", 0xa405},
        {EXIF_TAG_SCENE_CAPTURE_TYPE, "SceneCaptureType", 0xa406},
        {EXIF_TAG_GAIN_CONTROL, "GainControl", 0xa407},
        {EXIF_TAG_CONTRAST, "Contrast", 0xa408},
        {EXIF_TAG_SATURATION, "Saturation", 0xa409},
        {EXIF_TAG_SHARPNESS, "Sharpness", 0xa40a},
        {EXIF_TAG_DEVICE_SETTING_DESCRIPTION, "DeviceSettingDescription", 0xa40b},
        {EXIF_TAG_SUBJECT_DISTANCE_RANGE, "SubjectDistanceRange", 0xa40c},
        {EXIF_TAG_IMAGE_UNIQUE_ID, "ImageUniqueID", 0xa420},
        /* EXIF 2.3 */
        {EXIF_TAG_CAMERA_OWNER_NAME, "CameraOwnerName", 0xa430},
        /* EXIF 2.3 */
        {EXIF_TAG_BODY_SERIAL_NUMBER, "BodySerialNumber", 0xa431},
        /* EXIF 2.3 */
        {EXIF_TAG_LENS_SPECIFICATION, "LensSpecification", 0xa432},
        /* EXIF 2.3 */
        {EXIF_TAG_LENS_MAKE, "LensMake", 0xa433},
        /* EXIF 2.3 */
        {EXIF_TAG_LENS_MODEL, "LensModel", 0xa434},
        /* EXIF 2.3 */
        {EXIF_TAG_LENS_SERIAL_NUMBER, "LensSerialNumber", 0xa435},
        /* EXIF 2.32 */
        {EXIF_TAG_COMPOSITE_IMAGE, "CompositeImage", 0xa460},
        /* EXIF 2.32 */
        {EXIF_TAG_SOURCE_IMAGE_NUMBER_OF_COMPOSITE_IMAGE, "SourceImageNumberOfCompositeImage", 0xa461},
        /* EXIF 2.32 */
        {EXIF_TAG_SOURCE_EXPOSURE_TIMES_OF_COMPOSITE_IMAGE, "SourceExposureTimesOfCompositeImage", 0xa462},
        /* EXIF 2.3 */
        {EXIF_TAG_GAMMA, "Gamma", 0xa500},
        /* Not in EXIF 2.2 */
        {EXIF_TAG_PRINT_IMAGE_MATCHING, "PrintImageMatching", 0xc4a5},
        /* Not in EXIF 2.2 (from the Microsoft HD Photo specification) */
        {EXIF_TAG_PADDING, "Padding", 0xea1c},
        {static_cast<ExifTag>(0xffff), "", 0xffff}};
}

const std::string EXIFInfo::DEFAULT_EXIF_VALUE = "default_exif_value";

const std::string DATE_TIME_ORIGINAL = "DateTimeOriginal";
const std::string DATE_TIME_ORIGINAL_MEDIA = "DateTimeOriginalForMedia";
const std::string TAG_ORIENTATION_STRING = "Orientation";
const std::string TAG_ORIENTATION_INT = "OrientationInt";
const std::map<ExifTag, std::string> TAG_MAP = {
    {ExifTag::EXIF_TAG_BITS_PER_SAMPLE, "BitsPerSample"},
    {ExifTag::EXIF_TAG_ORIENTATION, TAG_ORIENTATION_STRING},
    {ExifTag::EXIF_TAG_IMAGE_LENGTH, "ImageLength"},
    {ExifTag::EXIF_TAG_IMAGE_WIDTH, "ImageWidth"},
    {ExifTag::EXIF_TAG_GPS_LATITUDE, "GPSLatitude"},
    {ExifTag::EXIF_TAG_GPS_LONGITUDE, "GPSLongitude"},
    {ExifTag::EXIF_TAG_GPS_LATITUDE_REF, "GPSLatitudeRef"},
    {ExifTag::EXIF_TAG_GPS_LONGITUDE_REF, "GPSLongitudeRef"},
    {ExifTag::EXIF_TAG_DATE_TIME_ORIGINAL, DATE_TIME_ORIGINAL},
    {ExifTag::EXIF_TAG_EXPOSURE_TIME, "ExposureTime"},
    {ExifTag::EXIF_TAG_FNUMBER, "FNumber"},
    {ExifTag::EXIF_TAG_ISO_SPEED_RATINGS, "ISOSpeedRatings"},
    {ExifTag::EXIF_TAG_SCENE_TYPE, "SceneType"},
    {ExifTag::EXIF_TAG_COMPRESSED_BITS_PER_PIXEL, "CompressedBitsPerPixel"},
    {ExifTag::EXIF_TAG_DATE_TIME, "DateTime"},
    {ExifTag::EXIF_TAG_GPS_TIME_STAMP, "GPSTimeStamp"},
    {ExifTag::EXIF_TAG_GPS_DATE_STAMP, "GPSDateStamp"},
    {ExifTag::EXIF_TAG_IMAGE_DESCRIPTION, "ImageDescription"},
    {ExifTag::EXIF_TAG_MAKE, "Make"},
    {ExifTag::EXIF_TAG_MODEL, "Model"},
    {ExifTag::EXIF_TAG_JPEG_PROC, "PhotoMode"},
    {ExifTag::EXIF_TAG_SENSITIVITY_TYPE, "SensitivityType"},
    {ExifTag::EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY, "StandardOutputSensitivity"},
    {ExifTag::EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX, "RecommendedExposureIndex"},
    {ExifTag::EXIF_TAG_ISO_SPEED, "ISOSpeedRatings"},
    {ExifTag::EXIF_TAG_APERTURE_VALUE, "ApertureValue"},
    {ExifTag::EXIF_TAG_EXPOSURE_BIAS_VALUE, "ExposureBiasValue"},
    {ExifTag::EXIF_TAG_METERING_MODE, "MeteringMode"},
    {ExifTag::EXIF_TAG_LIGHT_SOURCE, "LightSource"},
    {ExifTag::EXIF_TAG_FLASH, "Flash"},
    {ExifTag::EXIF_TAG_FOCAL_LENGTH, "FocalLength"},
    {ExifTag::EXIF_TAG_USER_COMMENT, "UserComment"},
    {ExifTag::EXIF_TAG_PIXEL_X_DIMENSION, "PixelXDimension"},
    {ExifTag::EXIF_TAG_PIXEL_Y_DIMENSION, "PixelYDimension"},
    {ExifTag::EXIF_TAG_WHITE_BALANCE, "WhiteBalance"},
    {ExifTag::EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, "FocalLengthIn35mmFilm"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_CAPTURE_MODE), "HwMnoteCaptureMode"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_PHYSICAL_APERTURE), "HwMnotePhysicalAperture"},
};

const static std::map<ExifTag, std::string> TAG_MAKER_NOTE_MAP = {
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_ROLL_ANGLE), "HwMnoteRollAngle"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_PITCH_ANGLE), "HwMnotePitchAngle"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_FOOD_CONF), "HwMnoteSceneFoodConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_STAGE_CONF), "HwMnoteSceneStageConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_BLUE_SKY_CONF), "HwMnoteSceneBlueSkyConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_GREEN_PLANT_CONF), "HwMnoteSceneGreenPlantConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_BEACH_CONF), "HwMnoteSceneBeachConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_SNOW_CONF), "HwMnoteSceneSnowConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_SUNSET_CONF), "HwMnoteSceneSunsetConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_FLOWERS_CONF), "HwMnoteSceneFlowersConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_NIGHT_CONF), "HwMnoteSceneNightConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_SCENE_TEXT_CONF), "HwMnoteSceneTextConf"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_FACE_COUNT), "HwMnoteFaceCount"},
    {static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_FOCUS_MODE), "HwMnoteFocusMode"},
};

static const std::map<std::string, uint32_t> ORIENTATION_INT_MAP = {
    {"Top-left", 0},
    {"Bottom-right", 180},
    {"Right-top", 90},
    {"Left-bottom", 270},
};

EXIFInfo::EXIFInfo()
    : bitsPerSample_(DEFAULT_EXIF_VALUE),
      orientation_(DEFAULT_EXIF_VALUE),
      imageLength_(DEFAULT_EXIF_VALUE),
      imageWidth_(DEFAULT_EXIF_VALUE),
      gpsLatitude_(DEFAULT_EXIF_VALUE),
      gpsLongitude_(DEFAULT_EXIF_VALUE),
      gpsLatitudeRef_(DEFAULT_EXIF_VALUE),
      gpsLongitudeRef_(DEFAULT_EXIF_VALUE),
      dateTimeOriginal_(DEFAULT_EXIF_VALUE),
      exposureTime_(DEFAULT_EXIF_VALUE),
      fNumber_(DEFAULT_EXIF_VALUE),
      isoSpeedRatings_(DEFAULT_EXIF_VALUE),
      sceneType_(DEFAULT_EXIF_VALUE),
      compressedBitsPerPixel_(DEFAULT_EXIF_VALUE),
      dateTime_(DEFAULT_EXIF_VALUE),
      gpsTimeStamp_(DEFAULT_EXIF_VALUE),
      gpsDateStamp_(DEFAULT_EXIF_VALUE),
      imageDescription_(DEFAULT_EXIF_VALUE),
      make_(DEFAULT_EXIF_VALUE),
      model_(DEFAULT_EXIF_VALUE),
      photoMode_(DEFAULT_EXIF_VALUE),
      sensitivityType_(DEFAULT_EXIF_VALUE),
      standardOutputSensitivity_(DEFAULT_EXIF_VALUE),
      recommendedExposureIndex_(DEFAULT_EXIF_VALUE),
      apertureValue_(DEFAULT_EXIF_VALUE),
      exposureBiasValue_(DEFAULT_EXIF_VALUE),
      meteringMode_(DEFAULT_EXIF_VALUE),
      lightSource_(DEFAULT_EXIF_VALUE),
      flash_(DEFAULT_EXIF_VALUE),
      focalLength_(DEFAULT_EXIF_VALUE),
      userComment_(DEFAULT_EXIF_VALUE),
      pixelXDimension_(DEFAULT_EXIF_VALUE),
      pixelYDimension_(DEFAULT_EXIF_VALUE),
      whiteBalance_(DEFAULT_EXIF_VALUE),
      focalLengthIn35mmFilm_(DEFAULT_EXIF_VALUE),
      hwMnoteCaptureMode_(DEFAULT_EXIF_VALUE),
      hwMnotePhysicalAperture_(DEFAULT_EXIF_VALUE),
      hwMnoteRollAngle_(DEFAULT_EXIF_VALUE),
      hwMnotePitchAngle_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneFoodConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneStageConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneBlueSkyConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneGreenPlantConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneBeachConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneSnowConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneSunsetConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneFlowersConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneNightConf_(DEFAULT_EXIF_VALUE),
      hwMnoteSceneTextConf_(DEFAULT_EXIF_VALUE),
      hwMnoteFaceCount_(DEFAULT_EXIF_VALUE),
      hwMnoteFocusMode_(DEFAULT_EXIF_VALUE),
      imageFileDirectory_(EXIF_IFD_COUNT),
      exifData_(nullptr),
      isExifDataParsed_(false)
{
    for (auto i = TAG_MAP.begin(); i != TAG_MAP.end(); i++) {
        exifTags_[i->first] = DEFAULT_EXIF_VALUE;
    }
}

EXIFInfo::~EXIFInfo()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
    }
    exifTags_.clear();
}

static void inline DumpTagsMap(std::map<ExifTag, std::string> &tags)
{
    for (auto i = tags.begin(); i != tags.end(); i++) {
        if (TAG_MAP.count(i->first) == 0) {
            HiLog::Debug(LABEL, "DumpTagsMap %{public}d -> %{public}s.", i->first, i->second.c_str());
            continue;
        }
        std::string name = TAG_MAP.at(i->first);
        HiLog::Debug(LABEL,
            "DumpTagsMap %{public}s(%{public}d) -> %{public}s.", name.c_str(), i->first, i->second.c_str());
    }
}

int EXIFInfo::ParseExifData(const unsigned char *buf, unsigned len)
{
    HiLog::Debug(LABEL, "ParseExifData ENTER");
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
    }
    exifData_ = exif_data_new();
    if (!exifData_) {
        return PARSE_EXIF_DATA_ERROR;
    }
    exif_data_unset_option(exifData_, EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS);
    exif_data_load_data (exifData_, buf, len);
    exif_data_foreach_content(exifData_,
        [](ExifContent *ec, void *userData) {
            ExifIfd ifd = exif_content_get_ifd(ec);
            (static_cast<EXIFInfo*>(userData))->imageFileDirectory_ = ifd;
            if (ifd == EXIF_IFD_COUNT) {
                HiLog::Debug(LABEL, "GetIfd ERROR");
                return;
            }
            exif_content_foreach_entry(ec,
                [](ExifEntry *ee, void* userData) {
                    if (ee == nullptr) {
                        return;
                    }
                    char tagValueChar[1024];
                    exif_entry_get_value(ee, tagValueChar, sizeof(tagValueChar));
                    std::string tagValueStr(&tagValueChar[0], &tagValueChar[strlen(tagValueChar)]);
                    if ((static_cast<EXIFInfo*>(userData))->CheckExifEntryValid(exif_entry_get_ifd(ee), ee->tag)) {
                        (static_cast<EXIFInfo*>(userData))->SetExifTagValues(ee->tag, tagValueStr);
                    }
                }, userData);
        }, this);

    if (imageFileDirectory_ == EXIF_IFD_COUNT) {
        return PARSE_EXIF_IFD_ERROR;
    }
    ExifMakerNote exifMakerNote;
    if (exifMakerNote.Parser(exifData_, buf, len) == Media::SUCCESS) {
        SetExifTagValues(static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_CAPTURE_MODE),
            exifMakerNote.hwCaptureMode);
        SetExifTagValues(static_cast<ExifTag>(ExifMakerNote::HW_MNOTE_TAG_PHYSICAL_APERTURE),
            exifMakerNote.hwPhysicalAperture);
    }
    makerInfoTagValueMap = exifMakerNote.makerTagValueMap;
    isExifDataParsed_ = true;
    DumpTagsMap(exifTags_);
    return PARSE_EXIF_SUCCESS;
}

int EXIFInfo::ParseExifData(const std::string &data)
{
    return ParseExifData((const unsigned char *)data.data(), data.length());
}

bool EXIFInfo::IsExifDataParsed()
{
    return isExifDataParsed_;
}

void EXIFInfo::SetExifTagValues(const ExifTag &tag, const std::string &value)
{
    exifTags_[tag] = value;
    if (tag == EXIF_TAG_BITS_PER_SAMPLE) {
        bitsPerSample_ = value;
    } else if (tag == EXIF_TAG_ORIENTATION) {
        orientation_ = value;
    } else if (tag == EXIF_TAG_IMAGE_LENGTH) {
        imageLength_ = value;
    } else if (tag == EXIF_TAG_IMAGE_WIDTH) {
        imageWidth_ = value;
    } else if (tag == EXIF_TAG_GPS_LATITUDE) {
        gpsLatitude_ = value;
    } else if (tag == EXIF_TAG_GPS_LONGITUDE) {
        gpsLongitude_ = value;
    } else if (tag == EXIF_TAG_GPS_LATITUDE_REF) {
        gpsLatitudeRef_ = value;
    } else if (tag == EXIF_TAG_GPS_LONGITUDE_REF) {
        gpsLongitudeRef_ = value;
    } else if (tag == EXIF_TAG_DATE_TIME_ORIGINAL) {
        dateTimeOriginal_ = value;
    } else if (tag == EXIF_TAG_EXPOSURE_TIME) {
        exposureTime_ = value;
    } else if (tag == EXIF_TAG_FNUMBER) {
        fNumber_ = value;
    } else if (tag == EXIF_TAG_ISO_SPEED_RATINGS) {
        isoSpeedRatings_ = value;
    } else if (tag == EXIF_TAG_SCENE_TYPE) {
        sceneType_ = value;
    } else if (tag == EXIF_TAG_COMPRESSED_BITS_PER_PIXEL) {
        compressedBitsPerPixel_ = value;
    } else {
        SetExifTagValuesEx(tag, value);
    }
}

void EXIFInfo::SetExifTagValuesEx(const ExifTag &tag, const std::string &value)
{
    if (tag == EXIF_TAG_DATE_TIME) {
        dateTime_ = value;
    } else if (tag == EXIF_TAG_GPS_TIME_STAMP) {
        gpsTimeStamp_ = value;
    } else if (tag == EXIF_TAG_GPS_DATE_STAMP) {
        gpsDateStamp_ = value;
    } else if (tag == EXIF_TAG_IMAGE_DESCRIPTION) {
        imageDescription_ = value;
    } else if (tag == EXIF_TAG_MAKE) {
        make_ = value;
    } else if (tag == EXIF_TAG_MODEL) {
        model_ = value;
    } else if (tag == EXIF_TAG_JPEG_PROC) {
        photoMode_ = value;
    } else if (tag == TAG_SENSITIVITY_TYPE) {
        sensitivityType_ = value;
    } else if (tag == TAG_STANDARD_OUTPUT_SENSITIVITY) {
        standardOutputSensitivity_ = value;
    } else if (tag == TAG_RECOMMENDED_EXPOSURE_INDEX) {
        recommendedExposureIndex_ = value;
    } else if (tag == EXIF_TAG_APERTURE_VALUE) {
        apertureValue_ = value;
    } else if (tag == EXIF_TAG_EXPOSURE_BIAS_VALUE) {
        exposureBiasValue_ = value;
    } else if (tag == EXIF_TAG_METERING_MODE) {
        meteringMode_ = value;
    } else if (tag == EXIF_TAG_LIGHT_SOURCE) {
        lightSource_ = value;
    } else if (tag == EXIF_TAG_FLASH) {
        flash_ = value;
    } else if (tag == EXIF_TAG_FOCAL_LENGTH) {
        focalLength_ = value;
    } else if (tag == EXIF_TAG_USER_COMMENT) {
        userComment_ = value;
    } else if (tag == EXIF_TAG_PIXEL_X_DIMENSION) {
        pixelXDimension_ = value;
    } else if (tag == EXIF_TAG_PIXEL_Y_DIMENSION) {
        pixelYDimension_ = value;
    } else if (tag == EXIF_TAG_WHITE_BALANCE) {
        whiteBalance_ = value;
    } else if (tag == EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM) {
        focalLengthIn35mmFilm_ = value;
    } else {
        HiLog::Error(LABEL, "No match tag name!");
    }
}

uint32_t EXIFInfo::ModifyExifData(const ExifTag &tag, const std::string &value, const std::string &path)
{
    FILE *file = fopen(path.c_str(), "rb");
    if (file == nullptr) {
        HiLog::Error(LABEL, "Error creating file %{public}s", path.c_str());
        return Media::ERR_MEDIA_IO_ABNORMAL;
    }

    // read jpeg file to buff
    unsigned long fileLength = GetFileSize(file);
    if (fileLength == 0 || fileLength > MAX_FILE_SIZE) {
        HiLog::Error(LABEL, "Get file size failed.");
        (void)fclose(file);
        return Media::ERR_MEDIA_BUFFER_TOO_SMALL;
    }

    unsigned char *fileBuf = static_cast<unsigned char *>(malloc(fileLength));
    if (fileBuf == nullptr) {
        HiLog::Error(LABEL, "Allocate buf for %{public}s failed.", path.c_str());
        (void)fclose(file);
        return Media::ERR_IMAGE_MALLOC_ABNORMAL;
    }

    if (fread(fileBuf, fileLength, 1, file) != 1) {
        HiLog::Error(LABEL, "Read %{public}s failed.", path.c_str());
        ReleaseSource(&fileBuf, &file);
        return Media::ERR_MEDIA_READ_PARCEL_FAIL;
    }

    if (!(fileBuf[0] == 0xFF && fileBuf[1] == 0xD8)) {
        HiLog::Error(LABEL, "%{public}s is not jpeg file.", path.c_str());
        ReleaseSource(&fileBuf, &file);
        return Media::ERR_IMAGE_MISMATCHED_FORMAT;
    }

    ExifData *ptrExifData = nullptr;
    bool isNewExifData = false;
    if (!CreateExifData(fileBuf, fileLength, &ptrExifData, isNewExifData)) {
        ReleaseSource(&fileBuf, &file);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    (void)fclose(file);
    file = nullptr;

    unsigned int orginExifDataLength = GetOrginExifDataLength(isNewExifData, fileBuf);
    if (!isNewExifData && orginExifDataLength == 0) {
        HiLog::Error(LABEL, "There is no orginExifDataLength node in %{public}s.", path.c_str());
        exif_data_unref(ptrExifData);
        free(fileBuf);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    ExifByteOrder order = GetExifByteOrder(isNewExifData, fileBuf);
    FILE *newFile = fopen(path.c_str(), "wb+");
    if (newFile == nullptr) {
        HiLog::Error(LABEL, "Error create new file %{public}s", path.c_str());
        ReleaseSource(&fileBuf, &newFile);
        return Media::ERR_MEDIA_IO_ABNORMAL;
    }
    ExifEntry *entry = nullptr;
    if (!CreateExifEntry(tag, ptrExifData, value, order, &entry)) {
        ReleaseSource(&fileBuf, &newFile);
        exif_data_unref(ptrExifData);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    if (!WriteExifDataToFile(ptrExifData, orginExifDataLength, fileLength, fileBuf, newFile)) {
        ReleaseSource(&fileBuf, &newFile);
        exif_data_unref(ptrExifData);
        return Media::ERR_MEDIA_WRITE_PARCEL_FAIL;
    }
    ReleaseSource(&fileBuf, &newFile);
    exif_data_unref(ptrExifData);
    return Media::SUCCESS;
}

uint32_t EXIFInfo::ModifyExifData(const ExifTag &tag, const std::string &value, const int fd)
{
    const int localFd = dup(fd);
    FILE *file = fdopen(localFd, "wb+");
    if (file == nullptr) {
        HiLog::Error(LABEL, "Error creating file %{public}d", localFd);
        return Media::ERR_MEDIA_IO_ABNORMAL;
    }

    // read jpeg file to buff
    unsigned long fileLength = GetFileSize(file);
    if (fileLength == 0 || fileLength > MAX_FILE_SIZE) {
        HiLog::Error(LABEL, "Get file size failed.");
        (void)fclose(file);
        return Media::ERR_MEDIA_BUFFER_TOO_SMALL;
    }

    unsigned char *fileBuf = static_cast<unsigned char *>(malloc(fileLength));
    if (fileBuf == nullptr) {
        HiLog::Error(LABEL, "Allocate buf for %{public}d failed.", localFd);
        (void)fclose(file);
        return Media::ERR_IMAGE_MALLOC_ABNORMAL;
    }

    // Set current position to begin of file.
    (void)fseek(file, 0L, 0);
    if (fread(fileBuf, fileLength, 1, file) != 1) {
        HiLog::Error(LABEL, "Read %{public}d failed.", localFd);
        ReleaseSource(&fileBuf, &file);
        return Media::ERR_MEDIA_READ_PARCEL_FAIL;
    }

    if (!(fileBuf[0] == 0xFF && fileBuf[1] == 0xD8)) {
        HiLog::Error(LABEL, "%{public}d is not jpeg file.", localFd);
        ReleaseSource(&fileBuf, &file);
        return Media::ERR_IMAGE_MISMATCHED_FORMAT;
    }

    ExifData *ptrExifData = nullptr;
    bool isNewExifData = false;
    if (!CreateExifData(fileBuf, fileLength, &ptrExifData, isNewExifData)) {
        ReleaseSource(&fileBuf, &file);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    unsigned int orginExifDataLength = GetOrginExifDataLength(isNewExifData, fileBuf);
    if (!isNewExifData && orginExifDataLength == 0) {
        HiLog::Error(LABEL, "There is no orginExifDataLength node in %{public}d.", localFd);
        free(fileBuf);
        exif_data_unref(ptrExifData);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    ExifByteOrder order = GetExifByteOrder(isNewExifData, fileBuf);
    // Set current position to begin of new file.
    (void)fseek(file, 0L, 0);
    ExifEntry *entry = nullptr;
    if (!CreateExifEntry(tag, ptrExifData, value, order, &entry)) {
        ReleaseSource(&fileBuf, &file);
        exif_data_unref(ptrExifData);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    if (!WriteExifDataToFile(ptrExifData, orginExifDataLength, fileLength, fileBuf, file)) {
        ReleaseSource(&fileBuf, &file);
        exif_data_unref(ptrExifData);
        return Media::ERR_MEDIA_WRITE_PARCEL_FAIL;
    }
    ReleaseSource(&fileBuf, &file);
    exif_data_unref(ptrExifData);
    return Media::SUCCESS;
}

void ReleaseExifDataBuffer(unsigned char* exifDataBuf)
{
    if (exifDataBuf != nullptr) {
        free(exifDataBuf);
        exifDataBuf = nullptr;
    }
}

uint32_t EXIFInfo::ModifyExifData(const ExifTag &tag, const std::string &value,
    unsigned char *data, uint32_t size)
{
    if (data == nullptr) {
        HiLog::Error(LABEL, "buffer is nullptr.");
        return Media::ERR_IMAGE_SOURCE_DATA;
    }

    if (size == 0) {
        HiLog::Error(LABEL, "buffer size is 0.");
        return Media::ERR_MEDIA_BUFFER_TOO_SMALL;
    }

    if (!(data[0] == 0xFF && data[1] == 0xD8)) {
        HiLog::Error(LABEL, "This is not jpeg file.");
        return Media::ERR_IMAGE_MISMATCHED_FORMAT;
    }

    ExifData *ptrExifData = nullptr;
    bool isNewExifData = false;
    if (!CreateExifData(data, size, &ptrExifData, isNewExifData)) {
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    unsigned int orginExifDataLength = GetOrginExifDataLength(isNewExifData, data);
    if (!isNewExifData && orginExifDataLength == 0) {
        HiLog::Error(LABEL, "There is no orginExifDataLength node in buffer.");
        exif_data_unref(ptrExifData);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    ExifByteOrder order = GetExifByteOrder(isNewExifData, data);
    ExifEntry *entry = nullptr;
    if (!CreateExifEntry(tag, ptrExifData, value, order, &entry)) {
        exif_data_unref(ptrExifData);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    unsigned char* exifDataBuf = nullptr;
    unsigned int exifDataBufLength = 0;
    exif_data_save_data(ptrExifData, &exifDataBuf, &exifDataBufLength);
    if (exifDataBuf == nullptr) {
        HiLog::Error(LABEL, "Get Exif Data Buf failed!");
        exif_data_unref(ptrExifData);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    if (size == 0 || size > MAX_FILE_SIZE) {
        HiLog::Error(LABEL, "Buffer size is out of range.");
        exif_data_unref(ptrExifData);
        ReleaseExifDataBuffer(exifDataBuf);
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    unsigned char *tempBuf = static_cast<unsigned char *>(malloc(size));
    if (tempBuf == nullptr) {
        HiLog::Error(LABEL, "Allocate temp buffer ailed.");
        exif_data_unref(ptrExifData);
        ReleaseExifDataBuffer(exifDataBuf);
        return Media::ERR_IMAGE_MALLOC_ABNORMAL;
    }

    // Write EXIF header to buffer
    uint32_t index = 0;
    if (sizeof(exifHeader) >= size) {
        HiLog::Error(LABEL, "There is not enough space for EXIF header!");
        free(tempBuf);
        tempBuf = nullptr;
        exif_data_unref(ptrExifData);
        ReleaseExifDataBuffer(exifDataBuf);
        return Media::ERR_MEDIA_OUT_OF_RANGE;
    }

    for (size_t i = 0; i < sizeof(exifHeader); i++) {
        tempBuf[index] = exifHeader[i];
        index += 1;
    }

    // Write EXIF block length in big-endian order
    unsigned char highBit = static_cast<unsigned char>((exifDataBufLength + LENGTH_OFFSET_2) >> MOVE_OFFSET_8);
    if (index >= size) {
        HiLog::Error(LABEL, "There is not enough space for writing EXIF block length!");
        free(tempBuf);
        tempBuf = nullptr;
        exif_data_unref(ptrExifData);
        ReleaseExifDataBuffer(exifDataBuf);
        return Media::ERR_MEDIA_OUT_OF_RANGE;
    }
    tempBuf[index] = highBit;
    index += 1;

    unsigned char lowBit = static_cast<unsigned char>((exifDataBufLength + LENGTH_OFFSET_2) & 0xff);
    if (index >= size) {
        HiLog::Error(LABEL, "There is not enough space for writing EXIF block length!");
        free(tempBuf);
        tempBuf = nullptr;
        exif_data_unref(ptrExifData);
        ReleaseExifDataBuffer(exifDataBuf);
        return Media::ERR_MEDIA_OUT_OF_RANGE;
    }
    tempBuf[index] = lowBit;
    index += 1;

    // Write EXIF data block
    if ((index +  exifDataBufLength) >= size) {
        HiLog::Error(LABEL, "There is not enough space for writing EXIF data block!");
        free(tempBuf);
        tempBuf = nullptr;
        exif_data_unref(ptrExifData);
        ReleaseExifDataBuffer(exifDataBuf);
        return Media::ERR_MEDIA_OUT_OF_RANGE;
    }
    for (unsigned int i = 0; i < exifDataBufLength; i++) {
        tempBuf[index] = exifDataBuf[i];
        index += 1;
    }

    // Write JPEG image data, skipping the non-EXIF header
    if ((index + size - orginExifDataLength - sizeof(exifHeader) - MOVE_OFFSET_8) > size) {
        HiLog::Error(LABEL, "There is not enough space for writing JPEG image data!");
        free(tempBuf);
        tempBuf = nullptr;
        exif_data_unref(ptrExifData);
        ReleaseExifDataBuffer(exifDataBuf);
        return Media::ERR_MEDIA_OUT_OF_RANGE;
    }

    for (unsigned int i = 0; i < (size - orginExifDataLength - sizeof(exifHeader)); i++) {
        tempBuf[index] = data[orginExifDataLength + sizeof(exifHeader) + i];
        index += 1;
    }

    for (unsigned int i = 0; i < size; i++) {
        data[i] = tempBuf[i];
    }

    ParseExifData(data, static_cast<unsigned int>(index));
    free(tempBuf);
    tempBuf = nullptr;
    exif_data_unref(ptrExifData);
    ReleaseExifDataBuffer(exifDataBuf);
    return Media::SUCCESS;
}

ExifEntry* EXIFInfo::InitExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag)
{
    ExifEntry *entry;
    /* Return an existing tag if one exists */
    if (!(entry = exif_content_get_entry(exif->ifd[ifd], tag))) {
        /* Allocate a new entry */
        entry = exif_entry_new();
        if (entry == nullptr) {
            HiLog::Error(LABEL, "Create new entry failed!");
            return nullptr;
        }
        entry->tag = tag; // tag must be set before calling exif_content_add_entry
        /* Attach the ExifEntry to an IFD */
        exif_content_add_entry (exif->ifd[ifd], entry);

        /* Allocate memory for the entry and fill with default data */
        exif_entry_initialize (entry, tag);

        /* Ownership of the ExifEntry has now been passed to the IFD.
         * One must be very careful in accessing a structure after
         * unref'ing it; in this case, we know "entry" won't be freed
         * because the reference count was bumped when it was added to
         * the IFD.
         */
        exif_entry_unref(entry);
    }
    return entry;
}
static void EXIFInfoBufferCheck(ExifEntry *exifEntry, size_t len)
{
    if (exifEntry == nullptr || (exifEntry->size >= len)) {
        return;
    }
    /* Create a memory allocator to manage this ExifEntry */
    ExifMem *exifMem = exif_mem_new_default();
    if (exifMem == nullptr) {
        HiLog::Error(LABEL, "Create mem failed!");
        return;
    }
    auto buf = exif_mem_realloc(exifMem, exifEntry->data, len);
    if (buf != nullptr) {
        exifEntry->data = static_cast<unsigned char*>(buf);
        exifEntry->size = len;
    }
    exif_mem_unref(exifMem);
}

ExifEntry* EXIFInfo::CreateExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag,
    size_t len, ExifFormat format)
{
    void *buf;
    ExifEntry *exifEntry;

    if ((exifEntry = exif_content_get_entry(exif->ifd[ifd], tag)) != nullptr) {
        EXIFInfoBufferCheck(exifEntry, len);
        return exifEntry;
    }

    /* Create a memory allocator to manage this ExifEntry */
    ExifMem *exifMem = exif_mem_new_default();
    if (exifMem == nullptr) {
        HiLog::Error(LABEL, "Create mem failed!");
        return nullptr;
    }

    /* Create a new ExifEntry using our allocator */
    exifEntry = exif_entry_new_mem (exifMem);
    if (exifEntry == nullptr) {
        HiLog::Error(LABEL, "Create entry by mem failed!");
        return nullptr;
    }

    /* Allocate memory to use for holding the tag data */
    buf = exif_mem_alloc(exifMem, len);
    if (buf == nullptr) {
        HiLog::Error(LABEL, "Allocate memory failed!");
        return nullptr;
    }

    /* Fill in the entry */
    exifEntry->data = static_cast<unsigned char*>(buf);
    exifEntry->size = len;
    exifEntry->tag = tag;
    exifEntry->components = len;
    exifEntry->format = format;

    /* Attach the ExifEntry to an IFD */
    exif_content_add_entry (exif->ifd[ifd], exifEntry);

    /* The ExifMem and ExifEntry are now owned elsewhere */
    exif_mem_unref(exifMem);
    exif_entry_unref(exifEntry);

    return exifEntry;
}

unsigned long EXIFInfo::GetFileSize(FILE *fp)
{
    long int position;
    long size;

    /* Save the current position. */
    position = ftell(fp);

    /* Jump to the end of the file. */
    (void)fseek(fp, 0L, SEEK_END);

    /* Get the end position. */
    size = ftell(fp);

    /* Jump back to the original position. */
    (void)fseek(fp, position, SEEK_SET);

    return static_cast<unsigned long>(size);
}

bool EXIFInfo::CreateExifData(unsigned char *buf, unsigned long length, ExifData **ptrData, bool &isNewExifData)
{
    if ((buf[BUFFER_POSITION_6] == 'E' && buf[BUFFER_POSITION_7] == 'x' &&
        buf[BUFFER_POSITION_8] == 'i' && buf[BUFFER_POSITION_9] == 'f')) {
        *ptrData = exif_data_new_from_data(buf, static_cast<unsigned int>(length));
        if (!(*ptrData)) {
            HiLog::Error(LABEL, "Create exif data from file failed.");
            return false;
        }
        isNewExifData = false;
        HiLog::Debug(LABEL, "Create exif data from buffer.");
    } else {
        *ptrData = exif_data_new();
        if (!(*ptrData)) {
            HiLog::Error(LABEL, "Create exif data failed.");
            return false;
        }
        /* Set the image options */
        exif_data_set_option(*ptrData, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
        exif_data_set_data_type(*ptrData, EXIF_DATA_TYPE_COMPRESSED);
        exif_data_set_byte_order(*ptrData, EXIF_BYTE_ORDER_INTEL);

        /* Create the mandatory EXIF fields with default data */
        exif_data_fix(*ptrData);
        isNewExifData = true;
        HiLog::Debug(LABEL, "Create new exif data.");
    }
    return true;
}

unsigned int EXIFInfo::GetOrginExifDataLength(const bool &isNewExifData, unsigned char *buf)
{
    unsigned int orginExifDataLength = 0;
    if (!isNewExifData) {
        orginExifDataLength = static_cast<unsigned int>(buf[BUFFER_POSITION_5]) |
            static_cast<unsigned int>(buf[BUFFER_POSITION_4] << MOVE_OFFSET_8);
    }
    return orginExifDataLength;
}

ExifByteOrder EXIFInfo::GetExifByteOrder(const bool &isNewExifData, unsigned char *buf)
{
    if (isNewExifData) {
        return EXIF_BYTE_ORDER_INTEL;
    } else {
        if (buf[BUFFER_POSITION_12] == 'M' && buf[BUFFER_POSITION_13] == 'M') {
            return EXIF_BYTE_ORDER_MOTOROLA;
        } else {
            return EXIF_BYTE_ORDER_INTEL;
        }
    }
}

static int GCD(int a, int b)
{
    if (b == 0) {
        return a;
    }
    return GCD(b, a % b);
}

static bool GetFractionFromStr(const std::string &decimal, ExifRational &result)
{
    int intPart = stoi(decimal.substr(0, decimal.find(".")));
    double decPart = stod(decimal.substr(decimal.find(".")));

    int numerator = decPart * pow(10, decimal.length() - decimal.find(".") - 1);
    int denominator = pow(10, decimal.length() - decimal.find(".") - 1);

    int gcdVal = GCD(numerator, denominator);
    if (gcdVal == 0) {
        HiLog::Error(LABEL, "gcdVal is zero");
        return false;
    }
    numerator /= gcdVal;
    denominator /= gcdVal;

    numerator += intPart * denominator;

    result.numerator = numerator;
    result.denominator = denominator;
    return true;
}

static void ExifIntValueByFormat(unsigned char *b, ExifByteOrder order, ExifFormat format, long value)
{
    switch (format) {
        case EXIF_FORMAT_SHORT:
            exif_set_short(b, order, (ExifShort)value);
            break;
        case EXIF_FORMAT_SSHORT:
            exif_set_sshort(b, order, (ExifSShort)value);
            break;
        case EXIF_FORMAT_LONG:
            exif_set_long(b, order, (ExifLong)value);
            break;
        case EXIF_FORMAT_SLONG:
            exif_set_slong(b, order, (ExifSLong)value);
            break;
        case EXIF_FORMAT_BYTE:
        case EXIF_FORMAT_SRATIONAL:
        case EXIF_FORMAT_UNDEFINED:
        case EXIF_FORMAT_ASCII:
        case EXIF_FORMAT_RATIONAL:
        default:
            HiLog::Error(LABEL, "ExifIntValueByFormat unsupport format %{public}d.", format);
            break;
    }
}

bool EXIFInfo::CreateExifEntry(const ExifTag &tag, ExifData *data, const std::string &value,
    ExifByteOrder order, ExifEntry **ptrEntry)
{
    switch (tag) {
        case EXIF_TAG_BITS_PER_SAMPLE: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_0, EXIF_TAG_BITS_PER_SAMPLE);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            std::vector<std::string> bitsVec;
            SplitStr(value, ",", bitsVec);
            if (bitsVec.size() > CONSTANT_4) {
                HiLog::Error(LABEL, "BITS_PER_SAMPLE Invalid value %{public}s", value.c_str());
                return false;
            }
            if (bitsVec.size() != 0) {
                for (size_t i = 0; i < bitsVec.size(); i++) {
                    exif_set_short((*ptrEntry)->data + i * CONSTANT_2, order, (ExifShort)atoi(bitsVec[i].c_str()));
                }
            }
            break;
        }
        case EXIF_TAG_ORIENTATION: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_0, EXIF_TAG_ORIENTATION);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_IMAGE_LENGTH: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_0, EXIF_TAG_IMAGE_LENGTH);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_IMAGE_WIDTH: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_0, EXIF_TAG_IMAGE_WIDTH);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_COMPRESSED_BITS_PER_PIXEL: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_COMPRESSED_BITS_PER_PIXEL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            ExifRational rat;
            if (!GetFractionFromStr(value, rat)) {
                HiLog::Error(LABEL, "Get fraction from value failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, rat);
            break;
        }
        case EXIF_TAG_GPS_LATITUDE: {
            std::vector<std::string> latVec;
            SplitStr(value, ",", latVec);
            if (latVec.size() != CONSTANT_2) {
                HiLog::Error(LABEL, "GPS_LATITUDE Invalid value %{public}s", value.c_str());
                return false;
            }

            ExifRational latRational;
            latRational.numerator = static_cast<ExifSLong>(atoi(latVec[0].c_str()));
            latRational.denominator = static_cast<ExifSLong>(atoi(latVec[1].c_str()));
            *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, EXIF_TAG_GPS_LATITUDE,
                sizeof(latRational), EXIF_FORMAT_RATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, latRational);
            break;
        }
        case EXIF_TAG_GPS_LONGITUDE: {
            std::vector<std::string> longVec;
            SplitStr(value, ",", longVec);
            if (longVec.size() != CONSTANT_2) {
                HiLog::Error(LABEL, "GPS_LONGITUDE Invalid value %{public}s", value.c_str());
                return false;
            }

            ExifRational longRational;
            longRational.numerator = static_cast<ExifSLong>(atoi(longVec[0].c_str()));
            longRational.denominator = static_cast<ExifSLong>(atoi(longVec[1].c_str()));
            *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, EXIF_TAG_GPS_LONGITUDE,
                sizeof(longRational), EXIF_FORMAT_RATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, longRational);
            break;
        }
        case EXIF_TAG_GPS_LATITUDE_REF: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, EXIF_TAG_GPS_LATITUDE_REF,
                value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "LATITUDE ref memcpy error");
            }
            break;
        }
        case EXIF_TAG_GPS_LONGITUDE_REF: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, EXIF_TAG_GPS_LONGITUDE_REF,
                value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr || (*ptrEntry)->size < value.length()) {
                HiLog::Error(LABEL, "Get exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "LONGITUDE ref memcpy error");
            }
            break;
        }
        case EXIF_TAG_WHITE_BALANCE: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_WHITE_BALANCE);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get white balance exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get focal length in 35 film exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_FLASH: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_FLASH);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get flash exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_APERTURE_VALUE: {
            std::vector<std::string> longVec;
            SplitStr(value, "/", longVec);
            if (longVec.size() != CONSTANT_2) {
                HiLog::Error(LABEL, "Aperture value Invalid value %{public}s", value.c_str());
                return false;
            }

            ExifRational longRational;
            longRational.numerator = static_cast<ExifSLong>(atoi(longVec[0].c_str()));
            longRational.denominator = static_cast<ExifSLong>(atoi(longVec[1].c_str()));
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_APERTURE_VALUE,
                                      sizeof(longRational), EXIF_FORMAT_RATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get Aperture value exif entry failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, longRational);
            break;
        }
        case EXIF_TAG_DATE_TIME_ORIGINAL: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL,
                                      value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get original date time exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "Original date time memcpy error");
            }
            break;
        }
        case EXIF_TAG_DATE_TIME: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_0, EXIF_TAG_DATE_TIME,
                                      value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get date time exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "Date Time memcpy error");
            }
            break;
        }
        case EXIF_TAG_EXPOSURE_BIAS_VALUE: {
            std::vector<std::string> longVec;
            SplitStr(value, "/", longVec);
            if (longVec.size() != CONSTANT_2) {
                HiLog::Error(LABEL, "Exposure bias value Invalid value %{public}s", value.c_str());
                return false;
            }

            ExifRational longRational;
            longRational.numerator = static_cast<ExifSLong>(atoi(longVec[0].c_str()));
            longRational.denominator = static_cast<ExifSLong>(atoi(longVec[1].c_str()));
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_BIAS_VALUE,
                                      sizeof(longRational), EXIF_FORMAT_SRATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get exposure bias value exif entry failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, longRational);
            break;
        }
        case EXIF_TAG_EXPOSURE_TIME: {
            std::vector<std::string> longVec;
            SplitStr(value, "/", longVec);
            if (longVec.size() != CONSTANT_2) {
                HiLog::Error(LABEL, "Exposure time Invalid value %{public}s", value.c_str());
                return false;
            }

            ExifRational longRational;
            longRational.numerator = static_cast<ExifSLong>(atoi(longVec[0].c_str()));
            longRational.denominator = static_cast<ExifSLong>(atoi(longVec[1].c_str()));
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_TIME,
                                      sizeof(longRational), EXIF_FORMAT_SRATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get Exposure time exif entry failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, longRational);
            break;
        }
        case EXIF_TAG_FNUMBER: {
            std::vector<std::string> longVec;
            SplitStr(value, "/", longVec);
            if (longVec.size() != CONSTANT_2) {
                HiLog::Error(LABEL, "Fnumber Invalid value %{public}s", value.c_str());
                return false;
            }

            ExifRational longRational;
            longRational.numerator = static_cast<ExifSLong>(atoi(longVec[0].c_str()));
            longRational.denominator = static_cast<ExifSLong>(atoi(longVec[1].c_str()));
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_FNUMBER,
                                      sizeof(longRational), EXIF_FORMAT_SRATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get Fnumber exif entry failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, longRational);
            break;
        }
        case EXIF_TAG_FOCAL_LENGTH: {
            std::vector<std::string> longVec;
            SplitStr(value, "/", longVec);
            if (longVec.size() != CONSTANT_2) {
                HiLog::Error(LABEL, "Focal length Invalid value %{public}s", value.c_str());
                return false;
            }

            ExifRational longRational;
            longRational.numerator = static_cast<ExifSLong>(atoi(longVec[0].c_str()));
            longRational.denominator = static_cast<ExifSLong>(atoi(longVec[1].c_str()));
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH,
                                      sizeof(longRational), EXIF_FORMAT_SRATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get focal length exif entry failed.");
                return false;
            }
            exif_set_rational((*ptrEntry)->data, order, longRational);
            break;
        }

        case EXIF_TAG_GPS_TIME_STAMP: {
            std::vector<std::string> longVec;
            SplitStr(value, ":", longVec);
            if (longVec.size() != CONSTANT_3) {
                HiLog::Error(LABEL, "GPS time stamp Invalid value %{public}s", value.c_str());
                return false;
            }
            *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, EXIF_TAG_GPS_TIME_STAMP,
                                      MOVE_OFFSET_24, EXIF_FORMAT_SRATIONAL);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get GPS time stamp exif entry failed.");
                return false;
            }
            exif_set_long((*ptrEntry)->data, order, static_cast<ExifSLong>(atoi(longVec[CONSTANT_0].c_str())));
            exif_set_long((*ptrEntry)->data + MOVE_OFFSET_8, order,
                          static_cast<ExifSLong>(atoi(longVec[CONSTANT_1].c_str())));
            exif_set_long((*ptrEntry)->data + MOVE_OFFSET_16, order,
                          static_cast<ExifSLong>(atoi(longVec[CONSTANT_2].c_str())));
            break;
        }

        case EXIF_TAG_GPS_DATE_STAMP: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, EXIF_TAG_GPS_DATE_STAMP,
                                      value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get GPS Date stamp exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "GPS Date stamp memcpy error");
            }
            break;
        }

        case EXIF_TAG_IMAGE_DESCRIPTION: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_0, EXIF_TAG_IMAGE_DESCRIPTION,
                                      value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get image description exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "Image description memcpy error");
            }
            break;
        }

        case EXIF_TAG_ISO_SPEED_RATINGS: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED_RATINGS);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get ISO speed ratings exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }

        case EXIF_TAG_ISO_SPEED: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get ISO  speed  exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_LIGHT_SOURCE: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_LIGHT_SOURCE);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get light source exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }

        case EXIF_TAG_MAKE: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_0, EXIF_TAG_MAKE,
                                      value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get maker exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "maker memcpy error");
            }
            break;
        }

        case EXIF_TAG_METERING_MODE: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_METERING_MODE);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get metering mode exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }

        case EXIF_TAG_MODEL: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_0, EXIF_TAG_MODEL,
                                      value.length(), EXIF_FORMAT_ASCII);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get model exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "model memcpy error");
            }
            break;
        }

        case EXIF_TAG_PIXEL_X_DIMENSION: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_X_DIMENSION);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get pixel X-dimension exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }

        case EXIF_TAG_PIXEL_Y_DIMENSION: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_PIXEL_Y_DIMENSION);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get pixel Y-dimension exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }

        case EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get recommended exposure index exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_SCENE_TYPE: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_SCENE_TYPE,
                                      value.length(), EXIF_FORMAT_UNDEFINED);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get scene type exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "Scene type memcpy error");
            }
            break;
        }
        case EXIF_TAG_SENSITIVITY_TYPE: {
            *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_SENSITIVITY_TYPE);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get sensitive type exif entry failed.");
                return false;
            }
            ExifIntValueByFormat((*ptrEntry)->data, order, (*ptrEntry)->format, atoi(value.c_str()));
            break;
        }
        case EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY,
                                      value.length(), EXIF_FORMAT_LONG);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get standard out sensitivity exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "Standard out sensitivity memcpy error");
            }
            break;
        }
        case EXIF_TAG_USER_COMMENT: {
            *ptrEntry = CreateExifTag(data, EXIF_IFD_EXIF, EXIF_TAG_USER_COMMENT,
                                      value.length(), EXIF_FORMAT_UNDEFINED);
            if ((*ptrEntry) == nullptr) {
                HiLog::Error(LABEL, "Get user comment exif entry failed.");
                return false;
            }
            if (memcpy_s((*ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
                HiLog::Error(LABEL, "User comment memcpy error");
            }
            break;
        }

        default:
            break;
    }
    return true;
}

bool EXIFInfo::WriteExifDataToFile(ExifData *data, unsigned int orginExifDataLength, unsigned long fileLength,
    unsigned char *buf, FILE *fp)
{
    unsigned char* exifDataBuf = nullptr;
    unsigned int exifDataBufLength = 0;
    exif_data_save_data(data, &exifDataBuf, &exifDataBufLength);
    if (exifDataBuf == nullptr) {
        HiLog::Error(LABEL, "Get Exif Data Buf failed!");
        return false;
    }

    // Write EXIF header
    if (fwrite(exifHeader, sizeof(exifHeader), 1, fp) != 1) {
        HiLog::Error(LABEL, "Error writing EXIF header to file!");
        ReleaseExifDataBuffer(exifDataBuf);
        return false;
    }

    // Write EXIF block length in big-endian order
    if (fputc((exifDataBufLength + LENGTH_OFFSET_2) >> MOVE_OFFSET_8, fp) < 0) {
        HiLog::Error(LABEL, "Error writing EXIF block length to file!");
        ReleaseExifDataBuffer(exifDataBuf);
        return false;
    }

    if (fputc((exifDataBufLength + LENGTH_OFFSET_2) & 0xff, fp) < 0) {
        HiLog::Error(LABEL, "Error writing EXIF block length to file!");
        ReleaseExifDataBuffer(exifDataBuf);
        return false;
    }

    // Write EXIF data block
    if (fwrite(exifDataBuf, exifDataBufLength, 1, fp) != 1) {
        HiLog::Error(LABEL, "Error writing EXIF data block to file!");
        ReleaseExifDataBuffer(exifDataBuf);
        return false;
    }
    // Write JPEG image data, skipping the non-EXIF header
    unsigned int dataOffset = orginExifDataLength + sizeof(exifHeader);
    if (fwrite(buf + dataOffset, fileLength - dataOffset, 1, fp) != 1) {
        HiLog::Error(LABEL, "Error writing JPEG image data to file!");
        ReleaseExifDataBuffer(exifDataBuf);
        return false;
    }

    UpdateCacheExifData(fp);
    ReleaseExifDataBuffer(exifDataBuf);
    return true;
}

void EXIFInfo::ReleaseSource(unsigned char **ptrBuf, FILE **ptrFile)
{
    if (*ptrBuf) {
        free(*ptrBuf);
        *ptrBuf = nullptr;
        ptrBuf = nullptr;
    }

    if (*ptrFile != nullptr) {
        fclose(*ptrFile);
        *ptrFile = nullptr;
        ptrFile = nullptr;
    }
}

void EXIFInfo::UpdateCacheExifData(FILE *fp)
{
    unsigned long fileLength = GetFileSize(fp);
    if (fileLength == 0 || fileLength > MAX_FILE_SIZE) {
        HiLog::Error(LABEL, "Get file size failed.");
        return;
    }

    unsigned char *fileBuf = static_cast<unsigned char *>(malloc(fileLength));
    if (fileBuf == nullptr) {
        HiLog::Error(LABEL, "Allocate buf failed.");
        return;
    }

    // Set current position to begin of file.
    (void)fseek(fp, 0L, 0);
    if (fread(fileBuf, fileLength, 1, fp) != 1) {
        HiLog::Error(LABEL, "Read new file failed.");
        free(fileBuf);
        fileBuf = nullptr;
        return;
    }

    ParseExifData(fileBuf, static_cast<unsigned int>(fileLength));
    free(fileBuf);
    fileBuf = nullptr;
}

uint32_t EXIFInfo::GetFilterArea(const uint8_t *buf,
                                 const uint32_t &bufSize,
                                 const int &privacyType,
                                 std::vector<std::pair<uint32_t, uint32_t>> &ranges)
{
    std::unique_ptr<ByteOrderedBuffer> byteOrderedBuffer = std::make_unique<ByteOrderedBuffer>(buf, bufSize);
    byteOrderedBuffer->GenerateDEArray();
    if (byteOrderedBuffer->directoryEntryArray_.size() == 0) {
        HiLog::Error(LABEL, "Read Exif info range failed.");
        return ERROR_PARSE_EXIF_FAILED;
    }

    GetAreaFromExifEntries(privacyType, byteOrderedBuffer->directoryEntryArray_, ranges);
    if (ranges.size() == 0) {
        HiLog::Error(LABEL, "There is no exif info need filtered in this image.");
        return ERROR_NO_EXIF_TAGS;
    }

    return Media::SUCCESS;
}

void EXIFInfo::GetAreaFromExifEntries(const int &privacyType,
                                      const std::vector<DirectoryEntry> &entryArray,
                                      std::vector<std::pair<uint32_t, uint32_t>> &ranges)
{
    if (privacyType == PERMISSION_GPS_TYPE) {
        for (size_t i = 0; i < entryArray.size(); i++) {
            if (entryArray[i].ifd == EXIF_IFD_GPS) {
                std::pair<uint32_t, uint32_t> range =
                    std::make_pair(entryArray[i].valueOffset, entryArray[i].valueLength);
                ranges.push_back(range);
            }
        }
    }
}

ByteOrderedBuffer::ByteOrderedBuffer(const uint8_t *fileBuf, uint32_t bufferLength)
    : buf_(fileBuf), bufferLength_(bufferLength)
{
    if (bufferLength >= BUFFER_POSITION_12 && bufferLength >= BUFFER_POSITION_13) {
        if (fileBuf[BUFFER_POSITION_12] == 'M' && fileBuf[BUFFER_POSITION_13] == 'M') {
            byteOrder_ = EXIF_BYTE_ORDER_MOTOROLA;
        } else {
            byteOrder_ = EXIF_BYTE_ORDER_INTEL;
        }
    }
}

ByteOrderedBuffer::~ByteOrderedBuffer() {}

void ByteOrderedBuffer::GenerateDEArray()
{
    // Move current position to begin of IFD0 offset segment
    curPosition_ = TIFF_OFFSET_FROM_FILE_BEGIN + CONSTANT_4;
    int32_t ifd0Offset = ReadInt32();
    if (ifd0Offset < 0) {
        HiLog::Error(LABEL, "Get IFD0 offset failed!");
        return;
    }
    // Transform tiff offset to position of file
    ifd0Offset = static_cast<int32_t>(TransformTiffOffsetToFilePos(ifd0Offset));
    // Move current position to begin of IFD0 segment
    curPosition_ = static_cast<uint32_t>(ifd0Offset);

    if (curPosition_ + CONSTANT_2 > bufferLength_) {
        HiLog::Error(LABEL, "There is no data from the offset: %{public}d.", curPosition_);
        return;
    }
    GetDataRangeFromIFD(EXIF_IFD_0);
}

void ByteOrderedBuffer::GetDataRangeFromIFD(const ExifIfd &ifd)
{
    handledIfdOffsets_.push_back(curPosition_);
    int16_t entryCount = ReadShort();
    if (static_cast<uint32_t>(curPosition_ + BYTE_COUNTS_12 * entryCount) > bufferLength_ || entryCount <= 0) {
        HiLog::Error(LABEL, " The size of entries is either too big or negative.");
        return;
    }
    GetDataRangeFromDE(ifd, entryCount);

    if (Peek() + CONSTANT_4 <= bufferLength_) {
        int32_t nextIfdOffset = ReadInt32();
        if (nextIfdOffset == 0) {
            HiLog::Error(LABEL, "Stop reading file since this IFD is finished");
            return;
        }
        // Transform tiff offset to position of file
        if (nextIfdOffset != -1) {
            nextIfdOffset = static_cast<int32_t>(TransformTiffOffsetToFilePos(nextIfdOffset));
        }
        // Check if the next IFD offset
        // 1. Exists within the boundaries of the buffer
        // 2. Does not point to a previously read IFD.
        if (nextIfdOffset > 0L && static_cast<uint32_t>(nextIfdOffset) < bufferLength_) {
            if (!IsIFDhandled(nextIfdOffset)) {
                curPosition_ = static_cast<uint32_t>(nextIfdOffset);
                ExifIfd nextIfd = GetNextIfdFromLinkList(ifd);
                GetDataRangeFromIFD(nextIfd);
            } else {
                HiLog::Error(LABEL, "Stop reading buffer since re-reading an IFD at %{public}d.", nextIfdOffset);
            }
        } else {
            HiLog::Error(LABEL, "Stop reading file since a wrong offset at %{public}d.", nextIfdOffset);
        }
    }
}

void ByteOrderedBuffer::GetDataRangeFromDE(const ExifIfd &ifd, const int16_t &count)
{
    for (int16_t i = 0; i < count; i++) {
        uint16_t tagNumber = ReadUnsignedShort();
        uint16_t dataFormat = ReadUnsignedShort();
        int32_t numberOfComponents = ReadInt32();
        uint32_t nextEntryOffset = Peek() + CONSTANT_4;

        uint32_t byteCount = 0;
        bool valid = SetDEDataByteCount(tagNumber, dataFormat, numberOfComponents, byteCount);
        if (!valid) {
            curPosition_ = static_cast<uint32_t>(nextEntryOffset);
            continue;
        }

        // Read a value from data field or seek to the value offset which is stored in data
        // field if the size of the entry value is bigger than 4.
        // If the size of the entry value is less than 4, value is stored in value offset area.
        if (byteCount > CONSTANT_4) {
            int32_t offset = ReadInt32();
            // Transform tiff offset to position of file
            if (offset != -1) {
                offset = static_cast<int32_t>(TransformTiffOffsetToFilePos(offset));
            }
            if ((static_cast<uint32_t>(offset) + byteCount) <= bufferLength_) {
                curPosition_ = static_cast<uint32_t>(offset);
            } else {
                // Skip if invalid data offset.
                HiLog::Error(LABEL, "Skip the tag entry since data offset is invalid: %{public}d.", offset);
                curPosition_ = nextEntryOffset;
                continue;
            }
        }

        // Recursively parse IFD when a IFD pointer tag appears
        if (IsIFDPointerTag(tagNumber)) {
            ExifIfd ifdOfIFDPointerTag = GetIFDOfIFDPointerTag(tagNumber);
            ParseIFDPointerTag(ifdOfIFDPointerTag, dataFormat);
            curPosition_ = nextEntryOffset;
            continue;
        }

        uint32_t bytesOffset = Peek();
        uint32_t bytesLength = byteCount;
        DirectoryEntry directoryEntry = {static_cast<ExifTag>(tagNumber), static_cast<ExifFormat>(dataFormat),
                                         numberOfComponents, bytesOffset, bytesLength, ifd};
        directoryEntryArray_.push_back(directoryEntry);

        // Seek to next tag offset
        if (Peek() != nextEntryOffset) {
            curPosition_ = nextEntryOffset;
        }
    }
}

uint32_t ByteOrderedBuffer::TransformTiffOffsetToFilePos(const uint32_t &offset)
{
    return offset + TIFF_OFFSET_FROM_FILE_BEGIN;
}

bool ByteOrderedBuffer::SetDEDataByteCount(const uint16_t &tagNumber,
                                           const uint16_t &dataFormat,
                                           const int32_t &numberOfComponents,
                                           uint32_t &count)
{
    if (IsValidTagNumber(tagNumber)) {
        HiLog::Error(LABEL, "Skip the tag entry since tag number is not defined: %{public}d.", tagNumber);
    } else if (dataFormat <= 0 || exif_format_get_size(static_cast<ExifFormat>(dataFormat)) == 0) {
        HiLog::Error(LABEL, "Skip the tag entry since data format is invalid: %{public}d.", dataFormat);
    } else {
        count = static_cast<uint32_t>(numberOfComponents) *
                static_cast<uint32_t>(exif_format_get_size(static_cast<ExifFormat>(dataFormat)));
        if (count < 0) {
            HiLog::Error(LABEL, "Skip the tag entry since the number of components is invalid: %{public}d.",
                         numberOfComponents);
        } else {
            return true;
        }
    }
    return false;
}

void ByteOrderedBuffer::ParseIFDPointerTag(const ExifIfd &ifd, const uint16_t &dataFormat)
{
    uint32_t offset = 0U;
    // Get offset from data field
    switch (static_cast<ExifFormat>(dataFormat)) {
        case EXIF_FORMAT_SHORT: {
            offset = static_cast<uint32_t>(ReadUnsignedShort());
            break;
        }
        case EXIF_FORMAT_SSHORT: {
            offset = ReadShort();
            break;
        }
        case EXIF_FORMAT_LONG: {
            offset = ReadUnsignedInt32();
            break;
        }
        case EXIF_FORMAT_SLONG: {
            offset = static_cast<uint32_t>(ReadInt32());
            break;
        }
        default: {
            // Nothing to do
            break;
        }
    }
    // Transform tiff offset to position of file
    offset = TransformTiffOffsetToFilePos(offset);
    // Check if the next IFD offset
    // 1. Exists within the boundaries of the buffer
    // 2. Does not point to a previously read IFD.
    if (offset > 0L && offset < bufferLength_) {
        if (!IsIFDhandled(offset)) {
            curPosition_ = offset;
            GetDataRangeFromIFD(ifd);
        } else {
            HiLog::Error(LABEL, "Skip jump into the IFD since it has already been read at %{public}d.", offset);
        }
    } else {
        HiLog::Error(LABEL, "Skip jump into the IFD since its offset is invalid: %{public}d.", offset);
    }
}

bool ByteOrderedBuffer::IsValidTagNumber(const uint16_t &tagNumber)
{
    for (uint32_t i = 0; (IsSameTextStr(ExifTagTable[i].name, "")); i++) {
        if (ExifTagTable[i].number == tagNumber) {
            return true;
        }
    }
    return false;
}

bool ByteOrderedBuffer::IsIFDhandled(const uint32_t &position)
{
    if (handledIfdOffsets_.size() == 0) {
        HiLog::Error(LABEL, "There is no handled IFD!");
        return false;
    }

    for (size_t i = 0; i < handledIfdOffsets_.size(); i++) {
        if (handledIfdOffsets_[i] == position) {
            return true;
        }
    }
    return false;
}

bool ByteOrderedBuffer::IsIFDPointerTag(const uint16_t &tagNumber)
{
    bool ret = false;
    switch (static_cast<ExifTag>(tagNumber)) {
        case EXIF_TAG_SUB_IFDS:
        case EXIF_TAG_EXIF_IFD_POINTER:
        case EXIF_TAG_GPS_INFO_IFD_POINTER:
        case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
            ret = true;
            break;
        default:
            break;
    }
    return ret;
}

ExifIfd ByteOrderedBuffer::GetIFDOfIFDPointerTag(const uint16_t &tagNumber)
{
    ExifIfd ifd = EXIF_IFD_COUNT;
    switch (static_cast<ExifTag>(tagNumber)) {
        case EXIF_TAG_EXIF_IFD_POINTER: {
            ifd = EXIF_IFD_EXIF;
            break;
        }
        case EXIF_TAG_GPS_INFO_IFD_POINTER: {
            ifd = EXIF_IFD_GPS;
            break;
        }
        case EXIF_TAG_INTEROPERABILITY_IFD_POINTER: {
            ifd = EXIF_IFD_INTEROPERABILITY;
            break;
        }
        default: {
            break;
        }
    }
    return ifd;
}

ExifIfd ByteOrderedBuffer::GetNextIfdFromLinkList(const ExifIfd &ifd)
{
    ExifIfd nextIfd = EXIF_IFD_COUNT;
    /**
     * In jpeg file, the next IFD of IFD_0 in IFD link list is IFD_1,
     * other IFD is pointed by tag.
     */
    if (ifd == EXIF_IFD_0) {
        nextIfd = EXIF_IFD_1;
    }
    return nextIfd;
}

int32_t ByteOrderedBuffer::ReadInt32()
{
    // Move current position to begin of next segment
    curPosition_ += CONSTANT_4;
    if (curPosition_ > bufferLength_) {
        HiLog::Error(LABEL, "Current Position %{public}u out of range.", curPosition_);
        return -1;
    }

    if (byteOrder_ == EXIF_BYTE_ORDER_MOTOROLA) {
        return ((static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_4]) << MOVE_OFFSET_24) |
                (static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_3]) << MOVE_OFFSET_16) |
                (static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_2]) << MOVE_OFFSET_8) |
                static_cast<unsigned int>(buf_[curPosition_ - 1]));
    } else {
        return ((static_cast<unsigned int>(buf_[curPosition_ - 1]) << MOVE_OFFSET_24) |
                (static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_2]) << MOVE_OFFSET_16) |
                (static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_3]) << MOVE_OFFSET_8) |
                static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_4]));
    }
}

uint32_t ByteOrderedBuffer::ReadUnsignedInt32()
{
    return (static_cast<uint32_t>(ReadInt32()) & 0xffffffff);
}

int16_t ByteOrderedBuffer::ReadShort()
{
    // Move current position to begin of next segment
    curPosition_ += CONSTANT_2;
    if (curPosition_ > bufferLength_) {
        HiLog::Error(LABEL, "Current Position %{public}u out of range.", curPosition_);
        return -1;
    }

    if (byteOrder_ == EXIF_BYTE_ORDER_MOTOROLA) {
        return ((static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_2]) << MOVE_OFFSET_8) |
                static_cast<unsigned int>(buf_[curPosition_ - 1]));
    } else {
        return ((static_cast<unsigned int>(buf_[curPosition_ - 1]) << MOVE_OFFSET_8) |
                static_cast<unsigned int>(buf_[curPosition_ - CONSTANT_2]));
    }
}

uint16_t ByteOrderedBuffer::ReadUnsignedShort()
{
    return (static_cast<uint32_t>(ReadShort()) & 0xffff);
}

uint32_t ByteOrderedBuffer::Peek()
{
    return curPosition_;
}

bool EXIFInfo::CheckExifEntryValid(const ExifIfd &ifd, const ExifTag &tag)
{
    bool ret = false;
    switch (ifd) {
        case EXIF_IFD_0: {
            if (tag == EXIF_TAG_ORIENTATION ||
                tag == EXIF_TAG_BITS_PER_SAMPLE ||
                tag == EXIF_TAG_IMAGE_LENGTH ||
                tag == EXIF_TAG_IMAGE_WIDTH) {
                ret = true;
            }
            break;
        }
        case EXIF_IFD_EXIF: {
            if (tag == EXIF_TAG_DATE_TIME_ORIGINAL ||
                tag == EXIF_TAG_EXPOSURE_TIME ||
                tag == EXIF_TAG_FNUMBER ||
                tag == EXIF_TAG_ISO_SPEED_RATINGS ||
                tag == EXIF_TAG_SCENE_TYPE ||
                tag == EXIF_TAG_COMPRESSED_BITS_PER_PIXEL) {
                ret = true;
            }
            break;
        }
        case EXIF_IFD_GPS: {
            if (tag == EXIF_TAG_GPS_LATITUDE ||
                tag == EXIF_TAG_GPS_LONGITUDE ||
                tag == EXIF_TAG_GPS_LATITUDE_REF ||
                tag == EXIF_TAG_GPS_LONGITUDE_REF) {
                ret = true;
            }
            break;
        }
        default:
            break;
    }

    if (!ret) {
        ret = CheckExifEntryValidEx(ifd, tag);
    }

    return ret;
}

bool EXIFInfo::CheckExifEntryValidEx(const ExifIfd &ifd, const ExifTag &tag)
{
    bool ret = false;
    switch (ifd) {
        case EXIF_IFD_0: {
            if (tag == EXIF_TAG_DATE_TIME ||
                tag == EXIF_TAG_IMAGE_DESCRIPTION ||
                tag == EXIF_TAG_MAKE ||
                tag == EXIF_TAG_MODEL) {
                ret = true;
            }
            break;
        }
        case EXIF_IFD_EXIF: {
            if (tag == TAG_SENSITIVITY_TYPE ||
                tag == TAG_STANDARD_OUTPUT_SENSITIVITY ||
                tag == TAG_RECOMMENDED_EXPOSURE_INDEX ||
                tag == EXIF_TAG_APERTURE_VALUE ||
                tag == EXIF_TAG_EXPOSURE_BIAS_VALUE ||
                tag == EXIF_TAG_METERING_MODE ||
                tag == EXIF_TAG_LIGHT_SOURCE ||
                tag == EXIF_TAG_FLASH ||
                tag == EXIF_TAG_FOCAL_LENGTH ||
                tag == EXIF_TAG_USER_COMMENT ||
                tag == EXIF_TAG_PIXEL_X_DIMENSION ||
                tag == EXIF_TAG_PIXEL_Y_DIMENSION ||
                tag == EXIF_TAG_WHITE_BALANCE ||
                tag == EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM) {
                ret = true;
            }
            break;
        }
        case EXIF_IFD_GPS: {
            if (tag == EXIF_TAG_GPS_TIME_STAMP ||
                tag == EXIF_TAG_GPS_DATE_STAMP) {
                ret = true;
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

static void NumSplit(std::string &src, std::vector<std::string> &out)
{
    if (src.size() == 0) {
        return;
    }
    std::vector<std::string> res;
    size_t last = 0;
    for (size_t i = 0; i < src.size(); i++) {
        if (!std::isdigit(src[i])) {
            size_t splitSize = i - last;
            if (splitSize != 0) {
                res.push_back(src.substr(last, splitSize));
            }
            last = i + SIZE_ONE;
        }
    }
    if (last <= (src.size() - SIZE_ONE)) {
        res.push_back(src.substr(last));
    }
    for (size_t i = 0; i < res.size() && i < out.size(); i++) {
        out[i] = res[i];
    }
}

static std::string JoinStr(std::vector<std::string> &in, const std::string &delim)
{
    std::string res = "";
    for (size_t i = 0; i < (in.size() - SIZE_ONE); i++) {
        res.append(in[i]).append(delim);
    }
    res.append(in.back());
    return res;
}

static void FormatTimeStamp(std::string &src, std::string &value)
{
    std::string date = src;
    std::string time = "";
    std::string::size_type position = src.find(" ");
    if (position != src.npos) {
        // Date and time
        date = src.substr(0, position);
        time = src.substr(position);
    }
    std::vector<std::string> dateVector = {"1970", "01", "01"};
    std::vector<std::string> timeVector = {"00", "00", "00"};
    NumSplit(date, dateVector);
    NumSplit(time, timeVector);
    value = JoinStr(dateVector, "-") + " " + JoinStr(timeVector, ":");
}

static uint32_t SpecialExifData(EXIFInfo* info, const std::string &name, std::string &value)
{
    if (IsSameTextStr(DATE_TIME_ORIGINAL_MEDIA, name)) {
        std::string orgValue;
        auto res = info->GetExifData(DATE_TIME_ORIGINAL, orgValue);
        if (res == Media::SUCCESS) {
            FormatTimeStamp(orgValue, value);
        }
        return res;
    } else if (IsSameTextStr(TAG_ORIENTATION_INT, name)) {
        std::string orgValue;
        auto res = info->GetExifData(TAG_ORIENTATION_STRING, orgValue);
        if (res != Media::SUCCESS) {
            return res;
        }
        if (ORIENTATION_INT_MAP.count(orgValue) == 0) {
            HiLog::Debug(LABEL, "SpecialExifData %{public}s not found %{public}s.",
                name.c_str(), orgValue.c_str());
            return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
        }
        value = std::to_string(ORIENTATION_INT_MAP.at(orgValue));
        return res;
    }
    return Media::ERR_MEDIA_STATUS_ABNORMAL;
}

static bool GetExifTagByName(const std::string &name, ExifTag &tag)
{
    auto find_item = std::find_if(TAG_MAP.begin(), TAG_MAP.end(),
        [name](const std::map<ExifTag, std::string>::value_type item) {
        return IsSameTextStr(item.second, name);
    });
    auto find_maker_item = std::find_if(TAG_MAKER_NOTE_MAP.begin(), TAG_MAKER_NOTE_MAP.end(),
        [name](const std::map<ExifTag, std::string>::value_type item) {
        return IsSameTextStr(item.second, name);
    });
    if (find_item == TAG_MAP.end() && find_maker_item == TAG_MAKER_NOTE_MAP.end()) {
        return false;
    }
    tag = find_item->first;
    return true;
}

uint32_t EXIFInfo::GetExifData(const std::string name, std::string &value)
{
    auto res = SpecialExifData(this, name, value);
    if (res == Media::SUCCESS || res != Media::ERR_MEDIA_STATUS_ABNORMAL) {
        HiLog::Debug(LABEL, "GetExifData %{public}s special result with %{public}d.", name.c_str(), res);
        return res;
    }
    ExifTag tag;
    if (!GetExifTagByName(name, tag)) {
        HiLog::Error(LABEL, "GetExifData %{public}s not in the TAGs map.", name.c_str());
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    DumpTagsMap(exifTags_);
    if (exifTags_.count(tag) == 0) {
        HiLog::Error(LABEL, "GetExifData has no tag %{public}s[%{public}d], tags Size: %{public}zu.",
            name.c_str(), tag, exifTags_.size());
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    value = exifTags_.at(tag);
    if (IsSameTextStr(value, DEFAULT_EXIF_VALUE)) {
        HiLog::Debug(LABEL, "GetExifData %{public}s[%{public}d] value is DEFAULT_EXIF_VALUE.",
            name.c_str(), tag);
        return Media::ERR_MEDIA_VALUE_INVALID;
    }
    return Media::SUCCESS;
}

uint32_t EXIFInfo::ModifyExifData(const std::string name, const std::string &value, const std::string &path)
{
    ExifTag tag;
    if (!GetExifTagByName(name, tag)) {
        HiLog::Error(LABEL, "ModifyExifData %{public}s not in the TAGs map.", name.c_str());
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    return ModifyExifData(tag, value, path);
}

uint32_t EXIFInfo::ModifyExifData(const std::string name, const std::string &value, const int fd)
{
    ExifTag tag;
    if (!GetExifTagByName(name, tag)) {
        HiLog::Error(LABEL, "ModifyExifData %{public}s not in the TAGs map.", name.c_str());
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    return ModifyExifData(tag, value, fd);
}

uint32_t EXIFInfo::ModifyExifData(const std::string name, const std::string &value, unsigned char *data, uint32_t size)
{
    ExifTag tag;
    if (!GetExifTagByName(name, tag)) {
        HiLog::Error(LABEL, "ModifyExifData %{public}s not in the TAGs map.", name.c_str());
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    return ModifyExifData(tag, value, data, size);
}
} // namespace ImagePlugin
} // namespace OHOS
