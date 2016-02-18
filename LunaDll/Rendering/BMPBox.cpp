
#include "BMPBox.h"
#include <math.h>
#include "../Globals.h"
#include "RenderUtils.h"
#include "GL/GLEngine.h"
#include "../Misc/FreeImageUtils/FreeImageHelper.h"
#include <Windows.h>
#include <gl/glew.h>

void BMPBox::DebugMsgBox(LPCSTR pszFormat, ...)
{
	va_list	argp;
	char pszBuf[256];
	va_start(argp, pszFormat);
	vsprintf(pszBuf, pszFormat, argp);
	va_end(argp);
	MessageBoxA(NULL, pszBuf, "debug info", MB_OK);
}


// CTOR
BMPBox::BMPBox():mp(NULL),scaleUpMode(GL_NEAREST),scaleDownMode(GL_NEAREST) {
    Init();
}


BMPBox::~BMPBox() {
	// If the GL Engine is running, deallocate the associated texture and erase the BMPBox->Texture mapping.
	
	if (g_GLEngine.IsEnabled())
	{
		g_GLEngine.ClearLunaTexture(*this);
	}
	if (mp) {
		delete mp;
		mp = NULL;
	}
	if (m_hbmp != NULL) {
		DeleteObject(m_hbmp); //vBuffer will also be freed
		m_hbmp = NULL;
	}
	if (m_hdc != NULL) {
		DeleteDC(m_hdc);
		m_hdc = NULL;
	}

}

// CTOR - Load from a file path
BMPBox::BMPBox(std::wstring filename, HDC screen_dc) {
	Init();

	if (filename.length() < 1)
		return;
	m_hbmp = LoadGfxAsBitmap(filename);
	m_hdc = CreateCompatibleDC(screen_dc);
	if (m_hbmp != NULL) {
		// Load any image, converted to pre-multiplied BGRA
		//m_hbmp = LoadGfxAsBitmap(filename);

		//gLogger.Log(L"Requested handle for: " + filename, LOG_STD);
		//int lasterr = GetLastError();
		//gLogger.Log(L"Last error: " + to_wstring((long long)lasterr), LOG_STD);

		SelectObject(m_hdc, m_hbmp);

		// Get dimensions
		BITMAP bm;
		memset(&bm, 0, sizeof(BITMAP));
		GetObject(m_hbmp, sizeof(BITMAP), &bm);
		m_H = bm.bmHeight;
		m_W = bm.bmWidth;
	}
	else {
		mp = new FFmpegMediaPlayer(filename, ffdset);
		realffdset = mp->getAppliedSetting();
		if (mp->isVideoPlayable()) {
			m_H = realffdset.video.height;
			m_W = realffdset.video.width;
			m_hbmp = FreeImageHelper::CreateEmptyBitmap(m_W, m_H, 32, &bmpPtr);
			SelectObject(m_hdc, m_hbmp);
			hasVideo = true;
			mp->setVideoBufferDest(bmpPtr);
		}
	}
	m_Filename = filename;

}


BMPBox::BMPBox(HBITMAP bitmapData, HDC screen_dc)
{
    Init();

    // Load any image, converted to pre-multiplied BGRA
    m_hbmp = bitmapData;

    //gLogger.Log(L"Requested handle for: " + filename, LOG_STD);
    //int lasterr = GetLastError();
    //gLogger.Log(L"Last error: " + to_wstring((long long)lasterr), LOG_STD);

    m_hdc = CreateCompatibleDC(screen_dc);
    SelectObject(m_hdc, m_hbmp);

    // Get dimensions
    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(m_hbmp, sizeof(BITMAP), &bm);
    m_H = bm.bmHeight;
    m_W = bm.bmWidth;
}

