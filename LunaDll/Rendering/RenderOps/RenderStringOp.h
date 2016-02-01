#ifndef RenderString_hhh
#define RenderString_hhh

#include <string>
#include "RenderOp.h"
#include "../../Defines.h"
#include "../../SMBXInternal/CameraInfo.h"

// String object to be rendered later
class RenderStringOp : public RenderOp {
public:
    // Quick ctor
    RenderStringOp() : RenderStringOp(L"", 1, 400.f, 400.f) {}

    RenderStringOp(std::wstring str, short font_type, float X, float Y) : 
        RenderOp(RENDEROP_DEFAULT_PRIORITY_TEXT), 
        m_String(str),
        m_FontType(font_type),
        m_X(X),
        m_Y(Y),
        sceneCoords(false)
    {}

    virtual ~RenderStringOp() { }
    virtual void Draw(Renderer* renderer) 
    {
        VB6StrPtr text(m_String);
        float x = m_X, y = m_Y;
        if (sceneCoords)
            SMBX_CameraInfo::transformSceneToScreen(renderer->GetCameraIdx(), x, y);
        native_print(&text, &m_FontType, &x, &y);
    }

    std::wstring m_String;
    short m_FontType;
    float m_X;
    float m_Y;
    bool   sceneCoords;     // If true, x and y are scene coordinates
};

#endif
