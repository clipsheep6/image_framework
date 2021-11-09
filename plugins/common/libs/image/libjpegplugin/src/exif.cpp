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

#include <stdio.h>
#include <algorithm>
#include "exif.h"

using std::string;

namespace {
    using namespace OHOS::HiviewDFX;
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, LOG_TAG_DOMAIN_ID_IMAGE, "Exif" };

    //constexpr uint32_t EXIF_MARKER_SIZE = 2;
    //constexpr uint32_t EXIF_MARKER_LENGTH = 2;
    //constexpr uint8_t EXIF_MARKER_LENGTH_0_OFFSET = 0;
    //constexpr uint8_t EXIF_MARKER_LENGTH_1_OFFSET = 1;
    //constexpr uint32_t EXIF_MARKER_LENGTH_SHIFT = 8;
    constexpr uint8_t EXIF_MARKER_PREFIX_OFFSET = 0;
    constexpr uint8_t EXIF_MARKER_CODE_OFFSET = 1;
    constexpr uint8_t EXIF_MARKER_PREFIX = 0XFF;
    constexpr uint8_t EXIF_MARKER_SOI = 0XD8;
    constexpr uint8_t EXIF_MARKER_HEADER = 0XE1;
    //constexpr uint8_t EXIF_MARKER_SOS = 0XDA;
    //constexpr uint8_t EXIF_MARKER_RST = 0XD0;
    //constexpr uint8_t EXIF_MARKER_RST0 = 0XD0;
    //constexpr uint8_t EXIF_MARKER_RSTN = 0XD7;
    //constexpr uint8_t EXIF_MARKER_APP = 0XE0;
    //constexpr uint8_t EXIF_MARKER_APP0 = 0XE0;
    //constexpr uint8_t EXIF_MARKER_APPN = 0XEF;

    constexpr uint16_t TAG_BITS_PER_SAMPLE = 0x102;         // Bits per sample
    constexpr uint16_t TAG_IMAGE_DESCRIPTION = 0x10E;       // Image description
    constexpr uint16_t TAG_DIGICAM_MAKE = 0x10F;            // Digicam make
    constexpr uint16_t TAG_DIGICAM_MODEL = 0x110;           // Digicam model
    constexpr uint16_t TAG_ORIENTATION_OF_IMAGE = 0x112;    // Orientation of image
    constexpr uint16_t TAG_SOFTWARE = 0x131;                // Software used for image
    constexpr uint16_t TAG_DATE_TIME = 0x132;               // EXIF/TIFF date/time of image modification
    constexpr uint16_t TAG_COPYRIGHT_INFO = 0x8298;         // Copyright information
    constexpr uint16_t TAG_GPS_IFS_OFFSET = 0x8825;         // GPS IFS offset 
    constexpr uint16_t TAG_EXIF_SUB_IFD_OFFSET = 0x8769;    // EXIF SubIFD offset


    constexpr uint16_t TAG_EXPOSURE_TIME = 0x829a;              // Exposure time in seconds
    constexpr uint16_t TAG_FNUMBER = 0x829d;                    // FNumber
    constexpr uint16_t TAG_ISO_SPEED_RATING = 0x8827;           // ISO Speed Rating
    constexpr uint16_t TAG_ORIGINAL_DATE_TIME = 0x9003;         // Original date and time
    constexpr uint16_t TAG_DIGITIZATION_DATE_TIME = 0x9004;     // Digitization date and time
    constexpr uint16_t TAG_SHUTTER_SPEED_VALUE = 0x9201;        // Shutter speed value
    constexpr uint16_t TAG_EXPOSURE_BIAS_VALUE = 0x9204;        // Exposure bias value
    constexpr uint16_t TAG_SUBJECT_DISTANCE = 0x9206;           // Subject distance
    constexpr uint16_t TAG_FLASH = 0x9209;                      // Flash used
    constexpr uint16_t TAG_FOCAL_LENGTH = 0x920a;               // Focal length
    constexpr uint16_t TAG_METERING_MODE = 0x9207;              // Metering mode
    constexpr uint16_t TAG_SUBSECOND_ORIGINAL_TIME = 0x9291;    // Subsecond original time
    constexpr uint16_t TAG_EXIF_IMAGE_WIDTH = 0xa002;           // EXIF Image width
    constexpr uint16_t TAG_EXIF_IMAGE_HEIGHT = 0xa003;          // EXIF Image height
    constexpr uint16_t TAG_FOCAL_LENGTH_IN_35MM = 0xa405;       // Focal length in 35mm film

    constexpr uint8_t T_GPS_NOS = 1;         // GPS north or south
    constexpr uint8_t T_GPS_LAT = 2;         // GPS latitude
    constexpr uint8_t T_GPS_EOW = 3;         // GPS east or west
    constexpr uint8_t T_GPS_LON = 4;         // GPS longitude
    constexpr uint8_t T_GPS_ALT_REF = 5;     // GPS altitude reference (below or above sea level)
    constexpr uint8_t T_GPS_ALT = 6;         // GPS altitude reference

    constexpr uint8_t FORMAT_UNSIGNED_BYTE = 1;
    constexpr uint8_t FORMAT_ASCII_STRING = 2;  //ascii strings
    constexpr uint8_t FORMAT_UNSIGNED_SHORT = 3;  //unsigned short
    constexpr uint8_t FORMAT_UNSIGNED_LONG = 4;  //unsigned long
    constexpr uint8_t FORMAT_UNSIGNED_RATIONAL = 5;  //unsigned rational
    //constexpr uint8_t FORMAT_SIGNED_BYTE = 6;  //signed byte
    constexpr uint8_t FORMAT_UNDEFINED = 7;  //undefined
    //constexpr uint8_t FORMAT_SIGNED_SHORT = 8;  //signed short
    constexpr uint8_t FORMAT_SIGNED_LONG = 9;  //signed long
    constexpr uint8_t FORMAT_SIGNED_RATIONAL = 10;  //signed rational
    //constexpr uint8_t FORMAT_SINGLE_FLOAT = 11;  //single float
    //constexpr uint8_t FORMAT_DOUBLE_FLOAT = 12;  //double float

    constexpr uint8_t FORMAT_DEFAULT_VALUE  = 0xFF;
    // 2 bytes: tag number (data field)
    // 2 bytes: data format
    // 4 bytes: number of components
    // 4 bytes: data value or offset to data value
    constexpr uint8_t OFFSET_1  = 1;
    constexpr uint8_t OFFSET_2  = 2;
    constexpr uint8_t OFFSET_4  = 4;
    constexpr uint8_t OFFSET_6  = 6;
    constexpr uint8_t OFFSET_8  = 8;
    //constexpr uint8_t OFFSET_10  = 10;
    constexpr uint8_t OFFSET_12  = 12;
    constexpr uint8_t OFFSET_16  = 16;

    constexpr char WEST = 'W';
    constexpr char SOUTH = 'S';
    constexpr char CHAR_I = 'I';
    constexpr char CHAR_M = 'M';

    // Helper functions
    unsigned int parseLong(const unsigned char *buf, bool intel) {
        if (intel) 
            return ((unsigned)buf[BUFFER_OFFSET_3] << BUFFER_OFFSET_24) | 
                   ((unsigned)buf[BUFFER_OFFSET_2] << BUFFER_OFFSET_16) | 
                   ((unsigned)buf[BUFFER_OFFSET_1] << BUFFER_OFFSET_8)  | 
                   buf[BUFFER_OFFSET_0];

        return ((unsigned)buf[BUFFER_OFFSET_0] << BUFFER_OFFSET_24) | 
               ((unsigned)buf[BUFFER_OFFSET_1] << BUFFER_OFFSET_16) | 
               ((unsigned)buf[BUFFER_OFFSET_2] << BUFFER_OFFSET_8)  | 
               buf[BUFFER_OFFSET_3];
    }

    unsigned short parseShort(const unsigned char *buf, bool intel) {
        if (intel)
            return ((unsigned) buf[BUFFER_OFFSET_1]<< BUFFER_OFFSET_8) | buf[BUFFER_OFFSET_0];
        return ((unsigned) buf[BUFFER_OFFSET_0]<< BUFFER_OFFSET_8) | buf[BUFFER_OFFSET_1]; 
    }

    string parseEXIFString(const unsigned char *buf, 
                           const unsigned num_components, 
                           const unsigned data, 
                           const unsigned base, 
                           const unsigned len) {
        string value;
        if (num_components <= 4)
            value.assign( (const char*)&data, num_components );
        else {
            if (base+data+num_components <= len)
                value.assign( (const char*)(buf+base+data), num_components );
        }
        return value;
    }

    double parseEXIFRational(const unsigned char *buf, bool intel) {
        double numerator   = 0;
        double denominator = 1;

        numerator  = (double) parseLong(buf, intel);
        denominator= (double) parseLong(buf+4, intel);
        if(denominator < 1e-20)
            return 0;
        return numerator/denominator;
    }

    IFEntry parseIFEntry(const unsigned char *buf, 
                         const unsigned offs, 
                         const bool alignIntel, 
                         const unsigned base, 
                         const unsigned len) {
        IFEntry result;

        // Each directory entry is composed of:
        // 2 bytes: tag number (data field)
        // 2 bytes: data format
        // 4 bytes: number of components
        // 4 bytes: data value or offset to data value
        result.tag        = parseShort(buf + offs, alignIntel);
        result.format     = parseShort(buf + offs + OFFSET_2, alignIntel);
        result.length     = parseLong(buf + offs + OFFSET_4, alignIntel);
        result.data       = parseLong(buf + offs + OFFSET_8, alignIntel);

        // Parse value in specified format
        switch (result.format) {
            case FORMAT_UNSIGNED_BYTE:
                result.val_byte = (unsigned char) *(buf + offs + OFFSET_8);
                break;
            case FORMAT_ASCII_STRING:
                result.val_string = parseEXIFString(buf, result.length, result.data, base, len);
                break;
            case FORMAT_UNSIGNED_SHORT:
                result.val_short = parseShort((const unsigned char *) buf + offs + OFFSET_8, alignIntel);
                break;
            case FORMAT_UNSIGNED_LONG:
                result.val_long = result.data;
                break;
            case FORMAT_UNSIGNED_RATIONAL:
                if (base + result.data + OFFSET_8 <= len)
                    result.val_rational = parseEXIFRational(buf + base + result.data, alignIntel);
                break;
            case FORMAT_UNDEFINED:
            case FORMAT_SIGNED_LONG:
            case FORMAT_SIGNED_RATIONAL:
                break;
            default:
                result.tag = FORMAT_DEFAULT_VALUE;
        }
        return result;
    }
}

