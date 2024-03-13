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

#include <cmath>
#include <iostream>
#include <regex>
#include <string_view>
#include "exif_metadata_converter.h"
#include "hilog/log_cpp.h"
#include "string_ex.h"
#include "media_errors.h"
#include "hilog/log.h"
#include "image_log.h"
#include <set>
#include <utility>

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_IMAGE

#undef LOG_TAG
#define LOG_TAG "ExifMetadataConverter"

#define N_(String) (String)

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

//! Orientation, tag 0x0112
constexpr TagDetails exifOrientation[] = {
    {1, N_("top, left")},     {2, N_("top, right")},   {3, N_("bottom, right")},
    {4, N_("bottom, left")},  {5, N_("left, top")},    {6, N_("right, top")},
    {7, N_("right, bottom")}, {8, N_("left, bottom")}, {8, N_("left, bottom")}  // To silence compiler warning
};

//! GPS latitude reference, tag 0x0001; also GPSDestLatitudeRef, tag 0x0013
constexpr TagDetails exifGPSLatitudeRef[] = {
    {78, N_("North")},
    {83, N_("South")},
};

constexpr TagDetails exifGPSLongitudeRef[] = {
    {69, N_("East")},
    {87, N_("West")},
};

//! WhiteBalance, tag 0xa403
constexpr TagDetails exifWhiteBalance[] = {
    {0, N_("Auto")},
    {1, N_("Manual")},
};

//! Flash, Exif tag 0x9209
constexpr TagDetails exifFlash[] = {
    {0x00, N_("No flash")},
    {0x01, N_("Fired")},
    {0x05, N_("Fired, return light not detected")},
    {0x07, N_("Fired, return light detected")},
    {0x08, N_("Yes, did not fire")},
    {0x09, N_("Yes, compulsory")},
    {0x0d, N_("Yes, compulsory, return light not detected")},
    {0x0f, N_("Yes, compulsory, return light detected")},
    {0x10, N_("No, compulsory")},
    {0x14, N_("No, did not fire, return light not detected")},
    {0x18, N_("No, auto")},
    {0x19, N_("Yes, auto")},
    {0x1d, N_("Yes, auto, return light not detected")},
    {0x1f, N_("Yes, auto, return light detected")},
    {0x20, N_("No flash function")},
    {0x20, N_("No, no flash function")},
    {0x41, N_("Yes, red-eye reduction")},
    {0x45, N_("Yes, red-eye reduction, return light not detected")},
    {0x47, N_("Yes, red-eye reduction, return light detected")},
    {0x49, N_("Yes, compulsory, red-eye reduction")},
    {0x4d, N_("Yes, compulsory, red-eye reduction, return light not detected")},
    {0x4f, N_("Yes, compulsory, red-eye reduction, return light detected")},
    {0x50, N_("No, red-eye reduction")},
    {0x58, N_("No, auto, red-eye reduction")},
    {0x59, N_("Yes, auto, red-eye reduction")},
    {0x5d, N_("Yes, auto, red-eye reduction, return light not detected")},
    {0x5f, N_("Yes, auto, red-eye reduction, return light detected")},
};

//! ColorSpace, tag 0xa001
constexpr TagDetails exifColorSpace[] = {
    {1, N_("sRGB")},
    {2, N_("Adobe RGB")},  // Not defined to Exif 2.2 spec. But used by a lot of cameras.
    {0xffff, N_("Uncalibrated")},
};

//! LightSource, tag 0x9208
constexpr TagDetails exifLightSource[] = {
    {0, N_("Unknown")},
    {1, N_("Daylight")},
    {2, N_("Fluorescent")},
    {3, N_("Tungsten (incandescent light)")},
    {4, N_("Flash")},
    {9, N_("Fine weather")},
    {10, N_("Cloudy weather")},
    {11, N_("Shade")},
    {12, N_("Daylight fluorescent (D 5700 - 7100K)")},
    {13, N_("Day white fluorescent (N 4600 - 5400K)")},
    {14, N_("Cool white fluorescent (W 3900 - 4500K)")},
    {15, N_("White fluorescent (WW 3200 - 3700K)")},
    {17, N_("Standard light A")},
    {18, N_("Standard light B")},
    {19, N_("Standard light C")},
    {20, N_("D55")},
    {21, N_("D65")},
    {22, N_("D75")},
    {23, N_("D50")},
    {24, N_("ISO studio tungsten")},
    {255, N_("Other light source")},
};

//! MeteringMode, tag 0x9207
constexpr TagDetails exifMeteringMode[] = {
    {0, N_("Unknown")}, {1, N_("Average")},    {2, N_("Center weighted average")},
    {3, N_("Spot")},    {4, N_("Multi-spot")}, {5, N_("Multi-segment")},
    {6, N_("Partial")}, {255, N_("Other")},    {255, N_("Other")}  // To silence compiler warning
};

//! SceneType, tag 0xa301
constexpr TagDetails exifSceneType[] = {
    {1, N_("Directly photographed")},
};

