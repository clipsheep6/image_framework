/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <cmath>
#include <iostream>
#include <regex>
#include <string_view>
#include <set>
#include <utility>

#include "exif_metadata_formatter.h"
#include "hilog/log_cpp.h"
#include "hilog/log.h"
#include "image_log.h"
#include "media_errors.h"
#include "string_ex.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ExifMetadatFormatter"

namespace OHOS {
namespace Media {
enum PropertyPermision {
    READ_ONLY = 1,
    READ_WRITE,
};

const std::map<std::string, PropertyPermision> SUPPORTKEYS = {
    {"BitsPerSample", READ_WRITE},
    {"Orientation", READ_WRITE},
    {"ImageLength", READ_WRITE},
    {"ImageWidth", READ_WRITE},
    {"GPSLatitude", READ_WRITE},
    {"GPSLongitude", READ_WRITE},
    {"GPSLatitudeRef", READ_WRITE},
    {"GPSLongitudeRef", READ_WRITE},
    {"DateTimeOriginal", READ_WRITE},
    {"ExposureTime", READ_WRITE},
    {"FNumber", READ_WRITE},
    {"ISOSpeed", READ_WRITE},
    {"ISOSpeedRatings", READ_WRITE},
    {"SceneType", READ_WRITE},
    {"DateTime", READ_WRITE},
    {"GPSTimeStamp", READ_WRITE},
    {"GPSDateStamp", READ_WRITE},
    {"ImageDescription", READ_WRITE},
    {"Make", READ_WRITE},
    {"Model", READ_WRITE},
    {"SensitivityType", READ_WRITE},
    {"StandardOutputSensitivity", READ_WRITE},
    {"RecommendedExposureIndex", READ_WRITE},
    {"ApertureValue", READ_WRITE},
    {"ExposureBiasValue", READ_WRITE},
    {"MeteringMode", READ_WRITE},
    {"LightSource", READ_WRITE},
    {"Flash", READ_WRITE},
    {"FocalLength", READ_WRITE},
    {"UserComment", READ_WRITE},
    {"PixelXDimension", READ_WRITE},
    {"PixelYDimension", READ_WRITE},
    {"WhiteBalance", READ_WRITE},
    {"FocalLengthIn35mmFilm", READ_WRITE},
    {"Compression", READ_WRITE},
    {"PhotometricInterpretation", READ_WRITE},
    {"StripOffsets", READ_WRITE},
    {"SamplesPerPixel", READ_WRITE},
    {"RowsPerStrip", READ_WRITE},
    {"StripByteCounts", READ_WRITE},
    {"XResolution", READ_WRITE},
    {"YResolution", READ_WRITE},
    {"PlanarConfiguration", READ_WRITE},
    {"ResolutionUnit", READ_WRITE},
    {"TransferFunction", READ_WRITE},
    {"Software", READ_WRITE},
    {"Artist", READ_WRITE},
    {"WhitePoint", READ_WRITE},
    {"PrimaryChromaticities", READ_WRITE},
    {"ReferenceBlackWhite", READ_WRITE},
    {"Copyright", READ_WRITE},
    {"JPEGInterchangeFormat", READ_WRITE},
    {"JPEGInterchangeFormatLength", READ_WRITE},
    {"ExposureProgram", READ_WRITE},
    {"SpectralSensitivity", READ_WRITE},
    {"OECF", READ_WRITE},
    {"ExifVersion", READ_WRITE},
    {"DateTimeDigitized", READ_WRITE},
    {"ComponentsConfiguration", READ_WRITE},
    {"ShutterSpeedValue", READ_WRITE},
    {"BrightnessValue", READ_WRITE},
    {"MaxApertureValue", READ_WRITE},
    {"SubjectDistance", READ_WRITE},
    {"SubjectArea", READ_WRITE},
    {"MakerNote", READ_WRITE},
    {"SubsecTime", READ_WRITE},
    {"SubSecTimeOriginal", READ_WRITE},
    {"SubSecTimeDigitized", READ_WRITE},
    {"FlashpixVersion", READ_WRITE},
    {"ColorSpace", READ_WRITE},
    {"RelatedSoundFile", READ_WRITE},
    {"FlashEnergy", READ_WRITE},
    {"SpatialFrequencyResponse", READ_WRITE},
    {"FocalPlaneXResolution", READ_WRITE},
    {"FocalPlaneYResolution", READ_WRITE},
    {"FocalPlaneResolutionUnit", READ_WRITE},
    {"SubjectLocation", READ_WRITE},
    {"ExposureIndex", READ_WRITE},
    {"SensingMethod", READ_WRITE},
    {"FileSource", READ_WRITE},
    {"CFAPattern", READ_WRITE},
    {"CustomRendered", READ_WRITE},
    {"ExposureMode", READ_WRITE},
    {"DigitalZoomRatio", READ_WRITE},
    {"SceneCaptureType", READ_WRITE},
    {"GainControl", READ_WRITE},
    {"Contrast", READ_WRITE},
    {"Saturation", READ_WRITE},
    {"Sharpness", READ_WRITE},
    {"DeviceSettingDescription", READ_WRITE},
    {"SubjectDistanceRange", READ_WRITE},
    {"ImageUniqueID", READ_WRITE},
    {"GPSVersionID", READ_WRITE},
    {"GPSAltitudeRef", READ_WRITE},
    {"GPSAltitude", READ_WRITE},
    {"GPSSatellites", READ_WRITE},
    {"GPSStatus", READ_WRITE},
    {"GPSMeasureMode", READ_WRITE},
    {"GPSDOP", READ_WRITE},
    {"GPSSpeedRef", READ_WRITE},
    {"GPSSpeed", READ_WRITE},
    {"GPSTrackRef", READ_WRITE},
    {"GPSTrack", READ_WRITE},
    {"GPSImgDirectionRef", READ_WRITE},
    {"GPSImgDirection", READ_WRITE},
    {"GPSMapDatum", READ_WRITE},
    {"GPSDestLatitudeRef", READ_WRITE},
    {"GPSDestLatitude", READ_WRITE},
    {"GPSDestLongitudeRef", READ_WRITE},
    {"GPSDestLongitude", READ_WRITE},
    {"GPSDestBearingRef", READ_WRITE},
    {"GPSDestBearing", READ_WRITE},
    {"GPSDestDistanceRef", READ_WRITE},
    {"GPSDestDistance", READ_WRITE},
    {"GPSProcessingMethod", READ_WRITE},
    {"GPSAreaInformation", READ_WRITE},
    {"GPSDifferential", READ_WRITE},
    {"YCbCrCoefficients", READ_WRITE},
    {"YCbCrSubSampling", READ_WRITE},
    {"YCbCrPositioning", READ_WRITE},
    {"CompressedBitsPerPixel", READ_WRITE},
    {"JPEGProc", READ_WRITE},
    {"BodySerialNumber", READ_WRITE},
    {"CameraOwnerName", READ_WRITE},
    {"CompositeImage", READ_WRITE},
    {"CompressedBitsPerPixel", READ_WRITE},
    {"DNGVersion", READ_WRITE},
    {"DefaultCropSize", READ_WRITE},
    {"Gamma", READ_WRITE},
    {"ISOSpeedLatitudeyyy", READ_WRITE},
    {"ISOSpeedLatitudezzz", READ_WRITE},
    {"LensMake", READ_WRITE},
    {"LensModel", READ_WRITE},
    {"LensSerialNumber", READ_WRITE},
    {"LensSpecification", READ_WRITE},
    {"NewSubfileType", READ_WRITE},
    {"OffsetTime", READ_WRITE},
    {"OffsetTimeDigitized", READ_WRITE},
    {"OffsetTimeOriginal", READ_WRITE},
    {"SourceExposureTimesOfCompositeImage", READ_WRITE},
    {"SourceImageNumberOfCompositeImage", READ_WRITE},
    {"SubfileType", READ_WRITE},
    {"GPSHPositioningError", READ_WRITE},
    {"HwMnoteCaptureMode", READ_WRITE},
    {"HwMnotePhysicalAperture", READ_ONLY},
    {"HwMnoteRollAngle", READ_ONLY},
    {"HwMnotePitchAngle", READ_ONLY},
    {"HwMnoteSceneFoodConf", READ_ONLY},
    {"HwMnoteSceneStageConf", READ_ONLY},
    {"HwMnoteSceneBlueSkyConf", READ_ONLY},
    {"HwMnoteSceneGreenPlantConf", READ_ONLY},
    {"HwMnoteSceneBeachConf", READ_ONLY},
    {"HwMnoteSceneSnowConf", READ_ONLY},
    {"HwMnoteSceneSunsetConf", READ_ONLY},
    {"HwMnoteSceneFlowersConf", READ_ONLY},
    {"HwMnoteSceneNightConf", READ_ONLY},
    {"HwMnoteSceneTextConf", READ_ONLY},
    {"HwMnoteFaceCount", READ_ONLY},
    {"HwMnoteFocusMode", READ_ONLY},
    {"HwMnoteFrontCamera", READ_ONLY},
    {"HwMnoteSceneVersion", READ_ONLY},
    {"HwMnoteScenePointer", READ_ONLY},
    {"HwMnoteFacePointer", READ_ONLY},
    {"HwMnoteBurstNumber", READ_ONLY},
    {"HwMnoteFaceVersion", READ_ONLY},
    {"HwMnoteFaceConf", READ_ONLY},
    {"HwMnoteFaceSmileScore", READ_ONLY},
    {"HwMnoteFaceRect", READ_ONLY},
    {"HwMnoteFaceLeyeCenter", READ_ONLY},
    {"HwMnoteFaceReyeCenter", READ_ONLY},
    {"HwMnoteFaceMouthCenter", READ_ONLY},
};

// Orientation, tag 0x0112
constexpr TagDetails exifOrientation[] = {
    {1, "top, left"},     {2, "top, right"},   {3, "bottom, right"},
    {4, "bottom, left"},  {5, "left, top"},    {6, "right, top"},
    {7, "right, bottom" }, {8, "left, bottom"},
};

// GPS latitude reference, tag 0x0001; also GPSDestLatitudeRef, tag 0x0013
constexpr TagDetails exifGPSLatitudeRef[] = {
    {78, "North"},
    {83, "South"},
};

constexpr TagDetails exifGPSLongitudeRef[] = {
    {69, "East"},
    {87, "West"},
};

// WhiteBalance, tag 0xa403
constexpr TagDetails exifWhiteBalance[] = {
    {0, "Auto"},
    {1, "Manual"},
};

// Flash, Exif tag 0x9209
constexpr TagDetails exifFlash[] = {
    {0x00, "No flash"},
    {0x01, "Fired"},
    {0x05, "Fired, return light not detected"},
    {0x07, "Fired, return light detected"},
    {0x08, "Yes, did not fire"},
    {0x09, "Yes, compulsory"},
    {0x0d, "Yes, compulsory, return light not detected"},
    {0x0f, "Yes, compulsory, return light detected"},
    {0x10, "No, compulsory"},
    {0x14, "No, did not fire, return light not detected"},
    {0x18, "No, auto"},
    {0x19, "Yes, auto"},
    {0x1d, "Yes, auto, return light not detected"},
    {0x1f, "Yes, auto, return light detected"},
    {0x20, "No flash function"},
    {0x20, "No, no flash function"},
    {0x41, "Yes, red-eye reduction"},
    {0x45, "Yes, red-eye reduction, return light not detected"},
    {0x47, "Yes, red-eye reduction, return light detected"},
    {0x49, "Yes, compulsory, red-eye reduction"},
    {0x4d, "Yes, compulsory, red-eye reduction, return light not detected"},
    {0x4f, "Yes, compulsory, red-eye reduction, return light detected"},
    {0x50, "No, red-eye reduction"},
    {0x58, "No, auto, red-eye reduction"},
    {0x59, "Yes, auto, red-eye reduction"},
    {0x5d, "Yes, auto, red-eye reduction, return light not detected"},
    {0x5f, "Yes, auto, red-eye reduction, return light detected"},
};

// ColorSpace, tag 0xa001
constexpr TagDetails exifColorSpace[] = {
    {1, "sRGB"},
    {2, "Adobe RGB"},  // Not defined to Exif 2.2 spec. But used by a lot of cameras.
    {0xffff, "Uncalibrated"},
};

// LightSource, tag 0x9208
constexpr TagDetails exifLightSource[] = {
    {0, "Unknown"},
    {1, "Daylight"},
    {2, "Fluorescent"},
    {3, "Tungsten (incandescent light)"},
    {4, "Flash"},
    {9, "Fine weather"},
    {10, "Cloudy weather"},
    {11, "Shade"},
    {12, "Daylight fluorescent (D 5700 - 7100K)"},
    {13, "Day white fluorescent (N 4600 - 5400K)"},
    {14, "Cool white fluorescent (W 3900 - 4500K)"},
    {15, "White fluorescent (WW 3200 - 3700K)"},
    {17, "Standard light A"},
    {18, "Standard light B"},
    {19, "Standard light C"},
    {20, "D55"},
    {21, "D65"},
    {22, "D75"},
    {23, "D50"},
    {24, "ISO studio tungsten"},
    {255, "Other light source"},
};

// MeteringMode, tag 0x9207
constexpr TagDetails exifMeteringMode[] = {
    {0, "Unknown"}, {1, "Average"},    {2, "Center weighted average"},
    {3, "Spot"},    {4, "Multi-spot"}, {5, "Multi-segment"},
    {6, "Partial"}, {255, "Other"},
};

// SceneType, tag 0xa301
constexpr TagDetails exifSceneType[] = {
    {1, "Directly photographed"},
};

// Compression, tag 0x0103
constexpr TagDetails exifCompression[] = {
    {1, "Uncompressed"},
    {2, "CCITT RLE"},
    {3, "T4/Group 3 Fax"},
    {4, "T6/Group 4 Fax"},
    {5, "LZW"},
    {6, "JPEG (old-style)"},
    {7, "JPEG"},
    {8, "Adobe Deflate"},
    {9, "JBIG B&W"},
    {10, "JBIG Color"},
    {32766, "Next 2-bits RLE"},
    {32767, "Sony ARW Compressed"},
    {32769, "Epson ERF Compressed"},
    {32770, "Samsung SRW Compressed"},
    {32771, "CCITT RLE 1-word"},
    {32773, "PackBits (Macintosh RLE)"},
    {32809, "Thunderscan RLE"},
    {32895, "IT8 CT Padding"},
    {32896, "IT8 Linework RLE"},
    {32897, "IT8 Monochrome Picture"},
    {32898, "IT8 Binary Lineart"},
    {32908, "Pixar Film (10-bits LZW)"},
    {32909, "Pixar Log (11-bits ZIP)"},
    {32946, "Pixar Deflate"},
    {32947, "Kodak DCS Encoding"},
    {34661, "ISO JBIG"},
    {34676, "SGI Log Luminance RLE"},
    {34677, "SGI Log 24-bits packed"},
    {34712, "Leadtools JPEG 2000"},
    {34713, "Nikon NEF Compressed"},
    {34892, "JPEG (lossy)"},  // DNG 1.4
    {52546, "JPEG XL"},       // DNG 1.7
    {65000, "Kodak DCR Compressed"},
    {65535, "Pentax PEF Compressed"},
};

// PhotometricInterpretation, tag 0x0106
constexpr TagDetails exifPhotometricInterpretation[] = {
    {0, "White Is Zero"},
    {1, "Black Is Zero"},
    {2, "RGB"},
    {3, "RGB Palette"},
    {4, "Transparency Mask"},
    {5, "CMYK"},
    {6, "YCbCr"},
    {8, "CIELab"},
    {9, "ICCLab"},
    {10, "ITULab"},
    {32803, "Color Filter Array"},
    {32844, "Pixar LogL"},
    {32845, "Pixar LogLuv"},
    {34892, "Linear Raw"},
    {51177, "Depth Map"},         // DNG 1.5
    {52527, "Photometric Mask"},  // DNG 1.6
};

// PlanarConfiguration, tag 0x011c
constexpr TagDetails exifPlanarConfiguration[] = {
    {1, "Chunky"},
    {2, "Planar"},
};

// Units for measuring X and Y resolution, tags 0x0128, 0xa210
constexpr TagDetails exifUnit[] = {
    {1, "none"},
    {2, "inch"},
    {3, "cm"},
};

// YCbCrPositioning, tag 0x0213
constexpr TagDetails exifYCbCrPositioning[] = {
    {1, "Centered"},
    {2, "Co-sited"},
};

// ExposureProgram, tag 0x8822
constexpr TagDetails exifExposureProgram[] = {
    {0, "Not defined"},       {1, "Manual"},           {2, "Auto"},
    {3, "Aperture priority"}, {4, "Shutter priority"}, {5, "Creative program"},
    {6, "Action program"},    {7, "Portrait mode"},    {8, "Landscape mode"},
};

// SensingMethod, TIFF/EP tag 0x9217
constexpr TagDetails tiffSensingMethod[] = {
    {0, "Undefined"},           {1, "Monochrome area"},       {2, "One-chip color area"},
    {3, "Two-chip color area"}, {4, "Three-chip color area"}, {5, "Color sequential area"},
    {6, "Monochrome linear"},   {7, "Trilinear sensor"},      {8, "Color sequential linear"},
};

// CustomRendered, tag 0xa401
constexpr TagDetails exifCustomRendered[] = {
    {0, "Normal process"},
    {1, "Custom process"},
};

// ExposureMode, tag 0xa402
constexpr TagDetails exifExposureMode[] = {
    {0, "Auto"},
    {1, "Manual"},
    {2, "Auto bracket"},
};

// SceneCaptureType, tag 0xa406
constexpr TagDetails exifSceneCaptureType[] = {
    {0, "Standard"},
    {1, "Landscape"},
    {2, "Portrait"},
    {3, "Night scene"}
};

// GainControl, tag 0xa407
constexpr TagDetails exifGainControl[] = {
    {0, "None"},          {1, "Low gain up"},    {2, "High gain up"},
    {3, "Low gain down"}, {4, "High gain down"},
};

// Contrast, tag 0xa408 and Sharpness, tag 0xa40a
constexpr TagDetails exifNormalSoftHard[] = {
    {0, "Normal"},
    {1, "Soft"},
    {2, "Hard"},
};

// Saturation, tag 0xa409
constexpr TagDetails exifSaturation[] = {
    {0, "Normal"},
    {1, "Low"},
    {2, "High"},
};

// SubjectDistanceRange, tag 0xa40c
constexpr TagDetails exifSubjectDistanceRange[] = {
    {0, "Unknown"},
    {1, "Macro"},
    {2, "Close view"},
    {3, "Distant view"}
};

// GPS altitude reference, tag 0x0005
constexpr TagDetails exifGPSAltitudeRef[] = {
    {0, "Above sea level"},
    {1, "Below sea level"},
};

// NewSubfileType, TIFF tag 0x00fe - this is actually a bitmask
constexpr TagDetails exifNewSubfileType[] = {
    {0, "Primary image"},
    {1, "Thumbnail/Preview image"},
    {2, "Primary image, Multi page file"},
    {3, "Thumbnail/Preview image, Multi page file"},
    {4, "Primary image, Transparency mask"},
    {5, "Thumbnail/Preview image, Transparency mask"},
    {6, "Primary image, Multi page file, Transparency mask"},
    {7, "Thumbnail/Preview image, Multi page file, Transparency mask"},
    {8, "Primary image, Depth map"},                  // DNG 1.5
    {9, "Thumbnail/Preview image, Depth map"},        // DNG 1.5
    {16, "Enhanced image"},                           // DNG 1.5 (clashes w/ TIFF-FX)
    {65537, "Thumbnail/Preview image, Alternative"},  // DNG 1.2
    {65540, "Primary image, Semantic mask"}           // DNG 1.6
};

// SubfileType, TIFF tag 0x00ff
constexpr TagDetails exifSubfileType[] = {
    {1, "Full-resolution image data"},
    {2, "Reduced-resolution image data"},
    {3, "A single page of a multi-page image"},
};

// GPS status, tag 0x0009
constexpr TagDetails exifGpsStatus[] = {
    {'A', "Measurement in progress"},
    {'V', "Measurement interrupted"},
};

// GPS measurement mode, tag 0x000a
constexpr TagDetails exifGPSMeasureMode[] = {
    {2, "2-dimensional measurement"},
    {3, "3-dimensional measurement"},
};

// GPS speed reference, tag 0x000c
constexpr TagDetails exifGPSSpeedRef[] = {
    {'K', "km/h"},
    {'M', "mph"},
    {'N', "knots"},
};

// GPS direction reference, tags 0x000e, 0x0010, 0x0017
constexpr TagDetails exifGPSDirRef[] = {
    {'T', "True direction"},
    {'M', "Magnetic direction"},
};

// GPS destination distance reference, tag 0x0019
constexpr TagDetails exifGPSDestDistanceRef[] = {
    {'K', "km"},
    {'M', "miles"},
    {'N', "nautical miles"},
};

// GPS differential correction, tag 0x001e
constexpr TagDetails exifGPSDifferential[] = {
    {0, "Without correction"},
    {1, "Correction applied"},
};

// CompositeImage, tag 0xa460
constexpr TagDetails exifCompositeImage[] = {
    {0, "Unknown"},
    {1, "NonComposite"},
    {2, "GeneralComposite"},
    {3, "CompositeCapturedWhenShooting"},
};

// configuratioin for value range validation. For example GPSLatitudeRef the value must be 'N' or 'S'.
std::map<std::string, std::tuple<const TagDetails *, const size_t>> ExifMetadatFormatter::valueRangeValidateConfig = {
    {"Orientation", std::make_tuple(exifOrientation, std::size(exifOrientation))},
    {"GPSLatitudeRef", std::make_tuple(exifGPSLatitudeRef, std::size(exifGPSLatitudeRef))},
    {"GPSLongitudeRef", std::make_tuple(exifGPSLongitudeRef, std::size(exifGPSLongitudeRef))},
    {"WhiteBalance", std::make_tuple(exifWhiteBalance, std::size(exifWhiteBalance))},
    {"Flash", std::make_tuple(exifFlash, std::size(exifFlash))},
    {"LightSource", std::make_tuple(exifLightSource, std::size(exifLightSource))},
    {"MeteringMode", std::make_tuple(exifMeteringMode, std::size(exifMeteringMode))},
    {"SceneType", std::make_tuple(exifSceneType, std::size(exifSceneType))},
    {"Compression", std::make_tuple(exifCompression, std::size(exifCompression))},
    {"PhotometricInterpretation", std::make_tuple(exifPhotometricInterpretation,
        std::size(exifPhotometricInterpretation))},
    {"PlanarConfiguration", std::make_tuple(exifPlanarConfiguration, std::size(exifPlanarConfiguration))},
    {"ResolutionUnit", std::make_tuple(exifUnit, std::size(exifUnit))},
    {"YCbCrPositioning", std::make_tuple(exifYCbCrPositioning, std::size(exifYCbCrPositioning))},
    {"ExposureProgram", std::make_tuple(exifExposureProgram, std::size(exifExposureProgram))},
    {"ColorSpace", std::make_tuple(exifColorSpace, std::size(exifColorSpace))},
    {"FocalPlaneResolutionUnit", std::make_tuple(exifUnit, std::size(exifUnit))},
    {"SensingMethod", std::make_tuple(tiffSensingMethod, std::size(tiffSensingMethod))},
    {"CustomRendered", std::make_tuple(exifCustomRendered, std::size(exifCustomRendered))},
    {"ExposureMode", std::make_tuple(exifExposureMode, std::size(exifExposureMode))},
    {"SceneCaptureType", std::make_tuple(exifSceneCaptureType, std::size(exifSceneCaptureType))},
    {"GainControl", std::make_tuple(exifGainControl, std::size(exifGainControl))},
    {"Contrast", std::make_tuple(exifNormalSoftHard, std::size(exifNormalSoftHard))},
    {"Saturation", std::make_tuple(exifSaturation, std::size(exifSaturation))},
    {"Sharpness", std::make_tuple(exifNormalSoftHard, std::size(exifNormalSoftHard))},
    {"SubjectDistanceRange", std::make_tuple(exifSubjectDistanceRange, std::size(exifSubjectDistanceRange))},
    {"GPSAltitudeRef", std::make_tuple(exifGPSAltitudeRef, std::size(exifGPSAltitudeRef))},
    {"NewSubfileType", std::make_tuple(exifNewSubfileType, std::size(exifNewSubfileType))},
    {"SubfileType", std::make_tuple(exifSubfileType, std::size(exifSubfileType))},
    {"GPSStatus", std::make_tuple(exifGpsStatus, std::size(exifGpsStatus))},
    {"GPSMeasureMode", std::make_tuple(exifGPSMeasureMode, std::size(exifGPSMeasureMode))},
    {"GPSSpeedRef", std::make_tuple(exifGPSSpeedRef, std::size(exifGPSSpeedRef))},
    {"GPSImgDirectionRef", std::make_tuple(exifGPSDirRef, std::size(exifGPSDirRef))},
    {"GPSDestDistanceRef", std::make_tuple(exifGPSDestDistanceRef, std::size(exifGPSDestDistanceRef))},
    {"GPSDifferential", std::make_tuple(exifGPSDifferential, std::size(exifGPSDifferential))},
    {"CompositeImage", std::make_tuple(exifCompositeImage, std::size(exifCompositeImage))},
};

const std::string COMMAREGEX("\\,"), COLONREGEX("\\:"), DOTREGEX("\\.");

const auto ONE_RATIONAL_REGEX = R"(^[0-9]+/[1-9][0-9]*$)";
const auto ONE_INT_REGEX = R"(^[0-9]+$)";
const auto ONEDECIMALREGEX = "(\\d+)(\\.\\d+)?";
const auto DOUBLE_INT_WITH_BLANK_REGEX = R"(^[0-9]+\s[0-9]+$)";
const auto DOUBLE_INT_WITH_COMMA_REGEX = R"(^[0-9]+,[0-9]+$)";
const auto TRIBLE_INT_WITH_BLANK_REGEX = R"(^[0-9]+\s[0-9]+\s[0-9]+$)";
const auto TRIBLE_INT_WITH_COMMA_REGEX = R"(^[0-9]+,[0-9]+,[0-9]+$)";
const auto TRIBLE_RATIONAL_WITH_BLANK_REGEX = R"(^[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*$)";
const auto TRIBLE_INT_NZ_WITH_BLANK_REGEX = R"(^[1-9][0-9]*\s[1-9][0-9]*\s[1-9][0-9]*$)";
const auto TRIBLE_INT_NZ_WITH_COMMA_REGEX = R"(^[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*$)";
const auto TRIBLE_DECIMAL_WITH_BLANK_REGEX = "(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?";
const auto TRIBLE_DECIMAL_WITH_COMMA_REGEX = "(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?";
const auto TRIBLE_INT_WITH_COLON_REGEX = R"(^[1-9][0-9]*:[1-9][0-9]*:[1-9][0-9]*$)";
const auto TRIBLE_INT_WITH_DOT_REGEX = R"(^[0-9]+.[0-9]+.[0-9]+.[0-9]+$)";
const auto FOUR_INT_WITH_BLANK_REGEX = R"(^[0-9]+\s[0-9]+\s[0-9]+\s[0-9]+$)";
const auto FOUR_INT_WITH_COMMA_REGEX = R"(^[0-9]+,[0-9]+,[0-9]+,[0-9]+$)";
const auto FOUR_INT_NZ_WITH_COMMA_REGEX = R"(^[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*$)";
const auto FOUR_RATIONAL_WITH_BLANK_REGEX = R"(^[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*\s[0-9]
                                                               +/[1-9][0-9]*$)";
const auto FOUR_DECIMAL_WITH_BLANK_REGEX = "(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?";
const auto FOUR_DECIMAL_WITH_COMMA_REGEX = "(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?";
const auto DATETIME_REGEX = R"(^[0-9]{4}:[0-9]{2}:[0-9]{2}\s[0-9]{2}:[0-9]{2}:[0-9]{2}$)";
const auto DATE_REGEX = R"(^[0-9]{4}:[0-9]{2}:[0-9]{2}$)";

/*
 * validate the key is in value range array.
 * For example GPSLatitudeRef value should be 'N' or 'S' in exifGPSLatitudeRef array.
 */
bool ExifMetadatFormatter::IsValidValue(const TagDetails *array, const size_t &size, const int64_t &key)
{
    if (array == nullptr) {
        return false;
    }

    for (size_t i = 0; i < size; i++) {
        if (array[i].val_ == key) {
            return true;
        }
    }
    return false;
}

// validate regex only
bool ExifMetadatFormatter::ValidRegex(const std::string &value, const std::string &regex)
{
    IMAGE_LOGD("[validRegex] value is [%{public}s] regex is [%{public}s].", value.c_str(), regex.c_str());
    std::regex ratPattern(regex);
    if (!std::regex_match(value, ratPattern)) {
        IMAGE_LOGD("validRegex fail. value is [%{public}s] regex is [%{public}s].", value.c_str(), regex.c_str());
        return false;
    }
    return true;
}


// replace as space according to regex
void ExifMetadatFormatter::ReplaceAsSpace(std::string &value, const std::string &regex)
{
    std::regex pattern(regex);
    value = std::regex_replace(value, pattern, " ");
    IMAGE_LOGD("replaceAsSpace [%{public}s]", value.c_str());
}

// validate the regex & replace comma as space
bool ExifMetadatFormatter::ValidRegexWithComma(std::string &value, const std::string &regex)
{
    IMAGE_LOGD("[validRegexWithComma] value is [%{public}s] regex is [%{public}s].", value.c_str(), regex.c_str());
    if (!ValidRegex(value, regex)) {
        return false;
    }
    ReplaceAsSpace(value, COMMAREGEX);
    return true;
}

// convert integer to rational format. For example 23 15 83 --> 23/1 15/1 83/1
void ExifMetadatFormatter::RationalFormat(std::string &value)
{
    std::regex pattern("\\d+"); // regex for integer
    std::string result;
    int icount = 0;
    while (std::regex_search(value, pattern)) {
        std::smatch match;
        if (!std::regex_search(value, match, pattern)) {
            break; // break since there is no matched value
        }
        if (icount != 0) {
            result += " ";
        }
        result += match.str() + "/1"; // appending '/1' to integer
        value = match.suffix().str(); // skip handled value part
        icount++;
    }
    value = result;
}

// convert decimal to rational string. 2.5 -> 5/2
std::string ExifMetadatFormatter::GetFractionFromStr(const std::string &decimal)
{
    int intPart = stoi(decimal.substr(0, decimal.find(".")));
    double decPart = stod(decimal.substr(decimal.find(".")));

    int numerator = decPart * pow(10, decimal.length() - decimal.find(".") - 1);
    int denominator = pow(10, decimal.length() - decimal.find(".") - 1);

    int gcdVal = ExifMetadatFormatter::Gcd(numerator, denominator);
    if (gcdVal == 0) {
        return nullptr;
    }
    numerator /= gcdVal;
    denominator /= gcdVal;

    numerator += intPart * denominator;

    return std::to_string(numerator) + "/" + std::to_string(denominator);
}

// convert decial to rational format. For example 2.5 -> 5/2
void ExifMetadatFormatter::DecimalRationalFormat(std::string &value)
{
    std::string result;
    int icount = 0;
    std::regex parPattern("(\\d+)(\\.\\d+)?");
    // with partial regex For 2.5 26 1.2 to iterator each segment 2.5->5/2
    for (std::sregex_iterator it = std::sregex_iterator(value.begin(), value.end(), parPattern);
         it != std::sregex_iterator(); ++it) {
        std::smatch match = *it;
        // HiLog::Debug(LABEL, "decimalRationalFormat match. value is [%{public}s].", match[0]);
        IMAGE_LOGD("decimalRationalFormat match. value is [%{public}s].", ((std::string)match[0]).c_str());
        // add a space at begin of each segment except the first segment
        if (icount != 0) {
            result += " ";
        }
        //1.if segment is integer type 123->123/1
        if (ValidRegex(match[0], "\\d+")) {
            // append '/1' to integer 23 -> 23/1
            result += match.str() + "/1";
            IMAGE_LOGD("match integer value is [%{public}s].", ((std::string)match[0]).c_str());
        }
        if (ValidRegex(match[0], "\\d+\\.\\d+")) {
            // segment is decimal call decimalToFraction 2.5 -> 5/2
            result += GetFractionFromStr(match[0]);
            IMAGE_LOGD("match decimal value is [%{public}s].", ((std::string)match[0]).c_str());
        }
        icount++;
    }
    value = result;
}

// validate regex & convert integer to rational format. For example 23 15 83 --> 23/1 15/1 83
bool ExifMetadatFormatter::ValidRegexWithRationalFormat(std::string &value, const std::string &regex)
{
    // 1.validate regex
    if (!ValidRegex(value, regex)) {
        return false;
    }
    // 2.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
    RationalFormat(value);
    return true;
}

// validate regex & convert value to rational format. For example 9,9,9 -> 9 9 9 -> 9/1 9/1 9/1
bool ExifMetadatFormatter::ValidRegexWithCommaRationalFormat(std::string &value, const std::string &regex)
{
    // 1.validate regex
    if (!ValidRegex(value, regex)) {
        return false;
    }
    // 2.replace comma as a space
    ReplaceAsSpace(value, COMMAREGEX);
    // 3.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
    RationalFormat(value);
    return true;
}

// validate regex & convert value to rational format. For example 9:9:9 -> 9 9 9 -> 9/1 9/1 9/1
bool ExifMetadatFormatter::ValidRegexWithColonRationalFormat(std::string &value, const std::string &regex)
{
    // 1.validate regex
    if (!ValidRegex(value, regex)) {
        return false;
    }
    // 2.replace colon as a space
    ReplaceAsSpace(value, COLONREGEX);
    // 3.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
    RationalFormat(value);
    return true;
}

// validate regex & convert value to integer format.
bool ExifMetadatFormatter::ValidRegexWithDot(std::string &value, const std::string &regex)
{
    if (!ValidRegex(value, regex)) {
        return false;
    }
    ReplaceAsSpace(value, DOTREGEX);
    return true;
}

// regex validation & convert decimal to rational. For example GPSLatitude 2.5,23,3.4 -> 2.5 23 3.4 -> 5/2 23/1 17/5
bool ExifMetadatFormatter::ValidRegxWithCommaDecimalRationalFormat(std::string &value, const std::string &regex)
{
    if (!ValidRegex(value, regex)) {
        return false;
    }

    // replace comma with a space 1.5,2.5.3 -> 1.5 2.5 3
    ReplaceAsSpace(value, COMMAREGEX);
    // convert decimal to rationl 2.5 -> 5/2
    DecimalRationalFormat(value);
    return true;
}

// regex validation & convert decimal to rational. For example GPSLatitude 2.5 23 3.4 -> 5/2 23/1 17/5
bool ExifMetadatFormatter::ValidRegexWithDecimalRationalFormat(std::string &value, const std::string &regex)
{
    if (!ValidRegex(value, regex)) {
        return false;
    }
    // convert decimal to rationl 2.5 -> 5/2
    DecimalRationalFormat(value);
    return true;
}

// regex validation for two integer like DefaultCropSize 9 9 the format is [0-9]+ [0-9]+
ValueFormatDelegate ExifMetadatFormatter::doubleIntWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegex, DOUBLE_INT_WITH_BLANK_REGEX);

