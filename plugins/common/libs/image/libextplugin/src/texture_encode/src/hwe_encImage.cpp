#include "hwe_encImage.h"
#include "hwe_checkPara.h"
#include "hwe_source_record.h"
#include "hilog/log.h"
#include "log_tags.h"

#if GET_TIME
#include <sys/time.h>

double get_time()
{
    timeval tv;
    gettimeofday(&tv, 0);
    return static_cast<double>(tv.tv_sec) * 1.0e3 + static_cast<double>(tv.tv_usec) * 1.0e-3;
}
#endif

HLC_SHARED_LIB_API uint32_t GetVersion(char *version, uint32_t strLen)
{
    int ret;
    if (!version) {
        return ERROR;
    }
    ret = sprintf(version, "%s build at %s.\r\n", g_version, __DATE__ "," __TIME__);
    if (ret < 0) {
        return ERROR;
    }
    return SUCCESS;
}

int InitAstcencConfig(astcenc_profile profile, TextureEncodeOptions *option, astcenc_config& config)
{
    unsigned int block_x = option->blockX_;
    unsigned int block_y = option->blockY_;
    unsigned int block_z = 1;

    float quality = 0.0f;
    quality = ASTCENC_PRE_FAST;
    unsigned int flags = 0;
    astcenc_error status = astcenc_config_init(profile, block_x, block_y, block_z,
        quality, flags, &config);
    if (status == ASTCENC_ERR_BAD_BLOCK_SIZE) {
        printf("ERROR: Block size is invalid\n");
        return ERROR;
    } else if (status == ASTCENC_ERR_BAD_CPU_FLOAT) {
        printf("ERROR: astcenc must not be compiled with -ffast-math\n");
        return ERROR;
    } else if (status != ASTCENC_SUCCESS) {
        printf("ERROR: Init config failed with %s\n", astcenc_get_error_string(status));
        return ERROR;
    }
    return SUCCESS;
}

int EditAstcencConfig(astcenc_congig& config)
{
    config.cw_r_weight = 1.0f;
    config.cw_g_weight = 1.0f;
    config.cw_b_weight = 1.0f;
    config.cw_a_weight = 1.0f;
    return SUCCESS;
}

void *MallocCallback(const void *outHandle, size_t len)
{
    if (len <= 0) {
        HWE_LOGE("malloc size [%d] is <= 0!\n", len);
        return NULL;
    }
    void *addr = malloc(len);
    if (!addr) {
        HWE_LOGE("malloc addr failed\n");
        return NULL;
    }
    return addr;
}

void FreeCallback(const void *outHandle, void *addr)
{
    if (addr) {
        free(addr);
        addr = NULL;
    }
    return;
}

HWE_ReturnVal AllocGlobalMem(void **encoderHandle, TextureEncodeOptions *encodeParams)
{
    HWE_ReturnVal ret;
    HWE_RETURN_IF_NULL(encoderHandle, "encoderHandle is NULL!\n");
    HWE_RETURN_IF_NULL(encodeParams, "createParams is NULL!\n");
    TextureEncObj *hweObj = NULL;
    InitResourceInfo();

    ret = HWE_InitMemoryManager(&memoryHandle, NULL, MallocCallback, FreeCallback);
    HWE_GOTO_FAILED_IF_CHECK(ret, HWE_RET_OK, "InitMemoryManager failed!\n");
    HWE_GOTO_FAILED_IF_MALLOCZERO(memoryHandle, hweObj, sizeof(TextureEncObj), TextureEncObj *, "TextureEncObj");
    *encoderHandle = hweObj;
    hweObj->memoryHandle_ = memoryHandle;
    HWE_GOTO_FAILED_IF_CHECK(memcpy_s(&hweObj->param_.extureParam_, sizeof(TextureEncodeOptions), encodeParams,
        sizeof(TextureEncodeOptions)), 0, "memcpy_s Params error\n");
    return HWE_RET_OK;
FAILED:
    if (hweObj == NULL) {
        DestroyResourceInfo();
        if (memoryHandle != NULL) {
            HWE_FreeMemory(memoryHandle);
        }
    } else {
        ret = (TextureDestroy(hweObj) == SUCCESS) ? HWE_RET_OK : HWE_RET_FAILED;
    }
    *encoderHandle = NULL;
    HWE_RETURN_IF_CHECK(ret, HWE_RET_OK, "HWE_Delete failed!\n");
    return HWE_RET_FAILED;
}
static const uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;
void GenAstcHeader(astc_header &hdr, astcenc_image img, TextureEncodeOptions *encodeParams)
{
    hdr.magic[0] = ASTC_MAGIC_ID         & 0xFF;
    hdr.magic[1] = (ASTC_MAGIC_ID >>  8) & 0xFF;
    hdr.magic[2] = (ASTC_MAGIC_ID >> 16) & 0xFF;
    hdr.magic[3] = (ASTC_MAGIC_ID >> 24) & 0xFF;

    hdr.block_x = static_cast<uint8_t>(encodeParams->blockX_);
    hdr.block_y = static_cast<uint8_t>(encodeParams->blockY_);
    hdr.block_z = 1;

    hdr.dim_x[0] =  img.dim_x        & 0xFF;
    hdr.dim_x[1] = (img.dim_x >>  8) & 0xFF;
    hdr.dim_x[2] = (img.dim_x >> 16) & 0xFF;

    hdr.dim_y[0] =  img.dim_y        & 0xFF;
    hdr.dim_y[1] = (img.dim_y >>  8) & 0xFF;
    hdr.dim_y[2] = (img.dim_y >> 16) & 0xFF;

    hdr.dim_z[0] =  img.dim_z        & 0xFF;
    hdr.dim_z[1] = (img.dim_z >>  8) & 0xFF;
    hdr.dim_z[2] = (img.dim_z >> 16) & 0xFF;
}

