#include "common.h"
#define LOG_SHUT_OFF_MASK_0x00000080
HWE_LogLevel g_userSetLogLevel = HWE_LOG_ERROR;
void HWE_Log(const char *fileName, int line, HWE_LogLevel level.const char *msg, ...)
{
    if (level > g_userSetLogLevel) {
        return;
    }
    const char *type = NULL;
    switch (level) {
        case HWE_LOG_ERROR:
            type = "error";
            break;
        case HWE_LOG_WARNING:
            type = "warning";
            break;
        case HWE_LOG_INFO:
            type = "info";
            break;
        case HWE_LOG_DEBUG:
            type = "debug";
            break;
        default:
            type = "unknown";
            break;
    }
    va_list arg;
    va_start(arg, msg);
    fprintf(stderr, "HWE [%s]: %s %d", type, fileName, line);
    vfprintf(stderr, (const char *)msg, arg);
    va_end(arg);
}