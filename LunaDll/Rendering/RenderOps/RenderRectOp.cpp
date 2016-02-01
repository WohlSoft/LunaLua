#include <Windows.h>
#include <gdiplus.h>
#include "../../Globals.h"
#include "../GL/GLEngine.h"
#include "RenderRectOp.h"
#include "../../SMBXInternal/CameraInfo.h"

using namespace Gdiplus;

static inline BYTE ColorFloatToByte(float f)
{
    f = roundf(f * 255.0f);
    if (f >= 255.0f) return 255;
    if (f <= 0.0f) return 0;
    return static_cast<BYTE>(f);
}

static inline Color RenderOpColorToGDIPlus(RenderOpColor c)
{
    return Color::MakeARGB(
        ColorFloatToByte(c.a),
        ColorFloatToByte(c.r),
        ColorFloatToByte(c.g),
        ColorFloatToByte(c.b)
        );
}

// DRAW
void RenderRectOp::Draw(Renderer* renderer) {
    if (borderColor.a <= 0.0 && fillColor.a <= 0.0) return;

    // Convert coordinates
    double x1 = this->x1, y1 = this->y1, x2 = this->x2, y2 = this->y2;
    if (sceneCoords) {
        SMBX_CameraInfo::transformSceneToScreen(renderer->GetCameraIdx(), x1, y1);
        SMBX_CameraInfo::transformSceneToScreen(renderer->GetCameraIdx(), x2, y2);
    }

    if (g_GLEngine.IsEnabled())
    {
        if (fillColor.a > 0.0f)
        {
            // Render with OpenGL
            float* vert = static_cast<float*>(malloc(sizeof(float) * 8));
            vert[0] = static_cast<float>(x1); vert[1] = static_cast<float>(y1);
            vert[2] = static_cast<float>(x1); vert[3] = static_cast<float>(y2);
            vert[4] = static_cast<float>(x2); vert[5] = static_cast<float>(y1);
            vert[6] = static_cast<float>(x2); vert[7] = static_cast<float>(y2);

            auto obj = std::make_shared<GLEngineCmd_LuaDraw>();
            obj->mBmp = nullptr;
            obj->mColor[0] = fillColor.r;
            obj->mColor[1] = fillColor.g;
            obj->mColor[2] = fillColor.b;
            obj->mColor[3] = fillColor.a;
            obj->mType = GL_TRIANGLE_STRIP;
            obj->mVert = vert;
            obj->mTex = nullptr;
            obj->mVertColor = nullptr;
            obj->mCount = 4;
            g_GLEngine.QueueCmd(obj);
        }
        if (borderColor.a > 0.0f)
        {
            // Render with OpenGL
            float* vert = static_cast<float*>(malloc(sizeof(float) * 8));
            vert[0] = static_cast<float>(x1); vert[1] = static_cast<float>(y1);
            vert[2] = static_cast<float>(x1); vert[3] = static_cast<float>(y2);
            vert[4] = static_cast<float>(x2); vert[5] = static_cast<float>(y2);
            vert[6] = static_cast<float>(x2); vert[7] = static_cast<float>(y1);

            auto obj = std::make_shared<GLEngineCmd_LuaDraw>();
            obj->mBmp = nullptr;
            obj->mColor[0] = borderColor.r;
            obj->mColor[1] = borderColor.g;
            obj->mColor[2] = borderColor.b;
            obj->mColor[3] = borderColor.a;
            obj->mType = GL_LINE_LOOP;
            obj->mVert = vert;
            obj->mTex = nullptr;
            obj->mVertColor = nullptr;
            obj->mCount = 4;
            g_GLEngine.QueueCmd(obj);
        }
    }
    else
    {
        // Render with GDIPlus
        Graphics g(renderer->GetScreenDC());
        if (fillColor.a > 0.0f)
        {
            SolidBrush brush(RenderOpColorToGDIPlus(fillColor));
            g.FillRectangle(
                &brush,
                static_cast<REAL>(x1), static_cast<REAL>(y1),
                static_cast<REAL>(x2-x1), static_cast<REAL>(y2-y1));
        }
        if (borderColor.a > 0.0f)
        {
            Pen pen(RenderOpColorToGDIPlus(borderColor));
            g.DrawRectangle(
                &pen,
                static_cast<REAL>(x1), static_cast<REAL>(y1),
                static_cast<REAL>(x2 - x1), static_cast<REAL>(y2 - y1));
        }
    }
}
