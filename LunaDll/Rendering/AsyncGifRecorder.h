#pragma once
#ifndef AsyncGifRecorder_hhhhh
#define AsyncGifRecorder_hhhhh

#include <Windows.h>
#include "../Misc/ThreadedCmdQueue.h"
#include <memory>
#include <thread>
#include "RenderUtils.h"
#include <atomic>
#include <gif.h>

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
        BYTE* data;
    };

private:
    ThreadedCmdQueue<GifRecorderCMDItem> nextFrames;

    std::thread* m_workerThread;
    std::atomic_bool m_isRunning;
    
    //On worker Thread:
    std::wstring m_fileName;
    bool m_opened;
    bool m_error;
    GIF_H::GifWriter m_gifWriter;

public:
    AsyncGifRecorder();
    ~AsyncGifRecorder();
    
    
    inline bool isRunning() { return m_isRunning.load(std::memory_order_relaxed); }
    
    void init();
    void exitWorkerThread();

    void start();
    void stop();
    void addNextFrameToProcess(int width, int height, BYTE* pData);


    void workerFunc();

};


#endif
