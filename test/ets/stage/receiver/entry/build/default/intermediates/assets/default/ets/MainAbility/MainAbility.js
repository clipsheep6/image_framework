var _bedf604a4a2a5251c93f50fd33130eef;
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/MainAbility/MainAbility.ts?entry":
/*!*******************************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/MainAbility/MainAbility.ts?entry ***!
  \*******************************************************************************************************************************/
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
var _ohos_application_Ability_1  = globalThis.requireNapi('application.Ability') || (isSystemplugin('application.Ability', 'ohos') ? globalThis.ohosplugin.application.Ability : isSystemplugin('application.Ability', 'system') ? globalThis.systemplugin.application.Ability : undefined);
const Logger_1 = __importDefault(__webpack_require__(/*! ../model/Logger */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts"));
const TAG = 'MainAbility';
class MainAbility extends _ohos_application_Ability_1 {
    onCreate(want, launchParam) {
        Logger_1.default.info(TAG, 'MainAbility onCreate');
        globalThis.abilityWant = want;
        globalThis.abilityContext = this.context;
    }
    onDestroy() {
        Logger_1.default.info(TAG, 'MainAbility onDestroy');
    }
    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        Logger_1.default.info(TAG, 'MainAbility onWindowStageCreate');
        windowStage.setUIContent(this.context, "pages/Index", null);
    }
    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        Logger_1.default.info(TAG, 'MainAbility onWindowStageDestroy');
    }
    onForeground() {
        // Ability has brought to foreground
        Logger_1.default.info(TAG, 'MainAbility onForeground');
    }
    onBackground() {
        // Ability has back to background
        Logger_1.default.info(TAG, 'MainAbility onBackground');
    }
}
globalThis.exports.default = MainAbility;
;


/***/ }),

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
/******/ 	var __webpack_exports__ = __webpack_require__("../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/MainAbility/MainAbility.ts?entry");
/******/ 	_bedf604a4a2a5251c93f50fd33130eef = __webpack_exports__;
/******/ 	
/******/ })()
;
//# sourceMappingURL=MainAbility.js.map