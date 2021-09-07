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

package ohos.media.image.common;

import ohos.utils.system.SystemCap;
import ohos.utils.system.SystemCapability;

/**
 * Provides definitions of image property keys.
 *
 * @Syscap {@link SystemCapability.Multimedia#IMAGE}
 * @since 1
 */
@SystemCap("SystemCapability.Multimedia.Image")
public final class PropertyKey {
    private PropertyKey() {
    }

    /**
     * Provides definitions of image Exif property keys.
     *
     * @since 1
     */
    public static final class Exif {
        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String ARTIST = "Artist";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String BITS_PER_SAMPLE = "BitsPerSample";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String COMPRESSION = "Compression";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String COPYRIGHT = "Copyright";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String DATETIME = "DateTime";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String IMAGE_DESCRIPTION = "ImageDescription";

        /**
         * Property value type is unsigned int.
         *
         * @since 1
         */
        public static final String IMAGE_LENGTH = "ImageLength";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String IMAGE_WIDTH = "ImageWidth";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String JPEG_INTERCHANGE_FORMAT = "JPEGInterchangeFormat";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String JPEG_INTERCHANGE_FORMAT_LENGTH = "JPEGInterchangeFormatLength";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String ORIENTATION = "Orientation";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String PHOTOMETRIC_INTERPRETATION = "PhotometricInterpretation";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String PLANAR_CONFIGURATION = "PlanarConfiguration";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String RESOLUTION_UNIT = "ResolutionUnit";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String ROWS_PER_STRIP = "RowsPerStrip";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SAMPLES_PER_PIXEL = "SamplesPerPixel";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String STRIP_BYTE_COUNTS = "StripByteCounts";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String STRIP_OFFSETS = "StripOffsets";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String TRANSFER_FUNCTION = "TransferFunction";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String Y_CB_CR_SUB_SAMPLING = "YCbCrSubSampling";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String Y_CB_CR_POSITIONING = "YCbCrPositioning";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String COLOR_SPACE = "ColorSpace";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String CONTRAST = "Contrast";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String CUSTOM_RENDERED = "CustomRendered";

        /**
         * Property value type is double not negative.
         *
         * @since 1
         */
        public static final String DIGITAL_ZOOM_RATIO = "DigitalZoomRatio";

        /**
         * Property value type is double.
         *
         * @since 1
         */
        public static final String EXPOSURE_BIAS_VALUE = "ExposureBiasValue";

        /**
         * Property value type is rational not negative.
         *
         * @since 1
         */
        public static final String EXPOSURE_INDEX = "ExposureIndex";

        /**
         * Property value type is unsigned short.
         *
         * @since 1
         */
        public static final String EXPOSURE_MODE = "ExposureMode";

        /**
         * Property value type is unsigned short.
         *
         * @since 1
         */
        public static final String EXPOSURE_PROGRAM = "ExposureProgram";

        /**
         * Property value type is double not negative.
         *
         * @since 1
         */
        public static final String EXPOSURE_TIME = "ExposureTime";

        /**
         * Property value type is double not negative.
         *
         * @since 1
         */
        public static final String F_NUMBER = "FNumber";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String FLASH = "Flash";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String FOCAL_LENGTH_IN_35MM_FILM = "FocalLengthIn35mmFilm";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String FOCAL_PLANE_RESOLUTION_UNIT = "FocalPlaneResolutionUnit";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String GAIN_CONTROL = "GainControl";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String ISO_SPEED_RATINGS = "ISOSpeedRatings";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String METERING_MODE = "MeteringMode";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String NEW_SUBFILE_TYPE = "NewSubfileType";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String PIXEL_X_DIMENSION = "PixelXDimension";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String PIXEL_Y_DIMENSION = "PixelYDimension";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SATURATION = "Saturation";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SCENE_CAPTURE_TYPE = "SceneCaptureType";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SENSING_METHOD = "SensingMethod";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SHARPNESS = "Sharpness";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SUBFILE_TYPE = "SubfileType";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SUBJECT_AREA = "SubjectArea";

        /**
         * Property value type is double not negative.
         *
         * @since 1
         */
        public static final String SUBJECT_DISTANCE = "SubjectDistance";

        /**
         * Property value type is unsigned short.
         *
         * @since 1
         */
        public static final String SUBJECT_DISTANCE_RANGE = "SubjectDistanceRange";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String SUBJECT_LOCATION = "SubjectLocation";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String WHITE_BALANCE = "WhiteBalance";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String GPS_ALTITUDE_REF = "GPSAltitudeRef";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String GPS_DIFFERENTIAL = "GPSDifferential";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String THUMBNAIL_IMAGE_LENGTH = "ThumbnailImageLength";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String THUMBNAIL_IMAGE_WIDTH = "ThumbnailImageWidth";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String DNG_VERSION = "DNGVersion";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String DEFAULT_CROP_SIZE = "DefaultCropSize";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String ORF_PREVIEW_IMAGE_START = "PreviewImageStart";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String ORF_PREVIEW_IMAGE_LENGTH = "PreviewImageLength";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String ORF_ASPECT_FRAME = "AspectFrame";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String RW2_SENSOR_BOTTOM_BORDER = "SensorBottomBorder";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String RW2_SENSOR_LEFT_BORDER = "SensorLeftBorder";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String RW2_SENSOR_RIGHT_BORDER = "SensorRightBorder";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String RW2_SENSOR_TOP_BORDER = "SensorTopBorder";

