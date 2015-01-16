#include "BMPBox.h"
#include "../Globals.h"

// CTOR
BMPBox::BMPBox() {
	Init();
}

// DTOR
BMPBox::~BMPBox() {
	if(m_hbmp != NULL) {
		DeleteObject(m_hbmp);
		m_hbmp = NULL;
	}
	if(m_hdc !=NULL) {
		DeleteDC(m_hdc);
		m_hdc = NULL;
	}
}

// CTOR - Load from a file path
BMPBox::BMPBox(std::wstring filename, HDC screen_dc) {
	Init();

	if(filename.length() < 1)
		return;

	m_hbmp = (HBITMAP) LoadImage(NULL, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//gLogger.Log(L"Requested handle for: " + filename, LOG_STD);
	//int lasterr = GetLastError();
	//gLogger.Log(L"Last error: " + to_wstring((long long)lasterr), LOG_STD);

	m_hdc = CreateCompatibleDC(screen_dc);
	SelectObject(m_hdc, m_hbmp);

	// Get dimensions
	BITMAP bm;
	memset(&bm, 0, sizeof(BITMAP) );
	GetObject(m_hbmp, sizeof(BITMAP), &bm);
	m_H = bm.bmHeight;
	m_W = bm.bmWidth;
}

// INIT
void BMPBox::Init() {
	m_H = 0;
	m_W = 0;
	m_hbmp = NULL;
	m_hdc = NULL;
	m_TransColor = DEFAULT_TRANS_COLOR;
}

// IMAGE LOADED - Returns true if this object loaded correctly / the bitmap handle isn't null
bool BMPBox::ImageLoaded() {
	if(m_hbmp == NULL)
		return false;
	return true;
}
