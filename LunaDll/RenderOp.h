#pragma once

//#include "Defines.h"
#include "Rendering.h"

// Base class respresenting a rendering operation
// Rendering operations include a draw function and a count of how many frames of activity remain
class RenderOp {
public:
	RenderOp() : m_PerCycleOnly(false), m_LastRenderedOn(0), m_FramesLeft(1) { }
	virtual void Draw(Renderer* renderer) =0;

	int m_LastRenderedOn;	// The last frame this was rendered on
	int m_FramesLeft;		// How many frames until this op should be destroyed
	bool m_PerCycleOnly;	// Whether or not this renderop should only run once per full game frame (no running while minimized etc)
};