// regex validation for two integer with comma like BitPerSample 9,9 the format is [0-9]+,[0-9]+,[0-9]+
ValueFormatDelegate ExifMetadatFormatter::doubleIntWithComma =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithComma, DOUBLE_INT_WITH_COMMA_REGEX);

// regex validation for three integer like BitPerSample 9 9 9 the format is [0-9]+ [0-9]+ [0-9]+
ValueFormatDelegate ExifMetadatFormatter::tribleIntWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegex, TRIBLE_INT_WITH_BLANK_REGEX);

// regex validation for three integer with comma like BitPerSample 9,9,0 the format is [0-9]+,[0-9]+,[0-9]+,[0-9]+
ValueFormatDelegate ExifMetadatFormatter::tribleIntWithComma =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithComma, TRIBLE_INT_WITH_COMMA_REGEX);

// regex validation for four integer like DNGVersion 9 9 9 9 the format is [0-9]+ [0-9]+ [0-9]+ [0-9]+
ValueFormatDelegate ExifMetadatFormatter::fourIntWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegex, FOUR_INT_WITH_BLANK_REGEX);

// regex validation for four integer with comma like DNGVersion tag encodes the DNG four-tier version number
ValueFormatDelegate ExifMetadatFormatter::fourIntWithComma =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithComma, FOUR_INT_WITH_COMMA_REGEX);

