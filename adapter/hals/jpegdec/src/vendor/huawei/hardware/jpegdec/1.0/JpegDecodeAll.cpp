#define LOG_TAG "vendor.huawei.hardware.jpegdec@1.0::JpegDecode"

#include <log/log.h>
#include <cutils/trace.h>
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <android/hidl/manager/1.0/IServiceManager.h>
#include <vendor/huawei/hardware/jpegdec/1.0/BpHwJpegDecode.h>
#include <vendor/huawei/hardware/jpegdec/1.0/BnHwJpegDecode.h>
#include <vendor/huawei/hardware/jpegdec/1.0/BsJpegDecode.h>
#include <android/hidl/base/1.0/BpHwBase.h>
#include <hidl/ServiceManagement.h>

namespace vendor {
namespace huawei {
namespace hardware {
namespace jpegdec {
namespace V1_0 {

const char* IJpegDecode::descriptor("vendor.huawei.hardware.jpegdec@1.0::IJpegDecode");

__attribute__((constructor)) static void static_constructor() {
    ::android::hardware::details::getBnConstructorMap().set(IJpegDecode::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hardware::IBinder> {
                return new BnHwJpegDecode(static_cast<IJpegDecode *>(iIntf));
            });
    ::android::hardware::details::getBsConstructorMap().set(IJpegDecode::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hidl::base::V1_0::IBase> {
                return new BsJpegDecode(static_cast<IJpegDecode *>(iIntf));
            });
};

__attribute__((destructor))static void static_destructor() {
    ::android::hardware::details::getBnConstructorMap().erase(IJpegDecode::descriptor);
    ::android::hardware::details::getBsConstructorMap().erase(IJpegDecode::descriptor);
};

// Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.
// no default implementation for: ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> IJpegDecode::DoDecode(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& cinfo, const ::android::hardware::hidl_handle& out_info, const ::android::hardware::hidl_handle& in_info, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& region_info, int32_t samplesize, int32_t compressPos)
// no default implementation for: ::android::hardware::Return<void> IJpegDecode::Alloc_OutBuffer(int32_t outwidth, int32_t outheight, int32_t color_format, Alloc_OutBuffer_cb _hidl_cb)
// no default implementation for: ::android::hardware::Return<void> IJpegDecode::Alloc_InBuffer(int32_t size, Alloc_InBuffer_cb _hidl_cb)
// no default implementation for: ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> IJpegDecode::LockDevice()
// no default implementation for: ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> IJpegDecode::UnLockDevice()
// no default implementation for: ::android::hardware::Return<void> IJpegDecode::GetDecodeStatus(GetDecodeStatus_cb _hidl_cb)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IJpegDecode::interfaceChain(interfaceChain_cb _hidl_cb){
    _hidl_cb({
        ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode::descriptor,
        ::android::hidl::base::V1_0::IBase::descriptor,
    });
    return ::android::hardware::Void();}

::android::hardware::Return<void> IJpegDecode::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IJpegDecode::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IJpegDecode::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){106,196,221,203,183,4,97,68,117,94,171,142,124,114,121,135,32,10,117,144,123,3,84,127,151,107,34,165,17,3,223,187} /* 6ac4ddcbb7046144755eab8e7c727987200a75907b03547f976b22a51103dfbb */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IJpegDecode::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IJpegDecode::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IJpegDecode::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IJpegDecode::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = -1;
    info.ptr = 0;
    info.arch = 
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IJpegDecode::notifySyspropsChanged(){
    ::android::report_sysprop_change();
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IJpegDecode::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>> IJpegDecode::castFrom(const ::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode>> IJpegDecode::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::hardware::details::castInterface<IJpegDecode, ::android::hidl::base::V1_0::IBase, BpHwJpegDecode>(
            parent, "vendor.huawei.hardware.jpegdec@1.0::IJpegDecode", emitError);
}

BpHwJpegDecode::BpHwJpegDecode(const ::android::sp<::android::hardware::IBinder> &_hidl_impl)
        : BpInterface<IJpegDecode>(_hidl_impl),
          ::android::hardware::details::HidlInstrumentor("vendor.huawei.hardware.jpegdec@1.0", "IJpegDecode") {
}

// Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.
::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> BpHwJpegDecode::_hidl_DoDecode(::android::hardware::IInterface *_hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& cinfo, const ::android::hardware::hidl_handle& out_info, const ::android::hardware::hidl_handle& in_info, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& region_info, int32_t samplesize, int32_t compressPos) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this_instrumentor->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this_instrumentor->getInstrumentationCallbacks();
    #else
    (void) _hidl_this_instrumentor;
    #endif // __ANDROID_DEBUGGABLE__
    ::android::ScopedTrace PASTE(___tracer, __LINE__) (ATRACE_TAG_HAL, "HIDL::IJpegDecode::DoDecode::client");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&cinfo);
        _hidl_args.push_back((void *)&out_info);
        _hidl_args.push_back((void *)&in_info);
        _hidl_args.push_back((void *)&region_info);
        _hidl_args.push_back((void *)&samplesize);
        _hidl_args.push_back((void *)&compressPos);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "DoDecode", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error;

