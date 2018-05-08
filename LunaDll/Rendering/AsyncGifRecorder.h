#pragma once
#ifndef AsyncGifRecorder_hhhhh
#define AsyncGifRecorder_hhhhh

#include <Windows.h>
#include "../Misc/ThreadedCmdQueue.h"
#include <memory>
#include <thread>
#include <mutex>
#include "RenderUtils.h"
#include <atomic>
#include <gif.h>
#include <memory>

namespace GIF_H
{
    struct GifWriter;
}

class AsyncGifRecorder
{
public:
    enum GifRecorderCMD {
        GIFREC_START,
        GIFREC_NEXTFRAME,
        GIFREC_STOP,
        GIFREC_EXIT
    };

    struct GifRecorderCMDItem {
        GifRecorderCMD cmd;
        int width;
        int height;
        uint32_t timestamp;
        BYTE* data;
    };

private:
    ThreadedCmdQueue<GifRecorderCMDItem> nextFrames;

    std::unique_ptr<std::thread> m_workerThread;
    std::atomic_bool m_isEncoding;
    std::atomic_bool m_isRunning;
    std::mutex m_StateChangeMutex;
    std::atomic_uint32_t m_BufferCount;
    
    //On worker Thread:
    std::wstring m_fileName;
    bool m_opened;
    bool m_error;
    GIF_H::GifWriter *m_gifWriter;
    uint32_t mLastTimestamp;

public:
    AsyncGifRecorder();
    ~AsyncGifRecorder();
    
    
    inline bool isEncoding() { return m_isEncoding.load(std::memory_order_relaxed); }
    inline bool isRunning() { return m_isRunning.load(std::memory_order_relaxed); }
    inline uint32_t bufferLen() { return m_BufferCount.load(std::memory_order_relaxed); }
    
    void init();
    void exitWorkerThread();

    void start();
    void stop();
    void addNextFrameToProcess(int width, int height, BYTE* pData, uint32_t timestamp);


    void workerFunc();

};


#endif
