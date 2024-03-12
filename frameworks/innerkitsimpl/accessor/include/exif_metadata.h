#ifndef EXIF_METADATA_H_
#define EXIF_METADATA_H_

#include <stdint.h>
#include <string>
#include <libexif/exif-data.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-tag.h>

namespace OHOS {
namespace Media {
class ExifMetadata {
public:
    ExifMetadata();
    ExifMetadata(ExifData *exifData);
    ~ExifMetadata();
    int GetValue(const std::string &key, std::string &value) const;
    bool SetValue(const std::string &key, const std::string &value);
    int32_t SetValue_(const std::string &key, const std::string &value);
    ExifData* GetData();
    bool CreateExifdata();
private:
    ExifEntry* InitExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag);
    ExifEntry* CreateExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len, ExifFormat format);
    ExifEntry* GetExifTag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len);
    ExifIfd GetExifIfdByExifTag(const ExifTag &tag);
    ExifFormat GetExifFormatByExifTag(const ExifTag &tag);
    std::string GetExifNameByExifTag(const ExifTag &tag);
    bool CreateExifEntryOfBitsPerSample(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool CreateExifEntryOfRationalExif(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry, const std::string& separator, size_t sepSize);
    bool CreateExifEntryOfGpsTimeStamp(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool CreateExifEntryOfCompressedBitsPerPixel(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool CreateExifEntryOfGpsLatitudeOrLongitude(const ExifTag &tag, ExifData *data, const std::string &value,
        ExifByteOrder order, ExifEntry **ptrEntry);
    bool SetGpsDegreeRational(ExifData *data, ExifEntry **ptrEntry, ExifByteOrder order, const ExifTag &tag,
    const std::vector<std::string> &dataVec);
    bool SetGpsRationals(ExifData *data, ExifEntry **ptrEntry, ExifByteOrder order, const ExifTag &tag,
        const std::vector<ExifRational> &exifRationals);
private:
    ExifData *exifData_;
};
} // namespace Media
} // namespace OHOS

#endif
