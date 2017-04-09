#include <iostream>
#include "AsyncGifRecorder.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include <gif.h>

AsyncGifRecorder::AsyncGifRecorder() : 
    m_workerThread(nullptr),
    m_isRunning(),
    m_fileName(),
    m_error(false), m_opened(false)
{
    m_isRunning.store(false, std::memory_order_relaxed);
}

AsyncGifRecorder::~AsyncGifRecorder()
{
    if (m_workerThread) {
        if (m_workerThread->joinable()) {
            exitWorkerThread();
            m_workerThread->join();
        }
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
    m_workerThread = std::make_unique<std::thread>([this]() { workerFunc(); });
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
            std::wstring screenshotPath = gAppPathWCHAR + std::wstring(L"\\gif-recordings");
            if (GetFileAttributesW(screenshotPath.c_str()) & INVALID_FILE_ATTRIBUTES) {
                CreateDirectoryW(screenshotPath.c_str(), NULL);
            }
            screenshotPath += L"\\";
            screenshotPath += Str2WStr(generateTimestampForFilename()) + std::wstring(L".gif");

            m_fileName = screenshotPath;
            m_opened = false;
            m_error = false;
            memset(&m_gifWriter, 0, sizeof(GIF_H::GifWriter));

            m_isRunning.store(true, std::memory_order_relaxed);
            break;
        }
        case GIFREC_NEXTFRAME:
        {
            if (!m_isRunning.load(std::memory_order_relaxed))
                continue;

            if ((!m_opened) && (!m_error))
            {
                m_opened = GIF_H::GifBegin(&m_gifWriter, WStr2Str(m_fileName).c_str(), nextData.width, nextData.height, 3, 8, false);
                m_error = !m_opened;
            }

            if (!m_error)
            {
                if (!GIF_H::GifWriteFrame(&m_gifWriter, nextData.data, nextData.width, nextData.height, 3, 8, false))
                {
                    GIF_H::GifEnd(&m_gifWriter);
                    m_error = true;
                    m_opened = false;
                }
            }

            break;
        }
        case GIFREC_STOP:
        {
            if (!m_isRunning.load(std::memory_order_relaxed))
                continue;
            
            if (!m_error)
            {
                GIF_H::GifEnd(&m_gifWriter);
            }
            memset(&m_gifWriter, 0, sizeof(GIF_H::GifWriter));
            m_opened = false;

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

