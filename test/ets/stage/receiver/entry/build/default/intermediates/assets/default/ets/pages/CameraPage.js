var _bedf604a4a2a5251c93f50fd33130eef;
/******/ (() => { // webpackBootstrap
/******/ 	var __webpack_modules__ = ({

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/CameraService.ts":
/*!*********************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/CameraService.ts ***!
  \*********************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

"use strict";

var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", ({ value: true }));
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
var _ohos_multimedia_camera_1  = globalThis.requireNapi('multimedia.camera') || (isSystemplugin('multimedia.camera', 'ohos') ? globalThis.ohosplugin.multimedia.camera : isSystemplugin('multimedia.camera', 'system') ? globalThis.systemplugin.multimedia.camera : undefined);
var _ohos_fileio_1  = globalThis.requireNapi('fileio') || (isSystemplugin('fileio', 'ohos') ? globalThis.ohosplugin.fileio : isSystemplugin('fileio', 'system') ? globalThis.systemplugin.fileio : undefined);
var _ohos_multimedia_image_1  = globalThis.requireNapi('multimedia.image') || (isSystemplugin('multimedia.image', 'ohos') ? globalThis.ohosplugin.multimedia.image : isSystemplugin('multimedia.image', 'system') ? globalThis.systemplugin.multimedia.image : undefined);
__webpack_require__(/*! @ohos.multimedia.media */ "../../api/@ohos.multimedia.media.d.ts");
var _ohos_multimedia_mediaLibrary_1  = globalThis.requireNapi('multimedia.mediaLibrary') || (isSystemplugin('multimedia.mediaLibrary', 'ohos') ? globalThis.ohosplugin.multimedia.mediaLibrary : isSystemplugin('multimedia.mediaLibrary', 'system') ? globalThis.systemplugin.multimedia.mediaLibrary : undefined);
const Logger_1 = __importDefault(__webpack_require__(/*! ../model/Logger */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts"));
const MediaUtils_1 = __importDefault(__webpack_require__(/*! ../model/MediaUtils */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/MediaUtils.ts"));
const CameraMode = {
    MODE_PHOTO: 0,
    MODE_VIDEO: 1 // 录像模式
};
const CameraSize = {
    WIDTH: 1920,
    HEIGHT: 1080
};
class CameraService {
    constructor() {
        this.tag = 'CameraService';
        this.mediaUtil = MediaUtils_1.default.getInstance();
        this.cameraManager = undefined;
        this.cameras = undefined;
        this.cameraId = '';
        this.cameraInput = undefined;
        this.previewOutput = undefined;
        this.photoOutPut = undefined;
        this.captureSession = undefined;
        this.mReceiver = undefined;
        this.photoUri = '';
        this.fileAsset = undefined;
        this.fd = -1;
        this.curMode = CameraMode.MODE_PHOTO;
        this.handleTakePicture = undefined;
        Logger_1.default.info(this.tag, 'q_y1 createImageReceiver');
        this.mReceiver = _ohos_multimedia_image_1.createImageReceiver(CameraSize.WIDTH, CameraSize.HEIGHT, 4, 8);
        Logger_1.default.info(this.tag, 'q_y1 createImageReceiver');
        this.mReceiver.on('imageArrival', () => {
            Logger_1.default.info(this.tag, 'q_y2 imageArrival');
            ////      从ImageReceiver读取最新的图片，并使用callback返回结果。 success
            //      this.mReceiver.readLatestImage((err, image) => {
            //        Logger.info(this.tag, 'q_y3 readLatestImage')
            //        if (err || image === undefined) {
            //          Logger.error(this.tag, 'q_y4 failed to get valid image')
            //          return
            //        }
            //        image.getComponent(4, (errMsg, img) => {
            //          Logger.info(this.tag, 'q_y5 getComponent')
            //          if (errMsg || img === undefined) {
            //            Logger.info(this.tag, 'q_y6 failed to get valid buffer')
            //            return
            //          }
            //          let buffer = new ArrayBuffer(4096)
            //          if (img.byteBuffer) {
            //            buffer = img.byteBuffer
            //          } else {
            //            Logger.error(this.tag, ' q_y7 img.byteBuffer is undefined')
            //          }
            //          this.savePicture(buffer, image)
            //        })
            //      })
            ////      从ImageReceiver读取最新的图片，并使用promise返回结果。 success
            //      this.mReceiver.readLatestImage().then(image => {
            //        Logger.info(this.tag, 'q_y8 readLatestImage')
            //        if (image === undefined) {
            //          Logger.error(this.tag, 'q_y9 failed to get valid image')
            //          return
            //        }
            //        image.getComponent(4, (errMsg, img) => {
            //          Logger.info(this.tag, 'q_y10 getComponent')
            //          if (errMsg || img === undefined) {
            //            Logger.info(this.tag, 'q_y11 failed to get valid buffer')
            //            return
            //          }
            //          let buffer = new ArrayBuffer(4096)
            //          if (img.byteBuffer) {
            //            buffer = img.byteBuffer
            //          } else {
            //            Logger.error(this.tag, ' q_y12 img.byteBuffer is undefined')
            //          }
            //          this.savePicture(buffer, image)
            //        })
            //      }).catch(error => {
            //        Logger.info('q_y13 readLatestImage failed.');
            //      })
            //      从ImageReceiver读取下一张图片，并使用callback返回结果。 success
            //      this.mReceiver.readNextImage((err, image) => {
            //        Logger.info(this.tag, 'q_y14 readNextImage')
            //        if (err || image === undefined) {
            //          Logger.error(this.tag, 'q_y15 failed to get valid image')
            //          return
            //        }
            //        image.getComponent(4, (errMsg, img) => {
            //          Logger.info(this.tag, 'q_y16 getComponent')
            //          if (errMsg || img === undefined) {
            //            Logger.info(this.tag, 'q_y17 failed to get valid buffer')
            //            return
            //          }
            //          let buffer = new ArrayBuffer(4096)
            //          if (img.byteBuffer) {
            //            buffer = img.byteBuffer
            //          } else {
            //            Logger.error(this.tag, ' q_y18 img.byteBuffer is undefined')
            //          }
            //          this.savePicture(buffer, image)
            //        })
            //      })
            //      从ImageReceiver读取下一张图片，并使用callback返回结果。
            this.mReceiver.readNextImage().then(image => {
                Logger_1.default.info(this.tag, 'q_y19 readNextImage');
                if (image === undefined) {
                    Logger_1.default.error(this.tag, 'q_y20 failed to get valid image');
                    return;
                }
                image.getComponent(4, (errMsg, img) => {
                    Logger_1.default.info(this.tag, 'q_y21 getComponent');
                    if (errMsg || img === undefined) {
                        Logger_1.default.info(this.tag, 'q_y22 failed to get valid buffer');
                        return;
                    }
                    let buffer = new ArrayBuffer(4096);
                    if (img.byteBuffer) {
                        buffer = img.byteBuffer;
                    }
                    else {
                        Logger_1.default.error(this.tag, ' q_y23 img.byteBuffer is undefined');
                    }
                    this.savePicture(buffer, image);
                });
            }).catch(error => {
                console.log('q_y24 readNextImage failed.');
            });
            //      用于获取一个surface id供Camera或其他组件使用。使用callback返回结果。 success
            this.mReceiver.getReceivingSurfaceId((err, id) => {
                if (err) {
                    Logger_1.default.error('q_y25 getReceivingSurfaceId failed.');
                }
                else {
                    Logger_1.default.info('q_y26 getReceivingSurfaceId succeeded and id is ' + id);
                }
            });
            //      //      用于获取一个surface id供Camera或其他组件使用。使用promise返回结果。 success
            //      this.mReceiver.getReceivingSurfaceId().then( id => {
            //        Logger.info('q_y27 getReceivingSurfaceId succeeded and id is ' + id)
            //      }).catch(error => {
            //        Logger.error('q_y28 getReceivingSurfaceId failed.')
            //      })
        });
        //    释放ImageReceiver实例并使用promise返回结果
        //    this.mReceiver.release().then(() => {
        //      Logger.info('q_y29 release succeeded.');
        //    }).catch(error => {
        //      Logger.error('q_y30 release failed.');
        //    })
        //    释放ImageReceiver实例并使用callback返回结果
        //    this.mReceiver.release(() => {
        //      Logger.info('q_y31 release succeeded.');
        //    })
    }
    async savePicture(buffer, img) {
        Logger_1.default.info(this.tag, 'savePicture');
        this.fileAsset = await this.mediaUtil.createAndGetUri(_ohos_multimedia_mediaLibrary_1.MediaType.IMAGE);
        this.photoUri = this.fileAsset.uri;
        Logger_1.default.info(this.tag, `this.photoUri = ${this.photoUri}`);
        this.fd = await this.mediaUtil.getFdPath(this.fileAsset);
        Logger_1.default.info(this.tag, `this.fd = ${this.fd}`);
        await _ohos_fileio_1.write(this.fd, buffer);
        await this.fileAsset.close(this.fd);
        await img.release();
        Logger_1.default.info(this.tag, 'save image done');
        if (this.handleTakePicture) {
            this.handleTakePicture(this.photoUri);
        }
    }
    async initCamera(surfaceId) {
        Logger_1.default.info(this.tag, 'initCamera');
        if (this.curMode === CameraMode.MODE_VIDEO) {
            await this.releaseCamera();
            this.curMode = CameraMode.MODE_PHOTO;
        }
        this.cameraManager = await _ohos_multimedia_camera_1.getCameraManager(globalThis.abilityContext);
        Logger_1.default.info(this.tag, 'getCameraManager');
        this.cameras = await this.cameraManager.getCameras();
        Logger_1.default.info(this.tag, `get cameras ${this.cameras.length}`);
        if (this.cameras.length === 0) {
            Logger_1.default.info(this.tag, 'cannot get cameras');
            return;
        }
        this.cameraId = this.cameras[0].cameraId;
        this.cameraInput = await this.cameraManager.createCameraInput(this.cameraId);
        Logger_1.default.info(this.tag, 'createCameraInput');
        this.previewOutput = await _ohos_multimedia_camera_1.createPreviewOutput(surfaceId.toString());
        Logger_1.default.info(this.tag, 'createPreviewOutput');
        let mSurfaceId = await this.mReceiver.getReceivingSurfaceId();
        this.photoOutPut = await _ohos_multimedia_camera_1.createPhotoOutput(mSurfaceId);
        this.captureSession = await _ohos_multimedia_camera_1.createCaptureSession(globalThis.abilityContext);
        Logger_1.default.info(this.tag, 'createCaptureSession');
        await this.captureSession.beginConfig();
        Logger_1.default.info(this.tag, 'beginConfig');
        await this.captureSession.addInput(this.cameraInput);
        await this.captureSession.addOutput(this.previewOutput);
        await this.captureSession.addOutput(this.photoOutPut);
        await this.captureSession.commitConfig();
        await this.captureSession.start();
        Logger_1.default.info(this.tag, 'captureSession start');
    }
    setTakePictureCallback(callback) {
        this.handleTakePicture = callback;
    }
    async takePicture() {
        Logger_1.default.info(this.tag, 'takePicture');
        if (this.curMode === CameraMode.MODE_VIDEO) {
            this.curMode = CameraMode.MODE_PHOTO;
        }
        let photoSettings = {
            rotation: _ohos_multimedia_camera_1.ImageRotation.ROTATION_0,
            quality: _ohos_multimedia_camera_1.QualityLevel.QUALITY_LEVEL_MEDIUM,
            location: {
                latitude: 12.9698,
                longitude: 77.7500,
                altitude: 1000
            },
            mirror: false
        };
        await this.photoOutPut.capture(photoSettings);
        Logger_1.default.info(this.tag, 'takePicture done');
    }
    async releaseCamera() {
        Logger_1.default.info(this.tag, 'releaseCamera');
        await this.captureSession.stop();
        if (this.cameraInput) {
            await this.cameraInput.release();
        }
        if (this.previewOutput) {
            await this.previewOutput.release();
        }
        if (this.photoOutPut) {
            await this.photoOutPut.release();
        }
        await this.cameraInput.release();
        await this.captureSession.release();
    }
}
CameraService.instance = new CameraService();
exports["default"] = new CameraService();


/***/ }),

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/DateTimeUtil.ts":
/*!********************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/DateTimeUtil.ts ***!
  \********************************************************************************************************************/
/***/ ((__unused_webpack_module, exports) => {

"use strict";

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
Object.defineProperty(exports, "__esModule", ({ value: true }));
/**
 * @file 日期工具
 */
class DateTimeUtil {
    /**
   * 时分秒
   */
    getTime() {
        const DATETIME = new Date();
        return this.concatTime(DATETIME.getHours(), DATETIME.getMinutes(), DATETIME.getSeconds());
    }
    /**
   * 年月日
   */
    getDate() {
        const DATETIME = new Date();
        return this.concatDate(DATETIME.getFullYear(), DATETIME.getMonth() + 1, DATETIME.getDate());
    }
    /**
   * 日期不足两位补充0
   * @param value-数据值
   */
    fill(value) {
        return (value > 9 ? '' : '0') + value;
    }
    /**
   * 年月日格式修饰
   * @param year
   * @param month
   * @param date
   */
    concatDate(year, month, date) {
        return `${year}${this.fill(month)}${this.fill(date)}`;
    }
    /**
   * 时分秒格式修饰
   * @param hours
   * @param minutes
   * @param seconds
   */
    concatTime(hours, minutes, seconds) {
        return `${this.fill(hours)}${this.fill(minutes)}${this.fill(seconds)}`;
    }
}
exports["default"] = DateTimeUtil;


/***/ }),

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts":
/*!**************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts ***!
  \**************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports) {

"use strict";

var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", ({ value: true }));
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
var _ohos_hilog_1  = globalThis.requireNapi('hilog') || (isSystemplugin('hilog', 'ohos') ? globalThis.ohosplugin.hilog : isSystemplugin('hilog', 'system') ? globalThis.systemplugin.hilog : undefined);
class Logger {
    constructor(prefix) {
        this.format = "%{public}s, %{public}s";
        this.prefix = prefix;
        this.domain = 0xFF00;
    }
    debug(...args) {
        _ohos_hilog_1.debug(this.domain, this.prefix, this.format, args);
    }
    info(...args) {
        _ohos_hilog_1.info(this.domain, this.prefix, this.format, args);
    }
    warn(...args) {
        _ohos_hilog_1.warn(this.domain, this.prefix, this.format, args);
    }
    error(...args) {
        _ohos_hilog_1.error(this.domain, this.prefix, this.format, args);
    }
}
exports["default"] = new Logger('[MultiMedia]');


/***/ }),

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/MediaUtils.ts":
/*!******************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/MediaUtils.ts ***!
  \******************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

"use strict";

var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", ({ value: true }));
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
var _ohos_multimedia_mediaLibrary_1  = globalThis.requireNapi('multimedia.mediaLibrary') || (isSystemplugin('multimedia.mediaLibrary', 'ohos') ? globalThis.ohosplugin.multimedia.mediaLibrary : isSystemplugin('multimedia.mediaLibrary', 'system') ? globalThis.systemplugin.multimedia.mediaLibrary : undefined);
const DateTimeUtil_1 = __importDefault(__webpack_require__(/*! ../model/DateTimeUtil */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/DateTimeUtil.ts"));
const Logger_1 = __importDefault(__webpack_require__(/*! ../model/Logger */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts"));
class MediaUtils {
    constructor() {
        this.tag = 'MediaUtils';
        this.mediaTest = _ohos_multimedia_mediaLibrary_1.getMediaLibrary(globalThis.abilityContext);
    }
    static getInstance() {
        if (this.instance === undefined) {
            this.instance = new MediaUtils();
        }
        return this.instance;
    }
    async createAndGetUri(mediaType) {
        let info = this.getInfoFromType(mediaType);
        let dateTimeUtil = new DateTimeUtil_1.default();
        let name = `${dateTimeUtil.getDate()}_${dateTimeUtil.getTime()}`;
        let displayName = `${info.prefix}${name}${info.suffix}`;
        Logger_1.default.info(this.tag, `displayName = ${displayName},mediaType = ${mediaType}`);
        let publicPath = await this.mediaTest.getPublicDirectory(info.directory);
        Logger_1.default.info(this.tag, `publicPath = ${publicPath}`);
        return await this.mediaTest.createAsset(mediaType, displayName, publicPath);
    }
    async queryFile(dataUri) {
        let fileKeyObj = _ohos_multimedia_mediaLibrary_1.FileKey;
        if (dataUri !== undefined) {
            let args = dataUri.id.toString();
            let fetchOp = {
                selections: `${fileKeyObj.ID}=?`,
                selectionArgs: [args],
            };
            const fetchFileResult = await this.mediaTest.getFileAssets(fetchOp);
            Logger_1.default.info(this.tag, `fetchFileResult.getCount() = ${fetchFileResult.getCount()}`);
            const fileAsset = await fetchFileResult.getAllObject();
            return fileAsset[0];
        }
    }
    async getFdPath(fileAsset) {
        let fd = await fileAsset.open('Rw');
        Logger_1.default.info(this.tag, `fd = ${fd}`);
        return fd;
    }
    async createFile(mediaType) {
        let dataUri = await this.createAndGetUri(mediaType);
        if (dataUri) {
            let fileAsset = await this.queryFile(dataUri);
            if (fileAsset) {
                let fd = await this.getFdPath(fileAsset);
                return fd;
            }
        }
    }
    async getFileAssetsFromType(mediaType) {
        Logger_1.default.info(this.tag, `getFileAssetsFromType,mediaType = ${mediaType}`);
        let fileKeyObj = _ohos_multimedia_mediaLibrary_1.FileKey;
        let fetchOp = {
            selections: `${fileKeyObj.MEDIA_TYPE}=?`,
            selectionArgs: [`${mediaType}`],
        };
        const fetchFileResult = await this.mediaTest.getFileAssets(fetchOp);
        Logger_1.default.info(this.tag, `getFileAssetsFromType,fetchFileResult.count = ${fetchFileResult.getCount()}`);
        let fileAssets = [];
        if (fetchFileResult.getCount() > 0) {
            fileAssets = await fetchFileResult.getAllObject();
        }
        return fileAssets;
    }
    async getAlbums() {
        Logger_1.default.info(this.tag, 'getAlbums begin');
        let albums = [];
        const [files, images, videos, audios] = await Promise.all([
            this.getFileAssetsFromType(_ohos_multimedia_mediaLibrary_1.MediaType.FILE),
            this.getFileAssetsFromType(_ohos_multimedia_mediaLibrary_1.MediaType.IMAGE),
            this.getFileAssetsFromType(_ohos_multimedia_mediaLibrary_1.MediaType.VIDEO),
            this.getFileAssetsFromType(_ohos_multimedia_mediaLibrary_1.MediaType.AUDIO)
        ]);
        albums.push({
            albumName: 'Documents', count: files.length, mediaType: _ohos_multimedia_mediaLibrary_1.MediaType.FILE
        });
        albums.push({
            albumName: 'Pictures', count: images.length, mediaType: _ohos_multimedia_mediaLibrary_1.MediaType.IMAGE
        });
        albums.push({
            albumName: 'Videos', count: videos.length, mediaType: _ohos_multimedia_mediaLibrary_1.MediaType.VIDEO
        });
        albums.push({
            albumName: 'Audios', count: audios.length, mediaType: _ohos_multimedia_mediaLibrary_1.MediaType.AUDIO
        });
        return albums;
    }
    deleteFile(media) {
        let uri = media.uri;
        Logger_1.default.info(this.tag, `deleteFile,uri = ${uri}`);
        return this.mediaTest.deleteAsset(uri);
    }
    onDateChange(callback) {
        this.mediaTest.on('albumChange', () => {
            Logger_1.default.info(this.tag, 'albumChange called');
            callback();
        });
        this.mediaTest.on('imageChange', () => {
            Logger_1.default.info(this.tag, 'imageChange called');
            callback();
        });
        this.mediaTest.on('audioChange', () => {
            Logger_1.default.info(this.tag, 'audioChange called');
            callback();
        });
        this.mediaTest.on('videoChange', () => {
            Logger_1.default.info(this.tag, 'videoChange called');
            callback();
        });
        this.mediaTest.on('fileChange', () => {
            Logger_1.default.info(this.tag, 'fileChange called');
            callback();
        });
    }
    offDateChange() {
        this.mediaTest.off('albumChange');
        this.mediaTest.off('imageChange');
        this.mediaTest.off('audioChange');
        this.mediaTest.off('videoChange');
        this.mediaTest.off('fileChange');
    }
    getInfoFromType(mediaType) {
        let result = {
            prefix: '', suffix: '', directory: 0
        };
        switch (mediaType) {
            case _ohos_multimedia_mediaLibrary_1.MediaType.FILE:
                result.prefix = 'FILE_';
                result.suffix = '.txt';
                result.directory = _ohos_multimedia_mediaLibrary_1.DirectoryType.DIR_DOCUMENTS;
                break;
            case _ohos_multimedia_mediaLibrary_1.MediaType.IMAGE:
                result.prefix = 'IMG_';
                result.suffix = '.jpg';
                result.directory = _ohos_multimedia_mediaLibrary_1.DirectoryType.DIR_IMAGE;
                break;
            case _ohos_multimedia_mediaLibrary_1.MediaType.VIDEO:
                result.prefix = 'VID_';
                result.suffix = '.mp4';
                result.directory = _ohos_multimedia_mediaLibrary_1.DirectoryType.DIR_VIDEO;
                break;
            case _ohos_multimedia_mediaLibrary_1.MediaType.AUDIO:
                result.prefix = 'AUD_';
                result.suffix = '.wav';
                result.directory = _ohos_multimedia_mediaLibrary_1.DirectoryType.DIR_AUDIO;
                break;
        }
        return result;
    }
}
exports["default"] = MediaUtils;
MediaUtils.instance = new MediaUtils();


/***/ }),

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/CameraPage.ets?entry":
/*!*************************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/CameraPage.ets?entry ***!
  \*************************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

"use strict";

var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", ({ value: true }));
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
var _ohos_prompt_1  = globalThis.requireNapi('prompt') || (isSystemplugin('prompt', 'ohos') ? globalThis.ohosplugin.prompt : isSystemplugin('prompt', 'system') ? globalThis.systemplugin.prompt : undefined);
const Logger_1 = __importDefault(__webpack_require__(/*! ../model/Logger */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts"));
const CameraService_1 = __importDefault(__webpack_require__(/*! ../model/CameraService */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/CameraService.ts"));
//import TitleBar from '../view/TitleBar'
const CameraMode = {
    MODE_PHOTO: 0,
    MODE_VIDEO: 1 // 录像模式
};
class CameraPage extends View {
    constructor(compilerAssignedUniqueChildId, parent, params) {
        super(compilerAssignedUniqueChildId, parent);
        this.tag = 'CameraPage';
        this.mXComponentController = new XComponentController();
        this.surfaceId = undefined;
        this.__imgThumbnail = new ObservedPropertySimple('', this, "imgThumbnail");
        this.__videoThumbnail = new ObservedPropertyObject(undefined, this, "videoThumbnail");
        this.__curModel = new ObservedPropertySimple(CameraMode.MODE_PHOTO, this, "curModel");
        this.__isRecording = new ObservedPropertySimple(false, this, "isRecording");
        this.handleTakePicture = (thumbnail) => {
            this.imgThumbnail = thumbnail;
            Logger_1.default.info(this.tag, `takePicture end , thumbnail: ${this.imgThumbnail}`);
        };
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.tag !== undefined) {
            this.tag = params.tag;
        }
        if (params.mXComponentController !== undefined) {
            this.mXComponentController = params.mXComponentController;
        }
        if (params.surfaceId !== undefined) {
            this.surfaceId = params.surfaceId;
        }
        if (params.imgThumbnail !== undefined) {
            this.imgThumbnail = params.imgThumbnail;
        }
        if (params.videoThumbnail !== undefined) {
            this.videoThumbnail = params.videoThumbnail;
        }
        if (params.curModel !== undefined) {
            this.curModel = params.curModel;
        }
        if (params.isRecording !== undefined) {
            this.isRecording = params.isRecording;
        }
        if (params.handleTakePicture !== undefined) {
            this.handleTakePicture = params.handleTakePicture;
        }
    }
    aboutToBeDeleted() {
        this.__imgThumbnail.aboutToBeDeleted();
        this.__videoThumbnail.aboutToBeDeleted();
        this.__curModel.aboutToBeDeleted();
        this.__isRecording.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get imgThumbnail() {
        return this.__imgThumbnail.get();
    }
    set imgThumbnail(newValue) {
        this.__imgThumbnail.set(newValue);
    }
    get videoThumbnail() {
        return this.__videoThumbnail.get();
    }
    set videoThumbnail(newValue) {
        this.__videoThumbnail.set(newValue);
    }
    get curModel() {
        return this.__curModel.get();
    }
    set curModel(newValue) {
        this.__curModel.set(newValue);
    }
    get isRecording() {
        return this.__isRecording.get();
    }
    set isRecording(newValue) {
        this.__isRecording.set(newValue);
    }
    aboutToAppear() {
        this.surfaceId = this.mXComponentController.getXComponentSurfaceId();
        Logger_1.default.info(this.tag, `aboutToAppear,surfaceId=${this.surfaceId}`);
        CameraService_1.default.setTakePictureCallback(this.handleTakePicture.bind(this));
    }
    getCameraIcon() {
        if (this.curModel === CameraMode.MODE_PHOTO) {
            return { "id": 16777243, "type": 20000, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" };
        }
        else {
            if (this.isRecording) {
                return { "id": 16777245, "type": 20000, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" };
            }
            else {
                return { "id": 16777244, "type": 20000, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" };
            }
        }
    }
    render() {
        Column.create();
        Column.width('100%');
        Column.height('100%');
        //      TitleBar()
        Stack.create({ alignContent: Alignment.Bottom });
        //      TitleBar()
        Stack.width('100%');
        //      TitleBar()
        Stack.height('100%');
        //      TitleBar()
        Stack.layoutWeight(1);
        //      TitleBar()
        Stack.backgroundColor(Color.Grey);
        XComponent.create({
            id: 'componentId',
            type: 'surface',
            controller: this.mXComponentController
        });
        XComponent.onLoad(() => {
            Logger_1.default.info(this.tag, 'onLoad is called');
            this.mXComponentController.setXComponentSurfaceSize({ surfaceWidth: 1920, surfaceHeight: 1080 });
            this.surfaceId = this.mXComponentController.getXComponentSurfaceId();
            Logger_1.default.info(this.tag, `onLoad surfaceId: ${this.surfaceId}`);
            this.curModel = CameraMode.MODE_PHOTO;
            CameraService_1.default.initCamera(this.surfaceId);
        });
        XComponent.height('100%');
        XComponent.width('100%');
        XComponent.margin({ bottom: 130 });
        Column.create();
        Column.size({ height: 130, width: '100%' });
        Column.padding({ bottom: 10 });
        Column.backgroundColor(Color.Black);
        Row.create();
        Row.size({ height: 40, width: '100%' });
        Row.margin({ left: 50 });
        Row.justifyContent(FlexAlign.Center);
        Text.create({ "id": 16777224, "type": 10003, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" });
        Text.fontColor(this.curModel === CameraMode.MODE_PHOTO ? Color.White : Color.Gray);
        Text.fontSize(25);
        Text.pop();
        Row.pop();
        Row.create();
        Row.size({ height: 80, width: '100%' });
        Row.margin({ right: 50 });
        Row.justifyContent(FlexAlign.Center);
        Image.create(this.imgThumbnail);
        Image.size({ width: 70, height: 70 });
        Image.aspectRatio(1);
        Image.borderRadius(40);
        Image.objectFit(ImageFit.Fill);
        Image.backgroundColor(Color.Gray);
        Image.create(this.getCameraIcon());
        Image.size({ width: 70, height: 70 });
        Image.margin({ left: 50 });
        Image.onClick(() => {
            if (this.curModel === CameraMode.MODE_PHOTO) {
                Logger_1.default.info(this.tag, 'takePicture begin');
                _ohos_prompt_1.showToast({ message: '拍照中...', duration: 1000 });
                CameraService_1.default.takePicture();
            }
        });
        Row.pop();
        Column.pop();
        //      TitleBar()
        Stack.pop();
        Column.pop();
    }
    async aboutToDisappear() {
        await CameraService_1.default.releaseCamera();
    }
}
loadDocument(new CameraPage("1", undefined, {}));


/***/ }),

/***/ "../../api/@ohos.multimedia.media.d.ts":
/*!*********************************************!*\
  !*** ../../api/@ohos.multimedia.media.d.ts ***!
  \*********************************************/
/***/ (() => {



/***/ })

/******/ 	});
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		var commonCachedModule = globalThis["__common_module_cache___bedf604a4a2a5251c93f50fd33130eef"] ? globalThis["__common_module_cache___bedf604a4a2a5251c93f50fd33130eef"][moduleId]: null;
/******/ 		if (commonCachedModule) { return commonCachedModule.exports; }
/******/ 		// Check if module is in cache
/******/ 		var cachedModule = __webpack_module_cache__[moduleId];
/******/ 		if (cachedModule !== undefined) {
/******/ 			return cachedModule.exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			// no module.id needed
/******/ 			// no module.loaded needed
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		function isCommonModue(moduleId) {
/******/ 		                if (globalThis["webpackChunk_bedf604a4a2a5251c93f50fd33130eef"]) {
/******/ 		                  const length = globalThis["webpackChunk_bedf604a4a2a5251c93f50fd33130eef"].length;
/******/ 		                  switch (length) {
/******/ 		                    case 1:
/******/ 		                      return globalThis["webpackChunk_bedf604a4a2a5251c93f50fd33130eef"][0][1][moduleId];
/******/ 		                    case 2:
/******/ 		                      return globalThis["webpackChunk_bedf604a4a2a5251c93f50fd33130eef"][0][1][moduleId] ||
/******/ 		                      globalThis["webpackChunk_bedf604a4a2a5251c93f50fd33130eef"][1][1][moduleId];
/******/ 		                  }
/******/ 		                }
/******/ 		                return undefined;
/******/ 		              }
/******/ 		if (globalThis["__common_module_cache___bedf604a4a2a5251c93f50fd33130eef"] && moduleId.indexOf("?name=") < 0 && isCommonModue(moduleId)) {
/******/ 		  globalThis["__common_module_cache___bedf604a4a2a5251c93f50fd33130eef"][moduleId] = module;
/******/ 		}
/******/ 		__webpack_modules__[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/ 	
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module is referenced by other modules so it can't be inlined
/******/ 	var __webpack_exports__ = __webpack_require__("../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/CameraPage.ets?entry");
/******/ 	_bedf604a4a2a5251c93f50fd33130eef = __webpack_exports__;
/******/ 	
/******/ })()
;
//# sourceMappingURL=CameraPage.js.map