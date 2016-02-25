#include <iostream>
#include "AsyncGifRecorder.h"
#include "../GlobalFuncs.h"
#define FREEIMAGE_LIB
#include <FreeImageLite.h>

AsyncGifRecorder::AsyncGifRecorder() : 
    m_workerThread(nullptr),
    m_isRunning()
{
    m_isRunning.store(false, std::memory_order_relaxed);
}

AsyncGifRecorder::~AsyncGifRecorder()
{
    if (m_workerThread->joinable()){
        exitWorkerThread();
        m_workerThread->join();
    }
}


void AsyncGifRecorder::addNextFrameToProcess(int width, int height, BYTE* pData)
{
    if (!m_isRunning)
        return;

    GifRecorderCMDItem nextFrameItem;
    nextFrameItem.cmd = GIFREC_NEXTFRAME;
    nextFrameItem.width = width;
    nextFrameItem.height = height;
    nextFrameItem.data = pData;

    nextFrames.push(nextFrameItem);
}

void AsyncGifRecorder::init()
{
    m_workerThread = new std::thread([this](){ workerFunc();});
}

void AsyncGifRecorder::exitWorkerThread()
{
    GifRecorderCMDItem exitData;
    exitData.cmd = GIFREC_EXIT;
    nextFrames.push(exitData);
}

void AsyncGifRecorder::workerFunc()
{
    while (true) {
		if (nextFrames.empty())return;
        GifRecorderCMDItem nextData = nextFrames.pop();
        
        std::unique_ptr<BYTE[]> wrappedData;
        if (nextData.data)
            wrappedData.reset(nextData.data);

        if (nextData.cmd == GIFREC_EXIT)
            return;
        
        bool doCleanup = false;
        switch (nextData.cmd)
        {
        case GIFREC_START:
        {
            if (m_isRunning.load(std::memory_order_relaxed))
                continue;
            std::wstring screenshotPath = getModulePath() + std::wstring(L"\\gif-recordings");
            if (GetFileAttributesW(screenshotPath.c_str()) & INVALID_FILE_ATTRIBUTES) {
                CreateDirectoryW(screenshotPath.c_str(), NULL);
            }
            screenshotPath += L"\\";
            screenshotPath += Str2WStr(generateTimestampForFilename()) + std::wstring(L".gif");
            m_gifWriter.reset(new FreeImageGifData(WStr2Str(screenshotPath)));

            m_isRunning.store(true, std::memory_order_relaxed);
            break;
        }
        case GIFREC_NEXTFRAME:
        {
            if (!m_isRunning.load(std::memory_order_relaxed))
                continue;
            m_gifWriter->add24bitBGRDataPage(nextData.width, nextData.height, nextData.data);
            break;
        }
        case GIFREC_STOP:
        {
            if (!m_isRunning.load(std::memory_order_relaxed))
                continue;
            m_gifWriter->closeAndCleanup();

            m_isRunning.store(false, std::memory_order_relaxed);
            break;
        }
        default:
            break;
        }

    }
    
}

void AsyncGifRecorder::start()
{
    GifRecorderCMDItem startCmd;
    memset(&startCmd, 0, sizeof(startCmd));
    startCmd.cmd = GIFREC_START;
    nextFrames.push(startCmd);
}

void AsyncGifRecorder::stop()
{
    GifRecorderCMDItem stopCmd;
    memset(&stopCmd, 0, sizeof(stopCmd));
    stopCmd.cmd = GIFREC_STOP;
    nextFrames.push(stopCmd);
}

