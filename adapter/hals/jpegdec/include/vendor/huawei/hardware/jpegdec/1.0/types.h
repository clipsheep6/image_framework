#ifndef HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_TYPES_H
#define HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace vendor {
namespace huawei {
namespace hardware {
namespace jpegdec {
namespace V1_0 {

// Forward declaration for forward reference support:
enum class JPEG_Error : int32_t;
struct hwdecode_region_info;
struct jpeg_quant_hidl_tbl;
struct jpeg_huff_hidl_tbl;
struct jpeg_comp_hidl_info;
struct jpeg_decompress_hidl_t;

enum class JPEG_Error : int32_t {
    NONE = 0,
    BAD_DESCRIPTOR = 1,
    BAD_BUFFER = 2,
    BAD_VALUE = 3,
    NOT_SHARED = 4,
    NO_RESOURCES = 5,
    UNDEFINED = 6,
    UNSUPPORTED = 7,
};

struct hwdecode_region_info final {
    int32_t left __attribute__ ((aligned(4)));
    int32_t right __attribute__ ((aligned(4)));
    int32_t top __attribute__ ((aligned(4)));
    int32_t bottom __attribute__ ((aligned(4)));
    int32_t flag __attribute__ ((aligned(4)));
    int32_t rsv __attribute__ ((aligned(4)));
};

static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info, left) == 0, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info, right) == 4, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info, top) == 8, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info, bottom) == 12, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info, flag) == 16, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info, rsv) == 20, "wrong offset");
static_assert(sizeof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info) == 24, "wrong size");
static_assert(__alignof(::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info) == 4, "wrong alignment");

struct jpeg_quant_hidl_tbl final {
    ::android::hardware::hidl_array<uint16_t, 64> quantval __attribute__ ((aligned(2)));
    bool table_flag __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl, quantval) == 0, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl, table_flag) == 128, "wrong offset");
static_assert(sizeof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl) == 130, "wrong size");
static_assert(__alignof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl) == 2, "wrong alignment");

struct jpeg_huff_hidl_tbl final {
    ::android::hardware::hidl_array<uint8_t, 17> bits __attribute__ ((aligned(1)));
    ::android::hardware::hidl_array<uint8_t, 256> huffval __attribute__ ((aligned(1)));
    bool table_flag __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl, bits) == 0, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl, huffval) == 17, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl, table_flag) == 273, "wrong offset");
static_assert(sizeof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl) == 274, "wrong size");
static_assert(__alignof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl) == 1, "wrong alignment");

struct jpeg_comp_hidl_info final {
    int32_t component_id __attribute__ ((aligned(4)));
    int32_t component_index __attribute__ ((aligned(4)));
    int32_t h_samp_factor __attribute__ ((aligned(4)));
    int32_t v_samp_factor __attribute__ ((aligned(4)));
    int32_t quant_tbl_no __attribute__ ((aligned(4)));
    int32_t dc_tbl_no __attribute__ ((aligned(4)));
    int32_t ac_tbl_no __attribute__ ((aligned(4)));
    bool info_flag __attribute__ ((aligned(1)));
};

static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, component_id) == 0, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, component_index) == 4, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, h_samp_factor) == 8, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, v_samp_factor) == 12, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, quant_tbl_no) == 16, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, dc_tbl_no) == 20, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, ac_tbl_no) == 24, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info, info_flag) == 28, "wrong offset");
static_assert(sizeof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info) == 32, "wrong size");
static_assert(__alignof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info) == 4, "wrong alignment");

struct jpeg_decompress_hidl_t final {
    uint32_t image_width __attribute__ ((aligned(4)));
    uint32_t image_height __attribute__ ((aligned(4)));
    int32_t data_precision __attribute__ ((aligned(4)));
    int32_t num_components __attribute__ ((aligned(4)));
    uint32_t restart_interval __attribute__ ((aligned(4)));
    bool arith_code __attribute__ ((aligned(1)));
    bool progressive_mode __attribute__ ((aligned(1)));
    ::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info> comp_info __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl> dc_huff_tbl_ptrs __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl> ac_huff_tbl_ptrs __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl> quant_tbl_ptrs __attribute__ ((aligned(8)));
};

static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, image_width) == 0, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, image_height) == 4, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, data_precision) == 8, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, num_components) == 12, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, restart_interval) == 16, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, arith_code) == 20, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, progressive_mode) == 21, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, comp_info) == 24, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, dc_huff_tbl_ptrs) == 40, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, ac_huff_tbl_ptrs) == 56, "wrong offset");
static_assert(offsetof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t, quant_tbl_ptrs) == 72, "wrong offset");
static_assert(sizeof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t) == 88, "wrong size");
static_assert(__alignof(::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t) == 8, "wrong alignment");

//
// type declarations for package
//

template<typename>
static inline std::string toString(int32_t o);
static inline std::string toString(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error o);