        /**
         * Property value type is int.
         *
         * @since 1
         */
        public static final String RW2_ISO = "ISO";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String PRIMARY_CHROMATICITIES = "PrimaryChromaticities";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String REFERENCE_BLACK_WHITE = "ReferenceBlackWhite";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String WHITE_POINT = "WhitePoint";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String X_RESOLUTION = "XResolution";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String Y_CB_CR_COEFFICIENTS = "YCbCrCoefficients";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String Y_RESOLUTION = "YResolution";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String APERTURE_VALUE = "ApertureValue";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String BRIGHTNESS_VALUE = "BrightnessValue";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String COMPRESSED_BITS_PER_PIXEL = "CompressedBitsPerPixel";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String FLASH_ENERGY = "FlashEnergy";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String FOCAL_LENGTH = "FocalLength";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String FOCAL_PLANE_X_RESOLUTION = "FocalPlaneXResolution";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String FOCAL_PLANE_Y_RESOLUTION = "FocalPlaneYResolution";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String MAX_APERTURE_VALUE = "MaxApertureValue";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String SHUTTER_SPEED_VALUE = "ShutterSpeedValue";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_ALTITUDE = "GPSAltitude";

        /**
         * Property value type is rational, format is "num1/denom1,num2/denom2,num3/denom3".
         *
         * @since 1
         */
        public static final String GPS_LATITUDE = "GPSLatitude";

        /**
         * Property value type is rational, format is "num1/denom1,num2/denom2,num3/denom3".
         *
         * @since 1
         */
        public static final String GPS_LONGITUDE = "GPSLongitude";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_DEST_DISTANCE = "GPSDestDistance";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_DEST_LATITUDE = "GPSDestLatitude";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_DEST_LONGITUDE = "GPSDestLongitude";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_IMG_DIRECTION = "GPSImgDirection";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_SPEED = "GPSSpeed";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_TRACK = "GPSTrack";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_DOP = "GPSDOP";

        /**
         * Property value type is rational.
         *
         * @since 1
         */
        public static final String GPS_DEST_BEARING = "GPSDestBearing";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String MAKE = "Make";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String MODEL = "Model";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String SOFTWARE = "Software";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String CFA_PATTERN = "CFAPattern";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String COMPONENTS_CONFIGURATION = "ComponentsConfiguration";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String DATETIME_DIGITIZED = "DateTimeDigitized";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String DATETIME_ORIGINAL = "DateTimeOriginal";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String DEVICE_SETTING_DESCRIPTION = "DeviceSettingDescription";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String EXIF_VERSION = "ExifVersion";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String FILE_SOURCE = "FileSource";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String FLASHPIX_VERSION = "FlashpixVersion";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String IMAGE_UNIQUE_ID = "ImageUniqueID";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String LIGHT_SOURCE = "LightSource";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String MAKER_NOTE = "MakerNote";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String OECF = "OECF";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String RELATED_SOUND_FILE = "RelatedSoundFile";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String SCENE_TYPE = "SceneType";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String SPATIAL_FREQUENCY_RESPONSE = "SpatialFrequencyResponse";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String SPECTRAL_SENSITIVITY = "SpectralSensitivity";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String SUBSEC_TIME = "SubSecTime";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String SUBSEC_TIME_DIGITIZED = "SubSecTimeDigitized";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String SUBSEC_TIME_ORIGINAL = "SubSecTimeOriginal";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String USER_COMMENT = "UserComment";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_AREA_INFORMATION = "GPSAreaInformation";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_DATESTAMP = "GPSDateStamp";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_DEST_BEARING_REF = "GPSDestBearingRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_DEST_DISTANCE_REF = "GPSDestDistanceRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_DEST_LATITUDE_REF = "GPSDestLatitudeRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_DEST_LONGITUDE_REF = "GPSDestLongitudeRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_IMG_DIRECTION_REF = "GPSImgDirectionRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_LATITUDE_REF = "GPSLatitudeRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_LONGITUDE_REF = "GPSLongitudeRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_MAP_DATUM = "GPSMapDatum";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_MEASURE_MODE = "GPSMeasureMode";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_PROCESSING_METHOD = "GPSProcessingMethod";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_SATELLITES = "GPSSatellites";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_SPEED_REF = "GPSSpeedRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_STATUS = "GPSStatus";

        /**
         * Property value type is String, format is "hh:mm:ss".
         *
         * @since 1
         */
        public static final String GPS_TIMESTAMP = "GPSTimeStamp";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_TRACK_REF = "GPSTrackRef";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String GPS_VERSION_ID = "GPSVersionID";

        /**
         * Property value type is String.
         *
         * @since 1
         */
        public static final String INTEROPERABILITY_INDEX = "InteroperabilityIndex";

        /**
         * Property value type is undefined.
         *
         * @since 1
         */
        public static final String ORF_THUMBNAIL_IMAGE = "ThumbnailImage";

        /**
         * PanasonicRaw tags. Property value type is undefined.
         *
         * @since 1
         */
        public static final String RW2_JPG_FROM_RAW = "JpgFromRaw";

        /**
         * Property value type is undefined.
         *
         * @since 1
         */
        public static final String XMP = "Xmp";
    }

    /**
     * Provides definitions of GIF property keys.
     *
     * @since 4
     */
    public static final class GIF {
        /**
         * Property value type is Integer.
         *
         * @since 4
         */
        public static final String LOOP_COUNT = "GIFLoopCount";

        /**
         * Property value type is Integer.
         *
         * @since 4
         */
        public static final String DELAY_TIME = "GIFDelayTime";
    }

}
