#include "RenderSpriteOp.h"
#include "../../SMBXInternal/CameraInfo.h"
#include "../SMBXMaskedImage.h"

// CTOR
RenderSpriteOp::RenderSpriteOp() : RenderOp(RENDEROP_DEFAULT_PRIORITY_CGFX),
    x(0.0),
    y(0.0),
    sx(0.0),
    sy(0.0),
    sw(0.0),
    sh(0.0),
    sceneCoords(false),
    sprite(nullptr)
{}

// DRAW
void RenderSpriteOp::Draw(Renderer* renderer) {
    if (sprite == nullptr) {
        return;
    }

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

    sprite->DrawWithOverride(x, y, width, height, sx, sy, true, true);
}