// regex validation for one rational like ApertureValue 4/1 the format is [0-9]+/[1-9][0-9]
ValueFormatDelegate ExifMetadatFormatter::oneRational =
    std::make_pair(ExifMetadatFormatter::ValidRegex, ONE_RATIONAL_REGEX);

// regex validation for integer and convert it to rational like ApertureValue 4 --> 4/1
ValueFormatDelegate ExifMetadatFormatter::oneIntToRational =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithRationalFormat, ONE_INT_REGEX);

ValueFormatDelegate ExifMetadatFormatter::oneDecimalToRational =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithDecimalRationalFormat, ONEDECIMALREGEX);

// regex validation for three rational like GPSLatitude 39/1 54/1 20/1
ValueFormatDelegate ExifMetadatFormatter::tribleRationalWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegex, TRIBLE_RATIONAL_WITH_BLANK_REGEX);

// regex validation for three integer and convert to three rational like GPSLatitude 39 54 20 --> 39/1 54/1 20/1
ValueFormatDelegate ExifMetadatFormatter::tribleIntToRationalWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithRationalFormat, TRIBLE_INT_NZ_WITH_BLANK_REGEX);

// regex validation for three integer with comma like GPSLatitude 39,54,20 --> 39/1 54/1 20/1
ValueFormatDelegate ExifMetadatFormatter::tribleIntToRationalWithComma =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithCommaRationalFormat, TRIBLE_INT_NZ_WITH_COMMA_REGEX);

