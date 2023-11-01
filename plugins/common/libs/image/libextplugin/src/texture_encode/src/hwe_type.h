#ifndef HWE_TYPE_H
#define HWE_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