//! Compression, tag 0x0103
constexpr TagDetails exifCompression[] = {
    {1, N_("Uncompressed")},
    {2, N_("CCITT RLE")},
    {3, N_("T4/Group 3 Fax")},
    {4, N_("T6/Group 4 Fax")},
    {5, N_("LZW")},
    {6, N_("JPEG (old-style)")},
    {7, N_("JPEG")},
    {8, N_("Adobe Deflate")},
    {9, N_("JBIG B&W")},
    {10, N_("JBIG Color")},
    {32766, N_("Next 2-bits RLE")},
    {32767, N_("Sony ARW Compressed")},
    {32769, N_("Epson ERF Compressed")},
    {32770, N_("Samsung SRW Compressed")},
    {32771, N_("CCITT RLE 1-word")},
    {32773, N_("PackBits (Macintosh RLE)")},
    {32809, N_("Thunderscan RLE")},
    {32895, N_("IT8 CT Padding")},
    {32896, N_("IT8 Linework RLE")},
    {32897, N_("IT8 Monochrome Picture")},
    {32898, N_("IT8 Binary Lineart")},
    {32908, N_("Pixar Film (10-bits LZW)")},
    {32909, N_("Pixar Log (11-bits ZIP)")},
    {32946, N_("Pixar Deflate")},
    {32947, N_("Kodak DCS Encoding")},
    {34661, N_("ISO JBIG")},
    {34676, N_("SGI Log Luminance RLE")},
    {34677, N_("SGI Log 24-bits packed")},
    {34712, N_("Leadtools JPEG 2000")},
    {34713, N_("Nikon NEF Compressed")},
    {34892, N_("JPEG (lossy)")},  // DNG 1.4
    {52546, N_("JPEG XL")},       // DNG 1.7
    {65000, N_("Kodak DCR Compressed")},
    {65535, N_("Pentax PEF Compressed")},
};

//! PhotometricInterpretation, tag 0x0106
constexpr TagDetails exifPhotometricInterpretation[] = {
    {0, N_("White Is Zero")},
    {1, N_("Black Is Zero")},
    {2, N_("RGB")},
    {3, N_("RGB Palette")},
    {4, N_("Transparency Mask")},
    {5, N_("CMYK")},
    {6, N_("YCbCr")},
    {8, N_("CIELab")},
    {9, N_("ICCLab")},
    {10, N_("ITULab")},
    {32803, N_("Color Filter Array")},
    {32844, N_("Pixar LogL")},
    {32845, N_("Pixar LogLuv")},
    {34892, N_("Linear Raw")},
    {51177, N_("Depth Map")},         // DNG 1.5
    {52527, N_("Photometric Mask")},  // DNG 1.6
};

//! PlanarConfiguration, tag 0x011c
constexpr TagDetails exifPlanarConfiguration[] = {
    {1, N_("Chunky")},
    {2, N_("Planar")},
};

//! Units for measuring X and Y resolution, tags 0x0128, 0xa210
constexpr TagDetails exifUnit[] = {
    {1, N_("none")},
    {2, N_("inch")},
    {3, N_("cm")},
};

//! YCbCrPositioning, tag 0x0213
constexpr TagDetails exifYCbCrPositioning[] = {
    {1, N_("Centered")},
    {2, N_("Co-sited")},
};

//! ExposureProgram, tag 0x8822
constexpr TagDetails exifExposureProgram[] = {
    {0, N_("Not defined")},       {1, N_("Manual")},           {2, N_("Auto")},
    {3, N_("Aperture priority")}, {4, N_("Shutter priority")}, {5, N_("Creative program")},
    {6, N_("Action program")},    {7, N_("Portrait mode")},    {8, N_("Landscape mode")},
};

//! SensingMethod, TIFF/EP tag 0x9217
constexpr TagDetails tiffSensingMethod[] = {
    {0, N_("Undefined")},           {1, N_("Monochrome area")},       {2, N_("One-chip color area")},
    {3, N_("Two-chip color area")}, {4, N_("Three-chip color area")}, {5, N_("Color sequential area")},
    {6, N_("Monochrome linear")},   {7, N_("Trilinear sensor")},      {8, N_("Color sequential linear")},
};

//! CustomRendered, tag 0xa401
constexpr TagDetails exifCustomRendered[] = {
    {0, N_("Normal process")},
    {1, N_("Custom process")},
};

//! ExposureMode, tag 0xa402
constexpr TagDetails exifExposureMode[] = {
    {0, N_("Auto")},
    {1, N_("Manual")},
    {2, N_("Auto bracket")},
};

//! SceneCaptureType, tag 0xa406
constexpr TagDetails exifSceneCaptureType[] = {
    {0, N_("Standard")},
    {1, N_("Landscape")},
    {2, N_("Portrait")},
    {3, N_("Night scene")},
    {3, N_("Night scene")}  // To silence compiler warning
};

//! GainControl, tag 0xa407
constexpr TagDetails exifGainControl[] = {
    {0, N_("None")},          {1, N_("Low gain up")},    {2, N_("High gain up")},
    {3, N_("Low gain down")}, {4, N_("High gain down")},
};

//! Contrast, tag 0xa408 and Sharpness, tag 0xa40a
constexpr TagDetails exifNormalSoftHard[] = {
    {0, N_("Normal")},
    {1, N_("Soft")},
    {2, N_("Hard")},
};

//! Saturation, tag 0xa409
constexpr TagDetails exifSaturation[] = {
    {0, N_("Normal")},
    {1, N_("Low")},
    {2, N_("High")},
};

//! SubjectDistanceRange, tag 0xa40c
constexpr TagDetails exifSubjectDistanceRange[] = {
    {0, N_("Unknown")},
    {1, N_("Macro")},
    {2, N_("Close view")},
    {3, N_("Distant view")},
    {3, N_("Distant view")}  // To silence compiler warning
};

