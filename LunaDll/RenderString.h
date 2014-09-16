#pragma once
#include <string>

// String object to be rendered later
struct RenderString {

	// Quick ctor
	RenderString() {		
		m_FontType = 1;
		m_X = 400;
		m_Y = 400;
		m_FramesLeft = 1;
	}

	RenderString(std::wstring str, int font_type, float X, float Y) {
		m_String = str;
		m_FontType = font_type;
		m_X = X;
		m_Y = Y;
		m_FramesLeft = 1;
	}

	std::wstring m_String;
	int	m_FontType;
	float m_X;
	float m_Y;
	int m_FramesLeft;
};