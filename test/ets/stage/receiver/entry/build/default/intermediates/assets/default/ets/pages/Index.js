var _bedf604a4a2a5251c93f50fd33130eef;
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts":
/*!**************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts ***!
  \**************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports) {


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

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/Index.ets":
/*!**************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/Index.ets ***!
  \**************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {


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
var _ohos_router_1  = globalThis.requireNapi('router') || (isSystemplugin('router', 'ohos') ? globalThis.ohosplugin.router : isSystemplugin('router', 'system') ? globalThis.systemplugin.router : undefined);
const Logger_1 = __importDefault(__webpack_require__(/*! ../model/Logger */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts"));
//import TitleBar from '../view/TitleBar'
//import MediaUtils from '../model/MediaUtils'
const MediaView_1 = __webpack_require__(/*! ../view/MediaView */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/view/MediaView.ets");
const PERMISSIONS = [
    'ohos.permission.CAMERA',
    'ohos.permission.MICROPHONE',
    'ohos.permission.READ_MEDIA',
    'ohos.permission.WRITE_MEDIA',
    'ohos.permission.MEDIA_LOCATION'
];
const TAG = 'Index';
class Index extends View {
    constructor(compilerAssignedUniqueChildId, parent, params) {
        super(compilerAssignedUniqueChildId, parent);
        this.__albums = new ObservedPropertyObject([], this, "albums");
        this.__selectIndex = new ObservedPropertySimple(0, this, "selectIndex");
        this.__operateVisible = new ObservedPropertySimple(false
        //  getAlbums = async () => {
        //    this.albums = await this.mediaUtils.getAlbums()
        //  }
        , this, "operateVisible");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.albums !== undefined) {
            this.albums = params.albums;
        }
        if (params.selectIndex !== undefined) {
            this.selectIndex = params.selectIndex;
        }
        if (params.operateVisible !== undefined) {
            this.operateVisible = params.operateVisible;
        }
    }
    aboutToBeDeleted() {
        this.__albums.aboutToBeDeleted();
        this.__selectIndex.aboutToBeDeleted();
        this.__operateVisible.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get albums() {
        return this.__albums.get();
    }
    set albums(newValue) {
        this.__albums.set(newValue);
    }
    get selectIndex() {
        return this.__selectIndex.get();
    }
    set selectIndex(newValue) {
        this.__selectIndex.set(newValue);
    }
    get operateVisible() {
        return this.__operateVisible.get();
    }
    set operateVisible(newValue) {
        this.__operateVisible.set(newValue);
    }
    //  getAlbums = async () => {
    //    this.albums = await this.mediaUtils.getAlbums()
    //  }
    async aboutToAppear() {
        await globalThis.abilityContext.requestPermissionsFromUser(PERMISSIONS);
        Logger_1.default.info(TAG, 'grantPermission,requestPermissionsFromUser');
    }
    onPageShow() {
        //    if (this.albums.length === 0) {
        //      this.getAlbums()
        //      this.mediaUtils.onDateChange(this.getAlbums)
        //    }
    }
    OperateBtn(src, zIndex, translate, handleClick) {
        Button.createWithChild();
        Button.type(ButtonType.Circle);
        Button.size({ width: '40%', height: '40%' });
        Button.backgroundColor('#0D9FFB');
        Button.zIndex(zIndex);
        Button.translate({ x: translate.x, y: translate.y });
        Button.transition({ type: TransitionType.Insert, translate: { x: 0, y: 0 } });
        Button.transition({ type: TransitionType.Delete, opacity: 0 });
        Button.onClick(handleClick);
        Image.create(src);
        Image.size({ width: '70%', height: '70%' });
        Button.pop();
    }
    render() {
        Stack.create({ alignContent: Alignment.BottomEnd });
        Stack.width('100%');
        Stack.height('100%');
        Column.create();
        Column.width('100%');
        Column.height('100%');
        let earlierCreatedChild_2 = this.findChildById("2");
        if (earlierCreatedChild_2 == undefined) {
            //        TitleBar()
            View.create(new MediaView_1.MediaView("2", this, { albums: this.__albums }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            View.create(earlierCreatedChild_2);
        }
        Column.pop();
        Stack.create({ alignContent: Alignment.Center });
        Stack.size({ width: '30%', height: '30%' });
        Stack.translate({ x: 10 });
        Stack.margin({ bottom: 50 });
        this.OperateBtn({ "id": 16777231, "type": 20000, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" }, 1, { x: 0, y: 0 }, () => {
            Context.animateTo({ duration: 500, curve: Curve.Ease }, () => {
                this.operateVisible = !this.operateVisible;
            });
        });
        If.create();
        if (this.operateVisible) {
            If.branchId(0);
            this.OperateBtn({ "id": 16777239, "type": 20000, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" }, 0, { x: 0, y: '-120%' }, () => {
                this.operateVisible = !this.operateVisible;
                _ohos_router_1.push({ url: 'pages/CameraPage' });
            });
        }
        If.pop();
        Stack.pop();
        Stack.pop();
    }
}
loadDocument(new Index("1", undefined, {}));


/***/ }),

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/Index.ets?entry":
/*!********************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/Index.ets?entry ***!
  \********************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {


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
var _ohos_router_1  = globalThis.requireNapi('router') || (isSystemplugin('router', 'ohos') ? globalThis.ohosplugin.router : isSystemplugin('router', 'system') ? globalThis.systemplugin.router : undefined);
const Logger_1 = __importDefault(__webpack_require__(/*! ../model/Logger */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts"));
//import TitleBar from '../view/TitleBar'
//import MediaUtils from '../model/MediaUtils'
const MediaView_1 = __webpack_require__(/*! ../view/MediaView */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/view/MediaView.ets");
const PERMISSIONS = [
    'ohos.permission.CAMERA',
    'ohos.permission.MICROPHONE',
    'ohos.permission.READ_MEDIA',
    'ohos.permission.WRITE_MEDIA',
    'ohos.permission.MEDIA_LOCATION'
];
const TAG = 'Index';
class Index extends View {
    constructor(compilerAssignedUniqueChildId, parent, params) {
        super(compilerAssignedUniqueChildId, parent);
        this.__albums = new ObservedPropertyObject([], this, "albums");
        this.__selectIndex = new ObservedPropertySimple(0, this, "selectIndex");
        this.__operateVisible = new ObservedPropertySimple(false
        //  getAlbums = async () => {
        //    this.albums = await this.mediaUtils.getAlbums()
        //  }
        , this, "operateVisible");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
        if (params.albums !== undefined) {
            this.albums = params.albums;
        }
        if (params.selectIndex !== undefined) {
            this.selectIndex = params.selectIndex;
        }
        if (params.operateVisible !== undefined) {
            this.operateVisible = params.operateVisible;
        }
    }
    aboutToBeDeleted() {
        this.__albums.aboutToBeDeleted();
        this.__selectIndex.aboutToBeDeleted();
        this.__operateVisible.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get albums() {
        return this.__albums.get();
    }
    set albums(newValue) {
        this.__albums.set(newValue);
    }
    get selectIndex() {
        return this.__selectIndex.get();
    }
    set selectIndex(newValue) {
        this.__selectIndex.set(newValue);
    }
    get operateVisible() {
        return this.__operateVisible.get();
    }
    set operateVisible(newValue) {
        this.__operateVisible.set(newValue);
    }
    //  getAlbums = async () => {
    //    this.albums = await this.mediaUtils.getAlbums()
    //  }
    async aboutToAppear() {
        await globalThis.abilityContext.requestPermissionsFromUser(PERMISSIONS);
        Logger_1.default.info(TAG, 'grantPermission,requestPermissionsFromUser');
    }
    onPageShow() {
        //    if (this.albums.length === 0) {
        //      this.getAlbums()
        //      this.mediaUtils.onDateChange(this.getAlbums)
        //    }
    }
    OperateBtn(src, zIndex, translate, handleClick) {
        Button.createWithChild();
        Button.type(ButtonType.Circle);
        Button.size({ width: '40%', height: '40%' });
        Button.backgroundColor('#0D9FFB');
        Button.zIndex(zIndex);
        Button.translate({ x: translate.x, y: translate.y });
        Button.transition({ type: TransitionType.Insert, translate: { x: 0, y: 0 } });
        Button.transition({ type: TransitionType.Delete, opacity: 0 });
        Button.onClick(handleClick);
        Image.create(src);
        Image.size({ width: '70%', height: '70%' });
        Button.pop();
    }
    render() {
        Stack.create({ alignContent: Alignment.BottomEnd });
        Stack.width('100%');
        Stack.height('100%');
        Column.create();
        Column.width('100%');
        Column.height('100%');
        let earlierCreatedChild_2 = this.findChildById("2");
        if (earlierCreatedChild_2 == undefined) {
            //        TitleBar()
            View.create(new MediaView_1.MediaView("2", this, { albums: this.__albums }));
        }
        else {
            earlierCreatedChild_2.updateWithValueParams({});
            View.create(earlierCreatedChild_2);
        }
        Column.pop();
        Stack.create({ alignContent: Alignment.Center });
        Stack.size({ width: '30%', height: '30%' });
        Stack.translate({ x: 10 });
        Stack.margin({ bottom: 50 });
        this.OperateBtn({ "id": 16777231, "type": 20000, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" }, 1, { x: 0, y: 0 }, () => {
            Context.animateTo({ duration: 500, curve: Curve.Ease }, () => {
                this.operateVisible = !this.operateVisible;
            });
        });
        If.create();
        if (this.operateVisible) {
            If.branchId(0);
            this.OperateBtn({ "id": 16777239, "type": 20000, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" }, 0, { x: 0, y: '-120%' }, () => {
                this.operateVisible = !this.operateVisible;
                _ohos_router_1.push({ url: 'pages/CameraPage' });
            });
        }
        If.pop();
        Stack.pop();
        Stack.pop();
    }
}
loadDocument(new Index("1", undefined, {}));


/***/ }),

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/view/MediaView.ets":
/*!*****************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/view/MediaView.ets ***!
  \*****************************************************************************************************************/
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {


var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", ({ value: true }));
exports.MediaView = void 0;
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
var _ohos_router_1  = globalThis.requireNapi('router') || (isSystemplugin('router', 'ohos') ? globalThis.ohosplugin.router : isSystemplugin('router', 'system') ? globalThis.systemplugin.router : undefined);
__webpack_require__(/*! ../pages/Index */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/Index.ets");
class MediaView extends View {
    constructor(compilerAssignedUniqueChildId, parent, params) {
        super(compilerAssignedUniqueChildId, parent);
        this.__albums = new SynchedPropertyObjectTwoWay(params.albums, this, "albums");
        this.updateWithValueParams(params);
    }
    updateWithValueParams(params) {
    }
    aboutToBeDeleted() {
        this.__albums.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id());
    }
    get albums() {
        return this.__albums.get();
    }
    set albums(newValue) {
        this.__albums.set(newValue);
    }
    render() {
        Column.create();
        Column.height('100%');
        Column.width('100%');
        List.create();
        List.divider({ strokeWidth: 1, color: Color.Gray, startMargin: 16, endMargin: 16 });
        List.layoutWeight(1);
        ForEach.create("1", this, ObservedObject.GetRawObject(this.albums), (item, index) => {
            ListItem.create();
            ListItem.onClick(() => {
                if (item.count > 0) {
                    _ohos_router_1.push({
                        url: 'pages/AlbumPage',
                        params: { albumName: item.albumName, mediaType: item.mediaType }
                    });
                }
                else {
                    AlertDialog.show({
                        title: { "id": 16777220, "type": 10003, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" },
                        message: null,
                        primaryButton: {
                            value: { "id": 16777229, "type": 10003, params: [], "bundleName": "ohos.samples.multimedia", "moduleName": "entry" },
                            fontColor: Color.Red,
                            action: () => {
                            }
                        }
                    });
                }
            });
            Row.create();
            Row.width('100%');
            Row.padding(16);
            Row.constraintSize({ minHeight: 70 });
            Text.create(item.albumName);
            Text.fontSize(25);
            Text.fontWeight(FontWeight.Bold);
            Text.textAlign(TextAlign.Start);
            Text.pop();
            Blank.create();
            Blank.pop();
            Text.create(`${item.count}`);
            Text.fontSize(25);
            Text.fontWeight(FontWeight.Bold);
            Text.textAlign(TextAlign.End);
            Text.pop();
            Row.pop();
            ListItem.pop();
        }, item => item.albumName);
        ForEach.pop();
        List.pop();
        Column.pop();
    }
}
exports.MediaView = MediaView;


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
/******/ 	var __webpack_exports__ = __webpack_require__("../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/pages/Index.ets?entry");
/******/ 	_bedf604a4a2a5251c93f50fd33130eef = __webpack_exports__;
/******/ 	
/******/ })()
;
//# sourceMappingURL=Index.js.map