// regex validation for three decimal like YCbCrCoefficients 39.0 54 20.0 --> 39/1 54/1 20/1
ValueFormatDelegate ExifMetadatFormatter::tribleDecimalToRationalWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithDecimalRationalFormat, TRIBLE_DECIMAL_WITH_BLANK_REGEX);

// regex validation for three decimal like YCbCrCoefficients 39.0,54,20.0 --> 39.0 54 20.0 --> 39/1 54/1 20/1
ValueFormatDelegate ExifMetadatFormatter::tribleDecimalToRatiionalWithComma =
    std::make_pair(ExifMetadatFormatter::ValidRegxWithCommaDecimalRationalFormat, TRIBLE_DECIMAL_WITH_COMMA_REGEX);

// regex validation for four rational like LensSpecification 1/1 3/2 1/1 2/1
ValueFormatDelegate ExifMetadatFormatter::fourRationalWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegex, FOUR_RATIONAL_WITH_BLANK_REGEX);

// regex validation for four integer and convert to four rational like LensSpecification 1 3 1 2 --> 1/1 3/2 1/1 2/1
ValueFormatDelegate ExifMetadatFormatter::fourIntToRationalWithBlank =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithRationalFormat, FOUR_INT_WITH_BLANK_REGEX);

// regex validation for four integer like LensSpecification 1,3,1,2 --> 1/1 3/2 1/1 2/1
ValueFormatDelegate ExifMetadatFormatter::fourIntToRationalWithComma =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithCommaRationalFormat, FOUR_INT_NZ_WITH_COMMA_REGEX);