    _hidl_err = _hidl_data.writeInterfaceToken(BpHwJpegDecode::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    size_t _hidl_cinfo_parent;

    _hidl_err = _hidl_data.writeBuffer(&cinfo, sizeof(cinfo), &_hidl_cinfo_parent);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = writeEmbeddedToParcel(
            cinfo,
            &_hidl_data,
            _hidl_cinfo_parent,
            0 /* parentOffset */);

    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeNativeHandleNoDup(out_info);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeNativeHandleNoDup(in_info);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    size_t _hidl_region_info_parent;

    _hidl_err = _hidl_data.writeBuffer(&region_info, sizeof(region_info), &_hidl_region_info_parent);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeInt32(samplesize);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeInt32(compressPos);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::IInterface::asBinder(_hidl_this)->transact(1 /* DoDecode */, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::readFromParcel(&_hidl_status, _hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    _hidl_err = _hidl_reply.readInt32((int32_t *)&_hidl_out_error);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "DoDecode", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>(_hidl_out_error);

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>(_hidl_status);
}

::android::hardware::Return<void> BpHwJpegDecode::_hidl_Alloc_OutBuffer(::android::hardware::IInterface *_hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, int32_t outwidth, int32_t outheight, int32_t color_format, Alloc_OutBuffer_cb _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this_instrumentor->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this_instrumentor->getInstrumentationCallbacks();
    #else
    (void) _hidl_this_instrumentor;
    #endif // __ANDROID_DEBUGGABLE__
    if (_hidl_cb == nullptr) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_ILLEGAL_ARGUMENT,
                "Null synchronous callback passed.");
    }

    ::android::ScopedTrace PASTE(___tracer, __LINE__) (ATRACE_TAG_HAL, "HIDL::IJpegDecode::Alloc_OutBuffer::client");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&outwidth);
        _hidl_args.push_back((void *)&outheight);
        _hidl_args.push_back((void *)&color_format);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_OutBuffer", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error;
    ::android::hardware::hidl_handle _hidl_out_outinfo;

    _hidl_err = _hidl_data.writeInterfaceToken(BpHwJpegDecode::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeInt32(outwidth);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeInt32(outheight);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeInt32(color_format);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::IInterface::asBinder(_hidl_this)->transact(2 /* Alloc_OutBuffer */, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::readFromParcel(&_hidl_status, _hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    _hidl_err = _hidl_reply.readInt32((int32_t *)&_hidl_out_error);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    const native_handle_t *_hidl_out_outinfo_ptr;

    _hidl_err = _hidl_reply.readNullableNativeHandleNoDup(&_hidl_out_outinfo_ptr);

    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_out_outinfo = _hidl_out_outinfo_ptr;
    _hidl_cb(_hidl_out_error, _hidl_out_outinfo);

    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        _hidl_args.push_back((void *)&_hidl_out_outinfo);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_OutBuffer", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>();

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>(_hidl_status);
}

::android::hardware::Return<void> BpHwJpegDecode::_hidl_Alloc_InBuffer(::android::hardware::IInterface *_hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, int32_t size, Alloc_InBuffer_cb _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this_instrumentor->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this_instrumentor->getInstrumentationCallbacks();
    #else
    (void) _hidl_this_instrumentor;
    #endif // __ANDROID_DEBUGGABLE__
    if (_hidl_cb == nullptr) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_ILLEGAL_ARGUMENT,
                "Null synchronous callback passed.");
    }

    ::android::ScopedTrace PASTE(___tracer, __LINE__) (ATRACE_TAG_HAL, "HIDL::IJpegDecode::Alloc_InBuffer::client");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&size);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_InBuffer", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error;
    ::android::hardware::hidl_handle _hidl_out_outinfo;

