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
#ifndef __EXIF_H
#define __EXIF_H

#include <string>
#include "hilog/log.h"
#include "log_tags.h"

using std::string;
// IF Entry 
struct IFEntry {
    // Raw fields
    unsigned short tag;
    unsigned short format;
    unsigned data;
    unsigned length;
    
    // Parsed fields
    string val_string;
    unsigned short val_short;
    unsigned val_long;
    double val_rational;
     unsigned char val_byte;
}; 
// 
// Class responsible for storing and parsing EXIF information from a JPEG blob
//
class EXIFInfo {
public:
    // Parsing function for an entire JPEG image buffer.
    //
    // PARAM 'data': A pointer to a JPEG image.
    // PARAM 'length': The length of the JPEG image.
    // RETURN:  PARSE_EXIF_SUCCESS (0) on succes with 'result' filled out
    //          error code otherwise, as defined by the PARSE_EXIF_ERROR_* macros
    int ParseFrom(const unsigned char *data, unsigned length);
    int ParseFrom(const std::string &data);
    void setLatitude(const unsigned char *buf, bool alignIntel, unsigned tiff_header_start, 
        unsigned data);
    void setLongitude(const unsigned char *buf, bool alignIntel, unsigned tiff_header_start, 
        unsigned data);
    void setIFEntryValue(IFEntry result);
    void setSubIFEntryValue(IFEntry result);
    void setSubIFEntryValueExtend(IFEntry result);
    void setGpsEntryValue(unsigned short tag, unsigned short format, unsigned length, unsigned data,
        const unsigned char *buf, bool alignIntel, unsigned tiff_header_start, unsigned entry_offs);
    int ParseFromEXIFSegment(const unsigned char *buf, unsigned len);
    int ParseExifIFD(const unsigned char *buf, unsigned len, bool alignIntel, 
        unsigned tiff_header_start, unsigned exif_ifd_offset, unsigned &exif_sub_ifd_offset, 
        unsigned &gps_sub_ifd_offset);
    int ParseGpsSubIFD(const unsigned char *buf, unsigned len, bool alignIntel, 
        unsigned tiff_header_start, unsigned gps_sub_ifd_offset);
    int ParseEXIFSubIFD(const unsigned char *buf, unsigned len, bool alignIntel, 
        unsigned tiff_header_start, unsigned exif_sub_ifd_offset);
    // Set all data members to default values.
    void Clear();

    // Data fields filled out by parseFrom()
    char ByteAlign;                   // 0 = Motorola byte alignment, 1 = Intel 
    std::string ImageDescription;     // Image description
    std::string Make;                 // Camera manufacturer's name
    std::string Model;                // Camera model
    unsigned short Orientation;       // Image orientation, start of data corresponds to
                                      // 0: unspecified in EXIF data
                                      // 1: upper left of image
                                      // 3: lower right of image
                                      // 6: upper right of image
                                      // 8: lower left of image
                                      // 9: undefined
    unsigned short BitsPerSample;     // Number of bits per component
    std::string Software;             // Software used
    std::string DateTime;             // File change date and time
    std::string DateTimeOriginal;     // Original file date and time (may not exist)
    std::string DateTimeDigitized;    // Digitization date and time (may not exist)
    std::string SubSecTimeOriginal;   // Sub-second time that original picture was taken
    std::string Copyright;            // File copyright information
    double ExposureTime;              // Exposure time in seconds
    double FNumber;                   // F/stop
    unsigned short ISOSpeedRatings;   // ISO speed
    double ShutterSpeedValue;         // Shutter speed (reciprocal of exposure time)
    double ExposureBiasValue;         // Exposure bias value in EV
    double SubjectDistance;           // Distance to focus point in meters
    double FocalLength;               // Focal length of lens in millimeters
    unsigned short FocalLengthIn35mm; // Focal length in 35mm film
    char Flash;                       // 0 = no flash, 1 = flash used
    unsigned short MeteringMode;      // Metering mode
                                      // 1: average
                                      // 2: center weighted average
                                      // 3: spot
                                      // 4: multi-spot
                                      // 5: multi-segment
    unsigned ImageWidth;              // Image width reported in EXIF data
    unsigned ImageHeight;             // Image height reported in EXIF data
    struct Geolocation_t {            // GPS information embedded in file
        double Latitude;              // Image latitude expressed as decimal
        double Longitude;             // Image longitude expressed as decimal
        double Altitude;              // Altitude in meters, relative to sea level
        char AltitudeRef;             // 0 = above sea level, -1 = below sea level
        struct Coord_t {
            double degrees;               
            double minutes;
            double seconds;
            char direction;
        } LatComponents, LonComponents;   // Latitude, Longitude expressed in deg/min/sec 
    } GeoLocation;
    EXIFInfo() {
        Clear();
    }
};

#define PARSE_EXIF_SUCCESS                    0
#define PARSE_EXIF_ERROR_NO_JPEG              10001
#define PARSE_EXIF_ERROR_NO_EXIF              10002
#define PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN    10003
#define PARSE_EXIF_ERROR_CORRUPT              10004

#define BUFFER_OFFSET_0                       0
#define BUFFER_OFFSET_1                       1
#define BUFFER_OFFSET_2                       2
#define BUFFER_OFFSET_3                       3
#define BUFFER_OFFSET_4                       4
#define BUFFER_OFFSET_8                       8
#define BUFFER_OFFSET_16                      16
#define BUFFER_OFFSET_24                      24

#endif