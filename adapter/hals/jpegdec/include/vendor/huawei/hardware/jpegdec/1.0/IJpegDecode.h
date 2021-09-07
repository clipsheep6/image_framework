#ifndef HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_IJPEGDECODE_H
#define HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_IJPEGDECODE_H

#include <android/hidl/base/1.0/IBase.h>
#include <vendor/huawei/hardware/jpegdec/1.0/types.h>

#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace huawei {
namespace hardware {
namespace jpegdec {
namespace V1_0 {

struct IJpegDecode : public ::android::hidl::base::V1_0::IBase {
    /**
     * Type tag for use in template logic that indicates this is a 'pure' class.
     */
    typedef android::hardware::details::i_tag _hidl_tag;

    /**
     * Fully qualified interface name: "vendor.huawei.hardware.jpegdec@1.0::IJpegDecode"
     */
    static const char* descriptor;

    /**
     * Returns whether this object's implementation is outside of the current process.
     */
    virtual bool isRemote() const override { return false; }

    virtual ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> DoDecode(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& cinfo, const ::android::hardware::hidl_handle& out_info, const ::android::hardware::hidl_handle& in_info, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& region_info, int32_t samplesize, int32_t compressPos) = 0;

    /**
     * Return callback for Alloc_OutBuffer
     */
    using Alloc_OutBuffer_cb = std::function<void(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error error, const ::android::hardware::hidl_handle& outinfo)>;
    virtual ::android::hardware::Return<void> Alloc_OutBuffer(int32_t outwidth, int32_t outheight, int32_t color_format, Alloc_OutBuffer_cb _hidl_cb) = 0;

    /**
     * Return callback for Alloc_InBuffer
     */
    using Alloc_InBuffer_cb = std::function<void(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error error, const ::android::hardware::hidl_handle& outinfo)>;
    virtual ::android::hardware::Return<void> Alloc_InBuffer(int32_t size, Alloc_InBuffer_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> LockDevice() = 0;

    virtual ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> UnLockDevice() = 0;

    /**
     * Return callback for GetDecodeStatus
     */
    using GetDecodeStatus_cb = std::function<void(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error error, bool status)>;
    virtual ::android::hardware::Return<void> GetDecodeStatus(GetDecodeStatus_cb _hidl_cb) = 0;

    /**
     * Return callback for interfaceChain
     */
    using interfaceChain_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& descriptors)>;
    virtual ::android::hardware::Return<void> interfaceChain(interfaceChain_cb _hidl_cb) override;

    virtual ::android::hardware::Return<void> debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options) override;

    /**
     * Return callback for interfaceDescriptor
     */
    using interfaceDescriptor_cb = std::function<void(const ::android::hardware::hidl_string& descriptor)>;
    virtual ::android::hardware::Return<void> interfaceDescriptor(interfaceDescriptor_cb _hidl_cb) override;

    /**
     * Return callback for getHashChain
     */
    using getHashChain_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_array<uint8_t, 32>>& hashchain)>;
    virtual ::android::hardware::Return<void> getHashChain(getHashChain_cb _hidl_cb) override;

    virtual ::android::hardware::Return<void> setHALInstrumentation() override;

    virtual ::android::hardware::Return<bool> linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie) override;

    virtual ::android::hardware::Return<void> ping() override;

    /**
     * Return callback for getDebugInfo
     */
    using getDebugInfo_cb = std::function<void(const ::android::hidl::base::V1_0::DebugInfo& info)>;
    virtual ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb) override;

    virtual ::android::hardware::Return<void> notifySyspropsChanged() override;

    virtual ::android::hardware::Return<bool> unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient) override;

    // cast static functions
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>> castFrom(const ::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>& parent, bool emitError = false);
    /**
     * This performs a checked cast based on what the underlying implementation actually is.
     */
    static ::android::hardware::Return<::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    // helper methods for interactions with the hwservicemanager
    /**
     * This gets the service of this type with the specified instance name. If the
     * service is currently not available or not in the VINTF manifest on a Trebilized
     * device, this will return nullptr. This is useful when you don't want to block
     * during device boot. If getStub is true, this will try to return an unwrapped
     * passthrough implementation in the same process. This is useful when getting an
     * implementation from the same partition/compilation group.
     * 
     * In general, prefer getService(std::string,bool)
     */
    static ::android::sp<IJpegDecode> tryGetService(const std::string &serviceName="default", bool getStub=false);
    /**
     * Deprecated. See tryGetService(std::string, bool)
     */
    static ::android::sp<IJpegDecode> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    /**
     * Deprecated. See tryGetService(std::string, bool)
     */
    static ::android::sp<IJpegDecode> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    /**
     * Calls tryGetService("default", bool). This is the recommended instance name for singleton services.
     */
    static ::android::sp<IJpegDecode> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    /**
     * This gets the service of this type with the specified instance name. If the
     * service is not in the VINTF manifest on a Trebilized device, this will return
     * nullptr. If the service is not available, this will wait for the service to
     * become available. If the service is a lazy service, this will start the service
     * and return when it becomes available. If getStub is true, this will try to
     * return an unwrapped passthrough implementation in the same process. This is
     * useful when getting an implementation from the same partition/compilation group.
     */
    static ::android::sp<IJpegDecode> getService(const std::string &serviceName="default", bool getStub=false);
    /**
     * Deprecated. See getService(std::string, bool)
     */
    static ::android::sp<IJpegDecode> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    /**
     * Deprecated. See getService(std::string, bool)
     */
    static ::android::sp<IJpegDecode> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    /**
     * Calls getService("default", bool). This is the recommended instance name for singleton services.
     */
    static ::android::sp<IJpegDecode> getService(bool getStub) { return getService("default", getStub); }
    /**
     * Registers a service with the service manager. For Trebilized devices, the service
     * must also be in the VINTF manifest.
     */
    __attribute__ ((warn_unused_result))::android::status_t registerAsService(const std::string &serviceName="default");
    /**
     * Registers for notifications for when a service is registered.
     */
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

//
// type declarations for package
//

static inline std::string toString(const ::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>& o);

//
// type header definitions for package
//

static inline std::string toString(const ::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>& o) {
    std::string os = "[class or subclass of ";
    os += ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode::descriptor;
    os += "]";
    os += o->isRemote() ? "@remote" : "@local";
    return os;
}


} // namespace V1_0
} // namespace jpegdec
} // namespace hardware
} // namespace huawei
} // namespace vendor

//
// global type declarations for package
//


#endif // HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_IJPEGDECODE_H