//! GPS altitude reference, tag 0x0005
constexpr TagDetails exifGPSAltitudeRef[] = {
    {0, N_("Above sea level")},
    {1, N_("Below sea level")},
};

//! NewSubfileType, TIFF tag 0x00fe - this is actually a bitmask
constexpr TagDetails exifNewSubfileType[] = {
    {0, N_("Primary image")},
    {1, N_("Thumbnail/Preview image")},
    {2, N_("Primary image, Multi page file")},
    {3, N_("Thumbnail/Preview image, Multi page file")},
    {4, N_("Primary image, Transparency mask")},
    {5, N_("Thumbnail/Preview image, Transparency mask")},
    {6, N_("Primary image, Multi page file, Transparency mask")},
    {7, N_("Thumbnail/Preview image, Multi page file, Transparency mask")},
    {8, N_("Primary image, Depth map")},                  // DNG 1.5
    {9, N_("Thumbnail/Preview image, Depth map")},        // DNG 1.5
    {16, N_("Enhanced image")},                           // DNG 1.5 (clashes w/ TIFF-FX)
    {65537, N_("Thumbnail/Preview image, Alternative")},  // DNG 1.2
    {65540, N_("Primary image, Semantic mask")}           // DNG 1.6
};

//! SubfileType, TIFF tag 0x00ff
constexpr TagDetails exifSubfileType[] = {
    {1, N_("Full-resolution image data")},
    {2, N_("Reduced-resolution image data")},
    {3, N_("A single page of a multi-page image")},
};

//! GPS status, tag 0x0009
constexpr TagDetails exifGPSStatus[] = {
    {'A', N_("Measurement in progress")},
    {'V', N_("Measurement interrupted")},
};

//! GPS measurement mode, tag 0x000a
constexpr TagDetails exifGPSMeasureMode[] = {
    {2, N_("2-dimensional measurement")},
    {3, N_("3-dimensional measurement")},
};

//! GPS speed reference, tag 0x000c
constexpr TagDetails exifGPSSpeedRef[] = {
    {'K', N_("km/h")},
    {'M', N_("mph")},
    {'N', N_("knots")},
};

//! GPS direction reference, tags 0x000e, 0x0010, 0x0017
constexpr TagDetails exifGPSDirRef[] = {
    {'T', N_("True direction")},
    {'M', N_("Magnetic direction")},
};

//! GPS destination distance reference, tag 0x0019
constexpr TagDetails exifGPSDestDistanceRef[] = {
    {'K', N_("km")},
    {'M', N_("miles")},
    {'N', N_("nautical miles")},
};

//! GPS differential correction, tag 0x001e
constexpr TagDetails exifGPSDifferential[] = {
    {0, N_("Without correction")},
    {1, N_("Correction applied")},
};

//! CompositeImage, tag 0xa460
constexpr TagDetails exifCompositeImage[] = {
    {0, N_("Unknown")},
    {1, N_("NonComposite")},
    {2, N_("GeneralComposite")},
    {3, N_("CompositeCapturedWhenShooting")},
};

// configuratioin for value range validation. For example GPSLatitudeRef the value must be 'N' or 'S'.
std::map<std::string, std::tuple<const TagDetails*, const size_t>> ExifMetadataConverter::valueRangeValidateConfig = {
  {"Orientation", std::make_tuple(exifOrientation, std::size(exifOrientation))},
  {"GPSLatitudeRef", std::make_tuple(exifGPSLatitudeRef, std::size(exifGPSLatitudeRef))},
  {"GPSLongitudeRef", std::make_tuple(exifGPSLongitudeRef, std::size(exifGPSLongitudeRef))},
  {"WhiteBalance", std::make_tuple(exifWhiteBalance, std::size(exifWhiteBalance))},
  {"Flash", std::make_tuple(exifFlash, std::size(exifFlash))},
  {"LightSource", std::make_tuple(exifLightSource, std::size(exifLightSource))},
  {"MeteringMode", std::make_tuple(exifMeteringMode, std::size(exifMeteringMode))},
  {"SceneType", std::make_tuple(exifSceneType, std::size(exifSceneType))},
  {"Compression", std::make_tuple(exifCompression, std::size(exifCompression))},
  {"PhotometricInterpretation", std::make_tuple(exifPhotometricInterpretation, std::size(exifPhotometricInterpretation))},
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
  {"GPSStatus", std::make_tuple(exifGPSStatus, std::size(exifGPSStatus))},
  {"GPSMeasureMode", std::make_tuple(exifGPSMeasureMode, std::size(exifGPSMeasureMode))},
  {"GPSSpeedRef", std::make_tuple(exifGPSSpeedRef, std::size(exifGPSSpeedRef))},
  {"GPSImgDirectionRef", std::make_tuple(exifGPSDirRef, std::size(exifGPSDirRef))},
  {"GPSDestDistanceRef", std::make_tuple(exifGPSDestDistanceRef, std::size(exifGPSDestDistanceRef))},
  {"GPSDifferential", std::make_tuple(exifGPSDifferential, std::size(exifGPSDifferential))},
  {"CompositeImage", std::make_tuple(exifCompositeImage, std::size(exifCompositeImage))},
};

const std::set<std::string> HWALLOWMODIFYKEY = {"Exif.Huawei.CaptureMode"};
const std::string COMMAREGEX("\\,"), COLONREGEX("\\:"), DOTREGEX("\\.");

