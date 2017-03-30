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

    direct_img->draw(x, y, width, height, sx, sy, true, true, opacity);
}