//
// Locates the EXIF segment and parses it using parseFromEXIFSegment 
//
int EXIFInfo::ParseFrom(const unsigned char *buf, unsigned len) {
    HiLog::Debug(LABEL, "ParseFrom ENTER");
    if (!buf || len < 4)
        return PARSE_EXIF_ERROR_NO_EXIF;
    if (buf[EXIF_MARKER_PREFIX_OFFSET] != EXIF_MARKER_PREFIX || buf[EXIF_MARKER_CODE_OFFSET] != EXIF_MARKER_SOI)
        return PARSE_EXIF_ERROR_NO_JPEG;

    Clear();
    HiLog::Debug(LABEL, "parseFrom clear");

     unsigned offs = 0;        // current offset into buffer
    for (offs = 0; offs < len-1; offs++) 
      if (buf[offs] == EXIF_MARKER_PREFIX && buf[offs+1] == EXIF_MARKER_HEADER) 
          break;
    if (offs + OFFSET_4 > len)
        return PARSE_EXIF_ERROR_NO_EXIF;
    HiLog::Debug(LABEL, "parseFrom clear1");
    offs += OFFSET_2;
    unsigned short section_length = parseShort(buf + offs, false); 
    if (offs + section_length > len || section_length < 16)
        return PARSE_EXIF_ERROR_CORRUPT;
    offs += OFFSET_2;
    HiLog::Debug(LABEL, "parseFrom exit");
    return ParseFromEXIFSegment(buf + offs, len - offs);
}

