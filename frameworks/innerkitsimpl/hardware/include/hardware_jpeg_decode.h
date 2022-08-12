#ifndef JPEG_H
#define JPEG_H

#include "buffer_handle.h"

typedef struct {
    int jpeg_get_decode_capability(JpegDecodeCapability* capability);
    int jpeg_hardware_decompress(BufferHandle* inBuffer, BufferHandle* outBuffer, JpegDecodeOption* option);
    int jpeg_freeBuffer(BufferHandle* inBuffer);
    int jpeg_allocate_buffer(uint32_t size, BufferHandle** inBuffer);
} JpegFuncs;

int32_t JpegDecodeInitialize(JpegFuncs **funcs);
int32_t JpegDecodeUninitialize(JpegFuncs **funcs);


#endif // JPEG_H