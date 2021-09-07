#ifndef HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_HWTYPES_H
#define HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_HWTYPES_H

#include <vendor/huawei/hardware/jpegdec/1.0/types.h>


#include <hidl/Status.h>
#include <hwbinder/IBinder.h>
#include <hwbinder/Parcel.h>

namespace vendor {
namespace huawei {
namespace hardware {
namespace jpegdec {
namespace V1_0 {
::android::status_t readEmbeddedFromParcel(
        const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t writeEmbeddedToParcel(
        const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset);

} // namespace V1_0
} // namespace jpegdec
} // namespace hardware
} // namespace huawei
} // namespace vendor

#endif // HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_HWTYPES_H