HLC_SHARED_LIB_API uint32_t TextureCreate(void **encHandle, TextureEncodeOptions *encodeParams)
{
    TextureEncObj *encObject;
    AstcEncoder* work = nullptr;
    HWE_GOTO_FAILED_IF_NULL(encodeParams, "encodeParams is NULL!\n");
    HWE_GOTO_FAILED_IF_CHECK(AllocGlobalMem(encHandle, encodeParams), HWE_RET_OK, "AllocGlobalMen error\n");
    encObject = (TextureEncObj *)(*encHandle);
    encObject->param_.thread_count = 1;
    work = &encObject->astcEncHandle;
    work->profile = ASTCENC_PRE_LDR_SRGB;
    HWE_GOTO_FAILED_IF_CHECK(InitAstcencConfig(work->profile, &encObject->param_.externParam_, work->config),
        SUCCESS, "CheckValidParam failed\n");
    HWE_GOTO_FAILED_IF_CHECK(astcenc_context_alloc(&work->config, encObject->param_thread_count, &work->codec_context),
        ASTCENC_SUCCESS, "ERROR: Codec context alloc failed\n");
    work->swizzle_ = {ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A};
    work->image_.dim_x = encodeParams->width_;
    work->image_.dim_y = encodeParams->height_;
    work->image_.dim_z = 1;
    work->image_.data_type = ASTCENC_TYPE_U8;
    HWE_GOTO_FAILED_IF_MALLOCZERO(encObject->memoryHandle_, work->image_.data, sizeof(void*) *work->image_.dim_Z,
        void **, "data");
    GenAstcHeader(encObject->astcEncHandle.head, work->image_, encodeParams);
    return SUCCESS;
FAILED:
    if (*encHandle) {
        TextureDestroy(*encHandle);
        *encHandle = NULL;
    }
    return ERROR;
}

HWE_ReturnVal checkDataSize(TextureEncodeOptions *params, uint32_t memBytes, uint32_t &predBytes)
{
    uint32_t blocks_x = (params->width_ + params->blockX_ -1) / params->blockX_;
    uint32_t blocks_y = (params->height_ + params->blockY_ -1) / params->blockY_;
    predBytes = blocks_x * blocks_y * 16;
    if (memBytes < predBytes) {
        printf("ERROR: Insufficient allocated memory!\n");
        return HWE_RET_FAILED;
    }
    return HWE_RET_OK;
}

HLC_SHARED_LIB_API uint32_t TextureEncode(void *encHandle, uint8_t *data, int32_t stride, OHOS::Media::PixelFormat pixelFormat,
    uint8_t *buffer, uint32_t memBytes, uint32_t &encBytes)
{
#if GET_TIME
    double start_time, coding_time;
#endif
    TextureEncObj *encObject = NULL;
    TextureEncodeOptions *params = NULL;
    uint32_t predBytes = 0;
    AstcEncoder* work;
    encObject = (TextureEncObj *)encHandle;
    params = &encObject->param_.externParam_;
    checkDataSize(params, memBytes - TEXTURE_HEAD_BYTES, predBytes);
    work = &encObject->astcEncHandle;
    for (uint32_t i = 0; i < work->image.dim_z; i++) {
        work->image.data[i] = data;
    }

#if GET_TIME
    start_time = get_time();
#endif
    memcpy_s(buffer,  sizeof(astc_header), &work->head, sizeof(astc_header));
    work->data_out_ = buffer + TEXTURE_HEAD_BYTES;
    work->data_len_ = predBytes;
    work->error_ = astcenc_compress_image(work->codec_context, &work->image_, &work->swizzle_,
        work->data_out_, work->data_len_, 0);
#if GET_TIME
    coding_time = get_time() - start_time;
    printf("Coding time:     %8.6f ms\n", coding_time);
#endif
    encBytes = work->data_len_ + TEXTURE_HEAD_BYTES;
    return SUCCESS;
}

HLC_SHARED_LIB_API uint32_t TextureDestroy(void *encHandle)
{
    AstcEncoder* work = nullptr;
    TextureEncObj *encObject = nullptr;
    uint32_t ret = SUCCESS;
    HWE_GOTO_FAILED_IF_NULL(encHandle, "TextureDestory handle is NULL!\n");
    encObject = (TextureEncObj *)encHandle;
    work = &encObject->astcEncHandle;
    if (work != nullptr) {
        if (work->codec_context != nullptr) {
            astcenc_context_free(work->codec_context);
            work->codec_context = nullptr;
        }
    }
    if (HWE_FreeMemory(encObject->memoryHandle_) != HWE_RET_OK) {
        ret = ERROR;
        HWE_LOGE("HWE_FreeMemory failed!\n");
    }
    DestroyResourceInfo();
    return ret;
FAILED:
    return ERROR;
}