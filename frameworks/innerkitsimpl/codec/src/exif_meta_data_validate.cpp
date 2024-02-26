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
#include "exif_meta_data_validate.h"
#include "hilog/log_cpp.h"
#include "string_ex.h"
#include "media_errors.h"
#include "hilog/log.h"
#include "image_log.h"

#define N_(String) (String)

namespace OHOS {
namespace Media {
const std::map<std::string, std::string> TAG_MAP = {
    {"BitsPerSample",           "Exif.Image.BitsPerSample"},
    {"Orientation",             "Exif.Image.Orientation"},
    {"ImageLength",             "Exif.Image.ImageLength"},
    {"ImageWidth",              "Exif.Image.ImageWidth"},
    {"GPSLatitude",             "Exif.GPSInfo.GPSLatitude"},
    {"GPSLongitude",            "Exif.GPSInfo.GPSLongitude"},
    {"GPSLatitudeRef",          "Exif.GPSInfo.GPSLatitudeRef"},
    {"GPSLongitudeRef",         "Exif.GPSInfo.GPSLongitudeRef"},
    {"DateTimeOriginal",        "Exif.Photo.DateTimeOriginal"},
    {"ExposureTime",            "Exif.Photo.ExposureTime"},
    {"FNumber",                 "Exif.Photo.FNumber"},
    {"ISOSpeed",                "Exif.Photo.ISOSpeed"},
    {"ISOSpeedRatings",         "Exif.Photo.ISOSpeedRatings"},
    {"SceneType",               "Exif.Photo.SceneType"},
    {"DateTime",                "Exif.Image.DateTime"},
    {"GPSTimeStamp",            "Exif.GPSInfo.GPSTimeStamp"},
    {"GPSDateStamp",            "Exif.GPSInfo.GPSDateStamp"},
    {"ImageDescription",        "Exif.Image.ImageDescription"},
    {"Make",                    "Exif.Image.Make"},
    {"Model",                   "Exif.Image.Model"},
    {"SensitivityType",         "Exif.Photo.SensitivityType"},
    {"StandardOutputSensitivity","Exif.Photo.StandardOutputSensitivity"},
    {"RecommendedExposureIndex", "Exif.Photo.RecommendedExposureIndex"},
    {"ApertureValue",           "Exif.Photo.ApertureValue"},
    {"ExposureBiasValue",       "Exif.Photo.ExposureBiasValue"},
    {"MeteringMode",            "Exif.Photo.MeteringMode"},
    {"LightSource",             "Exif.Photo.LightSource"},
    {"Flash",                   "Exif.Photo.Flash"},
    {"FocalLength",             "Exif.Photo.FocalLength"},
    {"UserComment",             "Exif.Photo.UserComment"},
    {"PixelXDimension",         "Exif.Photo.PixelXDimension"},
    {"PixelYDimension",         "Exif.Photo.PixelYDimension"},
    {"WhiteBalance",            "Exif.Photo.WhiteBalance"},
    {"FocalLengthIn35mmFilm",   "Exif.Photo.FocalLengthIn35mmFilm"},
    {"Compression",             "Exif.Image.Compression"},
    {"PhotometricInterpretation", "Exif.Image.PhotometricInterpretation"},
    {"StripOffsets",            "Exif.Image.StripOffsets"},
    {"SamplesPerPixel",         "Exif.Image.SamplesPerPixel"},
    {"RowsPerStrip",            "Exif.Image.RowsPerStrip"},
    {"StripByteCounts",         "Exif.Image.StripByteCounts"},
    {"XResolution",             "Exif.Image.XResolution"},
    {"YResolution",             "Exif.Image.YResolution"},
    {"PlanarConfiguration",     "Exif.Image.PlanarConfiguration"},
    {"ResolutionUnit",          "Exif.Image.ResolutionUnit"},
    {"TransferFunction",        "Exif.Image.TransferFunction"},
    {"Software",                "Exif.Image.Software"},
    {"Artist",                  "Exif.Image.Artist"},
    {"WhitePoint",              "Exif.Image.WhitePoint"},
    {"PrimaryChromaticities",   "Exif.Image.PrimaryChromaticities"},
    {"ReferenceBlackWhite",     "Exif.Image.ReferenceBlackWhite"},
    {"Copyright",               "Exif.Image.Copyright"},
    {"JPEGInterchangeFormat",   "Exif.Image.JPEGInterchangeFormat"},
    {"JPEGInterchangeFormatLength", "Exif.Image.JPEGInterchangeFormatLength"},
    {"ExposureProgram",         "Exif.Photo.ExposureProgram"},
    {"SpectralSensitivity",     "Exif.Image.SpectralSensitivity"},
    {"OECF",                    "Exif.Image.OECF"},
    {"ExifVersion",             "Exif.Photo.ExifVersion"},
    {"DateTimeDigitized",       "Exif.Photo.DateTimeDigitized"},
    {"ComponentsConfiguration", "Exif.Photo.ComponentsConfiguration"},
    {"ShutterSpeedValue",       "Exif.Photo.ShutterSpeedValue"},
    {"BrightnessValue",         "Exif.Photo.BrightnessValue"},
    {"MaxApertureValue",        "Exif.Image.MaxApertureValue"},
    {"SubjectDistance",         "Exif.Image.SubjectDistance"},
    {"SubjectArea",             "Exif.Image.SubjectLocation"},
    {"MakerNote",               "Exif.Photo.MakerNote"},
    {"SubsecTime",              "Exif.Photo.SubSecTime"},
    {"SubSecTimeOriginal",      "Exif.Photo.SubSecTimeOriginal"},
    {"SubSecTimeDigitized",     "Exif.Photo.SubSecTimeDigitized"},
    {"FlashpixVersion",         "Exif.Photo.FlashpixVersion"},
    {"ColorSpace",              "Exif.Photo.ColorSpace"},
    {"RelatedSoundFile",        "Exif.Photo.RelatedSoundFile"},
    {"FlashEnergy",             "Exif.Photo.FlashEnergy"},
    {"SpatialFrequencyResponse","Exif.Photo.SpatialFrequencyResponse"},
    {"FocalPlaneXResolution",   "Exif.Photo.FocalPlaneXResolution"},
    {"FocalPlaneYResolution",   "Exif.Photo.FocalPlaneYResolution"},
    {"FocalPlaneResolutionUnit","Exif.Photo.FocalPlaneResolutionUnit"},
    {"SubjectLocation",         "Exif.Photo.SubjectLocation"},
    {"ExposureIndex",           "Exif.Photo.ExposureIndex"},
    {"SensingMethod",           "Exif.Photo.SensingMethod"},
    {"FileSource",              "Exif.Photo.FileSource"},
    {"CFAPattern",              "Exif.Photo.CFAPattern"},
    {"CustomRendered",          "Exif.Photo.CustomRendered"},
    {"ExposureMode",            "Exif.Photo.ExposureMode"},
    {"DigitalZoomRatio",        "Exif.Photo.DigitalZoomRatio"},
    {"SceneCaptureType",        "Exif.Photo.SceneCaptureType"},
    {"GainControl",             "Exif.Photo.GainControl"},
    {"Contrast",                "Exif.Photo.Contrast"},
    {"Saturation",              "Exif.Photo.Saturation"},
    {"Sharpness",               "Exif.Photo.Sharpness"},
    {"DeviceSettingDescription","Exif.Photo.DeviceSettingDescription"},
    {"SubjectDistanceRange",    "Exif.Photo.SubjectDistanceRange"},
    {"ImageUniqueID",           "Exif.Photo.ImageUniqueID"},
    {"GPSVersionID",            "Exif.GPSInfo.GPSVersionID"},
    {"GPSAltitudeRef",          "Exif.GPSInfo.GPSAltitudeRef"},
    {"GPSAltitude",             "Exif.GPSInfo.GPSAltitude"},
    {"GPSSatellites",           "Exif.GPSInfo.GPSSatellites"},
    {"GPSStatus",               "Exif.GPSInfo.GPSStatus"},
    {"GPSMeasureMode",          "Exif.GPSInfo.GPSMeasureMode"},
    {"GPSDOP",                  "Exif.GPSInfo.GPSDOP"},
    {"GPSSpeedRef",             "Exif.GPSInfo.GPSSpeedRef"},
    {"GPSSpeed",                "Exif.GPSInfo.GPSSpeed"},
    {"GPSTrackRef",             "Exif.GPSInfo.GPSTrackRef"},
    {"GPSTrack",                "Exif.GPSInfo.GPSTrack"},
    {"GPSImgDirectionRef",      "Exif.GPSInfo.GPSImgDirectionRef"},
    {"GPSImgDirection",         "Exif.GPSInfo.GPSImgDirection"},
    {"GPSMapDatum",             "Exif.GPSInfo.GPSMapDatum"},
    {"GPSDestLatitudeRef",      "Exif.GPSInfo.GPSDestLatitudeRef"},
    {"GPSDestLatitude",         "Exif.GPSInfo.GPSDestLatitude"},
    {"GPSDestLongitudeRef",     "Exif.GPSInfo.GPSDestLongitudeRef"},
    {"GPSDestLongitude",        "Exif.GPSInfo.GPSDestLongitude"},
    {"GPSDestBearingRef",       "Exif.GPSInfo.GPSDestBearingRef"},
    {"GPSDestBearing",          "Exif.GPSInfo.GPSDestBearing"},
    {"GPSDestDistanceRef",      "Exif.GPSInfo.GPSDestDistanceRef"},
    {"GPSDestDistance",         "Exif.GPSInfo.GPSDestDistance"},
    {"GPSProcessingMethod",     "Exif.GPSInfo.GPSProcessingMethod"},
    {"GPSAreaInformation",      "Exif.GPSInfo.GPSAreaInformation"},
    {"GPSDifferential",         "Exif.GPSInfo.GPSDifferential"},
    {"YCbCrCoefficients",       "Exif.Image.YCbCrCoefficients"},
    {"YCbCrSubSampling",        "Exif.Image.YCbCrSubSampling"},
    {"YCbCrPositioning",        "Exif.Image.YCbCrPositioning"},
    {"CompressedBitsPerPixel",  "Exif.Photo.CompressedBitsPerPixel"},
    {"JPEGProc",                "Exif.Image.JPEGProc"},
    {"BodySerialNumber",        "Exif.Photo.BodySerialNumber"},
    {"CameraOwnerName",         "Exif.Photo.CameraOwnerName"},
    {"CompositeImage",          "Exif.Photo.CompositeImage"},
    {"CompressedBitsPerPixel",  "Exif.Image.CompressedBitsPerPixel"},
    {"DNGVersion",              "Exif.Image.DNGVersion"},
    {"DefaultCropSize",         "Exif.Image.DefaultCropSize"},
    {"Gamma",                   "Exif.Photo.Gamma"},
    {"ISOSpeedLatitudeyyy",     "Exif.Photo.ISOSpeedLatitudeyyy"},
    {"ISOSpeedLatitudezzz",     "Exif.Photo.ISOSpeedLatitudezzz"},
    {"LensMake",                "Exif.Photo.LensMake"},
    {"LensModel",               "Exif.Photo.LensModel"},
    {"LensSerialNumber",        "Exif.Photo.LensSerialNumber"},
    {"LensSpecification",       "Exif.Photo.LensSpecification"},
    {"NewSubfileType",          "Exif.Image.NewSubfileType"},
    {"OffsetTime",              "Exif.Photo.OffsetTime"},
    {"OffsetTimeDigitized",     "Exif.Photo.OffsetTimeDigitized"},
    {"OffsetTimeOriginal",      "Exif.Photo.OffsetTimeOriginal"},
    {"SourceExposureTimesOfCompositeImage", "Exif.Photo.SourceExposureTimesOfCompositeImage"},
    {"SourceImageNumberOfCompositeImage", "Exif.Photo.SourceImageNumberOfCompositeImage"},
    {"SubfileType",             "Exif.Image.SubfileType"},
    {"GPSHPositioningError",    "Exif.GPSInfo.GPSHPositioningError"},
    {"HwMnoteCaptureMode",      "Exif.Huawei.CaptureMode"},
    {"HwMnotePhysicalAperture", "Exif.Huawei.PhysicalAperture"},
    {"HwMnoteRollAngle",        "Exif.Huawei.RollAngle"},
    {"HwMnotePitchAngle",       "Exif.Huawei.PitchAngle"},
    {"HwMnoteSceneFoodConf",    "Exif.HuaweiSi.FoodConf"},
    {"HwMnoteSceneStageConf",   "Exif.HuaweiSi.StageConf"},
    {"HwMnoteSceneBlueSkyConf", "Exif.HuaweiSi.BlueSkyConf"},
    {"HwMnoteSceneGreenPlantConf", "Exif.HuaweiSi.GreenPlantConf"},
    {"HwMnoteSceneBeachConf",   "Exif.HuaweiSi.BeachConf"},
    {"HwMnoteSceneSnowConf",    "Exif.HuaweiSi.SnowConf"},
    {"HwMnoteSceneSunsetConf",  "Exif.HuaweiSi.SunsetConf"},
    {"HwMnoteSceneFlowersConf", "Exif.HuaweiSi.FlowersConf"},
    {"HwMnoteSceneNightConf",   "Exif.HuaweiSi.NightConf"},
    {"HwMnoteSceneTextConf",    "Exif.HuaweiSi.TextConf"},
    {"HwMnoteFaceCount",        "Exif.Huawei.FaceCount"},
    {"HwMnoteFocusMode",        "Exif.Huawei.FocusMode"},
    {"HwMnoteFrontCamera",      "Exif.Huawei.FrontCamera"},
    {"HwMnoteSceneVersion",     "Exif.Huawei.SceneVersion"},
    {"HwMnoteScenePointer",     "Exif.Huawei.ScenePointer"},
    {"HwMnoteFacePointer",      "Exif.Huawei.FacePointer"},
    {"HwMnoteBurstNumber",      "Exif.Huawei.BurstNumber"},
    {"HwMnoteFaceVersion",      "Exif.HuaweiFi.FaceVersion"},
    {"HwMnoteFaceConf",         "Exif.HuaweiFi.FaceConf"},
    {"HwMnoteFaceSmileScore",   "Exif.HuaweiFi.FaceSmileScore"},
    {"HwMnoteFaceRect",         "Exif.HuaweiFi.FaceRect"},
    {"HwMnoteFaceLeyeCenter",   "Exif.HuaweiFi.FaceLeyeCenter"},
    {"HwMnoteFaceReyeCenter",   "Exif.HuaweiFi.FaceReyeCenter"},
    {"HwMnoteFaceMouthCenter",  "Exif.HuaweiFi.FaceMouthCenter"},
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
std::map<std::string, std::tuple<const TagDetails*, const size_t>> ExifMetaDataValidate::valueRangeValidateConfig = {
  {"Exif.Image.Orientation", std::make_tuple(exifOrientation, std::size(exifOrientation))},
  {"Exif.GPSInfo.GPSLatitudeRef", std::make_tuple(exifGPSLatitudeRef, std::size(exifGPSLatitudeRef))},
  {"Exif.GPSInfo.GPSLongitudeRef", std::make_tuple(exifGPSLongitudeRef, std::size(exifGPSLongitudeRef))},
  {"Exif.Photo.WhiteBalance", std::make_tuple(exifWhiteBalance, std::size(exifWhiteBalance))},
  {"Exif.Photo.Flash", std::make_tuple(exifFlash, std::size(exifFlash))},
  {"Exif.Photo.LightSource", std::make_tuple(exifLightSource, std::size(exifLightSource))},
  {"Exif.Photo.MeteringMode", std::make_tuple(exifMeteringMode, std::size(exifMeteringMode))},
  {"Exif.Photo.SceneType", std::make_tuple(exifSceneType, std::size(exifSceneType))},
  {"Exif.Image.Compression", std::make_tuple(exifCompression, std::size(exifCompression))},
  {"Exif.Image.PhotometricInterpretation", std::make_tuple(exifPhotometricInterpretation, std::size(exifPhotometricInterpretation))},
  {"Exif.Image.PlanarConfiguration", std::make_tuple(exifPlanarConfiguration, std::size(exifPlanarConfiguration))},
  {"Exif.Image.ResolutionUnit", std::make_tuple(exifUnit, std::size(exifUnit))},
  {"Exif.Image.YCbCrPositioning", std::make_tuple(exifYCbCrPositioning, std::size(exifYCbCrPositioning))},
  {"Exif.Photo.ExposureProgram", std::make_tuple(exifExposureProgram, std::size(exifExposureProgram))},
  {"Exif.Photo.ColorSpace", std::make_tuple(exifColorSpace, std::size(exifColorSpace))},
  {"Exif.Photo.FocalPlaneResolutionUnit", std::make_tuple(exifUnit, std::size(exifUnit))},
  {"Exif.Photo.SensingMethod", std::make_tuple(tiffSensingMethod, std::size(tiffSensingMethod))},
  {"Exif.Photo.CustomRendered", std::make_tuple(exifCustomRendered, std::size(exifCustomRendered))},
  {"Exif.Photo.ExposureMode", std::make_tuple(exifExposureMode, std::size(exifExposureMode))},
  {"Exif.Photo.SceneCaptureType", std::make_tuple(exifSceneCaptureType, std::size(exifSceneCaptureType))},
  {"Exif.Photo.GainControl", std::make_tuple(exifGainControl, std::size(exifGainControl))},
  {"Exif.Photo.Contrast", std::make_tuple(exifNormalSoftHard, std::size(exifNormalSoftHard))},
  {"Exif.Photo.Saturation", std::make_tuple(exifSaturation, std::size(exifSaturation))},
  {"Exif.Photo.Sharpness", std::make_tuple(exifNormalSoftHard, std::size(exifNormalSoftHard))},
  {"Exif.Photo.SubjectDistanceRange", std::make_tuple(exifSubjectDistanceRange, std::size(exifSubjectDistanceRange))},
  {"Exif.GPSInfo.GPSAltitudeRef", std::make_tuple(exifGPSAltitudeRef, std::size(exifGPSAltitudeRef))},
  {"Exif.Image.NewSubfileType", std::make_tuple(exifNewSubfileType, std::size(exifNewSubfileType))},
  {"Exif.Image.SubfileType", std::make_tuple(exifSubfileType, std::size(exifSubfileType))},
  {"Exif.GPSInfo.GPSStatus", std::make_tuple(exifGPSStatus, std::size(exifGPSStatus))},
  {"Exif.GPSInfo.GPSMeasureMode", std::make_tuple(exifGPSMeasureMode, std::size(exifGPSMeasureMode))},
  {"Exif.GPSInfo.GPSSpeedRef", std::make_tuple(exifGPSSpeedRef, std::size(exifGPSSpeedRef))},
  {"Exif.GPSInfo.GPSImgDirectionRef", std::make_tuple(exifGPSDirRef, std::size(exifGPSDirRef))},
  {"Exif.GPSInfo.GPSDestDistanceRef", std::make_tuple(exifGPSDestDistanceRef, std::size(exifGPSDestDistanceRef))},
  {"Exif.GPSInfo.GPSDifferential", std::make_tuple(exifGPSDifferential, std::size(exifGPSDifferential))},
  {"Exif.Photo.CompositeImage", std::make_tuple(exifCompositeImage, std::size(exifCompositeImage))},
};

// validate the key is in value range array. For example GPSLatitudeRef value should be 'N' or 'S' in exifGPSLatitudeRef array.
bool ExifMetaDataValidate::IsValidValue(const TagDetails *array, const size_t &size, const int64_t &key)
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
bool ExifMetaDataValidate::ValidRegex(const std::string &value, const std::string &regex){
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
void ExifMetaDataValidate::ReplaceAsSpace(std::string &value, const std::string &regex){
  std::regex pattern(regex);
  value = std::regex_replace(value, pattern, " ");
  IMAGE_LOGD("replaceAsSpace [%{public}s]", value.c_str());
}

// validate the regex & replace comma as space
bool ExifMetaDataValidate::ValidRegexWithComma(std::string &value, const std::string &regex){
  IMAGE_LOGD("[validRegexWithComma] value is [%{public}s] regex is [%{public}s].", value.c_str(), regex.c_str());
  if(!ValidRegex(value, regex)){
    return false;
  }

  ReplaceAsSpace(value, "\\,");
  return true;
}

// convert integer to rational format. For example 23 15 83 --> 23/1 15/1 83
void ExifMetaDataValidate::RationalFix(std::string &value)
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
std::string ExifMetaDataValidate::GetFractionFromStr(const std::string &decimal)
{
    int intPart = stoi(decimal.substr(0, decimal.find(".")));
    double decPart = stod(decimal.substr(decimal.find(".")));

    int numerator = decPart * pow(10, decimal.length() - decimal.find(".") - 1);
    int denominator = pow(10, decimal.length() - decimal.find(".") - 1);

    int gcdVal = ExifMetaDataValidate::Gcd(numerator, denominator);
    if (gcdVal == 0) {
    }
    numerator /= gcdVal;
    denominator /= gcdVal;

    numerator += intPart * denominator;

    return std::to_string(numerator) + "/" + std::to_string(denominator);
}

// convert decial to rational format. For example 2.5 -> 5/2
void ExifMetaDataValidate::DecimalRationalFix(std::string &value)
{
  std::string result;
  int icount = 0;
  std::regex parPattern("(\\d+)(\\.\\d+)?");
  // handle each segment with partial regex. For 2.5 26 1.2 to iterator each segment 2.5->5/2  segment 26->26/1 segment 1.2->6/5
  for (std::sregex_iterator it = std::sregex_iterator(value.begin(), value.end(), parPattern);
       it != std::sregex_iterator(); ++it)
  {
    std::smatch match = *it;
    // HiLog::Debug(LABEL, "decimalRationalFix match. value is [%{public}s].", match[0]);
    IMAGE_LOGD("decimalRationalFix match. value is [%{public}s].", match[0]);
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
      IMAGE_LOGD("decimalRationalFix identify match. integer value is [%{public}s].", match[0]);
    }
    else if (ValidRegex(match[0], "\\d+\\.\\d+"))
    {
      // convert decimal to rational. 2.5 -> 5/2
      result += GetFractionFromStr(match[0]);
      IMAGE_LOGD("decimalRationalFix identify match. decimal value is [%{public}s].", match[0]);
    }
    icount++;
  }
  value = result;
}

// validate regex & convert integer to rational format. For example 23 15 83 --> 23/1 15/1 83
bool ExifMetaDataValidate::ValidRegexWithRationalFix(std::string &value, const std::string &regex){
  // 1.validate regex
  if(!ValidRegex(value, regex)){
    return false;
  }
  // 2.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
  RationalFix(value);
  return true;
}


// validate regex & convert value to rational format. For example 9,9,9 -> 9 9 9 -> 9/1 9/1 9/1
bool ExifMetaDataValidate::ValidRegexWithCommaRationalFix(std::string &value, const std::string &regex){
  // 1.validate regex
  if(!ValidRegex(value, regex)){
    return false;
  }
  // 2.replace comma as a space
  ReplaceAsSpace(value, "\\,");
  // 3.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
  RationalFix(value);
  return true;
}

// validate regex & convert value to rational format. For example 9:9:9 -> 9 9 9 -> 9/1 9/1 9/1
bool ExifMetaDataValidate::ValidRegexWithColonRationalFix(std::string &value, const std::string &regex){
  // 1.validate regex
  if(!ValidRegex(value, regex)){
    return false;
  }
  // 2.replace colon as a space
  ReplaceAsSpace(value, "\\:");
  // 3.convert integer to rational format. 9 9 9 -> 9/1 9/1 9/1
  RationalFix(value);
  return true;
}

// validate regex & convert value to integer format. For example 2.2.0.0 -> 2 2 0 0
bool ExifMetaDataValidate::ValidRegexWithDot(std::string &value, const std::string &regex)
{
  if (!ValidRegex(value, regex))
  {
    return false;
  }
  ReplaceAsSpace(value, "\\.");
  return true;
}

// regex validation & convert decimal to rational. For example GPSLatitude 2.5,23,3.4 -> 2.5 23 3.4 -> 5/2 23/1 17/5
bool ExifMetaDataValidate::ValidRegxWithCommaDecimalRationalFix(std::string &value, const std::string &regex){
  if (!ValidRegex(value, regex))
  {
    return false;
  }

  // replace comma with a space 1.5,2.5.3 -> 1.5 2.5 3
  ReplaceAsSpace(value, "\\,");
  // convert decimal to rationl 2.5 -> 5/2
  DecimalRationalFix(value);
  return true;
}

// regex validation & convert decimal to rational. For example GPSLatitude 2.5 23 3.4 -> 5/2 23/1 17/5
bool ExifMetaDataValidate::ValidRegexWithDecimalRationalFix(std::string &value, const std::string &regex)
{
  if (!ValidRegex(value, regex))
  {
    return false;
  }
  // convert decimal to rationl 2.5 -> 5/2
  DecimalRationalFix(value);
  return true;
}


/* regex validation for two integer like DefaultCropSize 9 9 the format is [0-9]+ [0-9]+
   For example for DefaultCropSize, Raw images often store extra pixels around the edges of the final image. 
   These extra pixels help prevent interpolation artifacts near the edges of the final image. 
   DefaultCropSize specifies the size of the final image area, in raw image coordinates (i.e., before the DefaultScale has been applied).
   */
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int2Blank = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]+\s{0,}[0-9]+$)");
// regex validation for two integer with comma like BitPerSample 9,9 the format is [0-9]+,[0-9]+,[0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int2Comma = std::make_pair(ExifMetaDataValidate::ValidRegexWithComma, R"(^[0-9]+,[0-9]+$)");
// regex validation for three integer like BitPerSample 9 9 9 the format is [0-9]+ [0-9]+ [0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int3Blank = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]+\s{0,}[0-9]+\s{0,}[0-9]+$)");
// regex validation for three integer with comma like BitPerSample 9,9,0 the format is [0-9]+,[0-9]+,[0-9]+,[0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int3Comma = std::make_pair(ExifMetaDataValidate::ValidRegexWithComma, R"(^[0-9]+,[0-9]+,[0-9]+$)");
// regex validation for four integer like DNGVersion 9 9 9 9 the format is [0-9]+ [0-9]+ [0-9]+ [0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int4Blank = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]+\s{0,}[0-9]+\s{0,}[0-9]+\s{0,}[0-9]+$)");
/* regex validation for four integer with comma like DNGVersion 9,9,9,9 the format is [0-9]+,[0-9]+,[0-9]+
   For example, DNGVersion tag encodes the DNG four-tier version number.
   For files compliant with version 1.1.0.0 of the DNG specification, this tag should contain the bytes: 1, 1, 0, 0.
   */
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int4Comma = std::make_pair(ExifMetaDataValidate::ValidRegexWithComma, R"(^[0-9]+,[0-9]+,[0-9]+,[0-9]+$)");
// regex validation for one rational like ApertureValue 4/1 the format is [0-9]+/[1-9][0-9]
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::rational1 = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]+/[1-9][0-9]*$)");
// regex validation for integer and convert it to rational like ApertureValue 4 --> 4/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int1Rational1 = std::make_pair(ExifMetaDataValidate::ValidRegexWithRationalFix, R"(^[0-9]+$)");
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::decimal1Rational1 = std::make_pair(ExifMetaDataValidate::ValidRegexWithDecimalRationalFix, "(\\d+)(\\.\\d+)?");
// regex validation for three rational like GPSLatitude 39/1 54/1 20/1 the format is [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9]
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::rational3Blank = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]+/[1-9][0-9]*\s{0,}[0-9]+/[1-9][0-9]*\s{0,}[0-9]+/[1-9][0-9]*$)");
// regex validation for three integer and convert to three rational like GPSLatitude 39 54 20 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int3Rational3Blank = std::make_pair(ExifMetaDataValidate::ValidRegexWithRationalFix, R"(^[1-9][0-9]*\s{0,}[1-9][0-9]*\s{0,}[1-9][0-9]*$)");
// regex validation for three integer with comma and convert to three rational like GPSLatitude 39,54,20 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int3Ratiional3Comma = std::make_pair(ExifMetaDataValidate::ValidRegexWithCommaRationalFix, R"(^[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*$)");
// regex validation for three decimal or integer and convert to three rational like YCbCrCoefficients 39.0 54 20.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::decimal3Ratiional3 = std::make_pair(ExifMetaDataValidate::ValidRegexWithDecimalRationalFix, "(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?");
// regex validation for three decimal or integer and convert to three rational like YCbCrCoefficients 39.0,54,20.0 --> 39.0 54 20.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::decimal3Ratiional3Comma = std::make_pair(ExifMetaDataValidate::ValidRegxWithCommaDecimalRationalFix, "(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?");
// regex validation for four rational like LensSpecification 1/1 3/2 1/1 2/1 the format is [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9] [0-9]+/[1-9][0-9]
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::rational4Blank = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]+/[1-9][0-9]*\s{0,}[0-9]+/[1-9][0-9]*\s{0,}[0-9]+/[1-9][0-9]*\s{0,}[0-9]+/[1-9][0-9]*$)");
// regex validation for four integer and convert to four rational like LensSpecification 1 3 1 2 --> 1/1 3/2 1/1 2/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int4Rational4Blank = std::make_pair(ExifMetaDataValidate::ValidRegexWithRationalFix, R"(^[1-9][0-9]*\s{0,}[1-9][0-9]*\s{0,}[1-9][0-9]*\s{0,}[1-9][0-9]*$)");
// regex validation for four integer with comma and convert to four rational like LensSpecification 1,3,1,2 --> 1/1 3/2 1/1 2/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int4Ratiional4Comma = std::make_pair(ExifMetaDataValidate::ValidRegexWithCommaRationalFix, R"(^[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*,[1-9][0-9]*$)");
// regex validation for four decimal or integer and convert to four rational like LensSpecification 1.0 3.0 1.0 2.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::decimal4Ratiional4 = std::make_pair(ExifMetaDataValidate::ValidRegexWithDecimalRationalFix, "(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?\\s(\\d+)(\\.\\d+)?");
// regex validation for four decimal or integer and convert to four rational like LensSpecification 1.0,3.0,1.0,2.0 --> 39/1 54/1 20/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::decimal4Ratiional4Comma = std::make_pair(ExifMetaDataValidate::ValidRegxWithCommaDecimalRationalFix, "(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?,(\\d+)(\\.\\d+)?");
// regex validation for datetime format like DateTimeOriginal 2022:06:02 15:51:34
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::datetimeRegex = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]{4}:[0-9]{2}:[0-9]{2}\s[0-9]{2}:[0-9]{2}:[0-9]{2}$)");
// regex validation for datetime format like DateTimeOriginal 2022:06:02
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::dateRegex = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]{4}:[0-9]{2}:[0-9]{2}$)");
//regex validation for three integer with colon and convert to three rational like GPSLatitude 39,54,21 --> 39/1 54/1 21/1
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int3Rational3Colon = std::make_pair(ExifMetaDataValidate::ValidRegexWithColonRationalFix, R"(^[1-9][0-9]*:[1-9][0-9]*:[1-9][0-9]*$)");
// regex validation for fou integer like GPSVersionID 2 2 0 0 the format is [0-9]+ [0-9]+ [0-9]+ [0-9]+
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int4Rational4 = std::make_pair(ExifMetaDataValidate::ValidRegex, R"(^[0-9]+\s[0-9]+\s[0-9]+\s[0-9]+$)");
// regex validation for fou integer with pointer like GPSVersionID 2.2.0.0 -> 2 2 0 0 
std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string> ExifMetaDataValidate::int4Rational4Pointer = std::make_pair(ExifMetaDataValidate::ValidRegexWithDot, R"(^[0-9]+.[0-9]+.[0-9]+.[0-9]+$)");

