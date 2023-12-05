/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { AsyncCallback } from './@ohos.base';
import type colorSpaceManager from './@ohos.graphics.colorSpaceManager';
import type rpc from './@ohos.rpc';
import type resourceManager from './@ohos.resourceManager';

/**
 * @namespace image
 * @since 6
 */
declare namespace image {
  /**
   * Enumerates pixel map formats.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 7
   */
  /**
   * Enumerates pixel map formats.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  enum PixelMapFormat {
    /**
     * Indicates an unknown format.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Indicates an unknown format.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    UNKNOWN = 0,

    /**
     * Indicates that each pixel is stored on 16 bits. Only the R, G, and B components are encoded
     * from the higher-order to the lower-order bits: red is stored with 5 bits of precision,
     * green is stored with 6 bits of precision, and blue is stored with 5 bits of precision.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Indicates that each pixel is stored on 16 bits. Only the R, G, and B components are encoded
     * from the higher-order to the lower-order bits: red is stored with 5 bits of precision,
     * green is stored with 6 bits of precision, and blue is stored with 5 bits of precision.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    RGB_565 = 2,

    /**
     * Indicates that each pixel is stored on 32 bits. Each pixel contains 4 components：B(8bits), G(8bits), R(8bits), A(8bits)
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Indicates that each pixel is stored on 32 bits. Each pixel contains 4 components：B(8bits), G(8bits), R(8bits), A(8bits)
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    RGBA_8888 = 3,

    /**
     * Indicates that each pixel is stored on 32 bits. Each pixel contains 4 components：B(8bits), G(8bits), R(8bits), A(8bits)
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that each pixel is stored on 32 bits. Each pixel contains 4 components：B(8bits), G(8bits), R(8bits), A(8bits)
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    BGRA_8888 = 4,

    /**
     * Indicates that each pixel is stored on 24 bits. Each pixel contains 3 components：R(8bits), G(8bits), B(8bits)
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that each pixel is stored on 24 bits. Each pixel contains 3 components：R(8bits), G(8bits), B(8bits)
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    RGB_888 = 5,

    /**
     * Indicates that each pixel is stored on 8 bits. Each pixel contains 1 component：ALPHA(8bits)
     * and is stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that each pixel is stored on 8 bits. Each pixel contains 1 component：ALPHA(8bits)
     * and is stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    ALPHA_8 = 6,

    /**
     * Indicates that each pixel is stored on 32 bits. Each pixel contains 4 components：B(8bits), G(8bits), R(8bits), A(8bits)
     * and are stored from the higher-order to the lower-order bits in F16.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that each pixel is stored on 32 bits. Each pixel contains 4 components：B(8bits), G(8bits), R(8bits), A(8bits)
     * and are stored from the higher-order to the lower-order bits in F16.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    RGBA_F16 = 7,

    /**
     * Indicates that the storage order is to store Y first and then V U alternately each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that the storage order is to store Y first and then V U alternately each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    NV21 = 8,

    /**
     * Indicates that the storage order is to store Y first and then U V alternately each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that the storage order is to store Y first and then U V alternately each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    NV12 = 9
  }

  /**
   * Describes the size of an image.
   *
   * @typedef Size
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 6
   */
  /**
   * Describes the size of an image.
   *
   * @typedef Size
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  interface Size {
    /**
     * Height
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 6
     */
    /**
     * Height
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    height: number;

    /**
     * Width
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 6
     */
    /**
     * Width
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    width: number;
  }

  /**
   * Enumerates exchangeable image file format (Exif) information types of an image.
   *
   * @enum { string }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 7
   */
  /**
   * Enumerates exchangeable image file format (Exif) information types of an image.
   *
   * @enum { string }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  enum PropertyKey {
    /**
     * Number of bits in each pixel of an image.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Number of bits in each pixel of an image.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    BITS_PER_SAMPLE = 'BitsPerSample',

    /**
     * Image rotation mode.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Image rotation mode.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    ORIENTATION = 'Orientation',

    /**
     * Image length.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Image length.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    IMAGE_LENGTH = 'ImageLength',

    /**
     * Image width.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Image width.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    IMAGE_WIDTH = 'ImageWidth',

    /**
     * GPS latitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * GPS latitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    GPS_LATITUDE = 'GPSLatitude',

    /**
     * GPS longitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * GPS longitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    GPS_LONGITUDE = 'GPSLongitude',

    /**
     * GPS latitude reference. For example, N indicates north latitude and S indicates south latitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * GPS latitude reference. For example, N indicates north latitude and S indicates south latitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    GPS_LATITUDE_REF = 'GPSLatitudeRef',

    /**
     * GPS longitude reference. For example, E indicates east longitude and W indicates west longitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * GPS longitude reference. For example, E indicates east longitude and W indicates west longitude.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    GPS_LONGITUDE_REF = 'GPSLongitudeRef',

    /**
     * Shooting time
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Shooting time
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    DATE_TIME_ORIGINAL = 'DateTimeOriginal',

    /**
     * Exposure time
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Exposure time
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    EXPOSURE_TIME = 'ExposureTime',

    /**
     * Scene type
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Scene type
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    SCENE_TYPE = 'SceneType',

    /**
     * ISO speedratings
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * ISO speedratings
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    ISO_SPEED_RATINGS = 'ISOSpeedRatings',

    /**
     * Aperture value
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Aperture value
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    F_NUMBER = 'FNumber',

    /**
     * Date time
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    DATE_TIME = 'DateTime',

    /**
     * GPS time stamp
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    GPS_TIME_STAMP = 'GPSTimeStamp',
 
    /**
     * GPS date stamp
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    GPS_DATE_STAMP = 'GPSDateStamp',
 
    /**
     * Image description
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    IMAGE_DESCRIPTION = 'ImageDescription',
 
    /**
     * Make
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    MAKE = 'Make',
 
    /**
     * Model
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    MODEL = 'Model',
 
    /**
     * Photo mode
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    PHOTO_MODE = 'PhotoMode',
 
    /**
     * Sensitivity type
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    SENSITIVITY_TYPE = 'SensitivityType',
 
    /**
     * Standard output sensitivity
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    STANDARD_OUTPUT_SENSITIVITY = 'StandardOutputSensitivity',
 
    /**
     * Recommended exposure index
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    RECOMMENDED_EXPOSURE_INDEX = 'RecommendedExposureIndex',
 
    /**
     * ISO speed
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    ISO_SPEED = 'ISOSpeedRatings',
 
    /**
     * Aperture value
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    APERTURE_VALUE = 'ApertureValue',
 
    /**
     * Exposure bias value
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    EXPOSURE_BIAS_VALUE = 'ExposureBiasValue',
 
    /**
     * Metering mode
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    METERING_MODE = 'MeteringMode',
 
    /**
     * Light source
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    LIGHT_SOURCE = 'LightSource',
 
    /**
     * Flash
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    FLASH = 'Flash',
 
    /**
     * Focal length
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    FOCAL_LENGTH = 'FocalLength',
 
    /**
     * User comment
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    USER_COMMENT = 'UserComment',
 
    /**
     * Pixel x dimension
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    PIXEL_X_DIMENSION = 'PixelXDimension',
 
    /**
     * Pixel y dimension
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    PIXEL_Y_DIMENSION = 'PixelYDimension',
 
    /**
     * White balance
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    WHITE_BALANCE = 'WhiteBalance',
 
    /**
     * Focal length in 35mm film
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    FOCAL_LENGTH_IN_35_MM_FILM = 'FocalLengthIn35mmFilm',
 
    /**
     * Capture mode
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    CAPTURE_MODE = 'HwMnoteCaptureMode',
 
    /**
     * Physical aperture
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    PHYSICAL_APERTURE = 'HwMnotePhysicalAperture',

    /**
     * Roll Angle
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    ROLL_ANGLE = 'HwMnoteRollAngle',

    /**
     * Pitch Angle
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    PITCH_ANGLE = 'HwMnotePitchAngle',

    /**
     * Capture Scene: Food
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_FOOD_CONF = 'HwMnoteSceneFoodConf',

    /**
     * Capture Scene: Stage
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_STAGE_CONF = 'HwMnoteSceneStageConf',

    /**
     * Capture Scene: Blue Sky
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_BLUE_SKY_CONF = 'HwMnoteSceneBlueSkyConf',

    /**
     * Capture Scene: Green Plant
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_GREEN_PLANT_CONF = 'HwMnoteSceneGreenPlantConf',

    /**
     * Capture Scene: Beach
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_BEACH_CONF = 'HwMnoteSceneBeachConf',

    /**
     * Capture Scene: Snow
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_SNOW_CONF = 'HwMnoteSceneSnowConf',

    /**
     * Capture Scene: Sunset
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_SUNSET_CONF = 'HwMnoteSceneSunsetConf',

    /**
     * Capture Scene: Flowers
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_FLOWERS_CONF = 'HwMnoteSceneFlowersConf',

    /**
     * Capture Scene: Night
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_NIGHT_CONF = 'HwMnoteSceneNightConf',

    /**
     * Capture Scene: Text
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    SCENE_TEXT_CONF = 'HwMnoteSceneTextConf',

    /**
     * Face Count
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    FACE_COUNT = 'HwMnoteFaceCount',

    /**
     * Focus Mode
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 11
     */
    FOCUS_MODE = 'HwMnoteFocusMode'
  }

  /**
   * Enum for image formats.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 9
   */
  enum ImageFormat {
    /**
     * YCBCR422 semi-planar format.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    YCBCR_422_SP = 1000,

    /**
     * JPEG encoding format.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    JPEG = 2000
  }

  /**
   * Enumerates alpha types.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 9
   */
  /**
   * Enumerates alpha types.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  enum AlphaType {
    /**
     * Indicates an unknown alpha type.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates an unknown alpha type.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    UNKNOWN = 0,

    /**
     * Indicates that the image has no alpha channel, or all pixels in the image are fully opaque.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that the image has no alpha channel, or all pixels in the image are fully opaque.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    OPAQUE = 1,

    /**
     * Indicates that RGB components of each pixel in the image are premultiplied by alpha.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that RGB components of each pixel in the image are premultiplied by alpha.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    PREMUL = 2,

    /**
     * Indicates that RGB components of each pixel in the image are independent of alpha and are not premultiplied by alpha.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates that RGB components of each pixel in the image are independent of alpha and are not premultiplied by alpha.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    UNPREMUL = 3
  }

  /**
   * Enum for image scale mode.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 9
   */
  /**
   * Enum for image scale mode.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  enum ScaleMode {
    /**
     * Indicates the effect that fits the image into the target size.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates the effect that fits the image into the target size.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    FIT_TARGET_SIZE = 0,

    /**
     * Indicates the effect that scales an image to fill the target image area and center-crops the part outside the area.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates the effect that scales an image to fill the target image area and center-crops the part outside the area.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    CENTER_CROP = 1
  }

  /**
   * The component type of image.
   *
   * @enum { number }
   * @syscap SystemCapability.Multimedia.Image.ImageReceiver
   * @since 9
   */
  enum ComponentType {
    /**
     * Luma info.
     *
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    YUV_Y = 1,

    /**
     * Chrominance info.
     *
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    YUV_U = 2,

    /**
     * Chroma info.
     *
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    YUV_V = 3,

    /**
     * Jpeg type.
     *
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    JPEG = 4
  }

  /**
   * Describes region information.
   *
   * @typedef Region
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 8
   */
  /**
   * Describes region information.
   *
   * @typedef Region
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  interface Region {
    /**
     * Image size.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Image size.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    size: Size;

    /**
     * x-coordinate at the upper left corner of the image.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * x-coordinate at the upper left corner of the image.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    x: number;

    /**
     * y-coordinate at the upper left corner of the image.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * y-coordinate at the upper left corner of the image.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    y: number;
  }

  /**
   * Describes area information in an image.
   *
   * @typedef PositionArea
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 7
   */
  /**
   * Describes area information in an image.
   *
   * @typedef PositionArea
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  interface PositionArea {
    /**
     * Image data that will be read or written.
     *
     * @type { ArrayBuffer }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Image data that will be read or written.
     *
     * @type { ArrayBuffer }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    pixels: ArrayBuffer;

    /**
     * Offset for data reading.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Offset for data reading.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    offset: number;

    /**
     * Number of bytes to read.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Number of bytes to read.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    stride: number;

    /**
     * Region to read.
     *
     * @type { Region }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Region to read.
     *
     * @type { Region }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    region: Region;
  }

  /**
   * Describes image information.
   *
   * @typedef ImageInfo
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 6
   */
  /**
   * Describes image information.
   *
   * @typedef ImageInfo
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  interface ImageInfo {
    /**
     * Indicates image dimensions specified by a {@link Size} interface.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 6
     */
    /**
     * Indicates image dimensions specified by a {@link Size} interface.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    size: Size;

    /**
     * Indicates image default density.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Indicates image default density.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    density: number;
  }

  /**
   * Describes the option for image packing.
   *
   * @typedef PackingOption
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   * @since 6
   */
  /**
   * Describes the option for image packing.
   *
   * @typedef PackingOption
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   * @crossplatform
   * @since 10
   */
  interface PackingOption {
    /**
     * Multipurpose Internet Mail Extensions (MIME) format of the target image, for example, image/jpeg.
     *
     * @type { string }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 6
     */
    /**
     * Multipurpose Internet Mail Extensions (MIME) format of the target image, for example, image/jpeg.
     *
     * @type { string }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    format: string;

    /**
     * Quality of the target image. The value is an integer ranging from 0 to 100. A larger value indicates better.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 6
     */
    /**
     * Quality of the target image. The value is an integer ranging from 0 to 100. A larger value indicates better.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    quality: number;

    /**
     * BufferSize of the target image.
     * If this bufferSize is less than or equal to 0, it will be converted to 10MB.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 9
     */
    /**
     * BufferSize of the target image.
     * If this bufferSize is less than or equal to 0, it will be converted to 10MB.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    bufferSize?: number;
  }

  /**
   * Describes image properties.
   *
   * @typedef GetImagePropertyOptions
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 7
   */
  /**
   * Describes image properties.
   *
   * @typedef GetImagePropertyOptions
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  interface GetImagePropertyOptions {
    /**
     * Index of an image.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Index of an image.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    index?: number;

    /**
     * Default property value.
     *
     * @type { ?string }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Default property value.
     *
     * @type { ?string }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    defaultValue?: string;
  }

  /**
   * Describes image decoding parameters.
   *
   * @typedef DecodingOptions
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 7
   */
  /**
   * Describes image decoding parameters.
   *
   * @typedef DecodingOptions
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  interface DecodingOptions {
    /**
     * Number of image frames.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Number of image frames.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    index?: number;

    /**
     * Sampling ratio of the image pixel map.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Sampling ratio of the image pixel map.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    sampleSize?: number;

    /**
     * Rotation angle of the image pixel map. The value ranges from 0 to 360.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Rotation angle of the image pixel map. The value ranges from 0 to 360.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    rotate?: number;

    /**
     * Whether the image pixel map is editable.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Whether the image pixel map is editable.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    editable?: boolean;

    /**
     * Width and height of the image pixel map. The value (0, 0) indicates that the pixels are decoded
     * based on the original image size.
     *
     * @type { ?Size }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Width and height of the image pixel map. The value (0, 0) indicates that the pixels are decoded
     * based on the original image size.
     *
     * @type { ?Size }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    desiredSize?: Size;

    /**
     * Cropping region of the image pixel map.
     *
     * @type { ?Region }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Cropping region of the image pixel map.
     *
     * @type { ?Region }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    desiredRegion?: Region;

    /**
     * Data format of the image pixel map.
     *
     * @type { ?PixelMapFormat }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Data format of the image pixel map.
     *
     * @type { ?PixelMapFormat }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    desiredPixelFormat?: PixelMapFormat;

    /**
     * The density for image pixel map.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 9
     */
    /**
     * The density for image pixel map.
     *
     * @type { ?number }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    fitDensity?: number;
  }

  /**
   * Describes image color components.
   *
   * @typedef Component
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 9
   */
  interface Component {
    /**
     * Component type.
     *
     * @type { ComponentType }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    readonly componentType: ComponentType;

    /**
     * Row stride.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    readonly rowStride: number;

    /**
     * Pixel stride.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    readonly pixelStride: number;

    /**
     * Component buffer.
     *
     * @type { ArrayBuffer }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    readonly byteBuffer: ArrayBuffer;
  }

  /**
   * Initialization options for pixelmap.
   *
   * @typedef InitializationOptions
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 8
   */
  /**
   * Initialization options for pixelmap.
   *
   * @typedef InitializationOptions
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  interface InitializationOptions {
    /**
     * PixelMap size.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 8
     */
    /**
     * PixelMap size.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    size: Size;

    /**
     * PixelMap expected format.
     *
     * @type { ?PixelMapFormat }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 8
     */
    /**
     * PixelMap expected format.
     *
     * @type { ?PixelMapFormat }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    pixelFormat?: PixelMapFormat;

    /**
     * Editable or not.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 8
     */
    /**
     * Editable or not.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    editable?: boolean;

    /**
     * PixelMap expected alpha type.
     *
     * @type { ?AlphaType }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * PixelMap expected alpha type.
     *
     * @type { ?AlphaType }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    alphaType?: AlphaType;

    /**
     * PixelMap expected scaling effect.
     *
     * @type { ?ScaleMode }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * PixelMap expected scaling effect.
     *
     * @type { ?ScaleMode }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    scaleMode?: ScaleMode;
  }

  /**
   * Initialization options for ImageSource.
   *
   * @typedef SourceOptions
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 9
   */
  /**
   * Initialization options for ImageSource.
   *
   * @typedef SourceOptions
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  interface SourceOptions {
    /**
     * The density for ImageSource.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * The density for ImageSource.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    sourceDensity: number;

    /**
     * PixelMap expected format.
     *
     * @type { ?PixelMapFormat }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * PixelMap expected format.
     *
     * @type { ?PixelMapFormat }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    sourcePixelFormat?: PixelMapFormat;

    /**
     * PixelMap size.
     *
     * @type { ?Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * PixelMap size.
     *
     * @type { ?Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    sourceSize?: Size;
  }

  /**
   * Create pixelmap by data buffer.
   *
   * @param { ArrayBuffer } colors The image color buffer.
   * @param { InitializationOptions } options Initialization options for pixelmap.
   * @param { AsyncCallback<PixelMap> } callback Callback used to return the PixelMap object.
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 8
   */
  /**
   * Create pixelmap by data buffer.
   *
   * @param { ArrayBuffer } colors The image color buffer.
   * @param { InitializationOptions } options Initialization options for pixelmap.
   * @param { AsyncCallback<PixelMap> } callback Callback used to return the PixelMap object.
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  function createPixelMap(colors: ArrayBuffer, options: InitializationOptions, callback: AsyncCallback<PixelMap>): void;

  /**
   * Create pixelmap by data buffer.
   *
   * @param { ArrayBuffer } colors The image color buffer.
   * @param { InitializationOptions } options Initialization options for pixelmap.
   * @returns { Promise<PixelMap> } A Promise instance used to return the PixelMap object.
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 8
   */
  /**
   * Create pixelmap by data buffer.
   *
   * @param { ArrayBuffer } colors The image color buffer.
   * @param { InitializationOptions } options Initialization options for pixelmap.
   * @returns { Promise<PixelMap> } A Promise instance used to return the PixelMap object.
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  function createPixelMap(colors: ArrayBuffer, options: InitializationOptions): Promise<PixelMap>;

  /**
   * Creates a PixelMap object based on MessageSequence parameter.
   *
   * @param { rpc.MessageSequence } sequence - rpc.MessageSequence parameter.
   * @returns { PixelMap } Returns the instance if the operation is successful;Otherwise, an exception will be thrown.
   * @throws { BusinessError } 62980096 - Operation failed.
   * @throws { BusinessError } 62980097 - IPC error.
   * @throws { BusinessError } 62980115 - Invalid input parameter.
   * @throws { BusinessError } 62980105 - Failed to get the data.
   * @throws { BusinessError } 62980177 - Abnormal API environment.
   * @throws { BusinessError } 62980178 - Failed to create the PixelMap.
   * @throws { BusinessError } 62980179 - Abnormal buffer size.
   * @throws { BusinessError } 62980180 - FD mapping failed.
   * @throws { BusinessError } 62980246 - Failed to read the PixelMap.
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 11
   */
  function createPixelMapFromParcel(sequence: rpc.MessageSequence): PixelMap;

  /**
   * Creates an ImageSource instance based on the URI.
   *
   * @param { string } uri Image source URI.
   * @returns { ImageSource } returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 6
   */
  /**
   * Creates an ImageSource instance based on the URI.
   *
   * @param { string } uri Image source URI.
   * @returns { ImageSource } returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function createImageSource(uri: string): ImageSource;

  /**
   * Creates an ImageSource instance based on the URI.
   *
   * @param { string } uri Image source URI.
   * @param { SourceOptions } options The config of Image source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 9
   */
  /**
   * Creates an ImageSource instance based on the URI.
   *
   * @param { string } uri Image source URI.
   * @param { SourceOptions } options The config of Image source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function createImageSource(uri: string, options: SourceOptions): ImageSource;

  /**
   * Creates an ImageSource instance based on the file descriptor.
   *
   * @param { number } fd ID of a file descriptor.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 7
   */
  /**
   * Creates an ImageSource instance based on the file descriptor.
   *
   * @param { number } fd ID of a file descriptor.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function createImageSource(fd: number): ImageSource;

  /**
   * Creates an ImageSource instance based on the file descriptor.
   *
   * @param { number } fd ID of a file descriptor.
   * @param { SourceOptions } options The config of Image source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 9
   */
  /**
   * Creates an ImageSource instance based on the file descriptor.
   *
   * @param { number } fd ID of a file descriptor.
   * @param { SourceOptions } options The config of Image source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function createImageSource(fd: number, options: SourceOptions): ImageSource;

  /**
   * Creates an ImageSource instance based on the buffer.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 9
   */
  /**
   * Creates an ImageSource instance based on the buffer.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function createImageSource(buf: ArrayBuffer): ImageSource;

  /**
   * Creates an ImageSource instance based on the buffer.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @param { SourceOptions } options The config of Image source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 9
   */
  /**
   * Creates an ImageSource instance based on the buffer.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @param { SourceOptions } options The config of Image source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function createImageSource(buf: ArrayBuffer, options: SourceOptions): ImageSource;

  /**
   * Creates an ImageSource instance based on the raw file descriptor.
   *
   * @param { resourceManager.RawFileDescriptor } rawfile The raw file descriptor of the image.
   * @param { SourceOptions } options The config of Image source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 11
   */
  function createImageSource(rawfile: resourceManager.RawFileDescriptor, options?: SourceOptions): ImageSource;

  /**
   * Creates an ImageSource instance based on the buffer in incremental.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 9
   */
  /**
   * Creates an ImageSource instance based on the buffer in incremental.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function CreateIncrementalSource(buf: ArrayBuffer): ImageSource;

  /**
   * Creates an ImageSource instance based on the buffer in incremental.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @param { SourceOptions } options The config of source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 9
   */
  /**
   * Creates an ImageSource instance based on the buffer in incremental.
   *
   * @param { ArrayBuffer } buf The buffer of the image.
   * @param { SourceOptions } options The config of source.
   * @returns { ImageSource } Returns the ImageSource instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  function CreateIncrementalSource(buf: ArrayBuffer, options?: SourceOptions): ImageSource;

  /**
   * Creates an ImagePacker instance.
   *
   * @returns { ImagePacker } Returns the ImagePacker instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   * @since 6
   */
  /**
   * Creates an ImagePacker instance.
   *
   * @returns { ImagePacker } Returns the ImagePacker instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   * @crossplatform
   * @since 10
   */
  function createImagePacker(): ImagePacker;

  /**
   * Creates an ImageReceiver instance.
   *
   * @param { number } width The default width in pixels of the Images that this receiver will produce.
   * @param { number } height The default height in pixels of the Images that this receiver will produce.
   * @param { number } format The format of the Image that this receiver will produce. This must be one of the
   *            {@link ImageFormat} constants.
   * @param { number } capacity The maximum number of images the user will want to access simultaneously.
   * @returns { ImageReceiver } Returns the ImageReceiver instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageReceiver
   * @since 9
   */
  function createImageReceiver(width: number, height: number, format: number, capacity: number): ImageReceiver;

  /**
   * Creates an ImageCreator instance.
   *
   * @param { number } width The default width in pixels of the Images that this creator will produce.
   * @param { number } height The default height in pixels of the Images that this creator will produce.
   * @param { number } format The format of the Image that this creator will produce. This must be one of the
   *            {@link ImageFormat} constants.
   * @param { number } capacity The maximum number of images the user will want to access simultaneously.
   * @returns { ImageCreator } Returns the ImageCreator instance if the operation is successful; returns null otherwise.
   * @syscap SystemCapability.Multimedia.Image.ImageCreator
   * @since 9
   */
  function createImageCreator(width: number, height: number, format: number, capacity: number): ImageCreator;

  /**
   * PixelMap instance.
   *
   * @typedef PixelMap
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 7
   */
  /**
   * PixelMap instance.
   *
   * @typedef PixelMap
   * @syscap SystemCapability.Multimedia.Image.Core
   * @crossplatform
   * @since 10
   */
  interface PixelMap {
    /**
     * Whether the image pixel map can be edited.
     *
     * @type { boolean }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Whether the image pixel map can be edited.
     *
     * @type { boolean }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    readonly isEditable: boolean;

    /**
     * Reads image pixel map data and writes the data to an ArrayBuffer. This method uses
     * a promise to return the result.
     *
     * @param { ArrayBuffer } dst A buffer to which the image pixel map data will be written.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Reads image pixel map data and writes the data to an ArrayBuffer. This method uses
     * a promise to return the result.
     *
     * @param { ArrayBuffer } dst A buffer to which the image pixel map data will be written.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    readPixelsToBuffer(dst: ArrayBuffer): Promise<void>;

    /**
     * Reads image pixel map data and writes the data to an ArrayBuffer. This method uses
     * a callback to return the result.
     *
     * @param { ArrayBuffer } dst A buffer to which the image pixel map data will be written.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Reads image pixel map data and writes the data to an ArrayBuffer. This method uses
     * a callback to return the result.
     *
     * @param { ArrayBuffer } dst A buffer to which the image pixel map data will be written.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    readPixelsToBuffer(dst: ArrayBuffer, callback: AsyncCallback<void>): void;

    /**
     * Reads image pixel map data in an area. This method uses a promise to return the data read.
     *
     * @param { PositionArea } area Area from which the image pixel map data will be read.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Reads image pixel map data in an area. This method uses a promise to return the data read.
     *
     * @param { PositionArea } area Area from which the image pixel map data will be read.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    readPixels(area: PositionArea): Promise<void>;

    /**
     * Reads image pixel map data in an area. This method uses a callback to return the data read.
     *
     * @param { PositionArea } area Area from which the image pixel map data will be read.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Reads image pixel map data in an area. This method uses a callback to return the data read.
     *
     * @param { PositionArea } area Area from which the image pixel map data will be read.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    readPixels(area: PositionArea, callback: AsyncCallback<void>): void;

    /**
     * Writes image pixel map data to the specified area. This method uses a promise to return
     * the operation result.
     *
     * @param { PositionArea } area Area to which the image pixel map data will be written.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Writes image pixel map data to the specified area. This method uses a promise to return
     * the operation result.
     *
     * @param { PositionArea } area Area to which the image pixel map data will be written.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    writePixels(area: PositionArea): Promise<void>;

    /**
     * Writes image pixel map data to the specified area. This method uses a callback to return
     * the operation result.
     *
     * @param { PositionArea } area Area to which the image pixel map data will be written.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Writes image pixel map data to the specified area. This method uses a callback to return
     * the operation result.
     *
     * @param { PositionArea } area Area to which the image pixel map data will be written.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    writePixels(area: PositionArea, callback: AsyncCallback<void>): void;

    /**
     * Reads image data in an ArrayBuffer and writes the data to a PixelMap object. This method
     * uses a promise to return the result.
     *
     * @param { ArrayBuffer } src A buffer from which the image data will be read.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Reads image data in an ArrayBuffer and writes the data to a PixelMap object. This method
     * uses a promise to return the result.
     *
     * @param { ArrayBuffer } src A buffer from which the image data will be read.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    writeBufferToPixels(src: ArrayBuffer): Promise<void>;

    /**
     * Reads image data in an ArrayBuffer and writes the data to a PixelMap object. This method
     * uses a callback to return the result.
     *
     * @param { ArrayBuffer } src A buffer from which the image data will be read.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Reads image data in an ArrayBuffer and writes the data to a PixelMap object. This method
     * uses a callback to return the result.
     *
     * @param { ArrayBuffer } src A buffer from which the image data will be read.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    writeBufferToPixels(src: ArrayBuffer, callback: AsyncCallback<void>): void;

    /**
     * Obtains pixel map information about this image. This method uses a promise to return the information.
     *
     * @returns { Promise<ImageInfo> } A Promise instance used to return the image pixel map information. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Obtains pixel map information about this image. This method uses a promise to return the information.
     *
     * @returns { Promise<ImageInfo> } A Promise instance used to return the image pixel map information. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    getImageInfo(): Promise<ImageInfo>;

    /**
     * Obtains pixel map information about this image. This method uses a callback to return the information.
     *
     * @param { AsyncCallback<ImageInfo> } callback Callback used to return the image pixel map information.
     * If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Obtains pixel map information about this image. This method uses a callback to return the information.
     *
     * @param { AsyncCallback<ImageInfo> } callback Callback used to return the image pixel map information.
     * If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    getImageInfo(callback: AsyncCallback<ImageInfo>): void;

    /**
     * Obtains the number of bytes in each line of the image pixel map.
     *
     * @returns { number } Number of bytes in each line.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Obtains the number of bytes in each line of the image pixel map.
     *
     * @returns { number } Number of bytes in each line.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    getBytesNumberPerRow(): number;

    /**
     * Obtains the total number of bytes of the image pixel map.
     *
     * @returns { number } Total number of bytes.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Obtains the total number of bytes of the image pixel map.
     *
     * @returns { number } Total number of bytes.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    getPixelBytesNumber(): number;

    /**
     * Obtains the density of the image pixel map.
     *
     * @returns { number } The number of density.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Obtains the density of the image pixel map.
     *
     * @returns { number } The number of density.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    getDensity(): number;

    /**
     * Set the transparent rate of pixel map. This method uses a callback to return the operation result.
     *
     * @param { number } rate The value of transparent rate.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Set the transparent rate of pixel map. This method uses a callback to return the operation result.
     *
     * @param { number } rate The value of transparent rate.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    opacity(rate: number, callback: AsyncCallback<void>): void;

    /**
     * Set the transparent rate of pixel map. This method uses a promise to return the result.
     *
     * @param { number } rate The value of transparent rate.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Set the transparent rate of pixel map. This method uses a promise to return the result.
     *
     * @param { number } rate The value of transparent rate.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    opacity(rate: number): Promise<void>;

    /**
     * Obtains new pixel map with alpha information. This method uses a promise to return the information.
     *
     * @returns { Promise<PixelMap> } A Promise instance used to return the new image pixel map. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Obtains new pixel map with alpha information. This method uses a promise to return the information.
     *
     * @returns { Promise<PixelMap> } A Promise instance used to return the new image pixel map. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    createAlphaPixelmap(): Promise<PixelMap>;

    /**
     * Obtains new pixel map with alpha information. This method uses a callback to return the information.
     *
     * @param { AsyncCallback<PixelMap> } callback Callback used to return the new image pixel map. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Obtains new pixel map with alpha information. This method uses a callback to return the information.
     *
     * @param { AsyncCallback<PixelMap> } callback Callback used to return the new image pixel map. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    createAlphaPixelmap(callback: AsyncCallback<PixelMap>): void;

    /**
     * Image zoom in width and height. This method uses a callback to return the operation result.
     *
     * @param { number } x The zoom value of width.
     * @param { number } y The zoom value of height.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image zoom in width and height. This method uses a callback to return the operation result.
     *
     * @param { number } x The zoom value of width.
     * @param { number } y The zoom value of height.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    scale(x: number, y: number, callback: AsyncCallback<void>): void;

    /**
     * Image zoom in width and height. This method uses a promise to return the result.
     *
     * @param { number } x The zoom value of width.
     * @param { number } y The zoom value of height.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image zoom in width and height. This method uses a promise to return the result.
     *
     * @param { number } x The zoom value of width.
     * @param { number } y The zoom value of height.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    scale(x: number, y: number): Promise<void>;

    /**
     * Image position transformation. This method uses a callback to return the operation result.
     *
     * @param { number } x The position value of width.
     * @param { number } y The position value of height.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image position transformation. This method uses a callback to return the operation result.
     *
     * @param { number } x The position value of width.
     * @param { number } y The position value of height.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    translate(x: number, y: number, callback: AsyncCallback<void>): void;

    /**
     * Image position transformation. This method uses a promise to return the result.
     *
     * @param { number } x The position value of width.
     * @param { number } y The position value of height.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image position transformation. This method uses a promise to return the result.
     *
     * @param { number } x The position value of width.
     * @param { number } y The position value of height.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    translate(x: number, y: number): Promise<void>;

    /**
     * Image rotation. This method uses a callback to return the operation result.
     *
     * @param { number } angle The rotation angle.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image rotation. This method uses a callback to return the operation result.
     *
     * @param { number } angle The rotation angle.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    rotate(angle: number, callback: AsyncCallback<void>): void;

    /**
     * Image rotation. This method uses a promise to return the result.
     *
     * @param { number } angle The rotation angle.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image rotation. This method uses a promise to return the result.
     *
     * @param { number } angle The rotation angle.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    rotate(angle: number): Promise<void>;

    /**
     * Image flipping. This method uses a callback to return the operation result.
     *
     * @param { boolean } horizontal Is flip in horizontal.
     * @param { boolean } vertical Is flip in vertical.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image flipping. This method uses a callback to return the operation result.
     *
     * @param { boolean } horizontal Is flip in horizontal.
     * @param { boolean } vertical Is flip in vertical.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    flip(horizontal: boolean, vertical: boolean, callback: AsyncCallback<void>): void;

    /**
     * Image flipping. This method uses a promise to return the result.
     *
     * @param { boolean } horizontal Is flip in horizontal.
     * @param { boolean } vertical Is flip in vertical.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Image flipping. This method uses a promise to return the result.
     *
     * @param { boolean } horizontal Is flip in horizontal.
     * @param { boolean } vertical Is flip in vertical.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    flip(horizontal: boolean, vertical: boolean): Promise<void>;

    /**
     * Crop the image. This method uses a callback to return the operation result.
     *
     * @param { Region } region The region to crop.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Crop the image. This method uses a callback to return the operation result.
     *
     * @param { Region } region The region to crop.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    crop(region: Region, callback: AsyncCallback<void>): void;

    /**
     * Crop the image. This method uses a promise to return the result.
     *
     * @param { Region } region The region to crop.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * Crop the image. This method uses a promise to return the result.
     *
     * @param { Region } region The region to crop.
     * @returns { Promise<void> } A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    crop(region: Region): Promise<void>;

    /**
     * Get color space of pixel map.
     *
     * @returns { colorSpaceManager.ColorSpaceManager } If the operation fails, an error message is returned.
     * @throws { BusinessError } 62980101 - If the image data abnormal.
     * @throws { BusinessError } 62980103 - If the image data unsupport.
     * @throws { BusinessError } 62980115 - If the image parameter invalid.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    getColorSpace(): colorSpaceManager.ColorSpaceManager;

    /**
     * Marshalling pixelmap and write into MessageSequence.
     *
     * @param { rpc.MessageSequence } sequence rpc.MessageSequence parameter.
     * @throws { BusinessError } 62980115 - If the input parameter invalid.
     * @throws { BusinessError } 62980097 - If the ipc error.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    marshalling(sequence: rpc.MessageSequence): void;

    /**
     * Creates a PixelMap object based on MessageSequence parameter.
     *
     * @param { rpc.MessageSequence } sequence rpc.MessageSequence parameter.
     * @returns { Promise<PixelMap> } A Promise instance used to return the PixelMap object.
     * @throws { BusinessError } 62980115 - If the input parameter invalid.
     * @throws { BusinessError } 62980097 - If the ipc error.
     * @throws { BusinessError } 62980096 - If fail to create async work.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    unmarshalling(sequence: rpc.MessageSequence): Promise<PixelMap>;

    /**
     * Set color space of pixel map.
     *
     * @param { colorSpaceManager.ColorSpaceManager } colorSpace The color space for pixel map.
     * @throws { BusinessError } 62980111 - If the operation invalid.
     * @throws { BusinessError } 62980115 - If the image parameter invalid.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 10
     */
    setColorSpace(colorSpace: colorSpaceManager.ColorSpaceManager): void;

    /**
     * Releases this PixelMap object. This method uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback invoked for instance release. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Releases this PixelMap object. This method uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback invoked for instance release. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases this PixelMap object. This method uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the instance release result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 7
     */
    /**
     * Releases this PixelMap object. This method uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the instance release result. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 10
     */
    release(): Promise<void>;
  }

  /**
   * ImageSource instance.
   *
   * @typedef ImageSource
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @since 6
   */
  /**
   * ImageSource instance.
   *
   * @typedef ImageSource
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @crossplatform
   * @since 10
   */
  interface ImageSource {
    /**
     * Obtains information about an image with the specified sequence number and uses a callback
     * to return the result.
     *
     * @param { number } index Sequence number of an image.
     * @param { AsyncCallback<ImageInfo> } callback Callback used to return the image information.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 6
     */
    /**
     * Obtains information about an image with the specified sequence number and uses a callback
     * to return the result.
     *
     * @param { number } index Sequence number of an image.
     * @param { AsyncCallback<ImageInfo> } callback Callback used to return the image information.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getImageInfo(index: number, callback: AsyncCallback<ImageInfo>): void;

    /**
     * Obtains information about this image and uses a callback to return the result.
     *
     * @param { AsyncCallback<ImageInfo> } callback Callback used to return the image information.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 6
     */
    /**
     * Obtains information about this image and uses a callback to return the result.
     *
     * @param { AsyncCallback<ImageInfo> } callback Callback used to return the image information.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getImageInfo(callback: AsyncCallback<ImageInfo>): void;

    /**
     * Get image information from image source.
     *
     * @param { number } index Sequence number of an image. If this parameter is not specified, the default value 0 is used.
     * @returns { Promise<ImageInfo> } A Promise instance used to return the image information.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 6
     */
    /**
     * Get image information from image source.
     *
     * @param { number } index Sequence number of an image. If this parameter is not specified, the default value 0 is used.
     * @returns { Promise<ImageInfo> } A Promise instance used to return the image information.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getImageInfo(index?: number): Promise<ImageInfo>;

    /**
     * Creates a PixelMap object based on image decoding parameters. This method uses a promise to
     * return the object.
     *
     * @param { DecodingOptions } options Image decoding parameters.
     * @returns { Promise<PixelMap> } A Promise instance used to return the PixelMap object.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Creates a PixelMap object based on image decoding parameters. This method uses a promise to
     * return the object.
     *
     * @param { DecodingOptions } options Image decoding parameters.
     * @returns { Promise<PixelMap> } A Promise instance used to return the PixelMap object.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    createPixelMap(options?: DecodingOptions): Promise<PixelMap>;

    /**
     * Creates a PixelMap object. This method uses a callback to return the object.
     *
     * @param { AsyncCallback<PixelMap> } callback Callback used to return the PixelMap object.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Creates a PixelMap object. This method uses a callback to return the object.
     *
     * @param { AsyncCallback<PixelMap> } callback Callback used to return the PixelMap object.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    createPixelMap(callback: AsyncCallback<PixelMap>): void;

    /**
     * Creates a PixelMap object based on image decoding parameters. This method uses a callback to
     * return the object.
     *
     * @param { DecodingOptions } options Image decoding parameters.
     * @param { AsyncCallback<PixelMap> } callback Callback used to return the PixelMap object.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Creates a PixelMap object based on image decoding parameters. This method uses a callback to
     * return the object.
     *
     * @param { DecodingOptions } options Image decoding parameters.
     * @param { AsyncCallback<PixelMap> } callback Callback used to return the PixelMap object.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    createPixelMap(options: DecodingOptions, callback: AsyncCallback<PixelMap>): void;

    /**
     * Creates a PixelMap array based on image decoding parameters. This method uses a promise to
     * return the array.
     *
     * @param { DecodingOptions } options Image decoding parameters.
     * @returns { Promise<Array<PixelMap>> } A Promise instance used to return the PixelMap array.
     * @throws { BusinessError } 62980096 - If the operation failed.
     * @throws { BusinessError } 62980103 - If the image data unsupport.
     * @throws { BusinessError } 62980110 - If the image source data error.
     * @throws { BusinessError } 62980111 - If the image source data incomplete.
     * @throws { BusinessError } 62980118 - If the image plugin create failed.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    createPixelMapList(options?: DecodingOptions): Promise<Array<PixelMap>>;

    /**
     * Creates a PixelMap array. This method uses a callback to return the array.
     *
     * @param { AsyncCallback<Array<PixelMap>> } callback Callback used to return the PixelMap array.
     * @throws { BusinessError } 62980096 - If the operation failed.
     * @throws { BusinessError } 62980103 - If the image data unsupport.
     * @throws { BusinessError } 62980110 - If the image source data error.
     * @throws { BusinessError } 62980111 - If the image source data incomplete.
     * @throws { BusinessError } 62980118 - If the image plugin create failed.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    createPixelMapList(callback: AsyncCallback<Array<PixelMap>>): void;

    /**
     * Creates a PixelMap array based on image decoding parameters. This method uses a callback to
     * return the array.
     *
     * @param { DecodingOptions } options Image decoding parameters.
     * @param { AsyncCallback<Array<PixelMap>> } callback Callback used to return the PixelMap array.
     * @throws { BusinessError } 62980096 - If the operation failed.
     * @throws { BusinessError } 62980103 - If the image data unsupport.
     * @throws { BusinessError } 62980110 - If the image source data error.
     * @throws { BusinessError } 62980111 - If the image source data incomplete.
     * @throws { BusinessError } 62980118 - If the image plugin create failed.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    createPixelMapList(options: DecodingOptions, callback: AsyncCallback<Array<PixelMap>>): void;

    /**
     * Obtains the array of delay time in an image. This method uses a promise to return the array.
     *
     * @returns { Promise<Array<number>> } A Promise instance used to return the array.
     * @throws { BusinessError } 62980096 - If the operation failed.
     * @throws { BusinessError } 62980110 - If the image source data error.
     * @throws { BusinessError } 62980111 - If the image source data incomplete.
     * @throws { BusinessError } 62980113 - If the image format unknown.
     * @throws { BusinessError } 62980116 - If the image decode failed.
     * @throws { BusinessError } 62980118 - If the image plugin create failed.
     * @throws { BusinessError } 62980122 - If the image decode head abnormal.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getDelayTimeList(): Promise<Array<number>>;

    /**
     * Obtains the array of delay time in an image. This method uses a callback to to return the array.
     *
     * @param { AsyncCallback<Array<number>> } callback Callback used to return the array.
     * @throws { BusinessError } 62980096 - If the operation failed.
     * @throws { BusinessError } 62980110 - If the image source data error.
     * @throws { BusinessError } 62980111 - If the image source data incomplete.
     * @throws { BusinessError } 62980113 - If the image format unknown.
     * @throws { BusinessError } 62980116 - If the image decode failed.
     * @throws { BusinessError } 62980118 - If the image plugin create failed.
     * @throws { BusinessError } 62980122 - If the image decode head abnormal.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getDelayTimeList(callback: AsyncCallback<Array<number>>): void;

    /**
     * Obtains the count of frame in an image. This method uses a promise to return the number.
     *
     * @returns { Promise<number> } A Promise instance used to return the number.
     * @throws { BusinessError } 62980096 - If the operation failed.
     * @throws { BusinessError } 62980110 - If the image source data error.
     * @throws { BusinessError } 62980111 - If the image source data incomplete.
     * @throws { BusinessError } 62980113 - If the image format unknown.
     * @throws { BusinessError } 62980116 - If the image decode failed.
     * @throws { BusinessError } 62980118 - If the image plugin create failed.
     * @throws { BusinessError } 62980122 - If the image decode head abnormal.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getFrameCount(): Promise<number>;

    /**
     * Obtains the count of frame in an image. This method uses a callback to to return the number.
     *
     * @param { AsyncCallback<number> } callback Callback used to return the number.
     * @throws { BusinessError } 62980096 - If the operation failed.
     * @throws { BusinessError } 62980110 - If the image source data error.
     * @throws { BusinessError } 62980111 - If the image source data incomplete.
     * @throws { BusinessError } 62980113 - If the image format unknown.
     * @throws { BusinessError } 62980116 - If the image decode failed.
     * @throws { BusinessError } 62980118 - If the image plugin create failed.
     * @throws { BusinessError } 62980122 - If the image decode head abnormal.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getFrameCount(callback: AsyncCallback<number>): void;

    /**
     * Obtains the value of a property in an image with the specified index. This method uses a
     * promise to return the property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { GetImagePropertyOptions } options Index of the image.
     * @returns { Promise<string> } A Promise instance used to return the property value. If the operation fails, the default value is returned.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Obtains the value of a property in an image with the specified index. This method uses a
     * promise to return the property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { GetImagePropertyOptions } options Index of the image.
     * @returns { Promise<string> } A Promise instance used to return the property value. If the operation fails, the default value is returned.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getImageProperty(key: string, options?: GetImagePropertyOptions): Promise<string>;

    /**
     * Obtains the value of a property in this image. This method uses a callback to return the
     * property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { AsyncCallback<string> } callback Callback used to return the property value. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Obtains the value of a property in this image. This method uses a callback to return the
     * property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { AsyncCallback<string> } callback Callback used to return the property value. If the operation fails, an error message is returned.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getImageProperty(key: string, callback: AsyncCallback<string>): void;

    /**
     * Obtains the value of a property in an image with the specified index. This method uses
     * a callback to return the property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { GetImagePropertyOptions } options Index of the image.
     * @param { AsyncCallback<string> } callback Callback used to return the property value. If the operation fails, the default value is returned.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 7
     */
    /**
     * Obtains the value of a property in an image with the specified index. This method uses
     * a callback to return the property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { GetImagePropertyOptions } options Index of the image.
     * @param { AsyncCallback<string> } callback Callback used to return the property value. If the operation fails, the default value is returned.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    getImageProperty(key: string, options: GetImagePropertyOptions, callback: AsyncCallback<string>): void;

    /**
     * Modify the value of a property in an image with the specified key. This method uses a
     * promise to return the property value in a string.
     *
     * @param { string } key Name of the property whose value is to be modified.
     * @param { string } value The value to be set to property.
     * @returns { Promise<void> } A Promise instance used to return the property value.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 9
     */
    /**
     * Modify the value of a property in an image with the specified key. This method uses a
     * promise to return the property value in a string.
     *
     * @param { string } key Name of the property whose value is to be modified.
     * @param { string } value The value to be set to property.
     * @returns { Promise<void> } A Promise instance used to return the property value.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    modifyImageProperty(key: string, value: string): Promise<void>;

    /**
     * Modify the value of a property in an image with the specified key. This method uses a callback to return the
     * property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { string } value The value to be set to property.
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 9
     */
    /**
     * Modify the value of a property in an image with the specified key. This method uses a callback to return the
     * property value in a string.
     *
     * @param { string } key Name of the property whose value is to be obtained.
     * @param { string } value The value to be set to property.
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    modifyImageProperty(key: string, value: string, callback: AsyncCallback<void>): void;

    /**
     * Update the data in the incremental ImageSource.
     *
     * @param { ArrayBuffer } buf The data to be updated.
     * @param { boolean } isFinished If is it finished.
     * @param { number } value The offset of data.
     * @param { number } length The length fo buf.
     * @returns { Promise<void> } A Promise instance used to return the property value.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 9
     */
    /**
     * Update the data in the incremental ImageSource.
     *
     * @param { ArrayBuffer } buf The data to be updated.
     * @param { boolean } isFinished If is it finished.
     * @param { number } value The offset of data.
     * @param { number } length The length fo buf.
     * @returns { Promise<void> } A Promise instance used to return the property value.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    updateData(buf: ArrayBuffer, isFinished: boolean, value: number, length: number): Promise<void>;

    /**
     * Update the data in the incremental ImageSource.
     *
     * @param { ArrayBuffer } buf The data to be updated.
     * @param { boolean } isFinished If is it finished.
     * @param { number } value The offset of data.
     * @param { number } length The length fo buf.
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 9
     */
    /**
     * Update the data in the incremental ImageSource.
     *
     * @param { ArrayBuffer } buf The data to be updated.
     * @param { boolean } isFinished If is it finished.
     * @param { number } value The offset of data.
     * @param { number } length The length fo buf.
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    updateData(
      buf: ArrayBuffer,
      isFinished: boolean,
      value: number,
      length: number,
      callback: AsyncCallback<void>
    ): void;

    /**
     * Releases an ImageSource instance and uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 6
     */
    /**
     * Releases an ImageSource instance and uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases an ImageSource instance and uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 6
     */
    /**
     * Releases an ImageSource instance and uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    release(): Promise<void>;

    /**
     * Supported image formats.
     *
     * @type { Array<string> }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @since 6
     */
    /**
     * Supported image formats.
     *
     * @type { Array<string> }
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @crossplatform
     * @since 10
     */
    readonly supportedFormats: Array<string>;
  }

  /**
   * ImagePacker instance.
   *
   * @typedef ImagePacker
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   * @since 6
   */
  /**
   * ImagePacker instance.
   *
   * @typedef ImagePacker
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   * @crossplatform
   * @since 10
   */
  interface ImagePacker {
    /**
     * Compresses or packs an image and uses a callback to return the result.
     *
     * @param { ImageSource } source Image to be processed.
     * @param { PackingOption } option Option for image packing.
     * @param { AsyncCallback<ArrayBuffer> } callback Callback used to return the packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 6
     */
    /**
     * Compresses or packs an image and uses a callback to return the result.
     *
     * @param { ImageSource } source Image to be processed.
     * @param { PackingOption } option Option for image packing.
     * @param { AsyncCallback<ArrayBuffer> } callback Callback used to return the packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    packing(source: ImageSource, option: PackingOption, callback: AsyncCallback<ArrayBuffer>): void;

    /**
     * Compresses or packs an image and uses a promise to return the result.
     *
     * @param { ImageSource } source Image to be processed.
     * @param { PackingOption } option Option for image packing.
     * @returns { Promise<ArrayBuffer> } A Promise instance used to return the compressed or packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 6
     */
    /**
     * Compresses or packs an image and uses a promise to return the result.
     *
     * @param { ImageSource } source Image to be processed.
     * @param { PackingOption } option Option for image packing.
     * @returns { Promise<ArrayBuffer> } A Promise instance used to return the compressed or packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    packing(source: ImageSource, option: PackingOption): Promise<ArrayBuffer>;

    /**
     * Compresses or packs an image and uses a callback to return the result.
     *
     * @param { PixelMap } source PixelMap to be processed.
     * @param { PackingOption } option Option for image packing.
     * @param { AsyncCallback<ArrayBuffer> } callback Callback used to return the packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 8
     */
    /**
     * Compresses or packs an image and uses a callback to return the result.
     *
     * @param { PixelMap } source PixelMap to be processed.
     * @param { PackingOption } option Option for image packing.
     * @param { AsyncCallback<ArrayBuffer> } callback Callback used to return the packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    packing(source: PixelMap, option: PackingOption, callback: AsyncCallback<ArrayBuffer>): void;

    /**
     * Compresses or packs an image and uses a promise to return the result.
     *
     * @param { PixelMap } source PixelMap to be processed.
     * @param { PackingOption } option Option for image packing.
     * @returns { Promise<ArrayBuffer> } A Promise instance used to return the compressed or packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 8
     */
    /**
     * Compresses or packs an image and uses a promise to return the result.
     *
     * @param { PixelMap } source PixelMap to be processed.
     * @param { PackingOption } option Option for image packing.
     * @returns { Promise<ArrayBuffer> } A Promise instance used to return the compressed or packed data.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    packing(source: PixelMap, option: PackingOption): Promise<ArrayBuffer>;

    /**
     * Compresses or packs an image into a file and uses a callback to return the result.
     *
     * @param { ImageSource } source Image to be processed.
     * @param { number } fd ID of a file descriptor.
     * @param { PackingOption } options Options for image packing.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 11
     */
    packToFile(source: ImageSource, fd: number, options: PackingOption, callback: AsyncCallback<void>): void;

    /**
     * Compresses or packs an image into a file and uses a promise to return the result.
     *
     * @param { ImageSource } source Image to be processed.
     * @param { number } fd ID of a file descriptor.
     * @param { PackingOption } options Options for image packing.
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 11
     */
    packToFile(source: ImageSource, fd: number, options: PackingOption): Promise<void>;

    /**
     * Compresses or packs an image into a file and uses a callback to return the result.
     *
     * @param { PixelMap } source PixelMap to be processed.
     * @param { number } fd ID of a file descriptor.
     * @param { PackingOption } options Options for image packing.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 11
     */
    packToFile(source: PixelMap, fd: number, options: PackingOption, callback: AsyncCallback<void>): void;

    /**
     * Compresses or packs an image into a file and uses a promise to return the result.
     *
     * @param { PixelMap } source PixelMap to be processed.
     * @param { number } fd ID of a file descriptor.
     * @param { PackingOption } options Options for image packing.
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 11
     */
    packToFile(source: PixelMap, fd: number, options: PackingOption): Promise<void>;

     /**
     * Releases an ImagePacker instance and uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 6
     */
    /**
     * Releases an ImagePacker instance and uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases an ImagePacker instance and uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 6
     */
    /**
     * Releases an ImagePacker instance and uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    release(): Promise<void>;

    /**
     * Supported image formats.
     *
     * @type { Array<string> }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @since 6
     */
    /**
     * Supported image formats.
     *
     * @type { Array<string> }
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @crossplatform
     * @since 10
     */
    readonly supportedFormats: Array<string>;
  }

  /**
   * Provides basic image operations, including obtaining image information, and reading and writing image data.
   *
   * @typedef Image
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 9
   */
  interface Image {
    /**
     * Sets or gets the image area to crop, default is size.
     *
     * @type { Region }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    clipRect: Region;

    /**
     * Image size.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    readonly size: Size;

    /**
     * Image format.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    readonly format: number;

    /**
     * Get component buffer from image and uses a callback to return the result.
     *
     * @param { ComponentType } componentType The component type of image.
     * @param { AsyncCallback<Component> } callback Callback used to return the component buffer.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    getComponent(componentType: ComponentType, callback: AsyncCallback<Component>): void;

    /**
     * Get component buffer from image and uses a promise to return the result.
     *
     * @param { ComponentType } componentType The component type of image.
     * @returns { Promise<Component> } A Promise instance used to return the component buffer.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    getComponent(componentType: ComponentType): Promise<Component>;

    /**
     * Release current image to receive another and uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release current image to receive another and uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    release(): Promise<void>;
  }

  /**
   * Image receiver object.
   *
   * @typedef ImageReceiver
   * @syscap SystemCapability.Multimedia.Image.ImageReceiver
   * @since 9
   */
  interface ImageReceiver {
    /**
     * Image size.
     *
     * @type { Size }
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    readonly size: Size;

    /**
     * Image capacity.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    readonly capacity: number;

    /**
     * Image format.
     *
     * @type { ImageFormat }
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    readonly format: ImageFormat;

    /**
     * Get an id which indicates a surface and can be used to set to Camera or other component can receive a surface
     * and uses a callback to return the result.
     *
     * @param { AsyncCallback<string> } callback Callback used to return the surface id.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    getReceivingSurfaceId(callback: AsyncCallback<string>): void;

    /**
     * Get an id which indicates a surface and can be used to set to Camera or other component can receive a surface
     * and uses a promise to return the result.
     *
     * @returns { Promise<string> } A Promise instance used to return the surface id.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    getReceivingSurfaceId(): Promise<string>;

    /**
     * Get lasted image from receiver and uses a callback to return the result.
     *
     * @param { AsyncCallback<Image> } callback Callback used to return the latest image.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    readLatestImage(callback: AsyncCallback<Image>): void;

    /**
     * Get lasted image from receiver and uses a promise to return the result.
     *
     * @returns { Promise<Image> } A Promise instance used to return the latest image.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    readLatestImage(): Promise<Image>;

    /**
     * Get next image from receiver and uses a callback to return the result.
     *
     * @param { AsyncCallback<Image> } callback Callback used to return the next image.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    readNextImage(callback: AsyncCallback<Image>): void;

    /**
     * Get next image from receiver and uses a promise to return the result.
     *
     * @returns { Promise<Image> } A Promise instance used to return the next image.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    readNextImage(): Promise<Image>;

    /**
     * Subscribe callback when receiving an image
     *
     * @param { 'imageArrival' } type Callback used to return the next image.
     * @param { AsyncCallback<void> } callback Callback used to return image.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    on(type: 'imageArrival', callback: AsyncCallback<void>): void;

    /**
     * Release image receiver instance and uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release image receiver instance and uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @since 9
     */
    release(): Promise<void>;
  }

  /**
   * Image creator object.
   *
   * @typedef ImageCreator
   * @syscap SystemCapability.Multimedia.Image.ImageCreator
   * @since 9
   */
  interface ImageCreator {
    /**
     * Image capacity.
     *
     * @type { number }
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    readonly capacity: number;

    /**
     * Image format.
     *
     * @type { ImageFormat }
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    readonly format: ImageFormat;

    /**
     * Apply for new graphic buffer from free queue and use a callback to return the result.
     *
     * @param { AsyncCallback<Image> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    dequeueImage(callback: AsyncCallback<Image>): void;

    /**
     * Apply for new graphic buffer from free queue and uses a promise to return the result.
     *
     * @returns { Promise<Image> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    dequeueImage(): Promise<Image>;

    /**
     * Queue buffer to dirty queue and uses a callback to return the result.
     *
     * @param { Image } interface
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    queueImage(interface: Image, callback: AsyncCallback<void>): void;

    /**
     * Queue buffer to dirty queue and uses a promise to return the result.
     *
     * @param { Image } interface
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    queueImage(interface: Image): Promise<void>;

    /**
     * Subscribe callback when releasing buffer
     *
     * @param { 'imageRelease' } type Callback used to return the operation result.
     * @param { AsyncCallback<void> } callback Callback used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    on(type: 'imageRelease', callback: AsyncCallback<void>): void;

    /**
     * Releases buffer in bufferqueue instance and uses a callback to return the result.
     *
     * @param { AsyncCallback<void> } callback Callback to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases buffer in bufferqueue instance and uses a promise to return the result.
     *
     * @returns { Promise<void> } A Promise instance used to return the operation result.
     * @syscap SystemCapability.Multimedia.Image.ImageCreator
     * @since 9
     */
    release(): Promise<void>;
  }
}

export default image;
