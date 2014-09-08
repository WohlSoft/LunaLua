#pragma once

#include "Defines.h"
#include <string>

#define DEFAULT_TRANS_COLOR 0xFF00DC

// A user-loaded bitmap container
class BMPBox {
public:

	/// Functions ///
	BMPBox();
	~BMPBox();
	BMPBox(std::wstring filename, HDC screen_dc);
	void Init();

	bool ImageLoaded();

	/// Members ///
	int m_H;				// Height of bitmap
	int m_W;				// Width of bitmap
	int	m_TransColor;		// Value that represents transparency (will write nothing)
	HBITMAP m_hbmp;			// Handle to bitmap data
	HDC m_hdc;				// handle to compatible DC for this bitmap
};