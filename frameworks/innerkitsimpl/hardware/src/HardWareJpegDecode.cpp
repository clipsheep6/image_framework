#include "HardWareJpegDecode.h"
#include <thread>
#include <mutex>

using namespace std;

std::mutex decodeMutex;
JepgFuncs* g_JpegDecode;

HardWareJpegDecode::HardWareJpegDecode() {
    //if deviceFlag ==0, you can start decode.else if deviceFlag == 1,you must wait.
    deviceFlag = 0;
    g_JpegDecode = nullptr;
    run("HareWareJpegDecodeThread");
}

HardWareJpegDecode::~HardWareJpegDecode() {
    if(g_JpegDecode != nullptr) {
        free(g_JpegDecode);
    }
    HiLog::Info(LABEL, "Do ~HardWareJpegDecode success");
    requestExitAndWait();
}

bool HardWareJpegDecode::HardWareJpegLockDevice() {
    HiLog::Info(LABEL, "lock Device in");
    if (deviceFlag == 1) {
        return false;
    } else {
        decodeMutex.try_lock();
        if (g_JpegDecode == nullptr) {
            //we need hardware function to start hardwareservice
            //g_JpegDecode = TryGetService();
            HiLog::Info(LABEL, "g_JpegDecode = TryGetService()");
        }
    }
    //get decode capability
    if (!g_JpegDecode->jpeg_get_decode_capability()) {
        HiLog(LABEL, "need softwaredecode ");
        return false;
    }
    //lock hardware device
    //auto ret = g_JpegDecode->LockDevice();
    HiLog::Info(LABEL, "ret = g_JpegDecode->LockDevice()");
    if (!ret.isOK()) {
        HiLog(LABEL, "hardware lock device fail!");
        return false;
    }
    HiLog::Info(LABEL, "lock Device success");
    return true;
}

bool HardWareJpegDecode::HardWareJpegUnLockDevice() {
    HiLog::Info(LABEL, "unlock Device in");
    if (g_JpegDecode == nullptr) {
        HiLog(LABEL, "g_JpegDevice is null, hardware unlock device fail!");
        return false;
    }
    //unlock hardware device
    //auto ret = g_JpegDecode->UnLockDevice();
    HiLog::Info(LABEL, "ret = g_JpegDecode->UnLockDevice()");
    if (!ret.isOK()) {
        HiLog(LABEL, "hardware unlock device fail!");
        return false;
    }
    HiLog::Info(LABEL, "unlock Device success");
    return true;
}

void HardWareJpegDecode::HardWareJpegGetInBuffer(uint32_t size, BufferHandle **inBuffer) {
    HiLog::Info(LABEL, "HardWareJpegGetInBuffer in");
    if (inBuffer == nullptr) {
        HiLog(LABEL, "inBuffer is null, hardware getInBuffer fail!");
        return;
    }
    if (g_JpegDecode == nullptr) {
        HiLog(LABEL, "g_JpegDevice is null, hardware getInBuffer fail!");
        return;
    }
    auto ret = g_JpegDecode->jpeg_allocate_buffer(size, inBuffer);
    if (!ret.isOK()) {
        HiLog::Info(LABEL, "HardWareJpegGetInBuffer fail");
        return;
    }
    HiLog::Info(LABEL, "HardWareJpegGetInBuffer success");
}

void HardWareJpegDecode::HardWareJpegGetOutBuffer(uint32_t size, BufferHandle **outBuffer) {
    HiLog::Info(LABEL, "HardWareJpegGetOutBuffer in");
    if (inBuffer == nullptr) {
        HiLog(LABEL, "inBuffer is null, hardware GetOutBuffer fail!");
        return;
    }
    if (g_JpegDecode == nullptr) {
        HiLog(LABEL, "g_JpegDevice is null, hardware GetOutBuffer fail!");
        return;
    }
    auto ret = g_JpegDecode->jpeg_allocate_buffer(size, outBuffer);
    if (!ret.isOK()) {
        HiLog::Info(LABEL, "HardWareJpegGetOutBuffer fail");
        return;
    }
    HiLog::Info(LABEL, "HardWareJpegGetOutBuffer success");
}

void HardWareJpegDecode::HardWareJpegReadInfo() {
    //i think this function used to change info to format data
    HiLog::Info(LABEL, "HardWareJpegReadInfo get");
}

JpegDecodeErrorCode HardWareJpegDecode::HardWareJpegDoDecode(BufferHandle* inBuffer, BufferHandle* outBuffer, JpegDecodeOption* option) {
    HiLog::Info(LABEL, "HardWareJpegDoDecode in");
    if (g_JpegDecode == nullptr) {
        HiLog::Info(LABEL, "g_JpegDevice is null,HardWareJpegDoDecode fail");
        return JpegDecodeErrorCode::JPEG_DECODE_FAILURE;
    }
    auto ret = g_JpegDecode->jpeg_hardware_decompress(inBuffer, outBuffer, option);
    if (!ret.isOK()) {
        HiLog::Info(LABEL, "HardWareJpegDoDecode fail");
    }
    HiLog::Info(LABEL, "HardWareJpegDoDecode out");
    return ret;
}

void HardWareJpegDecode::HardWareJpegReleaseInBuffer(BufferHandle **inBuffer) {
    HiLog::Info(LABEL, "HardWareJpegReleaseInBuffer in");
    if (inBuffer != nullptr) {
        g_JpegDecode->jpeg_freeBuffer(inBuffer);
        free(*inBuffer);
        free(inBuffer);
        inBuffer = nullptr;
    }
    HiLog::Info(LABEL, "HardWareJpegReleaseInBuffer out");
}

void HardWareJpegDecode::WaitForEvent() const {
    HiLog::Info(LABEL, "WaitForEvent");
    sleep(100);
}

bool HardWareJpegDecode::threadLoop() override {
    while (!exitPending()) {
        WaitForEvent();
    }
    return false;
}

status_t HardWareJpegDecode::readyToRun() override {
    return Thread::readyToRun();
}