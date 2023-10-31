#include "hwe_checkPara.h"

static void LogDefault(const int8_t *fileName, uint32_t line, uint32_t uiChannelID, LogLevel elevel,
    const int8_t *pszFmt, va_list arg)
{
    const char *pszPrefix;
    pszPrefix = NULL;
    switch (elvel) {
        case LOG_ERROR:
            pszPrefix = "error";
            break;
        case LOG_WARN:
            pszPrefix = "warning";
            break;
        case LOG_INFO:
            pszPrefix = "info";
            break;
        case LOG_DEBUG:
            pszPrefix = "debug";
            break;
        default:
            pszPrefix = "unknown";
            break;
    }
    fprintf(stderr, "[%s\t]: %d %s %d", pszPrefix, uiChannelID, fileName, line);
    vfprintf(stderr, (const char *)pszFmt, arg);
}

#define LOG_SHUT_OFF_MASK 0X000080

void DFX_ParamLog(const int8_t *fileName, uint32_t line, int32_t level, const char *msg, ...)
{
    LogLevel eLevel = (LogLevel)((uint32_t)level & (~LOG_SHUT_OFF_MASK));
    const int8_t *pszTempMsg = (const int8_t *)msg;
    if (((uint32_t)level & LOG_SHUT_OFF_MASK) == LOG_SHUT_OFF_MASK) {
        return;
    }
    va_list arg;
    va_start(arg, msg);
    LogDefault(fileName, line, 0, elevel, pszTempMsg, arg);
    va_end(arg);
    return;
}

#define CHECK_PARAM_VALID_CONDITION(expression, val, msg)                         \
    do {                                                                          \  
        if (expression) {                                                         \
            DFX_ParamLog((const int8_t *)DFX_FILE, DFX_LINE, LOG_ERROR, msg, val);\
            goto fail;                                                            \  
        }                                                                         \
    } while (0)

#define CHECK_PARAM_VALID(val, minValue, maxValue, msg)                                               \
    do {                                                                                              \  
        if ((val) > (maxValue) || (val) < (minValue)) {                                                         \
            DFX_ParamLog((const int8_t *)DFX_FILE, DFX_LINE, LOG_ERROR, msg, val, minValue, maxValue);\
            goto fail;                                                                                \  
        }                                                                                             \
    } while (0)

bool CheckValidParam(TextureEncodeOptions *param)
{
    return true;
}