#ifndef HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_BNHWJPEGDECODE_H
#define HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_BNHWJPEGDECODE_H

#include <vendor/huawei/hardware/jpegdec/1.0/IHwJpegDecode.h>

namespace vendor {
namespace huawei {
namespace hardware {
namespace jpegdec {
namespace V1_0 {

struct BnHwJpegDecode : public ::android::hidl::base::V1_0::BnHwBase {
    explicit BnHwJpegDecode(const ::android::sp<IJpegDecode> &_hidl_impl);
    explicit BnHwJpegDecode(const ::android::sp<IJpegDecode> &_hidl_impl, const std::string& HidlInstrumentor_package, const std::string& HidlInstrumentor_interface);

    virtual ~BnHwJpegDecode();

    ::android::status_t onTransact(
            uint32_t _hidl_code,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            uint32_t _hidl_flags = 0,
            TransactCallback _hidl_cb = nullptr) override;


    /**
     * The pure class is what this class wraps.
     */
    typedef IJpegDecode Pure;

    /**
     * Type tag for use in template logic that indicates this is a 'native' class.
     */
    typedef android::hardware::details::bnhw_tag _hidl_tag;

    ::android::sp<IJpegDecode> getImpl() { return _hidl_mImpl; }
    // Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.
    static ::android::status_t _hidl_DoDecode(
            ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            TransactCallback _hidl_cb);


    static ::android::status_t _hidl_Alloc_OutBuffer(
            ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            TransactCallback _hidl_cb);


    static ::android::status_t _hidl_Alloc_InBuffer(
            ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            TransactCallback _hidl_cb);


    static ::android::status_t _hidl_LockDevice(
            ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            TransactCallback _hidl_cb);


    static ::android::status_t _hidl_UnLockDevice(
            ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            TransactCallback _hidl_cb);


    static ::android::status_t _hidl_GetDecodeStatus(
            ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            TransactCallback _hidl_cb);



private:
    // Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    ::android::hardware::Return<void> ping();
    using getDebugInfo_cb = ::android::hidl::base::V1_0::IBase::getDebugInfo_cb;
    ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb);

    ::android::sp<IJpegDecode> _hidl_mImpl;
};

} // namespace V1_0
} // namespace jpegdec
} // namespace hardware
} // namespace huawei
} // namespace vendor

#endif // HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_BNHWJPEGDECODE_H