auto ONERATIONALREGEX = R"(^[0-9]+/[1-9][0-9]*$)";
auto ONEINTREGEX = R"(^[0-9]+$)";
auto ONEDECIMALREGEX = "(\\d+)(\\.\\d+)?";
auto DOUBLEINTWITHBLANKREGEX = R"(^[0-9]+\s[0-9]+$)";
auto DOUBLEINTWITHCOMMAREGEX = R"(^[0-9]+,[0-9]+$)";
auto TRIBLEINTWITHBLANKREGEX = R"(^[0-9]+\s[0-9]+\s[0-9]+$)";
auto TRIBLEINTWITHCOMMAREGEX = R"(^[0-9]+,[0-9]+,[0-9]+$)";
auto TRIBLERATIONALWITHBLANKREGEX = R"(^[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*$)";
auto TRIBLEINTWITHBLANKREGEX_ = R"(^[1-9][0-9]*\s[1-9][0-9]*\s[1-9][0-9]*$)";
auto TRIBLEINTWITHCOMMAREGEX_ = R"(^[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*$)";
auto TRIBLEDECIMALWITHBLANKREGEX = "(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?";
auto TRIBLEDECIMALWITHCOMMAREGEX = "(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?";
auto TRIBLEINTWITHCOLONREGEX = R"(^[1-9][0-9]*:[1-9][0-9]*:[1-9][0-9]*$)";
auto TRIBLEINTWITHDOTREGEX = R"(^[0-9]+.[0-9]+.[0-9]+.[0-9]+$)";
auto FOURINTWITHBLANKREGEX = R"(^[0-9]+\s[0-9]+\s[0-9]+\s[0-9]+$)";
auto FOURINTWITHCOMMAREGEX = R"(^[0-9]+,[0-9]+,[0-9]+,[0-9]+$)";
auto FOURINTWITHCOMMAREGEX_ = R"(^[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*$)";
auto FOURRATIONALWITHBLANKREGEX = R"(^[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*\s[0-9]+/[1-9][0-9]*$)";
auto FOURDECIMALWITHBLANKREGEX = "(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?";
auto FOURDECIMALWITHCOMMAREGEX = "(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?";
auto DATETIMEREGEX = R"(^[0-9]{4}:[0-9]{2}:[0-9]{2}\s[0-9]{2}:[0-9]{2}:[0-9]{2}$)";
auto DATEREGEX = R"(^[0-9]{4}:[0-9]{2}:[0-9]{2}$)";

// validate the key is in value range array. For example GPSLatitudeRef value should be 'N' or 'S' in exifGPSLatitudeRef array.
bool ExifMetadataConverter::IsValidValue(const TagDetails *array, const size_t &size, const int64_t &key)
{
  if (array == nullptr)
  {
    return false;
  }
  
  for (size_t i = 0; i < size; i++)
  {
    if (array[i].val_ == key)
    {
      return true;
    }
  }
  return false;
}

// validate regex only
bool ExifMetadataConverter::ValidRegex(const std::string &value, const std::string &regex)
{
  IMAGE_LOGD("[validRegex] value is [%{public}s] regex is [%{public}s].", value.c_str(), regex.c_str());
  std::regex ratPattern(regex);
  if (!std::regex_match(value, ratPattern))
  {
    IMAGE_LOGD("validRegex fail. value is [%{public}s] regex is [%{public}s].", value.c_str(), regex.c_str());
    return false;
  }
  return true;
}


// replace as space according to regex
void ExifMetadataConverter::ReplaceAsSpace(std::string &value, const std::string &regex)
{
  std::regex pattern(regex);
  value = std::regex_replace(value, pattern, " ");
  IMAGE_LOGD("replaceAsSpace [%{public}s]", value.c_str());
}

// validate the regex & replace comma as space
bool ExifMetadataConverter::ValidRegexWithComma(std::string &value, const std::string &regex)
{
  IMAGE_LOGD("[validRegexWithComma] value is [%{public}s] regex is [%{public}s].", value.c_str(), regex.c_str());
  if(!ValidRegex(value, regex)){
    return false;
  }

  ReplaceAsSpace(value, COMMAREGEX);
  return true;
}

// convert integer to rational format. For example 23 15 83 --> 23/1 15/1 83
void ExifMetadataConverter::RationalFormat(std::string &value)
{
  std::regex pattern("\\d+"); // regex for integer
  std::string result;
  int icount = 0;
  while (std::regex_search(value, pattern))
  {
    std::smatch match;
    if (std::regex_search(value, match, pattern))
    {
      if (icount != 0)
      {
        result += " ";
      }
      result += match.str() + "/1"; // appending '/1' to integer
      value = match.suffix().str(); // skip handled value part
      icount++;
    }
    else
    {
      break; // break since there is no matched value
    }
  }
  value = result;
}


// convert decimal to rational string. 2.5 -> 5/2
std::string ExifMetadataConverter::GetFractionFromStr(const std::string &decimal)
{
    int intPart = stoi(decimal.substr(0, decimal.find(".")));
    double decPart = stod(decimal.substr(decimal.find(".")));

    int numerator = decPart * pow(10, decimal.length() - decimal.find(".") - 1);
    int denominator = pow(10, decimal.length() - decimal.find(".") - 1);

    int gcdVal = ExifMetadataConverter::Gcd(numerator, denominator);
    if (gcdVal == 0) {
    }
    numerator /= gcdVal;
    denominator /= gcdVal;

    numerator += intPart * denominator;

    return std::to_string(numerator) + "/" + std::to_string(denominator);
}

