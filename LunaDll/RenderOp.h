#pragma once

//#include "Defines.h"
#include "Rendering.h"

// Base class respresenting a rendering operation
// Rendering operations include a draw function and a count of how many frames of activity remain
class RenderOp {
public:
	virtual void Draw(Renderer* renderer) =0;

	int m_FramesLeft; // How many frames until this op should be destroyed
};