// regex validation for four decimal like LensSpecification 1.0 3.0 1.0 2.0 --> 39/1 54/1 20/1
ValueFormatDelegate ExifMetadatFormatter::decimal4Ratiional4 =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithDecimalRationalFormat, FOUR_DECIMAL_WITH_BLANK_REGEX);

// regex validation for four decimal like LensSpecification 1.0,3.0,1.0,2.0 --> 39/1 54/1 20/1
ValueFormatDelegate ExifMetadatFormatter::decimal4Ratiional4Comma =
    std::make_pair(ExifMetadatFormatter::ValidRegxWithCommaDecimalRationalFormat, FOUR_DECIMAL_WITH_COMMA_REGEX);

// regex validation for datetime format like DateTimeOriginal 2022:06:02 15:51:34
ValueFormatDelegate ExifMetadatFormatter::dateTimeValidation =
    std::make_pair(ExifMetadatFormatter::ValidRegex, DATETIME_REGEX);

// regex validation for datetime format like DateTimeOriginal 2022:06:02
ValueFormatDelegate ExifMetadatFormatter::dateValidation =
    std::make_pair(ExifMetadatFormatter::ValidRegex, DATE_REGEX);

// regex validation for three integer like GPSLatitude 39,54,21 --> 39/1 54/1 21/1
ValueFormatDelegate ExifMetadatFormatter::tribleIntToRationalWithColon =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithColonRationalFormat, TRIBLE_INT_WITH_COLON_REGEX);