int EXIFInfo::ParseFrom(const string &data) {
    return ParseFrom((const unsigned char *)data.data(), data.length());
}

//
// Main parsing function for an EXIF segment.
//
// PARAM: 'buf' start of the EXIF TIFF, which must be the bytes "Exif\0\0".
// PARAM: 'len' length of buffer
//
int EXIFInfo::ParseFromEXIFSegment(const unsigned char *buf, unsigned len) {
    HiLog::Debug(LABEL, "parseFromEXIFSegment enter");
    bool alignIntel = true;     // byte alignment (defined in EXIF header)
    unsigned offs = 0;        // current offset into buffer
    if (!buf || len < OFFSET_6)
        return PARSE_EXIF_ERROR_NO_EXIF;

    if (!std::equal(buf, buf + OFFSET_6, "Exif\0\0"))
        return PARSE_EXIF_ERROR_NO_EXIF;
    offs += OFFSET_6;

    if (offs + OFFSET_8 > len)
      return PARSE_EXIF_ERROR_CORRUPT;
    unsigned tiff_header_start = offs;
    if (buf[offs] == CHAR_I && buf[offs+OFFSET_1] == CHAR_I)
        alignIntel = true;
    else {
        if(buf[offs] == CHAR_M && buf[offs+OFFSET_1] == CHAR_M)
            alignIntel = false;
        else 
            return PARSE_EXIF_ERROR_UNKNOWN_BYTEALIGN;
    }
    this->ByteAlign = alignIntel;
    offs += OFFSET_2;
    if (0x2a != parseShort(buf+offs, alignIntel))
        return PARSE_EXIF_ERROR_CORRUPT;
    offs += OFFSET_2;
    unsigned first_ifd_offset = parseLong(buf + offs, alignIntel);
    offs += first_ifd_offset - OFFSET_4;
    if (offs >= len)
        return PARSE_EXIF_ERROR_CORRUPT;
    unsigned exif_sub_ifd_offset = len;
    unsigned gps_sub_ifd_offset  = len;
    int resIFD = ParseExifIFD(buf, len, alignIntel, tiff_header_start, offs, exif_sub_ifd_offset, gps_sub_ifd_offset);
    if(resIFD != PARSE_EXIF_SUCCESS){
        return resIFD;
    }
    int resSubIFD = ParseEXIFSubIFD(buf, len, alignIntel, tiff_header_start, exif_sub_ifd_offset);
    if(resSubIFD != PARSE_EXIF_SUCCESS){
        return resSubIFD;
    }
    int resGps = ParseGpsSubIFD(buf, len, alignIntel, tiff_header_start, gps_sub_ifd_offset);
    if(resGps != PARSE_EXIF_SUCCESS){
        return resGps;
    }
    return PARSE_EXIF_SUCCESS;
}

