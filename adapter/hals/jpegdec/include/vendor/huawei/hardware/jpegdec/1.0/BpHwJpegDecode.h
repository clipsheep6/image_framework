#ifndef HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_BPHWJPEGDECODE_H
#define HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_BPHWJPEGDECODE_H

#include <hidl/HidlTransportSupport.h>

#include <vendor/huawei/hardware/jpegdec/1.0/IHwJpegDecode.h>

namespace vendor {
namespace huawei {
namespace hardware {
namespace jpegdec {
namespace V1_0 {

struct BpHwJpegDecode : public ::android::hardware::BpInterface<IJpegDecode>, public ::android::hardware::details::HidlInstrumentor {
    explicit BpHwJpegDecode(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);

    /**
     * The pure class is what this class wraps.
     */
    typedef IJpegDecode Pure;

    /**
     * Type tag for use in template logic that indicates this is a 'proxy' class.
     */
    typedef android::hardware::details::bphw_tag _hidl_tag;

    virtual bool isRemote() const override { return true; }

    // Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.
    static ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>  _hidl_DoDecode(::android::hardware::IInterface* _hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& cinfo, const ::android::hardware::hidl_handle& out_info, const ::android::hardware::hidl_handle& in_info, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& region_info, int32_t samplesize, int32_t compressPos);
    static ::android::hardware::Return<void>  _hidl_Alloc_OutBuffer(::android::hardware::IInterface* _hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, int32_t outwidth, int32_t outheight, int32_t color_format, Alloc_OutBuffer_cb _hidl_cb);
    static ::android::hardware::Return<void>  _hidl_Alloc_InBuffer(::android::hardware::IInterface* _hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, int32_t size, Alloc_InBuffer_cb _hidl_cb);
    static ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>  _hidl_LockDevice(::android::hardware::IInterface* _hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor);
    static ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>  _hidl_UnLockDevice(::android::hardware::IInterface* _hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor);
    static ::android::hardware::Return<void>  _hidl_GetDecodeStatus(::android::hardware::IInterface* _hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, GetDecodeStatus_cb _hidl_cb);

    // Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.
    ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> DoDecode(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& cinfo, const ::android::hardware::hidl_handle& out_info, const ::android::hardware::hidl_handle& in_info, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& region_info, int32_t samplesize, int32_t compressPos) override;
    ::android::hardware::Return<void> Alloc_OutBuffer(int32_t outwidth, int32_t outheight, int32_t color_format, Alloc_OutBuffer_cb _hidl_cb) override;
    ::android::hardware::Return<void> Alloc_InBuffer(int32_t size, Alloc_InBuffer_cb _hidl_cb) override;
    ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> LockDevice() override;
    ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> UnLockDevice() override;
    ::android::hardware::Return<void> GetDecodeStatus(GetDecodeStatus_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    ::android::hardware::Return<void> interfaceChain(interfaceChain_cb _hidl_cb) override;
    ::android::hardware::Return<void> debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options) override;
    ::android::hardware::Return<void> interfaceDescriptor(interfaceDescriptor_cb _hidl_cb) override;
    ::android::hardware::Return<void> getHashChain(getHashChain_cb _hidl_cb) override;
    ::android::hardware::Return<void> setHALInstrumentation() override;
    ::android::hardware::Return<bool> linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie) override;
    ::android::hardware::Return<void> ping() override;
    ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb) override;
    ::android::hardware::Return<void> notifySyspropsChanged() override;
    ::android::hardware::Return<bool> unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient) override;

private:
    std::mutex _hidl_mMutex;
    std::vector<::android::sp<::android::hardware::hidl_binder_death_recipient>> _hidl_mDeathRecipients;
};

} // namespace V1_0
} // namespace jpegdec
} // namespace hardware
} // namespace huawei
} // namespace vendor

#endif // HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_BPHWJPEGDECODE_H