    _hidl_err = _hidl_data.writeInterfaceToken(BpHwJpegDecode::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_data.writeInt32(size);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::IInterface::asBinder(_hidl_this)->transact(3 /* Alloc_InBuffer */, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::readFromParcel(&_hidl_status, _hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    _hidl_err = _hidl_reply.readInt32((int32_t *)&_hidl_out_error);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    const native_handle_t *_hidl_out_outinfo_ptr;

    _hidl_err = _hidl_reply.readNullableNativeHandleNoDup(&_hidl_out_outinfo_ptr);

    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_out_outinfo = _hidl_out_outinfo_ptr;
    _hidl_cb(_hidl_out_error, _hidl_out_outinfo);

    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        _hidl_args.push_back((void *)&_hidl_out_outinfo);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_InBuffer", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>();

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>(_hidl_status);
}

::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> BpHwJpegDecode::_hidl_LockDevice(::android::hardware::IInterface *_hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this_instrumentor->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this_instrumentor->getInstrumentationCallbacks();
    #else
    (void) _hidl_this_instrumentor;
    #endif // __ANDROID_DEBUGGABLE__
    ::android::ScopedTrace PASTE(___tracer, __LINE__) (ATRACE_TAG_HAL, "HIDL::IJpegDecode::LockDevice::client");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "LockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error;

    _hidl_err = _hidl_data.writeInterfaceToken(BpHwJpegDecode::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::IInterface::asBinder(_hidl_this)->transact(4 /* LockDevice */, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::readFromParcel(&_hidl_status, _hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    _hidl_err = _hidl_reply.readInt32((int32_t *)&_hidl_out_error);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "LockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>(_hidl_out_error);

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>(_hidl_status);
}

::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> BpHwJpegDecode::_hidl_UnLockDevice(::android::hardware::IInterface *_hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this_instrumentor->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this_instrumentor->getInstrumentationCallbacks();
    #else
    (void) _hidl_this_instrumentor;
    #endif // __ANDROID_DEBUGGABLE__
    ::android::ScopedTrace PASTE(___tracer, __LINE__) (ATRACE_TAG_HAL, "HIDL::IJpegDecode::UnLockDevice::client");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "UnLockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error;

    _hidl_err = _hidl_data.writeInterfaceToken(BpHwJpegDecode::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::IInterface::asBinder(_hidl_this)->transact(5 /* UnLockDevice */, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::readFromParcel(&_hidl_status, _hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    _hidl_err = _hidl_reply.readInt32((int32_t *)&_hidl_out_error);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "UnLockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>(_hidl_out_error);

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>(_hidl_status);
}

::android::hardware::Return<void> BpHwJpegDecode::_hidl_GetDecodeStatus(::android::hardware::IInterface *_hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, GetDecodeStatus_cb _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this_instrumentor->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this_instrumentor->getInstrumentationCallbacks();
    #else
    (void) _hidl_this_instrumentor;
    #endif // __ANDROID_DEBUGGABLE__
    if (_hidl_cb == nullptr) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_ILLEGAL_ARGUMENT,
                "Null synchronous callback passed.");
    }

    ::android::ScopedTrace PASTE(___tracer, __LINE__) (ATRACE_TAG_HAL, "HIDL::IJpegDecode::GetDecodeStatus::client");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "GetDecodeStatus", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::hardware::Status _hidl_status;

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error;
    bool _hidl_out_status;

    _hidl_err = _hidl_data.writeInterfaceToken(BpHwJpegDecode::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::IInterface::asBinder(_hidl_this)->transact(6 /* GetDecodeStatus */, _hidl_data, &_hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::readFromParcel(&_hidl_status, _hidl_reply);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    if (!_hidl_status.isOk()) { return _hidl_status; }

    _hidl_err = _hidl_reply.readInt32((int32_t *)&_hidl_out_error);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = _hidl_reply.readBool(&_hidl_out_status);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_cb(_hidl_out_error, _hidl_out_status);

    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        _hidl_args.push_back((void *)&_hidl_out_status);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "GetDecodeStatus", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>();

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>(_hidl_status);
}


// Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.
::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> BpHwJpegDecode::DoDecode(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& cinfo, const ::android::hardware::hidl_handle& out_info, const ::android::hardware::hidl_handle& in_info, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& region_info, int32_t samplesize, int32_t compressPos){
    ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>  _hidl_out = ::vendor::huawei::hardware::jpegdec::V1_0::BpHwJpegDecode::_hidl_DoDecode(this, this, cinfo, out_info, in_info, region_info, samplesize, compressPos);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::Alloc_OutBuffer(int32_t outwidth, int32_t outheight, int32_t color_format, Alloc_OutBuffer_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::huawei::hardware::jpegdec::V1_0::BpHwJpegDecode::_hidl_Alloc_OutBuffer(this, this, outwidth, outheight, color_format, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::Alloc_InBuffer(int32_t size, Alloc_InBuffer_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::huawei::hardware::jpegdec::V1_0::BpHwJpegDecode::_hidl_Alloc_InBuffer(this, this, size, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> BpHwJpegDecode::LockDevice(){
    ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>  _hidl_out = ::vendor::huawei::hardware::jpegdec::V1_0::BpHwJpegDecode::_hidl_LockDevice(this, this);

    return _hidl_out;
}

::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> BpHwJpegDecode::UnLockDevice(){
    ::android::hardware::Return<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>  _hidl_out = ::vendor::huawei::hardware::jpegdec::V1_0::BpHwJpegDecode::_hidl_UnLockDevice(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::GetDecodeStatus(GetDecodeStatus_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::huawei::hardware::jpegdec::V1_0::BpHwJpegDecode::_hidl_GetDecodeStatus(this, this, _hidl_cb);

    return _hidl_out;
}


// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BpHwJpegDecode::interfaceChain(interfaceChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_debug(this, this, fd, options);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceDescriptor(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::getHashChain(getHashChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getHashChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::setHALInstrumentation(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_setHALInstrumentation(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwJpegDecode::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    ::android::hardware::ProcessState::self()->startThreadPool();
    ::android::hardware::hidl_binder_death_recipient *binder_recipient = new ::android::hardware::hidl_binder_death_recipient(recipient, cookie, this);
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    _hidl_mDeathRecipients.push_back(binder_recipient);
    return (remote()->linkToDeath(binder_recipient) == ::android::OK);
}

::android::hardware::Return<void> BpHwJpegDecode::ping(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_ping(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getDebugInfo(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwJpegDecode::notifySyspropsChanged(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_notifySyspropsChanged(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwJpegDecode::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    for (auto it = _hidl_mDeathRecipients.rbegin();it != _hidl_mDeathRecipients.rend();++it) {
        if ((*it)->getRecipient() == recipient) {
            ::android::status_t status = remote()->unlinkToDeath(*it);
            _hidl_mDeathRecipients.erase(it.base()-1);
            return status == ::android::OK;
        }
    }
    return false;
}


BnHwJpegDecode::BnHwJpegDecode(const ::android::sp<IJpegDecode> &_hidl_impl)
        : ::android::hidl::base::V1_0::BnHwBase(_hidl_impl, "vendor.huawei.hardware.jpegdec@1.0", "IJpegDecode") { 
            _hidl_mImpl = _hidl_impl;
            auto prio = ::android::hardware::details::gServicePrioMap->get(_hidl_impl, {SCHED_NORMAL, 0});
            mSchedPolicy = prio.sched_policy;
            mSchedPriority = prio.prio;
            setRequestingSid(::android::hardware::details::gServiceSidMap->get(_hidl_impl, false));
}

BnHwJpegDecode::~BnHwJpegDecode() {
    ::android::hardware::details::gBnMap->eraseIfEqual(_hidl_mImpl.get(), this);
}

// Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.
::android::status_t BnHwJpegDecode::_hidl_DoDecode(
        ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        TransactCallback _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this->getInstrumentationCallbacks();
    #endif // __ANDROID_DEBUGGABLE__

    ::android::status_t _hidl_err = ::android::OK;
    if (!_hidl_data.enforceInterface(BnHwJpegDecode::Pure::descriptor)) {
        _hidl_err = ::android::BAD_TYPE;
        return _hidl_err;
    }

    ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t* cinfo;
    ::android::hardware::hidl_handle out_info;
    ::android::hardware::hidl_handle in_info;
    ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info* region_info;
    int32_t samplesize;
    int32_t compressPos;

    size_t _hidl_cinfo_parent;

    _hidl_err = _hidl_data.readBuffer(sizeof(*cinfo), &_hidl_cinfo_parent,  const_cast<const void**>(reinterpret_cast<void **>(&cinfo)));
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    _hidl_err = readEmbeddedFromParcel(
            const_cast<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t &>(*cinfo),
            _hidl_data,
            _hidl_cinfo_parent,
            0 /* parentOffset */);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    const native_handle_t *out_info_ptr;

    _hidl_err = _hidl_data.readNullableNativeHandleNoDup(&out_info_ptr);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    out_info = out_info_ptr;
    const native_handle_t *in_info_ptr;

    _hidl_err = _hidl_data.readNullableNativeHandleNoDup(&in_info_ptr);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    in_info = in_info_ptr;
    size_t _hidl_region_info_parent;

    _hidl_err = _hidl_data.readBuffer(sizeof(*region_info), &_hidl_region_info_parent,  const_cast<const void**>(reinterpret_cast<void **>(&region_info)));
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    _hidl_err = _hidl_data.readInt32(&samplesize);
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    _hidl_err = _hidl_data.readInt32(&compressPos);
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    atrace_begin(ATRACE_TAG_HAL, "HIDL::IJpegDecode::DoDecode::server");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)cinfo);
        _hidl_args.push_back((void *)&out_info);
        _hidl_args.push_back((void *)&in_info);
        _hidl_args.push_back((void *)region_info);
        _hidl_args.push_back((void *)&samplesize);
        _hidl_args.push_back((void *)&compressPos);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "DoDecode", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error = static_cast<IJpegDecode*>(_hidl_this->getImpl().get())->DoDecode(*cinfo, out_info, in_info, *region_info, samplesize, compressPos);

    ::android::hardware::writeToParcel(::android::hardware::Status::ok(), _hidl_reply);

    _hidl_err = _hidl_reply->writeInt32((int32_t)_hidl_out_error);
    /* _hidl_err ignored! */

    atrace_end(ATRACE_TAG_HAL);
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "DoDecode", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_cb(*_hidl_reply);
    return _hidl_err;
}

::android::status_t BnHwJpegDecode::_hidl_Alloc_OutBuffer(
        ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        TransactCallback _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this->getInstrumentationCallbacks();
    #endif // __ANDROID_DEBUGGABLE__

    ::android::status_t _hidl_err = ::android::OK;
    if (!_hidl_data.enforceInterface(BnHwJpegDecode::Pure::descriptor)) {
        _hidl_err = ::android::BAD_TYPE;
        return _hidl_err;
    }

    int32_t outwidth;
    int32_t outheight;
    int32_t color_format;

    _hidl_err = _hidl_data.readInt32(&outwidth);
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    _hidl_err = _hidl_data.readInt32(&outheight);
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    _hidl_err = _hidl_data.readInt32(&color_format);
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    atrace_begin(ATRACE_TAG_HAL, "HIDL::IJpegDecode::Alloc_OutBuffer::server");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&outwidth);
        _hidl_args.push_back((void *)&outheight);
        _hidl_args.push_back((void *)&color_format);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_OutBuffer", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    bool _hidl_callbackCalled = false;

    ::android::hardware::Return<void> _hidl_ret = static_cast<IJpegDecode*>(_hidl_this->getImpl().get())->Alloc_OutBuffer(outwidth, outheight, color_format, [&](const auto &_hidl_out_error, const auto &_hidl_out_outinfo) {
        if (_hidl_callbackCalled) {
            LOG_ALWAYS_FATAL("Alloc_OutBuffer: _hidl_cb called a second time, but must be called once.");
        }
        _hidl_callbackCalled = true;

        ::android::hardware::writeToParcel(::android::hardware::Status::ok(), _hidl_reply);

        _hidl_err = _hidl_reply->writeInt32((int32_t)_hidl_out_error);
        /* _hidl_err ignored! */

        _hidl_err = _hidl_reply->writeNativeHandleNoDup(_hidl_out_outinfo);
        /* _hidl_err ignored! */

        atrace_end(ATRACE_TAG_HAL);
        #ifdef __ANDROID_DEBUGGABLE__
        if (UNLIKELY(mEnableInstrumentation)) {
            std::vector<void *> _hidl_args;
            _hidl_args.push_back((void *)&_hidl_out_error);
            _hidl_args.push_back((void *)&_hidl_out_outinfo);
            for (const auto &callback: mInstrumentationCallbacks) {
                callback(InstrumentationEvent::SERVER_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_OutBuffer", &_hidl_args);
            }
        }
        #endif // __ANDROID_DEBUGGABLE__

        _hidl_cb(*_hidl_reply);
    });

    _hidl_ret.assertOk();
    if (!_hidl_callbackCalled) {
        LOG_ALWAYS_FATAL("Alloc_OutBuffer: _hidl_cb not called, but must be called once.");
    }

    return _hidl_err;
}

::android::status_t BnHwJpegDecode::_hidl_Alloc_InBuffer(
        ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        TransactCallback _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this->getInstrumentationCallbacks();
    #endif // __ANDROID_DEBUGGABLE__

    ::android::status_t _hidl_err = ::android::OK;
    if (!_hidl_data.enforceInterface(BnHwJpegDecode::Pure::descriptor)) {
        _hidl_err = ::android::BAD_TYPE;
        return _hidl_err;
    }

    int32_t size;

    _hidl_err = _hidl_data.readInt32(&size);
    if (_hidl_err != ::android::OK) { return _hidl_err; }

    atrace_begin(ATRACE_TAG_HAL, "HIDL::IJpegDecode::Alloc_InBuffer::server");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&size);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_InBuffer", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    bool _hidl_callbackCalled = false;

    ::android::hardware::Return<void> _hidl_ret = static_cast<IJpegDecode*>(_hidl_this->getImpl().get())->Alloc_InBuffer(size, [&](const auto &_hidl_out_error, const auto &_hidl_out_outinfo) {
        if (_hidl_callbackCalled) {
            LOG_ALWAYS_FATAL("Alloc_InBuffer: _hidl_cb called a second time, but must be called once.");
        }
        _hidl_callbackCalled = true;

        ::android::hardware::writeToParcel(::android::hardware::Status::ok(), _hidl_reply);

        _hidl_err = _hidl_reply->writeInt32((int32_t)_hidl_out_error);
        /* _hidl_err ignored! */

        _hidl_err = _hidl_reply->writeNativeHandleNoDup(_hidl_out_outinfo);
        /* _hidl_err ignored! */

        atrace_end(ATRACE_TAG_HAL);
        #ifdef __ANDROID_DEBUGGABLE__
        if (UNLIKELY(mEnableInstrumentation)) {
            std::vector<void *> _hidl_args;
            _hidl_args.push_back((void *)&_hidl_out_error);
            _hidl_args.push_back((void *)&_hidl_out_outinfo);
            for (const auto &callback: mInstrumentationCallbacks) {
                callback(InstrumentationEvent::SERVER_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "Alloc_InBuffer", &_hidl_args);
            }
        }
        #endif // __ANDROID_DEBUGGABLE__

        _hidl_cb(*_hidl_reply);
    });

    _hidl_ret.assertOk();
    if (!_hidl_callbackCalled) {
        LOG_ALWAYS_FATAL("Alloc_InBuffer: _hidl_cb not called, but must be called once.");
    }

    return _hidl_err;
}

::android::status_t BnHwJpegDecode::_hidl_LockDevice(
        ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        TransactCallback _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this->getInstrumentationCallbacks();
    #endif // __ANDROID_DEBUGGABLE__

    ::android::status_t _hidl_err = ::android::OK;
    if (!_hidl_data.enforceInterface(BnHwJpegDecode::Pure::descriptor)) {
        _hidl_err = ::android::BAD_TYPE;
        return _hidl_err;
    }

    atrace_begin(ATRACE_TAG_HAL, "HIDL::IJpegDecode::LockDevice::server");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "LockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error = static_cast<IJpegDecode*>(_hidl_this->getImpl().get())->LockDevice();

    ::android::hardware::writeToParcel(::android::hardware::Status::ok(), _hidl_reply);

    _hidl_err = _hidl_reply->writeInt32((int32_t)_hidl_out_error);
    /* _hidl_err ignored! */

    atrace_end(ATRACE_TAG_HAL);
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "LockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_cb(*_hidl_reply);
    return _hidl_err;
}

::android::status_t BnHwJpegDecode::_hidl_UnLockDevice(
        ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        TransactCallback _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this->getInstrumentationCallbacks();
    #endif // __ANDROID_DEBUGGABLE__

    ::android::status_t _hidl_err = ::android::OK;
    if (!_hidl_data.enforceInterface(BnHwJpegDecode::Pure::descriptor)) {
        _hidl_err = ::android::BAD_TYPE;
        return _hidl_err;
    }

    atrace_begin(ATRACE_TAG_HAL, "HIDL::IJpegDecode::UnLockDevice::server");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "UnLockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error _hidl_out_error = static_cast<IJpegDecode*>(_hidl_this->getImpl().get())->UnLockDevice();

    ::android::hardware::writeToParcel(::android::hardware::Status::ok(), _hidl_reply);

    _hidl_err = _hidl_reply->writeInt32((int32_t)_hidl_out_error);
    /* _hidl_err ignored! */

    atrace_end(ATRACE_TAG_HAL);
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&_hidl_out_error);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "UnLockDevice", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_cb(*_hidl_reply);
    return _hidl_err;
}

::android::status_t BnHwJpegDecode::_hidl_GetDecodeStatus(
        ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        TransactCallback _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this->getInstrumentationCallbacks();
    #endif // __ANDROID_DEBUGGABLE__

    ::android::status_t _hidl_err = ::android::OK;
    if (!_hidl_data.enforceInterface(BnHwJpegDecode::Pure::descriptor)) {
        _hidl_err = ::android::BAD_TYPE;
        return _hidl_err;
    }

    atrace_begin(ATRACE_TAG_HAL, "HIDL::IJpegDecode::GetDecodeStatus::server");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_ENTRY, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "GetDecodeStatus", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    bool _hidl_callbackCalled = false;

    ::android::hardware::Return<void> _hidl_ret = static_cast<IJpegDecode*>(_hidl_this->getImpl().get())->GetDecodeStatus([&](const auto &_hidl_out_error, const auto &_hidl_out_status) {
        if (_hidl_callbackCalled) {
            LOG_ALWAYS_FATAL("GetDecodeStatus: _hidl_cb called a second time, but must be called once.");
        }
        _hidl_callbackCalled = true;

        ::android::hardware::writeToParcel(::android::hardware::Status::ok(), _hidl_reply);

        _hidl_err = _hidl_reply->writeInt32((int32_t)_hidl_out_error);
        /* _hidl_err ignored! */

        _hidl_err = _hidl_reply->writeBool(_hidl_out_status);
        /* _hidl_err ignored! */

        atrace_end(ATRACE_TAG_HAL);
        #ifdef __ANDROID_DEBUGGABLE__
        if (UNLIKELY(mEnableInstrumentation)) {
            std::vector<void *> _hidl_args;
            _hidl_args.push_back((void *)&_hidl_out_error);
            _hidl_args.push_back((void *)&_hidl_out_status);
            for (const auto &callback: mInstrumentationCallbacks) {
                callback(InstrumentationEvent::SERVER_API_EXIT, "vendor.huawei.hardware.jpegdec", "1.0", "IJpegDecode", "GetDecodeStatus", &_hidl_args);
            }
        }
        #endif // __ANDROID_DEBUGGABLE__

        _hidl_cb(*_hidl_reply);
    });

    _hidl_ret.assertOk();
    if (!_hidl_callbackCalled) {
        LOG_ALWAYS_FATAL("GetDecodeStatus: _hidl_cb not called, but must be called once.");
    }

    return _hidl_err;
}


// Methods from ::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode follow.

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BnHwJpegDecode::ping() {
    return ::android::hardware::Void();
}
::android::hardware::Return<void> BnHwJpegDecode::getDebugInfo(getDebugInfo_cb _hidl_cb) {
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = ::android::hardware::details::getPidIfSharable();
    info.ptr = ::android::hardware::details::debuggable()? reinterpret_cast<uint64_t>(this) : 0;
    info.arch = 
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::status_t BnHwJpegDecode::onTransact(
        uint32_t _hidl_code,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        uint32_t _hidl_flags,
        TransactCallback _hidl_cb) {
    ::android::status_t _hidl_err = ::android::OK;

    switch (_hidl_code) {
        case 1 /* DoDecode */:
        {
            bool _hidl_is_oneway = _hidl_flags & 1u /* oneway */;
            if (_hidl_is_oneway != false) {
                return ::android::UNKNOWN_ERROR;
            }

            _hidl_err = ::vendor::huawei::hardware::jpegdec::V1_0::BnHwJpegDecode::_hidl_DoDecode(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 2 /* Alloc_OutBuffer */:
        {
            bool _hidl_is_oneway = _hidl_flags & 1u /* oneway */;
            if (_hidl_is_oneway != false) {
                return ::android::UNKNOWN_ERROR;
            }

            _hidl_err = ::vendor::huawei::hardware::jpegdec::V1_0::BnHwJpegDecode::_hidl_Alloc_OutBuffer(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 3 /* Alloc_InBuffer */:
        {
            bool _hidl_is_oneway = _hidl_flags & 1u /* oneway */;
            if (_hidl_is_oneway != false) {
                return ::android::UNKNOWN_ERROR;
            }

            _hidl_err = ::vendor::huawei::hardware::jpegdec::V1_0::BnHwJpegDecode::_hidl_Alloc_InBuffer(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 4 /* LockDevice */:
        {
            bool _hidl_is_oneway = _hidl_flags & 1u /* oneway */;
            if (_hidl_is_oneway != false) {
                return ::android::UNKNOWN_ERROR;
            }

            _hidl_err = ::vendor::huawei::hardware::jpegdec::V1_0::BnHwJpegDecode::_hidl_LockDevice(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 5 /* UnLockDevice */:
        {
            bool _hidl_is_oneway = _hidl_flags & 1u /* oneway */;
            if (_hidl_is_oneway != false) {
                return ::android::UNKNOWN_ERROR;
            }

            _hidl_err = ::vendor::huawei::hardware::jpegdec::V1_0::BnHwJpegDecode::_hidl_UnLockDevice(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        case 6 /* GetDecodeStatus */:
        {
            bool _hidl_is_oneway = _hidl_flags & 1u /* oneway */;
            if (_hidl_is_oneway != false) {
                return ::android::UNKNOWN_ERROR;
            }

            _hidl_err = ::vendor::huawei::hardware::jpegdec::V1_0::BnHwJpegDecode::_hidl_GetDecodeStatus(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

        default:
        {
            return ::android::hidl::base::V1_0::BnHwBase::onTransact(
                    _hidl_code, _hidl_data, _hidl_reply, _hidl_flags, _hidl_cb);
        }
    }

    if (_hidl_err == ::android::UNEXPECTED_NULL) {
        _hidl_err = ::android::hardware::writeToParcel(
                ::android::hardware::Status::fromExceptionCode(::android::hardware::Status::EX_NULL_POINTER),
                _hidl_reply);
    }return _hidl_err;
}

BsJpegDecode::BsJpegDecode(const ::android::sp<::vendor::huawei::hardware::jpegdec::V1_0::IJpegDecode> impl) : ::android::hardware::details::HidlInstrumentor("vendor.huawei.hardware.jpegdec@1.0", "IJpegDecode"), mImpl(impl) {
    mOnewayQueue.start(3000 /* similar limit to binderized */);
}

::android::hardware::Return<void> BsJpegDecode::addOnewayTask(std::function<void(void)> fun) {
    if (!mOnewayQueue.push(fun)) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_TRANSACTION_FAILED,
                "Passthrough oneway function queue exceeds maximum size.");
    }
    return ::android::hardware::Status();
}

::android::sp<IJpegDecode> IJpegDecode::tryGetService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwJpegDecode>(serviceName, false, getStub);
}

::android::sp<IJpegDecode> IJpegDecode::getService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwJpegDecode>(serviceName, true, getStub);
}

::android::status_t IJpegDecode::registerAsService(const std::string &serviceName) {
    return ::android::hardware::details::registerAsServiceInternal(this, serviceName);
}

bool IJpegDecode::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm
            = ::android::hardware::defaultServiceManager();
    if (sm == nullptr) {
        return false;
    }
    ::android::hardware::Return<bool> success =
            sm->registerForNotifications("vendor.huawei.hardware.jpegdec@1.0::IJpegDecode",
                    serviceName, notification);
    return success.isOk() && success;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

} // namespace V1_0
} // namespace jpegdec
} // namespace hardware
} // namespace huawei
} // namespace vendor
