#ifndef HARDWARE_JPEG_DECODE_H
#define HARDWARE_JPEG_DECODE_H

#include "JpegDecode"
#include "hardware_jpeg_decode.h"
#include "hardware_jpeg_types.h"
#include <thread>

using namespace std;

class HareWareJpegDecode : public std::Thread {
public:
    HareWareJpegDecode();
    ~HardWareJpegDecode();
    static bool HardWareJpegLockDevice();
    bool HardWareJpegUnLockDevice();
    void HardWareJpegGetInBuffer(uint32_t size, BufferHandle **inBuffer);
    void HardWareJpegGetOutBuffer(uint32_t size, BufferHandle **outBuffer);
    void HardWareJpegReadInfo();
    static JpegDecodeErrorCode HardWareJpegDoDecode(BufferHandle* inBuffer, BufferHandle* outBuffer, JpegDecodeOption* option);
    void HardWareJpegReleaseInBuffer(BufferHandle **inBuffer);
    void WaitForEvent() const;
    bool threadLoop() override;
    status_t readyToRun() override;
    static int32_t deviceFlag;


};



#endif // HARDWARE_JPEG_DECODE_H