// regex validation for fou integer with pointer like GPSVersionID
ValueFormatDelegate ExifMetadatFormatter::fourIntToRationalWithDot =
    std::make_pair(ExifMetadatFormatter::ValidRegexWithDot, TRIBLE_INT_WITH_DOT_REGEX);

// configuration for value format validation. For example BitPerSample the value format should be 9 9 9 or 9,9,9
std::multimap<std::string, ValueFormatDelegate> ExifMetadatFormatter::valueFormatConvertConfig = {
    {"BitsPerSample", tribleIntWithBlank},
    {"BitsPerSample", tribleIntWithComma},
    {"CompressedBitsPerPixel", oneRational},
    {"CompressedBitsPerPixel", oneIntToRational},
    {"CompressedBitsPerPixel", oneDecimalToRational},
    {"GPSLatitude", tribleRationalWithBlank},
    {"GPSLatitude", tribleIntToRationalWithBlank},
    {"GPSLatitude", tribleIntToRationalWithComma},
    {"GPSLongitude", tribleRationalWithBlank},
    {"GPSLongitude", tribleIntToRationalWithBlank},
    {"GPSLongitude", tribleIntToRationalWithComma},
    {"ApertureValue", oneRational},
    {"ApertureValue", oneIntToRational},
    {"ApertureValue", oneDecimalToRational},
    {"DateTimeOriginal", dateTimeValidation},
    {"DateTimeOriginal", dateValidation},
    {"Exif.Image.DateTime", dateTimeValidation},
    {"Exif.Image.DateTime", dateValidation},
    {"ExposureBiasValue", oneRational},
    {"ExposureBiasValue", oneIntToRational},
    {"ExposureBiasValue", oneDecimalToRational},
    {"ExposureTime", oneRational},
    {"ExposureTime", oneIntToRational},
    {"ExposureTime", oneDecimalToRational},
    {"FNumber", oneRational},
    {"FNumber", oneIntToRational},
    {"FNumber", oneDecimalToRational},
    {"FocalLength", oneRational},
    {"FocalLength", oneIntToRational},
    {"FocalLength", oneDecimalToRational},
    {"GPSTimeStamp", tribleRationalWithBlank},
    {"GPSTimeStamp", tribleIntToRationalWithBlank},
    {"GPSTimeStamp", tribleIntToRationalWithColon},
    {"GPSDateStamp", dateValidation},
    {"XResolution", oneRational},
    {"XResolution", oneIntToRational},
    {"XResolution", oneDecimalToRational},
    {"YResolution", oneRational},
    {"YResolution", oneIntToRational},
    {"YResolution", oneDecimalToRational},
    {"WhitePoint", oneRational},
    {"WhitePoint", oneIntToRational},
    {"WhitePoint", oneDecimalToRational},
    {"PrimaryChromaticities", oneRational},
    {"PrimaryChromaticities", oneIntToRational},
    {"PrimaryChromaticities", oneDecimalToRational},
    {"YCbCrCoefficients", tribleRationalWithBlank},
    {"YCbCrCoefficients", tribleIntToRationalWithBlank},
    {"YCbCrCoefficients", tribleIntToRationalWithComma},
    {"YCbCrCoefficients", tribleDecimalToRationalWithBlank},
    {"YCbCrCoefficients", tribleDecimalToRatiionalWithComma},
    {"ReferenceBlackWhite", oneRational},
    {"ReferenceBlackWhite", oneIntToRational},
    {"ReferenceBlackWhite", oneDecimalToRational},
    {"ShutterSpeedValue", oneRational},
    {"ShutterSpeedValue", oneIntToRational},
    {"ShutterSpeedValue", oneDecimalToRational},
    {"BrightnessValue", oneRational},
    {"BrightnessValue", oneIntToRational},
    {"BrightnessValue", oneDecimalToRational},
    {"MaxApertureValue", oneRational},
    {"MaxApertureValue", oneIntToRational},
    {"MaxApertureValue", oneDecimalToRational},
    {"SubjectDistance", oneRational},
    {"SubjectDistance", oneIntToRational},
    {"SubjectDistance", oneDecimalToRational},
    {"FlashEnergy", oneRational},
    {"FlashEnergy", oneIntToRational},
    {"FlashEnergy", oneDecimalToRational},
    {"FocalPlaneXResolution", oneRational},
    {"FocalPlaneXResolution", oneIntToRational},
    {"FocalPlaneXResolution", oneDecimalToRational},
    {"FocalPlaneYResolution", oneRational},
    {"FocalPlaneYResolution", oneIntToRational},
    {"FocalPlaneYResolution", oneDecimalToRational},
    {"ExposureIndex", oneRational},
    {"ExposureIndex", oneIntToRational},
    {"ExposureIndex", oneDecimalToRational},
    {"DigitalZoomRatio", oneRational},
    {"DigitalZoomRatio", oneIntToRational},
    {"DigitalZoomRatio", oneDecimalToRational},
    {"GPSAltitude", oneRational},
    {"GPSAltitude", oneIntToRational},
    {"GPSAltitude", oneDecimalToRational},
    {"GPSDOP", oneRational},
    {"GPSDOP", oneIntToRational},
    {"GPSDOP", oneDecimalToRational},
    {"GPSSpeed", oneRational},
    {"GPSSpeed", oneIntToRational},
    {"GPSSpeed", oneDecimalToRational},
    {"GPSTrack", oneRational},
    {"GPSTrack", oneIntToRational},
    {"GPSTrack", oneDecimalToRational},
    {"GPSImgDirection", oneRational},
    {"GPSImgDirection", oneIntToRational},
    {"GPSImgDirection", oneDecimalToRational},
    {"GPSDestLatitude", tribleRationalWithBlank},
    {"GPSDestLatitude", tribleIntToRationalWithBlank},
    {"GPSDestLatitude", tribleIntToRationalWithComma},
    {"GPSDestLongitude", tribleRationalWithBlank},
    {"GPSDestLongitude", tribleIntToRationalWithBlank},
    {"GPSDestLongitude", tribleIntToRationalWithComma},
    {"GPSDestBearing", oneRational},
    {"GPSDestBearing", oneIntToRational},
    {"GPSDestBearing", oneDecimalToRational},
    {"GPSDestDistance", oneRational},
    {"GPSDestDistance", oneIntToRational},
    {"GPSDestDistance", oneDecimalToRational},
    {"GPSVersionID", fourIntWithBlank},
    {"GPSVersionID", fourIntToRationalWithDot},
    {"CompressedBitsPerPixel", oneRational},
    {"CompressedBitsPerPixel", oneIntToRational},
    {"CompressedBitsPerPixel", oneDecimalToRational},
    {"DNGVersion", fourIntWithBlank},
    {"DNGVersion", fourIntWithComma},
    {"DefaultCropSize", doubleIntWithBlank},
    {"DefaultCropSize", doubleIntWithComma},
    {"Gamma", oneRational},
    {"Gamma", oneIntToRational},
    {"Gamma", oneDecimalToRational},
    {"GPSHPositioningError", oneRational},
    {"GPSHPositioningError", oneIntToRational},
    {"GPSHPositioningError", oneDecimalToRational},
    {"LensSpecification", fourRationalWithBlank},
    {"LensSpecification", fourIntToRationalWithBlank},
    {"LensSpecification", fourIntToRationalWithComma},
    {"LensSpecification", decimal4Ratiional4},
    {"LensSpecification", decimal4Ratiional4Comma},
};

