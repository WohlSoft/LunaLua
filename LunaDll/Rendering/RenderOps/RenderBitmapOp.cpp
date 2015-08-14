#include "RenderBitmapOp.h"
#include "../BMPBox.h"
#include "../../Globals.h"
#include "../../Misc/MiscFuncs.h"
#include "../GLEngine.h"
#include "../../Defines.h"
#include "../../SMBXInternal/CameraInfo.h"

// CTOR
RenderBitmapOp::RenderBitmapOp() {
    x = 0;
    y = 0;
    sx = 0;
    sy = 0;
    sw = 0;
    sh = 0;
    opacity = 1.0f;
    sceneCoords = false;
    m_PerCycleOnly = false;
}

// DRAW
void RenderBitmapOp::Draw(Renderer* renderer) {
    BMPBox* bmp = NULL;
    auto it = renderer->LoadedImages.find(img_resource_code);
    if (it != renderer->LoadedImages.end()) bmp = it->second;

    float opacity = this->opacity;
    if (opacity > 1.0f) opacity = 1.0f;
    if (opacity < 0.0f) opacity = 0.0f;

    double x = this->x;
    double y = this->y;
    if (sceneCoords) {
        x -= (int)SMBX_CameraInfo::getCameraX(1);
        y -= (int)SMBX_CameraInfo::getCameraY(1);
    }

    if (bmp != NULL && bmp->m_hdc != NULL) {
        //BitBlt(renderer->m_hScreenDC, (int)x, (int)y, bmp->m_W, bmp->m_H, bmp->m_hdc, 0, 0, SRCCOPY);
        //TransparentBlt(renderer->m_hScreenDC, (int)x, (int)y, (int)sx2, (int)sy2,
        //    bmp->m_hdc, (int)sx1, (int)sy1, (int)sx2, (int)sy2, bmp->m_TransColor);

        if (g_GLEngine.IsEnabled())
        {
            g_GLEngine.DrawLunaSprite(
                (int)x, (int)y, (int)sw, (int)sh,
                *bmp, (int)sx, (int)sy, (int)sw, (int)sh, opacity);
        }
        else
        {
            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = (int)roundf(255 * opacity);
            bf.AlphaFormat = AC_SRC_ALPHA;
            AlphaBlend(renderer->m_hScreenDC, (int)x, (int)y, (int)sw, (int)sh,
                bmp->m_hdc, (int)sx, (int)sy, (int)sw, (int)sh, bf);
        }

        if (false) { //debug
            Render::Print(to_wstring((long long)x), 3, 300, 420);
            Render::Print(to_wstring((long long)y), 3, 300, 440);
            Render::Print(to_wstring((long long)sx), 3, 300, 460);
            Render::Print(to_wstring((long long)sy), 3, 300, 480);
            Render::Print(to_wstring((long long)sw), 3, 300, 500);
            Render::Print(to_wstring((long long)sh), 3, 300, 520);
        }
    }
}