constexpr int32_t operator|(const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const int32_t lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}
constexpr int32_t operator|(const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}
constexpr int32_t operator&(const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const int32_t lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}
constexpr int32_t operator&(const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}
constexpr int32_t &operator|=(int32_t& v, const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error e) {
    v |= static_cast<int32_t>(e);
    return v;
}
constexpr int32_t &operator&=(int32_t& v, const ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error e) {
    v &= static_cast<int32_t>(e);
    return v;
}

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& o);
static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& rhs);
static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& rhs);

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& o);
static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& rhs);
static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& rhs);

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& o);
static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& rhs);
static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& rhs);

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& o);
static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& rhs);
static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& rhs);

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& o);
static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& rhs);
static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& rhs);

//
// type header definitions for package
//

template<>
inline std::string toString<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error>(int32_t o) {
    using ::android::hardware::details::toHexString;
    std::string os;
    ::android::hardware::hidl_bitfield<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> flipped = 0;
    bool first = true;
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NONE) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NONE)) {
        os += (first ? "" : " | ");
        os += "NONE";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NONE;
    }
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_DESCRIPTOR) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_DESCRIPTOR)) {
        os += (first ? "" : " | ");
        os += "BAD_DESCRIPTOR";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_DESCRIPTOR;
    }
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_BUFFER) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_BUFFER)) {
        os += (first ? "" : " | ");
        os += "BAD_BUFFER";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_BUFFER;
    }
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_VALUE) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_VALUE)) {
        os += (first ? "" : " | ");
        os += "BAD_VALUE";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_VALUE;
    }
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NOT_SHARED) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NOT_SHARED)) {
        os += (first ? "" : " | ");
        os += "NOT_SHARED";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NOT_SHARED;
    }
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NO_RESOURCES) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NO_RESOURCES)) {
        os += (first ? "" : " | ");
        os += "NO_RESOURCES";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NO_RESOURCES;
    }
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNDEFINED) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNDEFINED)) {
        os += (first ? "" : " | ");
        os += "UNDEFINED";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNDEFINED;
    }
    if ((o & ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNSUPPORTED) == static_cast<int32_t>(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNSUPPORTED)) {
        os += (first ? "" : " | ");
        os += "UNSUPPORTED";
        first = false;
        flipped |= ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNSUPPORTED;
    }
    if (o != flipped) {
        os += (first ? "" : " | ");
        os += toHexString(o & (~flipped));
    }os += " (";
    os += toHexString(o);
    os += ")";
    return os;
}

static inline std::string toString(::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error o) {
    using ::android::hardware::details::toHexString;
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NONE) {
        return "NONE";
    }
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_DESCRIPTOR) {
        return "BAD_DESCRIPTOR";
    }
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_BUFFER) {
        return "BAD_BUFFER";
    }
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_VALUE) {
        return "BAD_VALUE";
    }
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NOT_SHARED) {
        return "NOT_SHARED";
    }
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NO_RESOURCES) {
        return "NO_RESOURCES";
    }
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNDEFINED) {
        return "UNDEFINED";
    }
    if (o == ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNSUPPORTED) {
        return "UNSUPPORTED";
    }
    std::string os;
    os += toHexString(static_cast<int32_t>(o));
    return os;
}

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".left = ";
    os += ::android::hardware::toString(o.left);
    os += ", .right = ";
    os += ::android::hardware::toString(o.right);
    os += ", .top = ";
    os += ::android::hardware::toString(o.top);
    os += ", .bottom = ";
    os += ::android::hardware::toString(o.bottom);
    os += ", .flag = ";
    os += ::android::hardware::toString(o.flag);
    os += ", .rsv = ";
    os += ::android::hardware::toString(o.rsv);
    os += "}"; return os;
}

static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& rhs) {
    if (lhs.left != rhs.left) {
        return false;
    }
    if (lhs.right != rhs.right) {
        return false;
    }
    if (lhs.top != rhs.top) {
        return false;
    }
    if (lhs.bottom != rhs.bottom) {
        return false;
    }
    if (lhs.flag != rhs.flag) {
        return false;
    }
    if (lhs.rsv != rhs.rsv) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::hwdecode_region_info& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".quantval = ";
    os += ::android::hardware::toString(o.quantval);
    os += ", .table_flag = ";
    os += ::android::hardware::toString(o.table_flag);
    os += "}"; return os;
}

static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& rhs) {
    if (lhs.quantval != rhs.quantval) {
        return false;
    }
    if (lhs.table_flag != rhs.table_flag) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_quant_hidl_tbl& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".bits = ";
    os += ::android::hardware::toString(o.bits);
    os += ", .huffval = ";
    os += ::android::hardware::toString(o.huffval);
    os += ", .table_flag = ";
    os += ::android::hardware::toString(o.table_flag);
    os += "}"; return os;
}

