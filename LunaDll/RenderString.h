#pragma once
#include <string>

// String object to be rendered later
struct RenderString {

	// Quick ctor
	RenderString() {		
		mFontType = 1;
		x = 400;
		y = 400;
	}

	RenderString(std::wstring str, int font_type, float X, float Y) {
		mString = str;
		mFontType = font_type;
		x = X;
		y = Y;
	}

	std::wstring mString;
	int	mFontType;
	float x;
	float y;

};