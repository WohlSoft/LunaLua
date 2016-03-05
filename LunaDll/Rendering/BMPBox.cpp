
#include "BMPBox.h"
#include <math.h>
#include "../Globals.h"
#include "RenderUtils.h"
#include "GL/GLEngine.h"
#include "../Misc/FreeImageUtils/FreeImageHelper.h"
#include <Windows.h>
#include <gl/glew.h>
#include "../../SMBXInternal/PlayerMOB.h"
#include <string>
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
	if (maskOutput)free(maskOutput);
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
		
		mp = new FFmpegMediaPlayer2(filename);
		
		if (mp->playable()) {
			
			m_W = mp->getWidth();
			m_H = mp->getHeight();
			m_hbmp = FreeImageHelper::CreateEmptyBitmap(m_W, m_H, 32, &bmpPtr);
			SelectObject(m_hdc, m_hbmp);
			hasVideo = true;
			mp->setVideoOutput((uint8_t*)bmpPtr, m_W, m_H);
			
			if (mp->maskExist()) {
				maskW = mp->getMaskWidth();
				maskH = mp->getMaskHeight();
				maskOutput = (uint8_t*)malloc(maskW*maskH*4*sizeof(uint8_t));
				mp->setMaskOutput(maskOutput, maskW, maskH);
				hasMask = true;
			}
		}

		else {
			
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
	for (int i = 0; i < 8; i++)maskThreshold[i] = 256;
	memset(pendingHarmArr, 0, sizeof(bool) * 8);
	clbc = NULL;
	hasMask = false;
	maskOutput = NULL;
	bmpPtr = false;
	maskW = 0;
	maskH = 0;
	mp = NULL;
	nowOnScreen = false;
	lastDecodedFrame = -1;
	offScrMode = PAUSE;
	onScrMode = PLAY;
	onScrClbk = NULL;
	offScrClbk = NULL;
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
bool BMPBox::ImageLoaded()const {
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
	if (nowOnScreen && !onScreen) {

			switch (offScrMode) {
			case CONTINUE:
				break;
			case PAUSE:
				pause();
				break;
			case STOP:
				stop();
				break;
			default:
				break;
			}
			
			nowOnScreen = false;
			shouldCallOffScrClbk = true;
	}
	else if(!nowOnScreen && onScreen) {
		switch (onScrMode) {
		case NOTHING:
			break;
		case PLAY:
			play();
			break;
		default:
			break;
		}

		nowOnScreen = true;
		shouldCallOnScrClbk = true;
	}
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

int BMPBox::getScaleUpMode()const {
	int m;
	switch (scaleUpMode) {
	case GL_NEAREST:
		m = 0;
		break;
	case GL_LINEAR:
		m = 1;
		break;
	default:
		m = 0;
		break;

	}

	return m;
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

int BMPBox::getScaleDownMode()const {
	int m;
	switch (scaleDownMode) {
	case GL_NEAREST:
		m = 0;
		break;
	case GL_LINEAR:
		m = 1;
		break;
	default:
		m = 0;
		break;

	}

	return m;
}

void BMPBox::setOffScreenMode(int m) {
	offScrMode = (OffScreenMode)m;
}

int BMPBox::getOffScreenMode()const {
	return offScrMode;
}

void BMPBox::setOnScreenMode(int m) {
	onScrMode = (OnScreenMode)m;
}

void BMPBox::setOnScreenCallback(void(*fn)()) {
	
	onScrClbk = fn;
}

void BMPBox::setOffScreenCallback(void(*fn)()) {
	offScrClbk = fn;
}

int BMPBox::getOnScreenMode()const {
	return onScrMode;
}

void BMPBox::setAlphaType(int m) {
	if (mp)mp->setAlphaType(m);
}

int BMPBox::getAlphaType()const {
	return mp ? mp->getAlphaType() : 0;
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

void BMPBox::setVideoDelay(double d) {
	if (mp)mp->setVideoDelay(d);
}

double BMPBox::getVideoDelay()const {
	return mp ? mp->getVideoDelay() : 0;
}

void BMPBox::setMaskDelay(double d) {
	if (mp)mp->setMaskDelay(d);
}

double BMPBox::getMaskDelay()const {
	return mp ? mp->getMaskDelay() : 0;
}
void BMPBox::setLoop(bool l) {
	if (mp)mp->loop = l;
}

bool BMPBox::getLoop()const {
	return mp ? mp->loop : false;
}

void BMPBox::setAltAlpha(int ch) {
	if (mp)mp->setAltAlpha(ch);
}


int BMPBox::getAltAlpha()const {
	return mp ? mp->getAltAlpha() : -1;
}

void BMPBox::setVolume(int v) {
	if (mp)mp->volume = min(max(v,0),128);
}

int BMPBox::getVolume()const {
	return mp ? mp->volume : 0;
}
void BMPBox::setCallback(void(*fn)(int)) {
	clbc = fn;
}

void BMPBox::colTest(int scrX,int scrY,int destWidth,int destHeight) {
	

	if (!mp)return;
	//if (!mp->collisionMap)return;
	//int mode = getHurtMode();
	//if (!(mode == 1 || mode == 2))return;
	
	
	if (GM_PLAYERS_COUNT <= 0)return;
	if (destWidth <= 0 || destHeight <= 0)return;
	auto pl = Player::Get(1);
	RECT pRect = Player::GetScreenPosition(pl);
	
	double wScale = m_W / (double)destWidth;
	double hScale = m_H / (double)destHeight;
	LONG top, bottom, left, right;
	//coord scaling
	top = (LONG)round(hScale*pRect.top); bottom = (LONG)round(hScale*pRect.bottom);
	left = (LONG)round(wScale*pRect.left); right = (LONG)round(wScale*pRect.right);
	int x = (int)round(wScale*scrX); int y = (int)round(hScale*scrY);
	int startH = min(max(top,y),m_H+y)-y; int endH = min(max(bottom, y), m_H+y)-y;
	int startW = min(max(left, x), m_W+x)-x; int endW = min(max(right, x), m_W+x)-x;
	//bool brk;
	for (int k = 0; k < 4; k++) {
		bool brk = false;
		for (int i = startH; i < endH; i++) {
			for (int j = startW; j < endW; j++) {
				if (((uint8_t*)bmpPtr)[m_W * 4 * i + j * 4+k] >= maskThreshold[k]) { //BGRA
					
					pendingHarmArr[k] = true;
					brk = true;
					break;
				}
			}
			if (brk)break;
		}
		
	}
	if (hasMask) {
		double mwScale = maskW / (double)destWidth;
		double mhScale = maskH / (double)destHeight;

		//coord scaling
		top = (LONG)round(mhScale*pRect.top);bottom = (LONG)round(mhScale*pRect.bottom);
		left = (LONG)round(mwScale*pRect.left); right = (LONG)round(mwScale*pRect.right);
		int mx = (int)round(mwScale*scrX); int my = (int)round(mhScale*scrY);
		int mstartH = min(max(top, my), maskH + my) - my; int mendH = min(max(bottom, my), maskH + my) - my;
		int mstartW = min(max(left, mx), maskW + mx) - mx; int mendW = min(max(right, mx), maskW + mx) - mx;
		for (int k = 0; k < 4; k++) {
			
			bool brk = false;
			for (int i = mstartH; i < mendH; i++) {
				for (int j = mstartW; j < mendW; j++) {
					if (maskOutput[maskW * 4 * i + j * 4 + k] >= maskThreshold[k+4]) { //BGRA

						pendingHarmArr[k+4] = true;
						brk = true;
						break;
					}
				}
				if (brk)break;
			}
			
		}
	}

}
void BMPBox::procCallback() {
	
	//if (GM_PLAYERS_COUNT <= 0 || !clbc)return;
	if (shouldCallOffScrClbk) {

		if(offScrClbk)offScrClbk();
		shouldCallOffScrClbk = false;
	}

	if (shouldCallOnScrClbk) {
		if (onScrClbk)onScrClbk();
		shouldCallOnScrClbk = false;
	}
	for (int i = 0; i < 8; i++) {
		if (pendingHarmArr[i]) {
			if(clbc)clbc(i);
			
			pendingHarmArr[i] = false;
		}
	}
	
}