// convert decial to rational format. For example 2.5 -> 5/2
void ExifMetadataConverter::DecimalRationalFormat(std::string &value)
{
  std::string result;
  int icount = 0;
  std::regex parPattern("(\\d+)(\\.\\d+)?");
  // handle each segment with partial regex. For 2.5 26 1.2 to iterator each segment 2.5->5/2  segment 26->26/1 segment 1.2->6/5
  for (std::sregex_iterator it = std::sregex_iterator(value.begin(), value.end(), parPattern);
       it != std::sregex_iterator(); ++it)
  {
    std::smatch match = *it;
    // HiLog::Debug(LABEL, "decimalRationalFormat match. value is [%{public}s].", match[0]);
    IMAGE_LOGD("decimalRationalFormat match. value is [%{public}s].", ((std::string)match[0]).c_str());
    // add a space at begin of each segment except the first segment
    if (icount != 0)
    {
      result += " ";
    }
    // handle each segment 1.if segment is integer type 123->123/1  2.if segment is decimal call decimalToFraction 2.5 -> 5/2
    if (ValidRegex(match[0], "\\d+"))
    {
      // append '/1' to integer 23 -> 23/1
      result += match.str() + "/1";
      IMAGE_LOGD("decimalRationalFormat identify match. integer value is [%{public}s].", ((std::string)match[0]).c_str());
    }
    else if (ValidRegex(match[0], "\\d+\\.\\d+"))
    {
      // convert decimal to rational. 2.5 -> 5/2
      result += GetFractionFromStr(match[0]);
      IMAGE_LOGD("decimalRationalFormat identify match. decimal value is [%{public}s].", ((std::string)match[0]).c_str());
    }
    icount++;
  }
  value = result;
}

// validate regex & convert integer to rational format. For example 23 15 83 --> 23/1 15/1 83
bool ExifMetadataConverter::ValidRegexWithRationalFormat(std::string &value, const std::string &regex)
{
  // 1.validate regex
  if(!ValidRegex(value, regex)){
    return false;
  }
  // 2.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
  RationalFormat(value);
  return true;
}


// validate regex & convert value to rational format. For example 9,9,9 -> 9 9 9 -> 9/1 9/1 9/1
bool ExifMetadataConverter::ValidRegexWithCommaRationalFormat(std::string &value, const std::string &regex)
{
  // 1.validate regex
  if(!ValidRegex(value, regex)){
    return false;
  }
  // 2.replace comma as a space
  ReplaceAsSpace(value, COMMAREGEX);
  // 3.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
  RationalFormat(value);
  return true;
}

// validate regex & convert value to rational format. For example 9:9:9 -> 9 9 9 -> 9/1 9/1 9/1
bool ExifMetadataConverter::ValidRegexWithColonRationalFormat(std::string &value, const std::string &regex)
{
  // 1.validate regex
  if(!ValidRegex(value, regex)){
    return false;
  }
  // 2.replace colon as a space
  ReplaceAsSpace(value, COLONREGEX);
  // 3.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
  RationalFormat(value);
  return true;
}

// validate regex & convert value to integer format. For example 2.2.0.0 -> 2 2 0 0
bool ExifMetadataConverter::ValidRegexWithDot(std::string &value, const std::string &regex)
{
  if (!ValidRegex(value, regex))
  {
    return false;
  }
  ReplaceAsSpace(value, DOTREGEX);
  return true;
}

// regex validation & convert decimal to rational. For example GPSLatitude 2.5,23,3.4 -> 2.5 23 3.4 -> 5/2 23/1 17/5
bool ExifMetadataConverter::ValidRegxWithCommaDecimalRationalFormat(std::string &value, const std::string &regex)
{
  if (!ValidRegex(value, regex))
  {
    return false;
  }

  // replace comma with a space 1.5,2.5.3 -> 1.5 2.5 3
  ReplaceAsSpace(value, COMMAREGEX);
  // convert decimal to rationl 2.5 -> 5/2
  DecimalRationalFormat(value);
  return true;
}

// regex validation & convert decimal to rational. For example GPSLatitude 2.5 23 3.4 -> 5/2 23/1 17/5
bool ExifMetadataConverter::ValidRegexWithDecimalRationalFormat(std::string &value, const std::string &regex)
{
  if (!ValidRegex(value, regex))
  {
    return false;
  }
  // convert decimal to rationl 2.5 -> 5/2
  DecimalRationalFormat(value);
  return true;
}


