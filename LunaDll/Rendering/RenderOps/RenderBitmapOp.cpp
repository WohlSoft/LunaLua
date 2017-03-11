#include "RenderBitmapOp.h"
#include "../LunaImage.h"
#include "../../Globals.h"
#include "../../Misc/MiscFuncs.h"
#include "../GL/GLEngine.h"
#include "../../Defines.h"
#include "../../SMBXInternal/CameraInfo.h"

// CTOR
RenderBitmapOp::RenderBitmapOp() : RenderOp(RENDEROP_DEFAULT_PRIORITY_CGFX),
    x(0.0),
    y(0.0),
    sx(0.0),
    sy(0.0),
    sw(0.0),
    sh(0.0),
    opacity(1.0f),
    sceneCoords(false),
    direct_img(nullptr)
{}

// DRAW
void RenderBitmapOp::Draw(Renderer* renderer) {
    if (!direct_img || (direct_img->getH() == 0) || (direct_img->getW() == 0)) {
        return;
    }
    
    float opacity = this->opacity;
    if (opacity > 1.0f) opacity = 1.0f;
    if (opacity < 0.0f) opacity = 0.0f;

    double screenX = this->x;
    double screenY = this->y;
    if (sceneCoords) {
        SMBX_CameraInfo::transformSceneToScreen(renderer->GetCameraIdx(), screenX, screenY);
    }

    // Get integer values as current rendering backends prefer that
    int x = static_cast<int>(round(screenX));
    int y = static_cast<int>(round(screenY));
    int sx = static_cast<int>(round(this->sx));
    int sy = static_cast<int>(round(this->sy));
    int width = static_cast<int>(round(this->sw));
    int height = static_cast<int>(round(this->sh));

    // Trim height/width if necessary
    if (direct_img->getW() < width + sx)
    {
        width = direct_img->getW() - sx;
    }
    if (direct_img->getH() < height + sy)
    {
        height = direct_img->getH() - sy;
    }

    // Don't render if no size
    if ((width <= 0) || (height <= 0))
    {
        return;
    }

    if (g_GLEngine.IsEnabled())
    {
        // LUNAIMAGE_TODO: Support masked images from RenderBitmapOp
        // (Will need to think about how to handle the opacity argument)

        auto obj = std::make_shared<GLEngineCmd_DrawSprite>();
        obj->mXDest = x;
        obj->mYDest = y;
        obj->mWidthDest = width;
        obj->mHeightDest = height;
        obj->mXSrc = sx;
        obj->mYSrc = sy;
        obj->mWidthSrc = width;
        obj->mHeightSrc = height;

        obj->mImg = direct_img;
        obj->mOpacity = opacity;
        obj->mMode = GLDraw::RENDER_MODE_ALPHA;
        g_GLEngine.QueueCmd(obj);
    }
    else
    {
        // LUNAIMAGE_TODO: Support GDI renderer
        /*
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = (int)roundf(255 * opacity);
        bf.AlphaFormat = AC_SRC_ALPHA;
        AlphaBlend(renderer->GetScreenDC(), x, y, width, height,
            direct_img->m_hdc, sx, sy, width, height, bf);
        */
    }
}