static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& rhs) {
    if (lhs.bits != rhs.bits) {
        return false;
    }
    if (lhs.huffval != rhs.huffval) {
        return false;
    }
    if (lhs.table_flag != rhs.table_flag) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_huff_hidl_tbl& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".component_id = ";
    os += ::android::hardware::toString(o.component_id);
    os += ", .component_index = ";
    os += ::android::hardware::toString(o.component_index);
    os += ", .h_samp_factor = ";
    os += ::android::hardware::toString(o.h_samp_factor);
    os += ", .v_samp_factor = ";
    os += ::android::hardware::toString(o.v_samp_factor);
    os += ", .quant_tbl_no = ";
    os += ::android::hardware::toString(o.quant_tbl_no);
    os += ", .dc_tbl_no = ";
    os += ::android::hardware::toString(o.dc_tbl_no);
    os += ", .ac_tbl_no = ";
    os += ::android::hardware::toString(o.ac_tbl_no);
    os += ", .info_flag = ";
    os += ::android::hardware::toString(o.info_flag);
    os += "}"; return os;
}

static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& rhs) {
    if (lhs.component_id != rhs.component_id) {
        return false;
    }
    if (lhs.component_index != rhs.component_index) {
        return false;
    }
    if (lhs.h_samp_factor != rhs.h_samp_factor) {
        return false;
    }
    if (lhs.v_samp_factor != rhs.v_samp_factor) {
        return false;
    }
    if (lhs.quant_tbl_no != rhs.quant_tbl_no) {
        return false;
    }
    if (lhs.dc_tbl_no != rhs.dc_tbl_no) {
        return false;
    }
    if (lhs.ac_tbl_no != rhs.ac_tbl_no) {
        return false;
    }
    if (lhs.info_flag != rhs.info_flag) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_comp_hidl_info& rhs){
    return !(lhs == rhs);
}

static inline std::string toString(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& o) {
    using ::android::hardware::toString;
    std::string os;
    os += "{";
    os += ".image_width = ";
    os += ::android::hardware::toString(o.image_width);
    os += ", .image_height = ";
    os += ::android::hardware::toString(o.image_height);
    os += ", .data_precision = ";
    os += ::android::hardware::toString(o.data_precision);
    os += ", .num_components = ";
    os += ::android::hardware::toString(o.num_components);
    os += ", .restart_interval = ";
    os += ::android::hardware::toString(o.restart_interval);
    os += ", .arith_code = ";
    os += ::android::hardware::toString(o.arith_code);
    os += ", .progressive_mode = ";
    os += ::android::hardware::toString(o.progressive_mode);
    os += ", .comp_info = ";
    os += ::android::hardware::toString(o.comp_info);
    os += ", .dc_huff_tbl_ptrs = ";
    os += ::android::hardware::toString(o.dc_huff_tbl_ptrs);
    os += ", .ac_huff_tbl_ptrs = ";
    os += ::android::hardware::toString(o.ac_huff_tbl_ptrs);
    os += ", .quant_tbl_ptrs = ";
    os += ::android::hardware::toString(o.quant_tbl_ptrs);
    os += "}"; return os;
}

static inline bool operator==(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& rhs) {
    if (lhs.image_width != rhs.image_width) {
        return false;
    }
    if (lhs.image_height != rhs.image_height) {
        return false;
    }
    if (lhs.data_precision != rhs.data_precision) {
        return false;
    }
    if (lhs.num_components != rhs.num_components) {
        return false;
    }
    if (lhs.restart_interval != rhs.restart_interval) {
        return false;
    }
    if (lhs.arith_code != rhs.arith_code) {
        return false;
    }
    if (lhs.progressive_mode != rhs.progressive_mode) {
        return false;
    }
    if (lhs.comp_info != rhs.comp_info) {
        return false;
    }
    if (lhs.dc_huff_tbl_ptrs != rhs.dc_huff_tbl_ptrs) {
        return false;
    }
    if (lhs.ac_huff_tbl_ptrs != rhs.ac_huff_tbl_ptrs) {
        return false;
    }
    if (lhs.quant_tbl_ptrs != rhs.quant_tbl_ptrs) {
        return false;
    }
    return true;
}

static inline bool operator!=(const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& lhs, const ::vendor::huawei::hardware::jpegdec::V1_0::jpeg_decompress_hidl_t& rhs){
    return !(lhs == rhs);
}


} // namespace V1_0
} // namespace jpegdec
} // namespace hardware
} // namespace huawei
} // namespace vendor

//
// global type declarations for package
//

namespace android {
namespace hardware {
namespace details {
template<> constexpr std::array<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error, 8> hidl_enum_values<::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error> = {
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NONE,
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_DESCRIPTOR,
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_BUFFER,
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::BAD_VALUE,
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NOT_SHARED,
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::NO_RESOURCES,
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNDEFINED,
    ::vendor::huawei::hardware::jpegdec::V1_0::JPEG_Error::UNSUPPORTED,
};
} // namespace details
} // namespace hardware
} // namespace android


#endif // HIDL_GENERATED_VENDOR_HUAWEI_HARDWARE_JPEGDEC_V1_0_TYPES_H
