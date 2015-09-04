#include "AsyncGifRecorder.h"
#include <iostream>
#include "../GlobalFuncs.h"

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

        //std::cout << "Incoming command id: " << (int)nextData.cmd << std::endl;
        //std::cout << "Running? " << std::boolalpha << m_isRunning.load(std::memory_order_relaxed) << std::noboolalpha << std::endl;
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
            screenshotPath += utf8_decode(generateTimestampForFilename()) + std::wstring(L".gif");
            
            //std::cout << "Gif recoding started!" << std::endl;

            IWICImagingFactory* pFactory = NULL;
            IWICMetadataQueryWriter* pGifGlobalMetadataWriter = NULL;
            pFactory = getWICFactory();
            if (pFactory == NULL) continue;

            HRESULT hr = pFactory->CreateStream(&m_curStream);
            //std::cout << std::hex;

            //std::cout << "CreateStream hr: " << (int)hr << std::endl;

            if (SUCCEEDED(hr))
                hr = m_curStream->InitializeFromFilename(screenshotPath.c_str(), GENERIC_WRITE);

            //std::cout << "InitFromFilename hr: " << (int)hr << std::endl;

            if (SUCCEEDED(hr))
                hr = pFactory->CreateEncoder(GUID_ContainerFormatGif, NULL, &m_curEncoder);

            //std::cout << "CreateEncoder hr: " << (int)hr << std::endl;

            if (SUCCEEDED(hr))
                hr = m_curEncoder->Initialize(m_curStream, WICBitmapEncoderNoCache);

            //std::cout << "Initialize hr: " << (int)hr << std::endl;

            if (SUCCEEDED(hr))
                hr = m_curEncoder->GetMetadataQueryWriter(&pGifGlobalMetadataWriter);
            
            //std::cout << "Writing metadata 1!" << std::endl;
            if (SUCCEEDED(hr)){
                BYTE txtNetscape[] = "NETSCAPE2.0";
                PROPVARIANT netscapeVal;
                PropVariantInit(&netscapeVal);
                netscapeVal.vt = VT_UI1 | VT_VECTOR;
                netscapeVal.caub.cElems = 11;
                netscapeVal.caub.pElems = txtNetscape;
                hr = pGifGlobalMetadataWriter->SetMetadataByName(L"/appext/Application", &netscapeVal);
            //    PropVariantClear(&netscapeVal);
            }
            
            //std::cout << "Writing metadata 2!" << std::endl;
            if (SUCCEEDED(hr)){
                BYTE loopCode[] = { 3, 1, 0, 0, 0 };
                PROPVARIANT loopVal;
                PropVariantInit(&loopVal);
                loopVal.vt = VT_UI1 | VT_VECTOR;
                loopVal.caub.cElems = 5;
                loopVal.caub.pElems = loopCode;
                //std::cout << "Writing metadata 2.1!" << std::endl;
                hr = pGifGlobalMetadataWriter->SetMetadataByName(L"/appext/Data", &loopVal);
                //std::cout << "Writing metadata 2.2!" << std::endl;
                //PropVariantClear(&loopVal);
                //std::cout << "Writing metadata 2.3!" << std::endl;
            }
            
            //std::cout << "Writing metadata 3!" << std::endl;

            if (SUCCEEDED(hr)){
                m_isRunning.store(true, std::memory_order_relaxed);
            }
            else
            {
                doCleanup = true;
            }
            break;
        }
        case GIFREC_NEXTFRAME:
        {
            if (!m_isRunning.load(std::memory_order_relaxed))
                continue;

            //std::cout << "Gif next frame!" << std::endl;
            bool frameEncodeSuccess = false;
            IWICImagingFactory* pFactory = NULL;
            IWICBitmap* pBitmap = NULL;
            IWICBitmapFrameEncode* pGifFrame = NULL;
            IWICMetadataQueryWriter* pGifFrameMetadata = NULL;
            IWICBitmapFlipRotator* pRotator = NULL;

            pFactory = getWICFactory();
            if (pFactory == NULL) continue;

            HRESULT hr = pFactory->CreateBitmapFromMemory(
                nextData.width,
                nextData.height,
                GUID_WICPixelFormat24bppBGR,
                ((((nextData.width * 24) + 31) & ~31) >> 3),
                nextData.width * nextData.height * 3,
                (BYTE*)nextData.data,
                &pBitmap);
            if (FAILED(hr)) goto frameCleanup;

            hr = m_curEncoder->CreateNewFrame(&pGifFrame, NULL);
            if (FAILED(hr)) goto frameCleanup;

            hr = pGifFrame->Initialize(NULL);
            if (FAILED(hr)) goto frameCleanup;

            hr = pGifFrame->GetMetadataQueryWriter(&pGifFrameMetadata);
            if (FAILED(hr)) goto frameCleanup;

            PROPVARIANT delayVal;
            PropVariantInit(&delayVal);
            delayVal.vt = VT_UI2;
            delayVal.uiVal = 2u;
            hr = pGifFrameMetadata->SetMetadataByName(L"/grctlext/Delay", &delayVal);
            PropVariantClear(&delayVal);
            if (FAILED(hr)) goto frameCleanup;

            hr = pFactory->CreateBitmapFlipRotator(&pRotator);
            if (FAILED(hr)) goto frameCleanup;

            hr = pRotator->Initialize(pBitmap, WICBitmapTransformFlipVertical);
            if (FAILED(hr)) goto frameCleanup;

            hr = pGifFrame->WriteSource(pRotator, NULL);
            if (FAILED(hr)) goto frameCleanup;

            hr = pGifFrame->Commit();
            if (FAILED(hr)) goto frameCleanup;

            frameEncodeSuccess = true;

        frameCleanup:;
            if (pGifFrameMetadata)
                pGifFrameMetadata->Release();
            if (pRotator)
                pRotator->Release();
            if (pBitmap)
                pBitmap->Release();

            if (!frameEncodeSuccess){
                if (pGifFrame)
                    pGifFrame->Release();
            }
            else
            {
                m_curAllFrames.push_back(pGifFrame);
            }

            break;
        }
        case GIFREC_STOP:
        {
            if (!m_isRunning.load(std::memory_order_relaxed))
                continue;
            
            //std::cout << "Gif stopped!" << std::endl;
            m_curEncoder->Commit();

            doCleanup = true;
            m_isRunning.store(false, std::memory_order_relaxed);
            break;
        }
        default:
            break;
        }

        if (doCleanup){
            for (IWICBitmapFrameEncode* nextFrame : m_curAllFrames){
                //std::cout << "CLEANUP!" << std::endl;
                //std::cout << "Ref: " << nextFrame->Release() << std::endl;
            }
            m_curAllFrames.clear();

            if (m_curEncoder)
                m_curEncoder->Release();
            m_curEncoder = nullptr;

            if (m_curStream)
                m_curStream->Release();
            m_curStream = nullptr;
        }
    }
    
}

void AsyncGifRecorder::start()
{
    //std::cout << "Send start CMD!" << std::endl;
    GifRecorderCMDItem startCmd;
    memset(&startCmd, 0, sizeof(startCmd));
    startCmd.cmd = GIFREC_START;
    nextFrames.push(startCmd);
}

void AsyncGifRecorder::stop()
{
    //std::cout << "Send stop CMD!" << std::endl;
    GifRecorderCMDItem stopCmd;
    memset(&stopCmd, 0, sizeof(stopCmd));
    stopCmd.cmd = GIFREC_STOP;
    nextFrames.push(stopCmd);
}

