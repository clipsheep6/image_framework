
#include HWE_ENC_IMAGE_H
#define HWE_ENC_IMAGE_H
#include "common.h"
#include "texture_encoder.h"
#include "hwe_memoryManager.h"
#include "astcenc.h"

struct astc_header
{
    uint8_t magic[4];
    uint8_t block_x;
    uint8_t block_y;
    uint8_t block_z;
    uint8_t dim_x[3];
    uint8_t dim_y[3];
    uint8_t dim_z[3];
};

typedef struct AstcEncoderInfo {
    astc_header head;
    astcenc_config config;
    astcenc_profile profile;
    astcenc_context* codec_context;
    astcenc_image image_;
    astcenc_swizzle swizzle_;
    uint8_t* data_out_;
    size_t data_len_;
    astcenc_error error_;
} AstcEncoder;

typedef struct TextureEncParamsType {
    TextureEncOptions externParam_;
    uint8_t thread_count;
} TextureEncParams;

typedef struct TextureEncObjType
{
    TextureEncParams param_;
    AstcEncoder astcEncHeadle;
    HWE_HANDLE memoryHandle;
} TextureEncObj;

#endif