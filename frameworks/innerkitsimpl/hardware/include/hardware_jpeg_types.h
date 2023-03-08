#ifndef HARDWARE_JPEG_TYPES_H
#define HARDWARE_JPEG_TYPES_H

#include <inttypes.h>
#include <cstdint>

typedef enum {
    JPEG_DECODE_SUCCESS = 0,
    JPEG_DECODE_FAILURE = -1,
} JpegDecodeErrorCode;

typedef struct {
    uint32_t sample;
    bool needCrop;
    Irect crop;
} JpegDecodeOption;

typedef struct {
    uint32_t maxSample;
    uint64_t MASK;
} JpegDecodeCapability;





#endif // HARDWARE_JPEG_TYPES_H