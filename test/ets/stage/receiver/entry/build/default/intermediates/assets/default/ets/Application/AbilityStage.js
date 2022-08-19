var _bedf604a4a2a5251c93f50fd33130eef;
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/Application/AbilityStage.ts?entry":
/*!********************************************************************************************************************************!*\
  !*** ../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/Application/AbilityStage.ts?entry ***!
  \********************************************************************************************************************************/
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
var _ohos_application_AbilityStage_1  = globalThis.requireNapi('application.AbilityStage') || (isSystemplugin('application.AbilityStage', 'ohos') ? globalThis.ohosplugin.application.AbilityStage : isSystemplugin('application.AbilityStage', 'system') ? globalThis.systemplugin.application.AbilityStage : undefined);
const Logger_1 = __importDefault(__webpack_require__(/*! ../model/Logger */ "../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/model/Logger.ts"));
class MyAbilityStage extends _ohos_application_AbilityStage_1 {
    onCreate() {
        Logger_1.default.info('MyAbilityStage', 'onCreate');
    }
}
globalThis.exports.default = MyAbilityStage;


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
/******/ 	var __webpack_exports__ = __webpack_require__("../../../../../Users/qiyi/Desktop/code/app_samples/media/MultiMedia/entry/src/main/ets/Application/AbilityStage.ts?entry");
/******/ 	_bedf604a4a2a5251c93f50fd33130eef = __webpack_exports__;
/******/ 	
/******/ })()
;
//# sourceMappingURL=AbilityStage.js.map