/* regex validation for two integer like DefaultCropSize 9 9 the format is [0-9]+ [0-9]+
   For example for DefaultCropSize, Raw images often store extra pixels around the edges of the final image. 
   These extra pixels help prevent interpolation artifacts near the edges of the final image. 
   DefaultCropSize specifies the size of the final image area, in raw image coordinates (i.e., before the DefaultScale has been applied).
   */
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::doubleIntWithBlank = std::make_pair(ExifMetadataConverter::ValidRegex, DOUBLEINTWITHBLANKREGEX);
// regex validation for two integer with comma like BitPerSample 9,9 the format is [0-9]+,[0-9]+,[0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::doubleIntWithComma = std::make_pair(ExifMetadataConverter::ValidRegexWithComma, DOUBLEINTWITHCOMMAREGEX);
// regex validation for three integer like BitPerSample 9 9 9 the format is [0-9]+ [0-9]+ [0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleIntWithBlank = std::make_pair(ExifMetadataConverter::ValidRegex, TRIBLEINTWITHBLANKREGEX);
// regex validation for three integer with comma like BitPerSample 9,9,0 the format is [0-9]+,[0-9]+,[0-9]+,[0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleIntWithComma = std::make_pair(ExifMetadataConverter::ValidRegexWithComma, TRIBLEINTWITHCOMMAREGEX);
// regex validation for four integer like DNGVersion 9 9 9 9 the format is [0-9]+ [0-9]+ [0-9]+ [0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::fourIntWithBlank = std::make_pair(ExifMetadataConverter::ValidRegex, FOURINTWITHBLANKREGEX);
/* regex validation for four integer with comma like DNGVersion 9,9,9,9 the format is [0-9]+,[0-9]+,[0-9]+
   For example, DNGVersion tag encodes the DNG four-tier version number.
   For files compliant with version 1.1.0.0 of the DNG specification, this tag should contain the bytes: 1, 1, 0, 0.
   */
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::fourIntWithComma = std::make_pair(ExifMetadataConverter::ValidRegexWithComma, FOURINTWITHCOMMAREGEX);
// regex validation for one rational like ApertureValue 4/1 the format is [0-9]+/[1-9][0-9]
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::oneRational = std::make_pair(ExifMetadataConverter::ValidRegex, ONERATIONALREGEX);
// regex validation for integer and convert it to rational like ApertureValue 4 --> 4/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::oneIntToRational = std::make_pair(ExifMetadataConverter::ValidRegexWithRationalFormat, ONEINTREGEX);
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::oneDecimalToRational = std::make_pair(ExifMetadataConverter::ValidRegexWithDecimalRationalFormat, ONEDECIMALREGEX);
// regex validation for three rational like GPSLatitude 39/1 54/1 20/1 the format is [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9]
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleRationalWithBlank = std::make_pair(ExifMetadataConverter::ValidRegex, TRIBLERATIONALWITHBLANKREGEX);
// regex validation for three integer and convert to three rational like GPSLatitude 39 54 20 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleIntToRationalWithBlank = std::make_pair(ExifMetadataConverter::ValidRegexWithRationalFormat, TRIBLEINTWITHBLANKREGEX_);
// regex validation for three integer with comma and convert to three rational like GPSLatitude 39,54,20 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleIntToRationalWithComma = std::make_pair(ExifMetadataConverter::ValidRegexWithCommaRationalFormat, TRIBLEINTWITHCOMMAREGEX_);
// regex validation for three decimal or integer and convert to three rational like YCbCrCoefficients 39.0 54 20.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleDecimalToRationalWithBlank = std::make_pair(ExifMetadataConverter::ValidRegexWithDecimalRationalFormat, TRIBLEDECIMALWITHBLANKREGEX);
// regex validation for three decimal or integer and convert to three rational like YCbCrCoefficients 39.0,54,20.0 --> 39.0 54 20.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleDecimalToRatiionalWithComma = std::make_pair(ExifMetadataConverter::ValidRegxWithCommaDecimalRationalFormat, TRIBLEDECIMALWITHCOMMAREGEX);
// regex validation for four rational like LensSpecification 1/1 3/2 1/1 2/1 the format is [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9]
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::fourRationalWithBlank = std::make_pair(ExifMetadataConverter::ValidRegex, FOURRATIONALWITHBLANKREGEX);
// regex validation for four integer and convert to four rational like LensSpecification 1 3 1 2 --> 1/1 3/2 1/1 2/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::fourIntToRationalWithBlank = std::make_pair(ExifMetadataConverter::ValidRegexWithRationalFormat, FOURINTWITHBLANKREGEX);
// regex validation for four integer with comma and convert to four rational like LensSpecification 1,3,1,2 --> 1/1 3/2 1/1 2/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::fourIntToRationalWithComma = std::make_pair(ExifMetadataConverter::ValidRegexWithCommaRationalFormat, FOURINTWITHCOMMAREGEX_);
// regex validation for four decimal or integer and convert to four rational like LensSpecification 1.0 3.0 1.0 2.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::decimal4Ratiional4 = std::make_pair(ExifMetadataConverter::ValidRegexWithDecimalRationalFormat, FOURDECIMALWITHBLANKREGEX);
// regex validation for four decimal or integer and convert to four rational like LensSpecification 1.0,3.0,1.0,2.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::decimal4Ratiional4Comma = std::make_pair(ExifMetadataConverter::ValidRegxWithCommaDecimalRationalFormat, FOURDECIMALWITHCOMMAREGEX);
// regex validation for datetime format like DateTimeOriginal 2022:06:02 15:51:34
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::dateTimeValidation = std::make_pair(ExifMetadataConverter::ValidRegex, DATETIMEREGEX);
// regex validation for datetime format like DateTimeOriginal 2022:06:02
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::dateValidation = std::make_pair(ExifMetadataConverter::ValidRegex, DATEREGEX);
//regex validation for three integer with colon and convert to three rational like GPSLatitude 39,54,21 --> 39/1 54/1 21/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::tribleIntToRationalWithColon = std::make_pair(ExifMetadataConverter::ValidRegexWithColonRationalFormat, TRIBLEINTWITHCOLONREGEX);
// regex validation for fou integer with pointer like GPSVersionID 2.2.0.0 -> 2 2 0 0 
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetadataConverter::fourIntToRationalWithDot = std::make_pair(ExifMetadataConverter::ValidRegexWithDot, TRIBLEINTWITHDOTREGEX);