int EXIFInfo::ParseExifIFD(const unsigned char *buf, unsigned len, bool alignIntel, 
    unsigned tiff_header_start, unsigned exif_ifd_offset, unsigned &exif_sub_ifd_offset, 
    unsigned &gps_sub_ifd_offset)
{
    HiLog::Debug(LABEL, "ParseExifIFD enter");
    unsigned offs = exif_ifd_offset;// current offset into buffer
    // Now parsing the first Image File Directory (IFD0, for the main image).
    // An IFD consists of a variable number of 12-byte directory entries. The
    // first two bytes of the IFD section contain the number of directory
    // entries in the section. The last 4 bytes of the IFD contain an offset
    // to the next IFD, which means this IFD must contain exactly 6 + 12 * num
    // bytes of data.
    if (offs + OFFSET_2 > len)
        return PARSE_EXIF_ERROR_CORRUPT;
    int num_entries = parseShort(buf + offs, alignIntel);
    if (offs + OFFSET_6 + OFFSET_12 * num_entries > len)
        return PARSE_EXIF_ERROR_CORRUPT;
    offs += OFFSET_2;
    
    while (--num_entries >= 0) {
        IFEntry result = parseIFEntry(buf, offs, alignIntel, tiff_header_start, len);
        offs += OFFSET_12;
        setIFEntryValue(result);
        switch(result.tag) {
            case TAG_GPS_IFS_OFFSET:
                gps_sub_ifd_offset = tiff_header_start + result.data;
                break;
            case TAG_EXIF_SUB_IFD_OFFSET:
                exif_sub_ifd_offset = tiff_header_start + result.data;
                break;
        }
    }
    HiLog::Debug(LABEL, "ParseEXIFSubIFD EXIT");
    return PARSE_EXIF_SUCCESS;
}

