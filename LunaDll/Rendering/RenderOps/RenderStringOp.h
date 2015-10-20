#ifndef RenderString_hhh
#define RenderString_hhh

#include <string>
#include "RenderOp.h"
#include "../../Defines.h"

// String object to be rendered later
class RenderStringOp : public RenderOp {
public:
    // Quick ctor
    RenderStringOp() : RenderOp(RENDEROP_DEFAULT_PRIORITY_TEXT) {
        m_FontType = 1;
        m_X = 400;
        m_Y = 400;
        m_FramesLeft = 1;
    }

    RenderStringOp(std::wstring str, short font_type, float X, float Y) : RenderOp(RENDEROP_DEFAULT_PRIORITY_TEXT) {
        m_String = str;
        m_FontType = font_type;
        m_X = X;
        m_Y = Y;
        m_FramesLeft = 1;
    }

    virtual ~RenderStringOp() { }
    virtual void Draw(Renderer* renderer) 
    {
        VB6StrPtr text(m_String);
        native_print(&text, &m_FontType, &m_X, &m_Y);
    }

    std::wstring m_String;
    short m_FontType;
    float m_X;
    float m_Y;
};

#endif