std::multimap<std::string, std::string> ExifMetadatFormatter::valueFormatValidateConfig = {
    {"BitsPerSample", TRIBLE_INT_WITH_COMMA_REGEX},
    {"ImageLength", ONE_INT_REGEX},
    {"ImageWidth", ONE_INT_REGEX},
    {"GPSLatitude", DOUBLE_INT_WITH_COMMA_REGEX},
    {"GPSLatitude", TRIBLE_INT_WITH_COMMA_REGEX},
    {"GPSLatitude", TRIBLE_DECIMAL_WITH_COMMA_REGEX},
    {"GPSLongitude", DOUBLE_INT_WITH_COMMA_REGEX},
    {"GPSLongitude", TRIBLE_INT_WITH_COMMA_REGEX},
    {"GPSLongitude", TRIBLE_DECIMAL_WITH_COMMA_REGEX}
};

// validate the value range. For example GPSLatitudeRef the value must be 'N' or 'S'.
int32_t ExifMetadatFormatter::ValidateValueRange(const std::string &keyName, const std::string &value)
{
    // 1. to find if any value range validation configuratiion according to exif tag in std::map container
    auto iter = valueRangeValidateConfig.find(keyName);
    if (iter == valueRangeValidateConfig.end()) {
        // if no range validation for key default is success.
        return Media::SUCCESS;
    }
    // get value range array & size
    auto &[arrRef, arrSize] = iter->second;
    if (arrRef == nullptr) {
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    int32_t ivalue = -1;
    // validate value if integer or char 2.char ascii
    std::regex regNum(R"(^[0-9]+$)");    // regex for integer value. For example WhiteBalance support 0 or 1
    std::regex regChar(R"(^[a-zA-Z]$)"); // regex for char value. For example GPSLatitudeRef support N or S
    if (std::regex_match(value, regNum)) {
        // convert string to integer such as "15" -> 15
        ivalue = std::stoll(value);
        IMAGE_LOGD("convert to integer ivalue is[%{public}d].", ivalue);
    }
    if (std::regex_match(value, regChar)) {
        // convert char to integer such as "N" -> 78
        ivalue = static_cast<int32_t>(value[0]);
        IMAGE_LOGD("convert char to integer ivalue is[%{public}d].", ivalue);
    }

    // if ivalue is not converted then return FAIL
    if (ivalue == -1) {
        IMAGE_LOGD("invalid value is [%{public}s].", value.c_str());
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    // validate the ivalue is in value range array.
    auto isValid = IsValidValue(arrRef, arrSize, ivalue);
    if (!isValid) {
        IMAGE_LOGD("invalid value is [%{public}s].", value.c_str());
        return Media::ERR_MEDIA_OUT_OF_RANGE;
    } else {
        IMAGE_LOGD("isValueRangeValidate valid value is [%{public}s].", value.c_str());
        return Media::SUCCESS;
    }
    // Default is SUCCESS if there is no value range configuration
    return Media::SUCCESS; // SUCCESS
}

// check if has any value format configuration
bool ExifMetadatFormatter::IsFormatValidationConfigExisting(const std::string &keyName)
{
    IMAGE_LOGD("hasValueFormatValidate keyName is [%{public}s].", keyName.c_str());
    auto it = ExifMetadatFormatter::valueFormatValidateConfig.find(keyName);
    return it != ExifMetadatFormatter::valueFormatValidateConfig.end();
}

// validate value format. For example BitPerSample the value format should be 9 9 9 or 9,9,9
int32_t ExifMetadatFormatter::ConvertValueFormat(const std::string &keyName, std::string &value)
{
    auto it = ExifMetadatFormatter::valueFormatConvertConfig.find(keyName);
    if (it == ExifMetadatFormatter::valueFormatConvertConfig.end()) {
        IMAGE_LOGD("no format validate default success.");
        // if no format validation config default success
        return Media::SUCCESS;
    }
    IMAGE_LOGD("isValueFormatValidate keyName is [%{public}s] value is [%{public}s].",
        keyName.c_str(), value.c_str());
    // get first iterator according to keyName
    for (auto iterator = ExifMetadatFormatter::valueFormatConvertConfig.find(keyName);
        iterator != ExifMetadatFormatter::valueFormatConvertConfig.end() &&
        iterator != ExifMetadatFormatter::valueFormatConvertConfig.upper_bound(keyName);
        iterator++) {
        IMAGE_LOGD("isValueFormatValidate forloop keyName is [%{public}s] regex string is [%{public}s].",
            (iterator->first).c_str(), (iterator->second).second.c_str());
        auto func = (iterator->second).first;
        // call each value format function with value and regex
        int32_t isValid = func(value, (iterator->second).second);
        IMAGE_LOGD("isValueFormatValidate ret isValid is [%{public}d].", isValid);
        if (isValid) {
            IMAGE_LOGD("isValueFormatValidate ret SUCCESS.");
            return Media::SUCCESS; // SUCCESS
        }
    }

    IMAGE_LOGD("isValueFormatValidate ret ERR_IMAGE_DECODE_EXIF_UNSUPPORT.");
    return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; // FAILED
}

int32_t ExifMetadatFormatter::ValidateValueFormat(const std::string &keyName, const std::string &value)
{
    auto it = ExifMetadatFormatter::valueFormatValidateConfig.find(keyName);
    if (it == ExifMetadatFormatter::valueFormatValidateConfig.end()) {
        IMAGE_LOGD("no format validate default success.");
        // if no format validation config default success
        return Media::SUCCESS;
    }
    
    IMAGE_LOGD("isValueFormatValidate keyName is [%{public}s] value is [%{public}s].",
        keyName.c_str(), value.c_str());
    // get first iterator according to keyName
    for (auto iterator = ExifMetadatFormatter::valueFormatValidateConfig.find(keyName);
        iterator != ExifMetadatFormatter::valueFormatValidateConfig.end() &&
        iterator != ExifMetadatFormatter::valueFormatValidateConfig.upper_bound(keyName);
        iterator++) {
        bool isValidated = ExifMetadatFormatter::ValidRegex(value, iterator->second);
        IMAGE_LOGD("isValueFormatValidate ret i is [%{public}d].", isValidated);
        if (isValidated) {
            IMAGE_LOGD("isValueFormatValidate ret SUCCESS.");
            return Media::SUCCESS; // SUCCESS
        }
    }

    IMAGE_LOGD("isValueFormatValidate ret ERR_IMAGE_DECODE_EXIF_UNSUPPORT.");
    return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; // FAILED
}

bool ExifMetadatFormatter::IsKeySupported(const std::string &keyName)
{
    auto it = SUPPORTKEYS.find(keyName);
    return !(it == SUPPORTKEYS.end());
}

bool ExifMetadatFormatter::IsModifyAllowed(const std::string &keyName)
{
    auto it = SUPPORTKEYS.find(keyName);
    if (it != SUPPORTKEYS.end() && it->second == READ_WRITE) {
        return true;
    }
    return false;
}

// exif validation portal when modify exif
std::pair<int32_t, std::string> ExifMetadatFormatter::Format(const std::string &keyName, const std::string &value)
{
    std::string value_ = value;
    IMAGE_LOGD("ExifMetadatFormatter.");
    // translate exif tag. For example translate "BitsPerSample" to "Exif.Image.BitsPerSample"
    if (!ExifMetadatFormatter::IsKeySupported(keyName)) {
        return std::make_pair(Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT, nullptr);
    }

    if (!ExifMetadatFormatter::IsModifyAllowed(keyName)) {
        return std::make_pair(Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT, nullptr);
    }
    IMAGE_LOGD("ExifMetadatFormatter keyName is [%{public}s] value is [%{public}s].",
        keyName.c_str(), value.c_str());
    // 1.validate value format
    if (ExifMetadatFormatter::ConvertValueFormat(keyName, value_)) {
        IMAGE_LOGD("ExifMetadatFormatter value formate is invalid. keyName is [%{public}s] value is [%{public}s].",
            keyName.c_str(), value.c_str());
        // value format validate does not pass
        return std::make_pair(Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT, nullptr);
    }
    IMAGE_LOGD("ExifMetadatFormatter processed formate value is [%{public}s] value is [%{public}s].",
        keyName.c_str(), value.c_str());
    // 2.validate value range
    if (ExifMetadatFormatter::ValidateValueRange(keyName, value)) {
        IMAGE_LOGD("ExifMetadatFormatter value range is invalid. value is [%{public}s] value is [%{public}s].",
            keyName.c_str(), value.c_str());
        // value range validate does not pass
        return std::make_pair(Media::ERR_MEDIA_OUT_OF_RANGE, nullptr);
    }
    return std::make_pair(Media::SUCCESS, value_);
}

// exif validation portal
int32_t ExifMetadatFormatter::Validate(const std::string &keyName, const std::string &value)
{
    IMAGE_LOGD("Validate in exifValidate.");
    // translate exif tag. For example translate "BitsPerSample" to "Exif.Image.BitsPerSample"
    if (!ExifMetadatFormatter::IsKeySupported(keyName)) {
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }

    if (!ExifMetadatFormatter::IsModifyAllowed(keyName)) {
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
    }
    IMAGE_LOGD("Validate keyName is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
    // 1.validate value format
    if (ExifMetadatFormatter::ValidateValueFormat(keyName, value)) {
        IMAGE_LOGD("Validate value formate is invalid. keyName is [%{public}s] value is [%{public}s].",
            keyName.c_str(), value.c_str());
        return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; // value format validate does not pass
    }
    IMAGE_LOGD("Validate processed formate value is [%{public}s] value is [%{public}s].",
        keyName.c_str(), value.c_str());
    // 2.validate value range
    if (ExifMetadatFormatter::ValidateValueRange(keyName, value)) {
        IMAGE_LOGD("Validate value range is invalid. value is [%{public}s] value is [%{public}s].",
            keyName.c_str(), value.c_str());
        return Media::ERR_MEDIA_OUT_OF_RANGE; // value range validate does not pass
    }
    return Media::SUCCESS;
}
} // namespace Media
} // namespace OHOS