void EXIFInfo::setIFEntryValue(IFEntry result) {
    HiLog::Debug(LABEL, "setIFEntryValue enter");
    switch(result.tag) {
        case TAG_BITS_PER_SAMPLE:
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->BitsPerSample = result.val_short;
            break;
        case TAG_IMAGE_DESCRIPTION:
            if (result.format == FORMAT_ASCII_STRING)
                this->ImageDescription = result.val_string;
            break;
        case TAG_DIGICAM_MAKE:
            if (result.format == FORMAT_ASCII_STRING)
                this->Make = result.val_string;
            break;
        case TAG_DIGICAM_MODEL:
            if (result.format == FORMAT_ASCII_STRING)
                this->Model = result.val_string;
            break;
        case TAG_ORIENTATION_OF_IMAGE:
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->Orientation = result.val_short;
            break;
        case TAG_SOFTWARE:
            if (result.format == FORMAT_ASCII_STRING)
                this->Software = result.val_string;
            break;
        case TAG_DATE_TIME:
            if (result.format == FORMAT_ASCII_STRING)
                this->DateTime = result.val_string;
            break;
        case TAG_COPYRIGHT_INFO:
            if (result.format == FORMAT_ASCII_STRING)
                this->Copyright = result.val_string;
            break;
    }
    HiLog::Debug(LABEL, "setIFEntryValue exit");
}

void EXIFInfo::setSubIFEntryValue(IFEntry result) {
    switch(result.tag) {
        case TAG_EXPOSURE_TIME:
            if (result.format == FORMAT_UNSIGNED_RATIONAL)
                this->ExposureTime = result.val_rational;
            break;
        case TAG_FNUMBER:
            if (result.format == FORMAT_UNSIGNED_RATIONAL)
                this->FNumber = result.val_rational;
            break;
        case TAG_ISO_SPEED_RATING:
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->ISOSpeedRatings = result.val_short;
            break;
        case TAG_ORIGINAL_DATE_TIME:
            if (result.format == FORMAT_ASCII_STRING)
                this->DateTimeOriginal = result.val_string;
            break;
        case TAG_DIGITIZATION_DATE_TIME:
            if (result.format == FORMAT_ASCII_STRING)
                this->DateTimeDigitized = result.val_string;
            break;
        case TAG_SHUTTER_SPEED_VALUE:
            if (result.format == FORMAT_UNSIGNED_RATIONAL)
                this->ShutterSpeedValue = result.val_rational;
            break;
        case TAG_EXPOSURE_BIAS_VALUE: 
            if (result.format == FORMAT_UNSIGNED_RATIONAL)
                this->ExposureBiasValue = result.val_rational;
            break;
    }
}

void EXIFInfo::setSubIFEntryValueExtend(IFEntry result) {
    switch(result.tag) {
        case TAG_SUBJECT_DISTANCE:
            if (result.format == FORMAT_UNSIGNED_RATIONAL)
                this->SubjectDistance = result.val_rational;
            break;
        case TAG_FLASH:
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->Flash = result.data ? 1 : 0;
            break;
        case TAG_FOCAL_LENGTH:
            if (result.format == FORMAT_UNSIGNED_RATIONAL)
                this->FocalLength = result.val_rational;
            break;
        case TAG_METERING_MODE:
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->MeteringMode = result.val_short;
            break;
        case TAG_SUBSECOND_ORIGINAL_TIME:
            if (result.format == FORMAT_ASCII_STRING)
                this->SubSecTimeOriginal = result.val_string;
            break;
        case TAG_EXIF_IMAGE_WIDTH:
            if (result.format == FORMAT_UNSIGNED_LONG)
                this->ImageWidth = result.val_long;
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->ImageWidth = result.val_short;
            break;
        case TAG_EXIF_IMAGE_HEIGHT:
            if (result.format == FORMAT_UNSIGNED_LONG)
                this->ImageHeight = result.val_long;
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->ImageHeight = result.val_short;
            break;
        case TAG_FOCAL_LENGTH_IN_35MM:
            if (result.format == FORMAT_UNSIGNED_SHORT)
                this->FocalLengthIn35mm = result.val_short;
            break;
    }
}