// configuration for value format validation. For example BitPerSample the value format should be 9 9 9 or 9,9,9
std::multimap<std::string, std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string>> ExifMetadataConverter::valueFormatValidateConfig = {
  {"BitsPerSample",  tribleIntWithBlank},
  {"BitsPerSample",  tribleIntWithComma},
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

std::multimap<std::string, std::string> ExifMetadataConverter::valueFormatValidateConfig_ = {
  {"BitsPerSample",  TRIBLEINTWITHCOMMAREGEX},
  {"ImageLength",  ONEINTREGEX},
  {"ImageWidth",  ONEINTREGEX},
  {"GPSLatitude", DOUBLEINTWITHCOMMAREGEX},
  {"GPSLatitude", TRIBLEINTWITHCOMMAREGEX},
  {"GPSLatitude", TRIBLEDECIMALWITHCOMMAREGEX},
  {"GPSLongitude", DOUBLEINTWITHCOMMAREGEX},
  {"GPSLongitude", TRIBLEINTWITHCOMMAREGEX},
  {"GPSLongitude", TRIBLEDECIMALWITHCOMMAREGEX}
};

// validate the value range. For example GPSLatitudeRef the value must be 'N' or 'S'.
int32_t ExifMetadataConverter::ValidateValueRange(const std::string &keyName, const std::string &value)
{
    // 1. to find if any value range validation configuratiion according to exif tag in std::map container
    auto iter  = valueRangeValidateConfig.find(keyName);
    if (iter != valueRangeValidateConfig.end()) {
        // get value range array & size
        auto &[arrRef, arrSize] = iter->second;
        if (arrRef != nullptr)
        {
          int32_t ivalue = -1;
          // validate value if integer or char 2.char ascii
          std::regex regNum(R"(^[0-9]+$)"); // regex for integer value. For example WhiteBalance support 0 or 1
          std::regex regChar(R"(^[a-zA-Z]$)"); // regex for char value. For example GPSLatitudeRef support N or S
          if (std::regex_match(value, regNum)) {
              // convert string to integer such as "15" -> 15
              ivalue = std::stoll(value);
              IMAGE_LOGD("isValueRangeValidate convert to integer. value string is [%{public}s] ivalue is[%{public}d].", value.c_str(), ivalue);
          }else if(std::regex_match(value, regChar)){
              // convert char to integer such as "N" -> 78
              ivalue = static_cast<int32_t>(value[0]);
              IMAGE_LOGD("isValueRangeValidate convert char to integer. value char is [%{public}s] ivalue is[%{public}d].", value.c_str(), ivalue);
          }

          // if ivalue is not converted then return FAIL
          if (ivalue == -1)
          {
            IMAGE_LOGD("isValueRangeValidate input does not match regex invalid value is [%{public}s].", value.c_str());
            return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; //FAIL
          }
          // validate the ivalue is in value range array. For example GPSLatitudeRef value should be 'N' or 'S' in exifGPSLatitudeRef array.
          auto isValid = IsValidValue(arrRef, arrSize, ivalue);
          if (!isValid)
          {
            IMAGE_LOGD("isValueRangeValidate input is not in range array. invalid value is [%{public}s].", value.c_str());
            return Media::ERR_MEDIA_OUT_OF_RANGE;
          }else{
            IMAGE_LOGD("isValueRangeValidate valid value is [%{public}s].", value.c_str());
            return Media::SUCCESS;
          }
        }else{
          // return FAIL if the value range array nullptr.
          return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
        }
    }

    // Default is SUCCESS if there is no value range configuration
    return Media::SUCCESS; //SUCCESS
}

// check if has any value format configuration
bool ExifMetadataConverter::IsFormatValidationConfigExisting(const std::string &keyName)
{
  IMAGE_LOGD("hasValueFormatValidate keyName is [%{public}s].", keyName.c_str());
  return ExifMetadataConverter::valueFormatValidateConfig.find(keyName) != ExifMetadataConverter::valueFormatValidateConfig.end();
}

// validate value format. For example BitPerSample the value format should be 9 9 9 or 9,9,9
int32_t ExifMetadataConverter::ValidateValueFormat(const std::string &keyName, std::string &value)
{
  IMAGE_LOGD("isValueFormatValidate keyName is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
  // get first iterator according to keyName
  for (auto iterator = ExifMetadataConverter::valueFormatValidateConfig.find(keyName); iterator != ExifMetadataConverter::valueFormatValidateConfig.end() && iterator != ExifMetadataConverter::valueFormatValidateConfig.upper_bound(keyName); iterator++)
  {
    IMAGE_LOGD("isValueFormatValidate forloop keyName is [%{public}s] regex string is [%{public}s].", (iterator->first).c_str(), (iterator->second).second.c_str());
    auto func = (iterator->second).first;
    // call each value format function with value and regex
    int32_t i = func(value, (iterator->second).second);
    IMAGE_LOGD("isValueFormatValidate ret i is [%{public}d].", i);
    if (i)
    {
      IMAGE_LOGD("isValueFormatValidate ret SUCCESS.");
      return Media::SUCCESS; // SUCCESS
    }
  }

  IMAGE_LOGD("isValueFormatValidate ret ERR_IMAGE_DECODE_EXIF_UNSUPPORT.");
  return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; // FAILED
}

int32_t ExifMetadataConverter::ValidateValueFormat_(const std::string &keyName, const std::string &value)
{
  IMAGE_LOGD("isValueFormatValidate keyName is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
  // get first iterator according to keyName
  for (auto iterator = ExifMetadataConverter::valueFormatValidateConfig_.find(keyName); iterator != ExifMetadataConverter::valueFormatValidateConfig_.end() && iterator != ExifMetadataConverter::valueFormatValidateConfig_.upper_bound(keyName); iterator++)
  {
    bool isValidated = ExifMetadataConverter::ValidRegex(value, iterator->second);
    IMAGE_LOGD("isValueFormatValidate ret i is [%{public}d].", isValidated);
    if (isValidated)
    {
      IMAGE_LOGD("isValueFormatValidate ret SUCCESS.");
      return Media::SUCCESS; // SUCCESS
    }
  }

  IMAGE_LOGD("isValueFormatValidate ret ERR_IMAGE_DECODE_EXIF_UNSUPPORT.");
  return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; // FAILED
}

bool ExifMetadataConverter::IsKeySupported(const std::string &keyName)
{
  auto it = SUPPORTKEYS.find(keyName);
  return !(it == SUPPORTKEYS.end());
}

// disable modify huawei exif tag except for Exif.Huawei.CaptureMode
bool ExifMetadataConverter::IsModifyAllowed(const std::string &keyName)
{
  auto it = SUPPORTKEYS.find(keyName);
  if(it != SUPPORTKEYS.end() && it->second == READ_WRITE){
    return true;
  }
  return false;
}

// exif validation portal when modify exif
std::pair<int32_t, std::string> ExifMetadataConverter::Convert(const std::string &keyName, const std::string &value)
{
  std::string value_ = "";
  IMAGE_LOGD("[ValidateAndConvert] in exifValidate.");
  // translate exif tag. For example translate "BitsPerSample" to "Exif.Image.BitsPerSample"
  if(!ExifMetadataConverter::IsKeySupported(keyName))
  {
      return std::make_pair(Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT, nullptr);
  }

  // disable modify huawei exif tag except for Exif.Huawei.CaptureMode
  if (!ExifMetadataConverter::IsModifyAllowed(keyName)) {
      return std::make_pair(Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT, nullptr);
  }

  IMAGE_LOGD("[ValidateAndConvert] keyName is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
  // 1.validate value format
  IMAGE_LOGD("[ValidateAndConvert] hasValueFormatValidate is [%{public}d].", ExifMetadataConverter::IsFormatValidationConfigExisting(keyName));
  auto r = ExifMetadataConverter::ValidateValueFormat(keyName, value_);
  IMAGE_LOGD("[ValidateAndConvert] isValueFormatValidate is [%{public}d].", r);
  if (ExifMetadataConverter::IsFormatValidationConfigExisting(keyName) && r)
  {
    IMAGE_LOGD("[ValidateAndConvert] value formate is invalid. keyName is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
    return std::make_pair(Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT, nullptr); //value format validate does not pass
  }
  IMAGE_LOGD("[ValidateAndConvert] processed formate value is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
  // 2.validate value range
  if (ExifMetadataConverter::ValidateValueRange(keyName, value))
  {
    IMAGE_LOGD("[ValidateAndConvert] value range is invalid. value is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
    return std::make_pair(Media::ERR_MEDIA_OUT_OF_RANGE, nullptr); // value range validate does not pass
  }
  return std::make_pair(Media::SUCCESS, value_);
}

// exif validation portal
int32_t ExifMetadataConverter::Validate(const std::string &keyName, const std::string &value)
{
  IMAGE_LOGD("[ValidateAndConvert] in exifValidate.");
  // translate exif tag. For example translate "BitsPerSample" to "Exif.Image.BitsPerSample"
  if(!ExifMetadataConverter::IsKeySupported(keyName))
  {
      return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
  }

  // disable modify huawei exif tag except for Exif.Huawei.CaptureMode
  if (!ExifMetadataConverter::IsModifyAllowed(keyName)) {
      return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
  }

  IMAGE_LOGD("[ValidateAndConvert] keyName is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
  // 1.validate value format
  IMAGE_LOGD("[ValidateAndConvert] hasValueFormatValidate is [%{public}d].", ExifMetadataConverter::IsFormatValidationConfigExisting(keyName));
  auto r = ExifMetadataConverter::ValidateValueFormat_(keyName, value);
  IMAGE_LOGD("[ValidateAndConvert] isValueFormatValidate is [%{public}d].", r);
  bool isFormatExisting = (ExifMetadataConverter::valueFormatValidateConfig_.find(keyName) != ExifMetadataConverter::valueFormatValidateConfig_.end());
  if (isFormatExisting && r)
  {
    IMAGE_LOGD("[ValidateAndConvert] value formate is invalid. keyName is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
    return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; //value format validate does not pass
  }
  IMAGE_LOGD("[ValidateAndConvert] processed formate value is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
  // 2.validate value range
  if (ExifMetadataConverter::ValidateValueRange(keyName, value))
  {
    IMAGE_LOGD("[ValidateAndConvert] value range is invalid. value is [%{public}s] value is [%{public}s].", keyName.c_str(), value.c_str());
    return Media::ERR_MEDIA_OUT_OF_RANGE; // value range validate does not pass
  }
  return Media::SUCCESS;
}


} // namespace Media
} // namespace OHOS
