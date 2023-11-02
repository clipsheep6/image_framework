/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HWE_TYPE_H
#define HWE_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// using standard C99 type, support all compatible platforms
typedef uint8_t HW_U8;
typedef uint16_t HW_U16;
typedef uint32_t HW_U32;
typedef uint64_t HW_U64;

typedef int8_t HW_S8;
typedef int16_t HW_S16;
typedef int32_t HW_S32;
typedef int64_t HW_S64;

#if HWE_BIT_DEPTH == 8
typedef HW_U8 HW_PIXEL;
typedef HW_S16 HW_COEFF;
#else
typedef HW_U16 HW_PIXEL;
typedef HW_S16 HW_COEFF;
#endif

typedef float HW_F32;
typedef double HW_F64;

// 8-bit bool type
typedef signed char HW_B8;


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#ifdef __cplusplus
}
#endif

#endif