int EXIFInfo::ParseEXIFSubIFD(const unsigned char *buf, unsigned len, bool alignIntel, 
    unsigned tiff_header_start, unsigned exif_sub_ifd_offset) 
{
    HiLog::Debug(LABEL, "ParseEXIFSubIFD enter");
    unsigned offs = exif_sub_ifd_offset;// current offset into buffer
    // Jump to the EXIF SubIFD if it exists and parse all the information
    // there. Note that it's possible that the EXIF SubIFD doesn't exist.
    // The EXIF SubIFD contains most of the interesting information that a
    // typical user might want.
    if (exif_sub_ifd_offset + OFFSET_4 <= len) {
        int num_entries = parseShort(buf + offs, alignIntel);
        if (offs + OFFSET_6 + OFFSET_12 * num_entries > len)
            return PARSE_EXIF_ERROR_CORRUPT;
        offs += OFFSET_2;
        while (--num_entries >= 0) {
            IFEntry result = parseIFEntry(buf, offs, alignIntel, tiff_header_start, len);
            setSubIFEntryValue(result);
            setSubIFEntryValueExtend(result);
            offs += OFFSET_12;
        }
    }
    HiLog::Debug(LABEL, "ParseEXIFSubIFD EXIT");
    return PARSE_EXIF_SUCCESS;
}

void EXIFInfo::setLatitude(const unsigned char *buf, bool alignIntel, unsigned tiff_header_start, 
    unsigned data) {
    this->GeoLocation.LatComponents.degrees = 
        parseEXIFRational(buf + data + tiff_header_start, alignIntel);
    this->GeoLocation.LatComponents.minutes = 
        parseEXIFRational(buf + data + tiff_header_start + OFFSET_8, alignIntel);
    this->GeoLocation.LatComponents.seconds = 
        parseEXIFRational(buf + data + tiff_header_start + OFFSET_16, alignIntel);
    this->GeoLocation.Latitude = 
        this->GeoLocation.LatComponents.degrees +
        this->GeoLocation.LatComponents.minutes / 60 +
        this->GeoLocation.LatComponents.seconds / 3600;
    if (SOUTH == this->GeoLocation.LatComponents.direction)
        this->GeoLocation.Latitude = -this->GeoLocation.Latitude;
}

void EXIFInfo::setLongitude(const unsigned char *buf, bool alignIntel, unsigned tiff_header_start, 
    unsigned data) {
    this->GeoLocation.LonComponents.degrees = 
        parseEXIFRational(buf + data + tiff_header_start, alignIntel);
    this->GeoLocation.LonComponents.minutes = 
        parseEXIFRational(buf + data + tiff_header_start + OFFSET_8, alignIntel);
    this->GeoLocation.LonComponents.seconds = 
        parseEXIFRational(buf + data + tiff_header_start + OFFSET_16, alignIntel);
    this->GeoLocation.Longitude = 
        this->GeoLocation.LonComponents.degrees +
        this->GeoLocation.LonComponents.minutes / 60 +
        this->GeoLocation.LonComponents.seconds / 3600;
    if (WEST == this->GeoLocation.LonComponents.direction)
        this->GeoLocation.Longitude = -this->GeoLocation.Longitude;
}