// INIT
void BMPBox::Init() {
	mp = NULL;
	m_modified.store(false, std::memory_order_relaxed);
	m_H = 0;
	m_W = 0;
	m_hbmp = NULL;
	m_hdc = NULL;
	hasVideo = false;
	scaleDownMode = GL_NEAREST;
	scaleUpMode = GL_NEAREST;
	ffdset.video.pixFmt = AV_PIX_FMT_BGRA;
	ffdset.video.resampling_mode = SWS_FAST_BILINEAR;
	ffdset.video.width = 0;		//use almost original size
	ffdset.video.height = 0;
	ffdset.audio.channelLayout = AV_CH_LAYOUT_STEREO;
	ffdset.audio.channel_num = 0; //auto
	ffdset.audio.sample_format = AV_SAMPLE_FMT_S16;
	ffdset.audio.sample_rate = 44100;
}


// Makes a specified color transparent
void BMPBox::MakeColorTransparent(int rgb_color) {
    union {
        struct {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        } asStruct;
        uint32_t asInt;
    } bgra_color;

    // Convert RGB to BGRA
    bgra_color.asStruct.b = (rgb_color & 0x0000FF);
    bgra_color.asStruct.g = (rgb_color & 0x00FF00) >> 8;
    bgra_color.asStruct.r = (rgb_color & 0xFF0000) >> 16;
    bgra_color.asStruct.a = 255;

    if (m_hbmp) {
        BITMAP bm;
        memset(&bm, 0, sizeof(BITMAP));
        GetObject(m_hbmp, sizeof(BITMAP), &bm);
        uint32_t *pData = (uint32_t *)bm.bmBits;
        uint32_t dataLen = bm.bmHeight * bm.bmWidth;

        // Presuming we can get the bitmap data, replace all of the color with
        // transparency.
        if (pData) {
            for (uint32_t idx = 0; idx < dataLen; idx++) {
                if (pData[idx] == bgra_color.asInt) {
                    pData[idx] = 0;
                }
            }
        }
    }
}

// IMAGE LOADED - Returns true if this object loaded correctly / the bitmap handle isn't null
bool BMPBox::ImageLoaded() {
    if (m_hbmp == NULL)
        return false;
    return true;
}

void BMPBox::forEachPixelValue(std::function<void(BYTE)> forEachFunc)
{
    if (!forEachFunc)
        return;

    if (!m_hbmp)
        return;

    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(m_hbmp, sizeof(BITMAP), &bm);
    BYTE *pData = (BYTE *)bm.bmBits;
    uint32_t dataLen = bm.bmHeight * bm.bmWidth * 4;

    if (pData) {
        for (uint32_t i = 0; i < dataLen; i++) {
            forEachFunc(pData[i]);
        }
    }
    
}

void* BMPBox::getBits()
{
    BITMAP bm = { 0 };
    GetObject(m_hbmp, sizeof(bm), &bm);
    return bm.bmBits;
}

BMPBox* BMPBox::loadIfExist(const std::wstring& filename, HDC screen_dc)
{
    DWORD fAttrib = GetFileAttributesW(filename.c_str());
    if (fAttrib == INVALID_FILE_ATTRIBUTES || fAttrib & FILE_ATTRIBUTE_DIRECTORY)
        return nullptr;
    return new BMPBox(filename, screen_dc);
}

void BMPBox::setOnScreen(bool onScreen) {
	if(mp)mp->setOnScreen(onScreen);
}

void BMPBox::setScaleUpMode(int m) {
	switch (m) {
	case 0:
		scaleUpMode = GL_NEAREST;
		break;
	case 1:
		scaleUpMode = GL_LINEAR;
		break;
	default:
		scaleUpMode = GL_NEAREST;
		break;

	}
}

void BMPBox::setScaleDownMode(int m) {
	switch (m) {
	case 0:
		scaleDownMode = GL_NEAREST;
		break;
	case 1:
		scaleDownMode = GL_LINEAR;
		break;
	default:
		scaleDownMode = GL_NEAREST;
		break;

	}
}

void BMPBox::play() {
	if (mp)mp->play();
}

void BMPBox::stop() {
	if (mp)mp->stop();
}

void BMPBox::pause() {
	if (mp)mp->pause();
}

void BMPBox::seek(double sec) {
	if (mp)mp->seek(sec);
}