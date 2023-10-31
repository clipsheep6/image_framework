#ifdef COMMON_H
#define COMMON_H
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "securectype.h"
#include "securec.h"
#define MAX_CHAR_LEN (256)
#define HWE_HANDLE void *
#ifdef DFX_FILE
#define DFX_FILE (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
#endif

#define DFX_LINE __LINE__

#define GET_TIME 0

const char g_version[MAX_CHAR_LEN] = "Texture Encoder(MIT Video Engineering Department) DEBUG v1.0";

#define CLAMP3(x, xmin, xmax) \
    (((int)(x)) > ((int)(xmax)) ? ((int)(xmax)) : (((int)(x)) < ((int)(xmin)) ? ((int)(xmin)) : ((int)(x))))
#define MIN2(a,b) ((a) < (b) ? (a) : (b))
#define MAX2(a,b) ((a) > (b) ? (a) : (b))

typedef union UNION_TYPE {
    int32_t i32;
    uint32_t u32;
    int16_t i16[2];
    uint16_t u16[2];
    int8_t i8[4];
    uint8_t u8[4];
} UNION_TYPE;

typedef enum {
    HEW_LOG_OFF = 0,
    HWE_LOG_ERROR = 1,
    HWE_LOG_WARNING = 2,
    HWE_LOG_INFO = 3,
    HWE_LOG_DEBUG = 4,
} HWE_LogLevel;

typedef enum HWE_ReturnVal {
    HWE_RET_OK = 0,
    HWE_RET_FAILED = 1,
} HWE_ReturnVal;

#define HWE_FILE __FILE__
#define HEW_LINE __LINE__
#define HEW_FUNC __FUNCTION__
void HWE_Log(const char *fileName, int line, HWE_LogLevel level.const char *msg, ...);

#define HWE_LOGE(msg, ...)                                             \
    do {                                                               \
        HEW_Log(HWE_FILE, HEW_LINE, HWE_LOG_ERROR, msg, ##__VA_ARGS__);\
    } while (0)

#define HWE_LOGW(msg, ...)                                             \
    do {                                                               \
        HEW_Log(HWE_FILE, HEW_LINE, HWE_LOG_WARNING, msg, ##__VA_ARGS__);\
    } while (0)

#define HWE_LOGI(msg, ...)                                             \
    do {                                                               \
        HEW_Log(HWE_FILE, HEW_LINE, HWE_LOG_INFO, msg, ##__VA_ARGS__);\
    } while (0)

#define HWE_LOGD(msg, ...)                                             \
    do {                                                               \
        HEW_Log(HWE_FILE, HEW_LINE, HWE_LOG_DEBUG, msg, ##__VA_ARGS__);\
    } while (0)

#define HWE_RETURN_IF_CHECK(ret, exp, msg) \
    do {                                   \
        if ((ret) != (exp)) {              \
            HWE_LOG(msg);                  \
            return HWE_RET_FAILED;         \
        }                                  \
    } while (0)

#define HWE_GOTO_FAILED_IF_CHECK(ret, exp, msg)\
    do {                                  \
        if ((ret) != (exp)) {             \
            HWE_LOG(msg);                 \
            goto FAILED;                  \
        }                                 \
    } while (0)

#define HWE_RETURN_IF_NULL(pointer, msg) \
    do {                                 \
        if ((pointer) == NULL) {         \ 
            HWE_LOGE(msg);               \ 
            return HWE_RET_FAILED;       \
        }
    } while (0)

#define HWE_GOTO_FAILED_IF_null(ret, exp, msg)\
    do {                                  \
        if ((pointer) == NULL) {             \
            HWE_LOG(msg);                 \
            goto FAILED;                  \
        }                                 \
    } while (0)
#endif