void EXIFInfo::setGpsEntryValue(unsigned short tag, unsigned short format, unsigned length, unsigned data,
    const unsigned char *buf, bool alignIntel, unsigned tiff_header_start, unsigned entry_offs) {
    switch(tag) {
        case T_GPS_NOS:
            this->GeoLocation.LatComponents.direction = *(buf + entry_offs + OFFSET_8);
            if (SOUTH == this->GeoLocation.LatComponents.direction)
                this->GeoLocation.Latitude = -this->GeoLocation.Latitude;
            break;
        case T_GPS_LAT:
            if (format == FORMAT_UNSIGNED_RATIONAL && length == 3) {
                setLatitude(buf, alignIntel, tiff_header_start, data);
            }
            break;
        case T_GPS_EOW:
            this->GeoLocation.LonComponents.direction = *(buf + entry_offs + OFFSET_8);
            if (WEST == this->GeoLocation.LonComponents.direction)
                this->GeoLocation.Longitude = -this->GeoLocation.Longitude;
            break;
        case T_GPS_LON:
            if (format == FORMAT_UNSIGNED_RATIONAL && length == 3) {
                setLongitude(buf, alignIntel, tiff_header_start, data);
            }
            break;
        case T_GPS_ALT_REF:
            this->GeoLocation.AltitudeRef = *(buf + entry_offs + OFFSET_8);
            if (1 == this->GeoLocation.AltitudeRef)
                this->GeoLocation.Altitude = -this->GeoLocation.Altitude;
            break;
        case T_GPS_ALT:
            if (format == FORMAT_UNSIGNED_RATIONAL) {
                this->GeoLocation.Altitude = 
                    parseEXIFRational(buf + data + tiff_header_start, alignIntel);
            if (1 == this->GeoLocation.AltitudeRef)
                this->GeoLocation.Altitude = -this->GeoLocation.Altitude;
            }
            break;
    }
}

int EXIFInfo::ParseGpsSubIFD(const unsigned char *buf, unsigned len, bool alignIntel, 
    unsigned tiff_header_start, unsigned gps_sub_ifd_offset) 
{
    HiLog::Debug(LABEL, "ParseGpsSubIFD enter");
    // Jump to the GPS SubIFD if it exists and parse all the information
    // there. Note that it's possible that the GPS SubIFD doesn't exist.
    unsigned offs = gps_sub_ifd_offset;// current offset into buffer
    if (offs + OFFSET_4 <= len) {
        int num_entries = parseShort(buf + offs, alignIntel);
        if (offs + OFFSET_6 + OFFSET_12 * num_entries > len)
            return PARSE_EXIF_ERROR_CORRUPT;
        offs += OFFSET_2;
        while (--num_entries >= 0) {
            unsigned short tag = parseShort(buf + offs, alignIntel);
            unsigned short format = parseShort(buf + offs + OFFSET_2, alignIntel);
            unsigned length = parseLong(buf + offs + OFFSET_4, alignIntel);
            unsigned data = parseLong(buf + offs + OFFSET_8, alignIntel);

            setGpsEntryValue(tag, format, length, data, buf, alignIntel, tiff_header_start, offs);
            offs += OFFSET_12;
        }
    }
    HiLog::Debug(LABEL, "ParseGpsSubIFD EXIT");
    return PARSE_EXIF_SUCCESS;
}

void EXIFInfo::Clear() {
    // Strings
    ImageDescription = "";
    Make = "";
    Model = "";
    Software = "";
    DateTime = "";
    DateTimeOriginal = "";
    DateTimeDigitized = ""; 
    SubSecTimeOriginal = "";
    Copyright = "";

    // Shorts / unsigned / double
    ByteAlign = 0;
    Orientation = 0; 

    BitsPerSample = 0;
    ExposureTime = 0;
    FNumber = 0;
    ISOSpeedRatings = 0;
    ShutterSpeedValue = 0;
    ExposureBiasValue = 0;
    SubjectDistance = 0;
    FocalLength = 0;
    FocalLengthIn35mm = 0;
    Flash = 0;
    MeteringMode = 0;
    ImageWidth = 0;
    ImageHeight = 0;

    // Geolocation
    GeoLocation.Latitude = 0;
    GeoLocation.Longitude = 0;
    GeoLocation.Altitude = 0;
    GeoLocation.AltitudeRef = 0;
    GeoLocation.LatComponents.degrees = 0;
    GeoLocation.LatComponents.minutes = 0;
    GeoLocation.LatComponents.seconds = 0;
    GeoLocation.LatComponents.direction = 0;
    GeoLocation.LonComponents.degrees = 0;
    GeoLocation.LonComponents.minutes = 0;
    GeoLocation.LonComponents.seconds = 0;
    GeoLocation.LonComponents.direction = 0;
}