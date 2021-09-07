#define LOG_TAG "vendor.huawei.hardware.jpegdec@1.0::types"

#include <log/log.h>
#include <cutils/trace.h>
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <vendor/huawei/hardware/jpegdec/1.0/types.h>
#include <vendor/huawei/hardware/jpegdec/1.0/hwtypes.h>

namespace vendor {
namespace huawei {
namespace hardware {
namespace jpegdec {
namespace V1_0 {

::android::status_t readEmbeddedFromParcel(
        const jpeg_decompress_hidl_t &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_comp_info_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info> &>(obj.comp_info),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, comp_info), &_hidl_comp_info_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    size_t _hidl_dc_huff_tbl_ptrs_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl> &>(obj.dc_huff_tbl_ptrs),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, dc_huff_tbl_ptrs), &_hidl_dc_huff_tbl_ptrs_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    size_t _hidl_ac_huff_tbl_ptrs_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl> &>(obj.ac_huff_tbl_ptrs),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, ac_huff_tbl_ptrs), &_hidl_ac_huff_tbl_ptrs_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    size_t _hidl_quant_tbl_ptrs_child;

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl> &>(obj.quant_tbl_ptrs),
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, quant_tbl_ptrs), &_hidl_quant_tbl_ptrs_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    return _hidl_err;
}

::android::status_t writeEmbeddedToParcel(
        const jpeg_decompress_hidl_t &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset) {
    ::android::status_t _hidl_err = ::android::OK;

    size_t _hidl_comp_info_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.comp_info,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, comp_info), &_hidl_comp_info_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    size_t _hidl_dc_huff_tbl_ptrs_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.dc_huff_tbl_ptrs,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, dc_huff_tbl_ptrs), &_hidl_dc_huff_tbl_ptrs_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    size_t _hidl_ac_huff_tbl_ptrs_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.ac_huff_tbl_ptrs,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, ac_huff_tbl_ptrs), &_hidl_ac_huff_tbl_ptrs_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    size_t _hidl_quant_tbl_ptrs_child;

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            obj.quant_tbl_ptrs,
            parcel,
            parentHandle,
            parentOffset + offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, quant_tbl_ptrs), &_hidl_quant_tbl_ptrs_child);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    return _hidl_err;
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
