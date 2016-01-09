#include "RenderBitmapOp.h"
#include "../BMPBox.h"
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
    if (!direct_img || !direct_img->ImageLoaded() || !direct_img->m_hdc) {
        return;
    }
    
    float opacity = this->opacity;
    if (opacity > 1.0f) opacity = 1.0f;
    if (opacity < 0.0f) opacity = 0.0f;

    double x = this->x;
    double y = this->y;
    if (sceneCoords) {
        SMBX_CameraInfo::transformSceneToScreen(1, x, y);
    }

    //BitBlt(renderer->m_hScreenDC, (int)x, (int)y, bmp->m_W, bmp->m_H, bmp->m_hdc, 0, 0, SRCCOPY);
    //TransparentBlt(renderer->m_hScreenDC, (int)x, (int)y, (int)sx2, (int)sy2,
    //    bmp->m_hdc, (int)sx1, (int)sy1, (int)sx2, (int)sy2, bmp->m_TransColor);

    if (g_GLEngine.IsEnabled())
    {
        g_GLEngine.DrawLunaSprite(
            (int)x, (int)y, (int)sw, (int)sh,
            *(direct_img.get()), (int)sx, (int)sy, (int)sw, (int)sh, opacity);
    }
    else
    {
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = (int)roundf(255 * opacity);
        bf.AlphaFormat = AC_SRC_ALPHA;
        AlphaBlend(renderer->m_hScreenDC, (int)x, (int)y, (int)sw, (int)sh,
            direct_img->m_hdc, (int)sx, (int)sy, (int)sw, (int)sh, bf);
    }
}
