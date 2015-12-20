#pragma once
#ifndef AsyncGifRecorder_hhhhh
#define AsyncGifRecorder_hhhhh

#include <Windows.h>
#include "../Misc/ThreadedCmdQueue.h"
#include <memory>
#include <thread>
#include "RenderUtils.h"
#include <wincodec.h>
#include <atomic>
#include "../Misc/FreeImageUtils/FreeImageGifData.h"

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
    std::unique_ptr<FreeImageGifData> m_gifWriter;
    /*
    IWICStream* m_curStream;
    IWICBitmapEncoder* m_curEncoder;
    std::vector<IWICBitmapFrameEncode*> m_curAllFrames;
    */
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
