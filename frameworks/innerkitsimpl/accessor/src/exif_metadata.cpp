#include "exif_metadata.h"
#include "exif_metadata_converter.h"
#include "image_log.h"
#include "media_errors.h"
#include "string_ex.h"
#include "securec.h"
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdint.h>
#include <vector>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ExifMetadata"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ExifMetadata"

namespace OHOS {
namespace Media {
namespace {
static constexpr int MOVE_OFFSET_8 = 8;
static constexpr int MOVE_OFFSET_16 = 16;
static constexpr int MOVE_OFFSET_24 = 24;
static constexpr int CONSTANT_0 = 0;
static constexpr int CONSTANT_1 = 1;
static constexpr int CONSTANT_2 = 2;
static constexpr int CONSTANT_3 = 3;
static constexpr int CONSTANT_4 = 4;
static constexpr unsigned long GPS_DIGIT_NUMBER = 1e6;
static constexpr uint32_t GPS_DMS_COUNT = 3;
static constexpr double GPS_MAX_LATITUDE = 90.0;
static constexpr double GPS_MIN_LATITUDE = 0.0;
static constexpr double GPS_MAX_LONGITUDE = 180.0;
static constexpr double GPS_MIN_LONGITUDE = 0.0;
static constexpr ExifTag EXIF_TAG_SENSITIVITY_TYPE = static_cast<ExifTag>(0x8830);
static constexpr ExifTag EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY = static_cast<ExifTag>(0x8831);
static constexpr ExifTag EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX = static_cast<ExifTag>(0x8832);
static constexpr size_t SIZE_2 = 2;
static constexpr size_t SIZE_3 = 3;
static const struct TagEntry {
        /*! Tag ID. There may be duplicate tags when the same number is used for
         * different meanings in different IFDs. */
        ExifTag tag;
        const std::string name;
        const uint16_t number;
    } EXIF_TAG_TABLE[] = {
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
        {EXIF_TAG_SENSITIVITY_TYPE, "SensitivityType", 0x8830},
        {EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY, "StandardOutputSensitivity", 0x8831},
        {EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX, "RecommendedExposureIndex", 0x8832},
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
template <typename T>
std::istream& streamToRational(std::istream& is, T& r) {
    int32_t nominator = 0;
    int32_t denominator = 0;
    char c('\0');
    is >> nominator >> c >> denominator;
    if (c != '/')
        is.setstate(std::ios::failbit);
    if (is)
        r = { nominator, denominator };
    return is;
}

std::istream& operator>>(std::istream& is, ExifRational& r) {
    return streamToRational(is, r);
}
std::istream& operator>>(std::istream& is, ExifSRational& r) {
    return streamToRational(is, r);
}

std::map<ExifTag, ExifIfd> TagIfdTable = {
    {EXIF_TAG_ORIENTATION, EXIF_IFD_0},
    {EXIF_TAG_BITS_PER_SAMPLE, EXIF_IFD_0},
    {EXIF_TAG_IMAGE_LENGTH, EXIF_IFD_0},
    {EXIF_TAG_IMAGE_WIDTH, EXIF_IFD_0},
    {EXIF_TAG_DATE_TIME, EXIF_IFD_0},
    {EXIF_TAG_IMAGE_DESCRIPTION, EXIF_IFD_0},
    {EXIF_TAG_MAKE, EXIF_IFD_0},
    {EXIF_TAG_MODEL, EXIF_IFD_0},
    {EXIF_TAG_DATE_TIME_ORIGINAL, EXIF_IFD_EXIF},
    {EXIF_TAG_EXPOSURE_TIME, EXIF_IFD_EXIF},
    {EXIF_TAG_FNUMBER, EXIF_IFD_EXIF},
    {EXIF_TAG_ISO_SPEED_RATINGS, EXIF_IFD_EXIF},
    {EXIF_TAG_SCENE_TYPE, EXIF_IFD_EXIF},
    {EXIF_TAG_COMPRESSED_BITS_PER_PIXEL, EXIF_IFD_EXIF},
    {EXIF_TAG_SENSITIVITY_TYPE, EXIF_IFD_EXIF},
    {EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY, EXIF_IFD_EXIF},
    {EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX, EXIF_IFD_EXIF},
    {EXIF_TAG_APERTURE_VALUE, EXIF_IFD_EXIF},
    {EXIF_TAG_EXPOSURE_BIAS_VALUE, EXIF_IFD_EXIF},
    {EXIF_TAG_METERING_MODE, EXIF_IFD_EXIF},
    {EXIF_TAG_LIGHT_SOURCE, EXIF_IFD_EXIF},
    {EXIF_TAG_METERING_MODE, EXIF_IFD_EXIF},
    {EXIF_TAG_FLASH, EXIF_IFD_EXIF},
    {EXIF_TAG_FOCAL_LENGTH, EXIF_IFD_EXIF},
    {EXIF_TAG_USER_COMMENT, EXIF_IFD_EXIF},
    {EXIF_TAG_PIXEL_X_DIMENSION, EXIF_IFD_EXIF},
    {EXIF_TAG_PIXEL_Y_DIMENSION, EXIF_IFD_EXIF},
    {EXIF_TAG_WHITE_BALANCE, EXIF_IFD_EXIF},
    {EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, EXIF_IFD_EXIF},
    {EXIF_TAG_GPS_VERSION_ID, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_LATITUDE_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_LATITUDE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_LONGITUDE_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_LONGITUDE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_ALTITUDE_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_ALTITUDE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_TIME_STAMP, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_SATELLITES, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_STATUS, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_MEASURE_MODE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DOP, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_SPEED_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_SPEED, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_TRACK_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_TRACK, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_IMG_DIRECTION_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_IMG_DIRECTION, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_MAP_DATUM, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_LATITUDE_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_LATITUDE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_LONGITUDE_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_LONGITUDE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_BEARING_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_BEARING, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_DISTANCE_REF, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DEST_DISTANCE, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_PROCESSING_METHOD, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_AREA_INFORMATION, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DATE_STAMP, EXIF_IFD_GPS},
    {EXIF_TAG_GPS_DIFFERENTIAL, EXIF_IFD_GPS}
};

std::set<ExifTag> UndefinedFormat = { EXIF_TAG_USER_COMMENT, EXIF_TAG_SCENE_TYPE };

static bool ConvertStringToDouble(const std::string &str, double &number)
{
    char* end = nullptr;
    number = std::strtod(str.c_str(), &end);
    return end != str.c_str() && *end == '\0' && number != HUGE_VAL;
}

static bool ConvertGpsDataToRationals(const std::vector<std::string> &dataVec,
    std::vector<ExifRational> &exifRationals)
{
    if (!(dataVec.size() == SIZE_3 && exifRationals.size() == SIZE_3)) {
        IMAGE_LOGD("Data size is invalid.");
        return false;
    }

    int32_t degree = static_cast<int32_t>(atoi(dataVec[CONSTANT_0].c_str()));
    int32_t minute = static_cast<int32_t>(atoi(dataVec[CONSTANT_1].c_str()));
    double secondDouble = 0.0;
    ConvertStringToDouble(dataVec[CONSTANT_2], secondDouble);
    int32_t second = static_cast<int32_t>(secondDouble * GPS_DIGIT_NUMBER);

    exifRationals[CONSTANT_0].numerator = static_cast<ExifSLong>(degree);
    exifRationals[CONSTANT_0].denominator = static_cast<ExifSLong>(1);
    exifRationals[CONSTANT_1].numerator = static_cast<ExifSLong>(minute);
    exifRationals[CONSTANT_1].denominator = static_cast<ExifSLong>(1);
    exifRationals[CONSTANT_2].numerator = static_cast<ExifSLong>(second);
    exifRationals[CONSTANT_2].denominator = static_cast<ExifSLong>(GPS_DIGIT_NUMBER);
    return true;
}
static void EXIFInfoBufferCheck(ExifEntry *exifEntry, size_t len)
{
    if (exifEntry == nullptr || (exifEntry->size >= len)) {
        return;
    }
    /* Create a memory allocator to manage this ExifEntry */
    ExifMem *exifMem = exif_mem_new_default();
    if (exifMem == nullptr) {
        IMAGE_LOGD("Create mem failed!");
        return;
    }
    auto buf = exif_mem_realloc(exifMem, exifEntry->data, len);
    if (buf != nullptr) {
        exifEntry->data = static_cast<unsigned char*>(buf);
        exifEntry->size = len;
    }
    exif_mem_unref(exifMem);
}

static int GCD(int a, int b)
{
    if (b == 0) {
        return a;
    }
    return GCD(b, a % b);
}

bool ExifMetadata::SetGpsRationals(ExifData *data, ExifEntry **ptrEntry, ExifByteOrder order,
    const ExifTag &tag, const std::vector<ExifRational> &exifRationals)
{
    if (exifRationals.size() != SIZE_3) {
        IMAGE_LOGD("ExifRationals size is invalid.");
        return false;
    }
    *ptrEntry = GetExifTag(data, EXIF_IFD_GPS, tag, MOVE_OFFSET_24);
    if ((*ptrEntry) == nullptr) {
        IMAGE_LOGD("Get exif entry failed.");
        return false;
    }
    exif_set_rational((*ptrEntry)->data, order, exifRationals[CONSTANT_0]);
    exif_set_rational((*ptrEntry)->data + MOVE_OFFSET_8, order, exifRationals[CONSTANT_1]);
    exif_set_rational((*ptrEntry)->data + MOVE_OFFSET_16, order, exifRationals[CONSTANT_2]);
    return true;
}

ExifEntry* ExifMetadata::GetExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len)
{
    ExifEntry *exifEntry;
    if ((exifEntry = exif_content_get_entry(exif->ifd[ifd], tag)) != nullptr) {
        EXIFInfoBufferCheck(exifEntry, len);
        return exifEntry;
    }
    return nullptr;
}


bool ExifMetadata::CreateExifdata()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
        return false;
    }
    exifData_ = exif_data_new();
    if (!(exifData_)) {
        IMAGE_LOGD("Create exif data failed.");
        return false;
    }
    /* Set the image options */
    exif_data_set_option(exifData_, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
    exif_data_set_data_type(exifData_, EXIF_DATA_TYPE_COMPRESSED);
    exif_data_set_byte_order(exifData_, EXIF_BYTE_ORDER_INTEL);

    /* Create the mandatory EXIF fields with default data */
    exif_data_fix(exifData_);
    IMAGE_LOGD("Create new exif data.");
    return true;
}

ExifMetadata::ExifMetadata(ExifData *exifData)
    : exifData_(exifData)
{
}

ExifMetadata::~ExifMetadata()
{
    if (exifData_ != nullptr) {
        exif_data_unref(exifData_);
        exifData_ = nullptr;
    }
}

int ExifMetadata::GetValue(const std::string &key, std::string &value) const
{
    if (exifData_ == nullptr) {
        value = "";
        return 1;
    }

    auto tag = exif_tag_from_name(key.c_str());
    auto entry = exif_data_get_entry(exifData_, tag);
    if (entry == nullptr) {
        IMAGE_LOGD("exif_data_get_entry leave");
        value = "";
        return 1;
    }
    char tagValueChar[1024];
    exif_entry_get_value(entry, tagValueChar, sizeof(tagValueChar));
    value = tagValueChar;
    return 0;
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
            IMAGE_LOGD("ExifIntValueByFormat unsupported format %{public}d.", format);
            break;
    }
}

static bool IsValidGpsData(const std::vector<std::string> &dataVec, const ExifTag &tag)
{
    if (dataVec.size() != SIZE_3 || (tag != EXIF_TAG_GPS_LATITUDE && tag != EXIF_TAG_GPS_LONGITUDE)) {
        IMAGE_LOGD("Gps dms data size is invalid.");
        return false;
    }
    double degree = 0.0;
    double minute = 0.0;
    double second = 0.0;
    if (!ConvertStringToDouble(dataVec[CONSTANT_0], degree) ||
        !ConvertStringToDouble(dataVec[CONSTANT_1], minute) ||
        !ConvertStringToDouble(dataVec[CONSTANT_2], second)) {
        IMAGE_LOGE("Convert gps data to double type failed.");
        return false;
    }
    constexpr uint32_t timePeriod = 60;
    double latOrLong = degree + minute / timePeriod + second / (timePeriod * timePeriod);
    if ((tag == EXIF_TAG_GPS_LATITUDE && (latOrLong > GPS_MAX_LATITUDE || latOrLong < GPS_MIN_LATITUDE)) ||
        (tag == EXIF_TAG_GPS_LONGITUDE && (latOrLong > GPS_MAX_LONGITUDE || latOrLong < GPS_MIN_LONGITUDE))) {
        IMAGE_LOGD("Gps latitude or longitude is out of range.");
        return false;
    }
    return true;
}

static bool GetFractionFromStr(const std::string &decimal, ExifRational &result)
{
    int intPart = stoi(decimal.substr(0, decimal.find(".")));
    double decPart = stod(decimal.substr(decimal.find(".")));

    int numerator = decPart * pow(10, decimal.length() - decimal.find(".") - 1);
    int denominator = pow(10, decimal.length() - decimal.find(".") - 1);

    int gcdVal = GCD(numerator, denominator);
    if (gcdVal == 0) {
        IMAGE_LOGD("gcdVal is zero");
        return false;
    }
    numerator /= gcdVal;
    denominator /= gcdVal;

    numerator += intPart * denominator;

    result.numerator = numerator;
    result.denominator = denominator;
    return true;
}

bool ExifMetadata::SetValue(const std::string &key, const std::string &value)
{
    static std::vector intProps{EXIF_TAG_ORIENTATION, EXIF_TAG_IMAGE_LENGTH, EXIF_TAG_IMAGE_WIDTH,
        EXIF_TAG_WHITE_BALANCE, EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, EXIF_TAG_FLASH, EXIF_TAG_ISO_SPEED_RATINGS,
        EXIF_TAG_ISO_SPEED, EXIF_TAG_LIGHT_SOURCE, EXIF_TAG_METERING_MODE, EXIF_TAG_PIXEL_X_DIMENSION,
        EXIF_TAG_PIXEL_Y_DIMENSION, EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX, EXIF_TAG_SENSITIVITY_TYPE};
    static std::vector strProps{EXIF_TAG_GPS_DATE_STAMP, EXIF_TAG_IMAGE_DESCRIPTION, EXIF_TAG_MAKE, EXIF_TAG_MODEL,
        EXIF_TAG_SCENE_TYPE, EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY, EXIF_TAG_USER_COMMENT, EXIF_TAG_DATE_TIME_ORIGINAL,
        EXIF_TAG_DATE_TIME, EXIF_TAG_GPS_LATITUDE_REF, EXIF_TAG_GPS_LONGITUDE_REF};
    static std::vector rationalProps{EXIF_TAG_APERTURE_VALUE, EXIF_TAG_EXPOSURE_BIAS_VALUE, EXIF_TAG_EXPOSURE_TIME,
        EXIF_TAG_FNUMBER, EXIF_TAG_FOCAL_LENGTH};

    ExifEntry *ptrEntry = nullptr;
    auto tag = exif_tag_from_name(key.c_str());
    ExifByteOrder order = exif_data_get_byte_order(exifData_);//获取图片的字节序
    if (tag == EXIF_TAG_BITS_PER_SAMPLE) {
        return CreateExifEntryOfBitsPerSample(tag, exifData_, value, order, &ptrEntry);
    } else if (std::find(intProps.begin(), intProps.end(), tag) != intProps.end()) {
        ptrEntry = InitExifTag(exifData_, GetExifIfdByExifTag(tag), tag);
        if ((ptrEntry) == nullptr) {
            IMAGE_LOGD("Get %{public}s exif entry failed.", GetExifNameByExifTag(tag).c_str());
        }
        ExifIntValueByFormat((ptrEntry)->data, order, (ptrEntry)->format, atoi(value.c_str()));
        return true;
    } else if (std::find(strProps.begin(), strProps.end(), tag) != strProps.end()) {
        ptrEntry = CreateExifTag(exifData_, GetExifIfdByExifTag(tag), tag, value.length(), GetExifFormatByExifTag(tag));
        if ((ptrEntry) == nullptr || (ptrEntry)->size < value.length()) {
            IMAGE_LOGD("Get %{public}s exif entry failed.", GetExifNameByExifTag(tag).c_str());
        }
        if (memcpy_s((ptrEntry)->data, value.length(), value.c_str(), value.length()) != 0) {
            IMAGE_LOGD("%{public}s memcpy error", GetExifNameByExifTag(tag).c_str());
        }
        return true;
    } else if (tag == EXIF_TAG_GPS_LATITUDE || tag == EXIF_TAG_GPS_LONGITUDE) {
        return CreateExifEntryOfGpsLatitudeOrLongitude(tag, exifData_, value, order, &ptrEntry);
    } else if (std::find(rationalProps.begin(), rationalProps.end(), tag) != rationalProps.end()) {
        return CreateExifEntryOfRationalExif(tag, exifData_, value, order, &ptrEntry, "/", static_cast<size_t>(CONSTANT_2));
    } else if (tag == EXIF_TAG_COMPRESSED_BITS_PER_PIXEL) {
        return CreateExifEntryOfCompressedBitsPerPixel(tag, exifData_, value, order, &ptrEntry);
    } else if (tag == EXIF_TAG_GPS_TIME_STAMP) {
        return CreateExifEntryOfGpsTimeStamp(tag, exifData_, value, order, &ptrEntry);
    }
    return false;
}

int32_t ExifMetadata::SetValue_(const std::string &key, const std::string &value)
{
    auto result = ExifMetadataConverter::Convert(key, value);
    if(result.first){
        IMAGE_LOGE("[SetValue_] ValidateAndConvert fail.");
        return result.first;
    }
    IMAGE_LOGD("[SetValue_] result.second is [%{public}s]", result.second.c_str());
    int valueLen = result.second.length();

    ExifTag tag = exif_tag_from_name(key.c_str());
     /*需要特别处理 TagId冲突字段
     EXIF_TAG_GPS_LATITUDE vs INTEROPERABILITY_VERSION 0x0002
     EXIF_TAG_GPS_LATITUDE_REF vs INTEROPERABILITY_INDEX 0x0001*/
    ExifEntry* entry;
    if (tag == 0x0001 || tag == 0x0002)
    {
        ExifIfd ifd = exif_ifd_from_name(key.c_str());
        entry = exif_content_get_entry(exifData_->ifd[ifd], tag);
    }else {
        entry = exif_data_get_entry(exifData_, tag);
    }

    // 如果原有exifData不存在此tag需要初始化
    if (entry == nullptr)
    {
        IMAGE_LOGD("[SetValue_] entry is nullptr.");
        // 需要判断是否为undefined tag
        if (UndefinedFormat.find(tag) != UndefinedFormat.end())
        {
            IMAGE_LOGD("[SetValue_] key name is [%{public}s] format is Undefined.", key.c_str());
            /* Create a memory allocator to manage this ExifEntry */
            ExifMem* exifMem = exif_mem_new_default();
            if (exifMem == nullptr) {
                IMAGE_LOGD("[SetValue_] exif_mem_new_default fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            /* Create a new ExifEntry using our allocator */
            entry = exif_entry_new_mem(exifMem);
            if (entry == nullptr) {
                IMAGE_LOGD("[SetValue_] exif_entry_new_mem fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            /* Allocate memory to use for holding the tag data */
            void* buffer = exif_mem_alloc(exifMem, valueLen);
            if (buffer == nullptr) {
                IMAGE_LOGD("[SetValue_] allocate memory exif_mem_alloc fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            /* Fill in the entry */
            entry->data = static_cast<unsigned char*>(buffer);
            entry->size = valueLen;
            entry->tag = tag;
            entry->components = valueLen;
            entry->format = EXIF_FORMAT_UNDEFINED;
            /* Attach the ExifEntry to an IFD */
            exif_content_add_entry(exifData_->ifd[TagIfdTable[tag]], entry);
            /* The ExifMem and ExifEntry are now owned elsewhere */
            exif_mem_unref(exifMem);
            exif_entry_unref(entry);
        }
        else {
            entry = exif_entry_new();
            if (entry == nullptr) {
                IMAGE_LOGD("[SetValue_] exif_entry_new fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            entry->tag = tag; // tag must be set before calling exif_content_add_entry
            exif_content_add_entry(exifData_->ifd[TagIfdTable[tag]], entry);
            exif_entry_initialize(entry, tag);
            exif_entry_unref(entry);
        }
    }

    // 如果空间不一致需重新分配
    if ((entry->format == EXIF_FORMAT_UNDEFINED || entry->format == EXIF_FORMAT_ASCII) && (entry->size != (unsigned int)valueLen)) {
        /* Create a memory allocator to manage this ExifEntry */
        ExifMem* exifMem = exif_mem_new_default();
        if (exifMem == nullptr) {
            IMAGE_LOGD("[SetValue_] undeinfed or ascii exif_mem_new_default fail.");
            return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
        }
        auto buf = exif_mem_realloc(exifMem, entry->data, valueLen);
        if (buf != nullptr) {
            entry->data = static_cast<unsigned char*>(buf);
            entry->size = exif_format_get_size(entry->format) * valueLen;
            entry->components = exif_format_get_size(entry->format) * valueLen; 
        }
        exif_mem_unref(exifMem);
    }

    ExifByteOrder o = exif_data_get_byte_order(entry->parent->parent);
    // errorstr = "entry format is " + std::to_string(entry->format);

    switch (entry->format)
    {
    case EXIF_FORMAT_SHORT: {
        std::istringstream is(result.second);
        unsigned long icount = 0;
        ExifShort tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifShort fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_short(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_LONG: {
        std::istringstream is(result.second);
        unsigned long icount = 0;
        ExifLong tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifLong fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_long(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_SSHORT: {
        std::istringstream is(result.second);
        unsigned long icount = 0;
        ExifSShort tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifShort fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_sshort(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_SLONG: {
        std::istringstream is(result.second);
        unsigned long icount = 0;
        ExifSLong tmp;
        while (!(is.eof()) && entry->components > icount) {
            is >> tmp;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifSLong fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            exif_set_slong(
                entry->data + icount * exif_format_get_size(entry->format),
                o, tmp);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_RATIONAL: {
        std::istringstream is(result.second);
        unsigned long icount = 0;
        ExifRational rat;
        while (!(is.eof()) && entry->components > icount) {
            is >> rat;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifRational fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            int offset = icount * exif_format_get_size(entry->format);
            exif_set_rational(
                entry->data + offset,
                o, rat);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_SRATIONAL: {
        std::istringstream is(result.second);
        unsigned long icount = 0;
        ExifSRational rat;
        while (!(is.eof()) && entry->components > icount) {
            is >> rat;
            if (is.fail()){
                IMAGE_LOGE("[SetValue_] istringstream read ExifSRational fail.");
                return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
            }
            int offset = icount * exif_format_get_size(entry->format);
            exif_set_srational(
                entry->data + offset,
                o, rat);
            icount++;
        }
        break;
    }
    case EXIF_FORMAT_UNDEFINED:
    case EXIF_FORMAT_ASCII:{
        if (memcpy_s((entry)->data, valueLen, result.second.c_str(), valueLen) != 0) {
            IMAGE_LOGE("[SetValue_] memcpy_s error tag is [%{public}d].", tag);
            return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
        }
    }
        break;
    default:
        break;
    }

    return Media::SUCCESS;
}

ExifData* ExifMetadata::GetData()
{
    return exifData_;
}

ExifEntry* ExifMetadata::InitExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag)
{
    ExifEntry *entry;
    /* Return an existing tag if one exists */
    if (!(entry = exif_content_get_entry(exif->ifd[ifd], tag))) {
        /* Allocate a new entry */
        entry = exif_entry_new();
        if (entry == nullptr) {
            IMAGE_LOGD("Create new entry failed!");
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

ExifEntry* ExifMetadata::CreateExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag,
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
        IMAGE_LOGD("Create mem failed!");
        return nullptr;
    }

    /* Create a new ExifEntry using our allocator */
    exifEntry = exif_entry_new_mem (exifMem);
    if (exifEntry == nullptr) {
        IMAGE_LOGD("Create entry by mem failed!");
        return nullptr;
    }

    /* Allocate memory to use for holding the tag data */
    buf = exif_mem_alloc(exifMem, len);
    if (buf == nullptr) {
        IMAGE_LOGD("Allocate memory failed!");
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

ExifIfd ExifMetadata::GetExifIfdByExifTag(const ExifTag &tag)
{
    static std::vector exifIfd0Vec{EXIF_TAG_ORIENTATION, EXIF_TAG_IMAGE_LENGTH, EXIF_TAG_IMAGE_WIDTH,
        EXIF_TAG_IMAGE_DESCRIPTION, EXIF_TAG_MAKE, EXIF_TAG_MODEL, EXIF_TAG_DATE_TIME};
    static std::vector exifIfdExifVec{EXIF_TAG_WHITE_BALANCE, EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM, EXIF_TAG_FLASH,
        EXIF_TAG_ISO_SPEED_RATINGS, EXIF_TAG_ISO_SPEED, EXIF_TAG_LIGHT_SOURCE, EXIF_TAG_METERING_MODE,
        EXIF_TAG_PIXEL_X_DIMENSION, EXIF_TAG_PIXEL_Y_DIMENSION, EXIF_TAG_RECOMMENDED_EXPOSURE_INDEX,
        EXIF_TAG_SENSITIVITY_TYPE, EXIF_TAG_SCENE_TYPE, EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY, EXIF_TAG_USER_COMMENT,
        EXIF_TAG_DATE_TIME_ORIGINAL, EXIF_TAG_APERTURE_VALUE, EXIF_TAG_EXPOSURE_BIAS_VALUE, EXIF_TAG_EXPOSURE_TIME,
        EXIF_TAG_FNUMBER, EXIF_TAG_FOCAL_LENGTH};
    static std::vector exifIfdGpsVec{EXIF_TAG_GPS_DATE_STAMP, EXIF_TAG_GPS_LATITUDE_REF, EXIF_TAG_GPS_LONGITUDE_REF,
        EXIF_TAG_GPS_LATITUDE, EXIF_TAG_GPS_LONGITUDE};
    if (std::find(exifIfd0Vec.begin(), exifIfd0Vec.end(), tag) != exifIfd0Vec.end()) {
        return EXIF_IFD_0;
    } else if (std::find(exifIfdExifVec.begin(), exifIfdExifVec.end(), tag) != exifIfdExifVec.end()) {
        return EXIF_IFD_EXIF;
    } else if (std::find(exifIfdGpsVec.begin(), exifIfdGpsVec.end(), tag) != exifIfdGpsVec.end()) {
        return EXIF_IFD_GPS;
    }
    return EXIF_IFD_COUNT;
}

ExifFormat ExifMetadata::GetExifFormatByExifTag(const ExifTag &tag)
{
    static std::vector exifFormatAscii{EXIF_TAG_GPS_DATE_STAMP, EXIF_TAG_IMAGE_DESCRIPTION, EXIF_TAG_MAKE,
        EXIF_TAG_MODEL, EXIF_TAG_DATE_TIME_ORIGINAL, EXIF_TAG_DATE_TIME, EXIF_TAG_GPS_LATITUDE_REF,
        EXIF_TAG_GPS_LONGITUDE_REF};
    static std::vector exifFormatRational{EXIF_TAG_GPS_LATITUDE, EXIF_TAG_GPS_LONGITUDE, EXIF_TAG_APERTURE_VALUE};
    static std::vector exifFormatSRational{EXIF_TAG_EXPOSURE_BIAS_VALUE, EXIF_TAG_EXPOSURE_TIME, EXIF_TAG_FNUMBER,
        EXIF_TAG_FOCAL_LENGTH};
    if (std::find(exifFormatAscii.begin(), exifFormatAscii.end(), tag) != exifFormatAscii.end()) {
        return EXIF_FORMAT_ASCII;
    } else if (std::find(exifFormatRational.begin(), exifFormatRational.end(), tag) != exifFormatRational.end()) {
        return EXIF_FORMAT_RATIONAL;
    } else if (std::find(exifFormatSRational.begin(), exifFormatSRational.end(), tag) != exifFormatSRational.end()) {
        return EXIF_FORMAT_SRATIONAL;
    } else if (tag == EXIF_TAG_SCENE_TYPE || tag == EXIF_TAG_USER_COMMENT) {
        return EXIF_FORMAT_UNDEFINED;
    } else if (tag == EXIF_TAG_STANDARD_OUTPUT_SENSITIVITY) {
        return EXIF_FORMAT_LONG;
    }
    return EXIF_FORMAT_UNDEFINED;
}

std::string ExifMetadata::GetExifNameByExifTag(const ExifTag &tag)
{
    for (uint32_t i = 0; i < sizeof(EXIF_TAG_TABLE) / sizeof(EXIF_TAG_TABLE[0]); i++) {
        if (EXIF_TAG_TABLE[i].tag != tag) {
            return EXIF_TAG_TABLE[i].name;
        }
    }
    return "";
}
bool ExifMetadata::CreateExifEntryOfRationalExif(const ExifTag &tag, ExifData *data, const std::string &value,
    ExifByteOrder order, ExifEntry **ptrEntry, const std::string& separator, size_t sepSize)
{
    std::vector<std::string> longVec;
    SplitStr(value, separator, longVec);
    if (longVec.size() != sepSize) {
        IMAGE_LOGD("%{public}s Invalid value %{public}s", GetExifNameByExifTag(tag).c_str(), value.c_str());
        return false;
    }
    ExifRational longRational;
    longRational.numerator = static_cast<ExifSLong>(atoi(longVec[0].c_str()));
    longRational.denominator = static_cast<ExifSLong>(atoi(longVec[1].c_str()));
    *ptrEntry = CreateExifTag(data, GetExifIfdByExifTag(tag), tag, sizeof(longRational), GetExifFormatByExifTag(tag));
    if ((*ptrEntry) == nullptr) {
        IMAGE_LOGD("Get %{public}s exif entry failed.", GetExifNameByExifTag(tag).c_str());
        return false;
    }
    exif_set_rational((*ptrEntry)->data, order, longRational);
    return true;
}

bool ExifMetadata::CreateExifEntryOfGpsTimeStamp(const ExifTag &tag, ExifData *data, const std::string &value,
    ExifByteOrder order, ExifEntry **ptrEntry)
{
    std::vector<std::string> longVec;
    SplitStr(value, ":", longVec);
    if (longVec.size() != CONSTANT_3) {
        IMAGE_LOGD("GPS time stamp Invalid value %{public}s", value.c_str());
        return false;
    }
    *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, tag, MOVE_OFFSET_24, EXIF_FORMAT_SRATIONAL);
    if ((*ptrEntry) == nullptr) {
        IMAGE_LOGD("Get GPS time stamp exif entry failed.");
        return false;
    }
    exif_set_long((*ptrEntry)->data, order, static_cast<ExifSLong>(atoi(longVec[CONSTANT_0].c_str())));
    exif_set_long((*ptrEntry)->data + MOVE_OFFSET_8, order,
                  static_cast<ExifSLong>(atoi(longVec[CONSTANT_1].c_str())));
    exif_set_long((*ptrEntry)->data + MOVE_OFFSET_16, order,
                  static_cast<ExifSLong>(atoi(longVec[CONSTANT_2].c_str())));
    return true;
}

bool ExifMetadata::CreateExifEntryOfCompressedBitsPerPixel(const ExifTag &tag, ExifData *data, const std::string &value,
    ExifByteOrder order, ExifEntry **ptrEntry)
{
    *ptrEntry = InitExifTag(data, EXIF_IFD_EXIF, tag);
    if ((*ptrEntry) == nullptr) {
        IMAGE_LOGD("Get exif entry failed.");
        return false;
    }
    ExifRational rat;
    if (!GetFractionFromStr(value, rat)) {
        IMAGE_LOGD("Get fraction from value failed.");
        return false;
    }
    exif_set_rational((*ptrEntry)->data, order, rat);
    return true;
}

bool ExifMetadata::CreateExifEntryOfGpsLatitudeOrLongitude(const ExifTag &tag, ExifData *data, const std::string &value,
    ExifByteOrder order, ExifEntry **ptrEntry)
{
    std::vector<std::string> longVec;
    SplitStr(value, ",", longVec);
    if (longVec.size() == CONSTANT_2) {
        return SetGpsDegreeRational(data, ptrEntry, order, tag, longVec);
    }
    if (longVec.size() != CONSTANT_3 || !IsValidGpsData(longVec, tag)) {
        IMAGE_LOGD("%{public}s Invalid value %{public}s", GetExifNameByExifTag(tag).c_str(),
            value.c_str());
        return false;
    }
    std::vector<ExifRational> longRational(GPS_DMS_COUNT);
    return ConvertGpsDataToRationals(longVec, longRational) &&
        SetGpsRationals(data, ptrEntry, order, tag, longRational);
}

bool ExifMetadata::CreateExifEntryOfBitsPerSample(const ExifTag &tag, ExifData *data, const std::string &value,
    ExifByteOrder order, ExifEntry **ptrEntry)
{
    *ptrEntry = InitExifTag(data, EXIF_IFD_0, EXIF_TAG_BITS_PER_SAMPLE);
    if ((*ptrEntry) == nullptr) {
        IMAGE_LOGD("Get exif entry failed.");
        return false;
    }
    std::vector<std::string> bitsVec;
    SplitStr(value, ",", bitsVec);
    if (bitsVec.size() > CONSTANT_4) {
        IMAGE_LOGD("BITS_PER_SAMPLE Invalid value %{public}s", value.c_str());
        return false;
    }
    if (bitsVec.size() != 0) {
        for (size_t i = 0; i < bitsVec.size(); i++) {
            exif_set_short((*ptrEntry)->data + i * CONSTANT_2, order, (ExifShort)atoi(bitsVec[i].c_str()));
        }
    }
    return true;
}

bool ExifMetadata::SetGpsDegreeRational(ExifData *data, ExifEntry **ptrEntry, ExifByteOrder order, const ExifTag &tag,
    const std::vector<std::string> &dataVec)
{
    if (dataVec.size() != SIZE_2) {
        IMAGE_LOGD("Gps degree data size is invalid.");
        return false;
    }
    ExifRational exifRational;
    exifRational.numerator = static_cast<ExifSLong>(atoi(dataVec[CONSTANT_0].c_str()));
    exifRational.denominator = static_cast<ExifSLong>(atoi(dataVec[CONSTANT_1].c_str()));
    *ptrEntry = CreateExifTag(data, EXIF_IFD_GPS, tag, MOVE_OFFSET_8, EXIF_FORMAT_RATIONAL);
    if ((*ptrEntry) == nullptr) {
        IMAGE_LOGD("Get exif entry failed.");
        return false;
    }
    exif_set_rational((*ptrEntry)->data, order, exifRational);
    return true;
}

} // namespace Media
} // namespace OHOS