// configuration for value format validation. For example BitPerSample the value format should be 9 9 9 or 9,9,9
std::multimap<std::string, std::pair<std::function<int32_t (std::string&, const std::string&)>, std::string>> ExifMetaDataValidate::valueFormatValidateConfig = {
  {"Exif.Image.BitsPerSample",  int3Blank},
  {"Exif.Image.BitsPerSample",  int3Comma},
  {"Exif.Photo.CompressedBitsPerPixel", rational1},
  {"Exif.Photo.CompressedBitsPerPixel", int1Rational1},
  {"Exif.Photo.CompressedBitsPerPixel", decimal1Rational1},
  {"Exif.GPSInfo.GPSLatitude", rational3Blank},
  {"Exif.GPSInfo.GPSLatitude", int3Rational3Blank},
  {"Exif.GPSInfo.GPSLatitude", int3Ratiional3Comma},
  {"Exif.GPSInfo.GPSLongitude", rational3Blank},
  {"Exif.GPSInfo.GPSLongitude", int3Rational3Blank},
  {"Exif.GPSInfo.GPSLongitude", int3Ratiional3Comma},
  {"Exif.Photo.ApertureValue", rational1},
  {"Exif.Photo.ApertureValue", int1Rational1},
  {"Exif.Photo.ApertureValue", decimal1Rational1},
  {"Exif.Photo.DateTimeOriginal", datetimeRegex},
  {"Exif.Photo.DateTimeOriginal", dateRegex},
  {"Exif.Image.DateTime", datetimeRegex},
  {"Exif.Image.DateTime", dateRegex},
  {"Exif.Photo.ExposureBiasValue", rational1},
  {"Exif.Photo.ExposureBiasValue", int1Rational1},
  {"Exif.Photo.ExposureBiasValue", decimal1Rational1},
  {"Exif.Photo.ExposureTime", rational1},
  {"Exif.Photo.ExposureTime", int1Rational1},
  {"Exif.Photo.ExposureTime", decimal1Rational1},
  {"Exif.Photo.FNumber", rational1},
  {"Exif.Photo.FNumber", int1Rational1},
  {"Exif.Photo.FNumber", decimal1Rational1},
  {"Exif.Photo.FocalLength", rational1},
  {"Exif.Photo.FocalLength", int1Rational1},
  {"Exif.Photo.FocalLength", decimal1Rational1},
  {"Exif.GPSInfo.GPSTimeStamp", rational3Blank},
  {"Exif.GPSInfo.GPSTimeStamp", int3Rational3Blank},
  {"Exif.GPSInfo.GPSTimeStamp", int3Rational3Colon},
  {"Exif.GPSInfo.GPSDateStamp", dateRegex},
  {"Exif.Image.XResolution", rational1},
  {"Exif.Image.XResolution", int1Rational1},
  {"Exif.Image.XResolution", decimal1Rational1},
  {"Exif.Image.YResolution", rational1},
  {"Exif.Image.YResolution", int1Rational1},
  {"Exif.Image.YResolution", decimal1Rational1},
  {"Exif.Image.WhitePoint", rational1},
  {"Exif.Image.WhitePoint", int1Rational1},
  {"Exif.Image.WhitePoint", decimal1Rational1},
  {"Exif.Image.PrimaryChromaticities", rational1},
  {"Exif.Image.PrimaryChromaticities", int1Rational1},
  {"Exif.Image.PrimaryChromaticities", decimal1Rational1},
  {"Exif.Image.YCbCrCoefficients", rational3Blank},
  {"Exif.Image.YCbCrCoefficients", int3Rational3Blank},
  {"Exif.Image.YCbCrCoefficients", int3Ratiional3Comma},
  {"Exif.Image.YCbCrCoefficients", decimal3Ratiional3},
  {"Exif.Image.YCbCrCoefficients", decimal3Ratiional3Comma},
  {"Exif.Image.ReferenceBlackWhite", rational1},
  {"Exif.Image.ReferenceBlackWhite", int1Rational1},
  {"Exif.Image.ReferenceBlackWhite", decimal1Rational1},
  {"Exif.Photo.ShutterSpeedValue", rational1},
  {"Exif.Photo.ShutterSpeedValue", int1Rational1},
  {"Exif.Photo.ShutterSpeedValue", decimal1Rational1},
  {"Exif.Photo.BrightnessValue", rational1},
  {"Exif.Photo.BrightnessValue", int1Rational1},
  {"Exif.Photo.BrightnessValue", decimal1Rational1},
  {"Exif.Image.MaxApertureValue", rational1},
  {"Exif.Image.MaxApertureValue", int1Rational1},
  {"Exif.Image.MaxApertureValue", decimal1Rational1},
  {"Exif.Image.SubjectDistance", rational1},
  {"Exif.Image.SubjectDistance", int1Rational1},
  {"Exif.Image.SubjectDistance", decimal1Rational1},
  {"Exif.Photo.FlashEnergy", rational1},
  {"Exif.Photo.FlashEnergy", int1Rational1},
  {"Exif.Photo.FlashEnergy", decimal1Rational1},
  {"Exif.Photo.FocalPlaneXResolution", rational1},
  {"Exif.Photo.FocalPlaneXResolution", int1Rational1},
  {"Exif.Photo.FocalPlaneXResolution", decimal1Rational1},
  {"Exif.Photo.FocalPlaneYResolution", rational1},
  {"Exif.Photo.FocalPlaneYResolution", int1Rational1},
  {"Exif.Photo.FocalPlaneYResolution", decimal1Rational1},
  {"Exif.Photo.ExposureIndex", rational1},
  {"Exif.Photo.ExposureIndex", int1Rational1},
  {"Exif.Photo.ExposureIndex", decimal1Rational1},
  {"Exif.Photo.DigitalZoomRatio", rational1},
  {"Exif.Photo.DigitalZoomRatio", int1Rational1},
  {"Exif.Photo.DigitalZoomRatio", decimal1Rational1},
  {"Exif.GPSInfo.GPSAltitude", rational1},
  {"Exif.GPSInfo.GPSAltitude", int1Rational1},
  {"Exif.GPSInfo.GPSAltitude", decimal1Rational1},
  {"Exif.GPSInfo.GPSDOP", rational1},
  {"Exif.GPSInfo.GPSDOP", int1Rational1},
  {"Exif.GPSInfo.GPSDOP", decimal1Rational1},
  {"Exif.GPSInfo.GPSSpeed", rational1},
  {"Exif.GPSInfo.GPSSpeed", int1Rational1},
  {"Exif.GPSInfo.GPSSpeed", decimal1Rational1},
  {"Exif.GPSInfo.GPSTrack", rational1},
  {"Exif.GPSInfo.GPSTrack", int1Rational1},
  {"Exif.GPSInfo.GPSTrack", decimal1Rational1},
  {"Exif.GPSInfo.GPSImgDirection", rational1},
  {"Exif.GPSInfo.GPSImgDirection", int1Rational1},
  {"Exif.GPSInfo.GPSImgDirection", decimal1Rational1},
  {"Exif.GPSInfo.GPSDestLatitude", rational3Blank},
  {"Exif.GPSInfo.GPSDestLatitude", int3Rational3Blank},
  {"Exif.GPSInfo.GPSDestLatitude", int3Ratiional3Comma},
  {"Exif.GPSInfo.GPSDestLongitude", rational3Blank},
  {"Exif.GPSInfo.GPSDestLongitude", int3Rational3Blank},
  {"Exif.GPSInfo.GPSDestLongitude", int3Ratiional3Comma},
  {"Exif.GPSInfo.GPSDestBearing", rational1},
  {"Exif.GPSInfo.GPSDestBearing", int1Rational1},
  {"Exif.GPSInfo.GPSDestBearing", decimal1Rational1},
  {"Exif.GPSInfo.GPSDestDistance", rational1},
  {"Exif.GPSInfo.GPSDestDistance", int1Rational1},
  {"Exif.GPSInfo.GPSDestDistance", decimal1Rational1},
  {"Exif.GPSInfo.GPSVersionID", int4Rational4},
  {"Exif.GPSInfo.GPSVersionID", int4Rational4Pointer},
  {"Exif.Image.CompressedBitsPerPixel", rational1},
  {"Exif.Image.CompressedBitsPerPixel", int1Rational1},
  {"Exif.Image.CompressedBitsPerPixel", decimal1Rational1},
  {"Exif.Image.DNGVersion", int4Blank},
  {"Exif.Image.DNGVersion", int4Comma},
  {"Exif.Image.DefaultCropSize", int2Blank},
  {"Exif.Image.DefaultCropSize", int2Comma},
  {"Exif.Photo.Gamma", rational1},
  {"Exif.Photo.Gamma", int1Rational1},
  {"Exif.Photo.Gamma", decimal1Rational1},
  {"Exif.GPSInfo.GPSHPositioningError", rational1},
  {"Exif.GPSInfo.GPSHPositioningError", int1Rational1},
  {"Exif.GPSInfo.GPSHPositioningError", decimal1Rational1},
  {"Exif.Photo.LensSpecification", rational4Blank},
  {"Exif.Photo.LensSpecification", int4Rational4Blank},
  {"Exif.Photo.LensSpecification", int4Ratiional4Comma},
  {"Exif.Photo.LensSpecification", decimal4Ratiional4},
  {"Exif.Photo.LensSpecification", decimal4Ratiional4Comma},
};


