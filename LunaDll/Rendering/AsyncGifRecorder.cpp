#include <iostream>
#include "AsyncGifRecorder.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../../libs/gif-h/gif.h"

AsyncGifRecorder::AsyncGifRecorder() : 
    m_workerThread(nullptr),
    m_isEncoding(),
    m_isRunning(),
    m_StateChangeMutex(),
    m_BufferCount(),
    m_fileName(),
    m_error(false), m_opened(false),
    m_gifWriter(new GIF_H::GifWriter()),
    mLastTimestamp(0)
{
    m_isEncoding.store(false, std::memory_order_relaxed);
    m_isRunning.store(false, std::memory_order_relaxed);
    m_BufferCount.store(0, std::memory_order_relaxed);
}

AsyncGifRecorder::~AsyncGifRecorder()
{
    if (m_workerThread->joinable()){
        exitWorkerThread();
        m_workerThread->join();
    }
    delete m_gifWriter;
    m_gifWriter = nullptr;
}


void AsyncGifRecorder::addNextFrameToProcess(int width, int height, BYTE* pData, uint32_t timestamp)
{
    std::unique_lock<std::mutex> lck(m_StateChangeMutex);
    if (!isRunning())
    {
        if (pData)
        {
            delete pData;
        }
        return;
    }

    GifRecorderCMDItem nextFrameItem;
    nextFrameItem.cmd = GIFREC_NEXTFRAME;
    nextFrameItem.width = width;
    nextFrameItem.height = height;
    nextFrameItem.timestamp = timestamp;
    nextFrameItem.data = pData;

    m_BufferCount++;
    nextFrames.push(nextFrameItem);
}

void AsyncGifRecorder::init()
{
    m_workerThread = new std::thread([this](){ workerFunc(); });
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
            if (m_isEncoding.load(std::memory_order_relaxed))
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
            memset(m_gifWriter, 0, sizeof(GIF_H::GifWriter));

            m_isEncoding.store(true, std::memory_order_relaxed);
            break;
        }
        case GIFREC_NEXTFRAME:
        {
            if (!m_isEncoding.load(std::memory_order_relaxed))
            {
                m_BufferCount--;
                continue;
            }

            if ((!m_opened) && (!m_error))
            {
                m_opened = GIF_H::GifBegin(m_gifWriter, WStr2Str(m_fileName).c_str(), nextData.width, nextData.height, 3, 8, false);
                m_error = !m_opened;
                mLastTimestamp = nextData.timestamp;
            }
            else if (!m_error)
            {
                // Edit last frame's delay
                uint32_t diffMs = nextData.timestamp - mLastTimestamp;
                GIF_H::GifOverwriteLastDelay(m_gifWriter, diffMs / 10);

                mLastTimestamp = nextData.timestamp - (diffMs % 10);
            }

            if (!m_error)
            {
                if (!GIF_H::GifWriteFrame(m_gifWriter, nextData.data, nextData.width, nextData.height, 3, 8, false))
                {
                    GIF_H::GifEnd(m_gifWriter);
                    m_error = true;
                    m_opened = false;
                }
            }

            m_BufferCount--;
            break;
        }
        case GIFREC_STOP:
        {
            if (!m_isEncoding.load(std::memory_order_relaxed))
                continue;
            
            if (!m_error)
            {
                GIF_H::GifEnd(m_gifWriter);
            }
            memset(m_gifWriter, 0, sizeof(GIF_H::GifWriter));
            m_opened = false;

            m_isEncoding.store(false, std::memory_order_relaxed);
            break;
        }
        default:
            break;
        }

    }
    
}

void AsyncGifRecorder::start()
{
    std::unique_lock<std::mutex> lck(m_StateChangeMutex);
    if (isRunning())
    {
        return;
    }
    GifRecorderCMDItem startCmd;
    memset(&startCmd, 0, sizeof(startCmd));
    startCmd.cmd = GIFREC_START;
    nextFrames.push(startCmd);
    m_isRunning.store(true, std::memory_order_relaxed);
}

void AsyncGifRecorder::stop()
{
    std::unique_lock<std::mutex> lck(m_StateChangeMutex);
    if (!isRunning())
    {
        return;
    }
    GifRecorderCMDItem stopCmd;
    memset(&stopCmd, 0, sizeof(stopCmd));
    stopCmd.cmd = GIFREC_STOP;
    nextFrames.push(stopCmd);
    m_isRunning.store(false, std::memory_order_relaxed);
}

