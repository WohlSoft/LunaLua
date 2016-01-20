#ifndef renderOp_HHHH
#define renderOp_HHHH

//#include "Defines.h"
#include "../Rendering.h"

static const double RENDEROP_PRIORITY_MIN = -95.5;
static const double RENDEROP_PRIORITY_MAX = 5.5;
static const double RENDEROP_DEFAULT_PRIORITY_RENDEROP = 1.0; // Default priority for RenderOp and RenderImage
static const double RENDEROP_DEFAULT_PRIORITY_CGFX = 2.0; // Default priority for Custom GFX
static const double RENDEROP_DEFAULT_PRIORITY_TEXT = 3.0; // Default priority for Text

// Base class respresenting a rendering operation
// Rendering operations include a draw function and a count of how many frames of activity remain
class RenderOp {
public:
    RenderOp() : m_FramesLeft(1), m_selectedCamera(0), m_renderPriority(RENDEROP_DEFAULT_PRIORITY_RENDEROP) { }
    RenderOp(double priority) : m_FramesLeft(1), m_selectedCamera(0), m_renderPriority(priority) { }
    virtual ~RenderOp() {}
    virtual void Draw(Renderer* renderer) {}

    int m_FramesLeft;		// How many frames until this op should be destroyed
    int m_selectedCamera;
    double m_renderPriority;
};

#endif