// validate the value range. For example GPSLatitudeRef the value must be 'N' or 'S'.
int32_t ExifMetaDataValidate::IsValueRangeValidate(const std::string &exiv2Tag, const std::string &value){
    // 1. to find if any value range validation configuratiion according to exif tag in std::map container
    auto iter  = valueRangeValidateConfig.find(exiv2Tag);
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
bool ExifMetaDataValidate::HasValueFormatValidate(const std::string &exiv2Tag){
  IMAGE_LOGD("hasValueFormatValidate exiv2Tag is [%{public}s].", exiv2Tag.c_str());
  return ExifMetaDataValidate::valueFormatValidateConfig.find(exiv2Tag) != ExifMetaDataValidate::valueFormatValidateConfig.end();
}

// validate value format. For example BitPerSample the value format should be 9 9 9 or 9,9,9
int32_t ExifMetaDataValidate::IsValueFormatValidate(const std::string &exiv2Tag, std::string &value)
{
  IMAGE_LOGD("isValueFormatValidate exiv2Tag is [%{public}s] value is [%{public}s].", exiv2Tag.c_str(), value.c_str());
  // get first iterator according to exiv2Tag
  for (auto iterator = ExifMetaDataValidate::valueFormatValidateConfig.find(exiv2Tag); iterator != ExifMetaDataValidate::valueFormatValidateConfig.end() && iterator != ExifMetaDataValidate::valueFormatValidateConfig.upper_bound(exiv2Tag); iterator++)
  {
    IMAGE_LOGD("isValueFormatValidate forloop exiv2Tag is [%{public}s] regex string is [%{public}s].", (iterator->first).c_str(), (iterator->second).second.c_str());
    auto func = (iterator->second).first;
    // call each value format function with value and regex
    int32_t i = func(value, (iterator->second).second);
    IMAGE_LOGD("isValueFormatValidate ret i is [%{public}d].", i);
    // 修正成功 返回1
    if (i)
    {
      IMAGE_LOGD("isValueFormatValidate ret SUCCESS.");
      return Media::SUCCESS; // SUCCESS
    }
  }

  IMAGE_LOGD("isValueFormatValidate ret ERR_IMAGE_DECODE_EXIF_UNSUPPORT.");
  return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; // FAILED
}

// disable modify huawei exif tag except for Exif.Huawei.CaptureMode
bool ExifMetaDataValidate::IsModifyAllow(const std::string &exiv2Tag){
  if (ExifMetaDataValidate::ValidRegex(exiv2Tag, R"(^Exif.Huawei.*)") && !ExifMetaDataValidate::ValidRegex(exiv2Tag, R"(^Exif.Huawei.CaptureMode$)")) {
    return false;
  }
  return true;
}

// exif validation portal when modify exif
int32_t ExifMetaDataValidate::ExifValidate(const std::string &keyName, std::string &exiv2Tag, std::string &value)
{
  IMAGE_LOGD("[ExifValidation] in exifValidate.");
  // translate exif tag. For example translate "BitsPerSample" to "Exif.Image.BitsPerSample"
  if(!ExifMetaDataValidate::GetExiv2TagByName(keyName, exiv2Tag))
  {
      return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
  }

  // disable modify huawei exif tag except for Exif.Huawei.CaptureMode
  if (!ExifMetaDataValidate::IsModifyAllow(exiv2Tag)) {
      return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT;
  }

  IMAGE_LOGD("[ExifValidation] exiv2Tag is [%{public}s] value is [%{public}s].", exiv2Tag.c_str(), value.c_str());
  // 1.validate value format
  IMAGE_LOGD("[ExifValidation] hasValueFormatValidate is [%{public}d].", ExifMetaDataValidate::HasValueFormatValidate(exiv2Tag));
  auto r = ExifMetaDataValidate::IsValueFormatValidate(exiv2Tag, value);
  IMAGE_LOGD("[ExifValidation] isValueFormatValidate is [%{public}d].", r);
  if (ExifMetaDataValidate::HasValueFormatValidate(exiv2Tag) && r)
  {
    // printf("1.验证数据格式是否合法 未通过 跳出modify\n");
    IMAGE_LOGD("[ExifValidation] value formate is invalid. exiv2Tag is [%{public}s] value is [%{public}s].", exiv2Tag.c_str(), value.c_str());
    return Media::ERR_IMAGE_DECODE_EXIF_UNSUPPORT; //value format validate does not pass
  }
  IMAGE_LOGD("[ExifValidation] processed formate value is [%{public}s] value is [%{public}s].", exiv2Tag.c_str(), value.c_str());
  // 2.validat value range
  if (ExifMetaDataValidate::IsValueRangeValidate(exiv2Tag, value))
  {
    IMAGE_LOGD("[ExifValidation] value range is invalid. value is [%{public}s] value is [%{public}s].", exiv2Tag.c_str(), value.c_str());
    return Media::ERR_MEDIA_OUT_OF_RANGE; // value range validate does not pass
  }
  return Media::SUCCESS;
}


// translate exif tag. For example translate "BitsPerSample" to "Exif.Image.BitsPerSample"
bool ExifMetaDataValidate::GetExiv2TagByName(const std::string &keyName, std::string &exiv2Tag)
{
    auto find_item = std::find_if(TAG_MAP.begin(), TAG_MAP.end(),
        [keyName](const std::map<std::string, std::string>::value_type item) {
        return IsSameTextStr(item.first, keyName);
    });
    if (find_item == TAG_MAP.end()) {
        return false;
    }
    exiv2Tag = find_item->second;
    return true;
}

} // namespace Media
} // namespace OHOS
