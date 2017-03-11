#ifndef RenderBitmapOp_H
#define RenderBitmapOp_H

#include <memory>
#include "RenderOp.h"
class LunaImage;

class RenderBitmapOp : public RenderOp {
public:
    RenderBitmapOp();
    void Draw(Renderer* renderer);

    double x;				// Absolute screen x position
    double y;				// Absolute screen y position
    double sx;				// Source x1 (left edge)
    double sy;				// Source y1 (top edge)
    double sw;				// Source x1 (right edge)
    double sh;				// Source x1 (bottom edge)

    float  opacity;         // Opacity value
    bool   sceneCoords;     // If true, x and y are scene coordinates

    std::shared_ptr<LunaImage> direct_img;
};

#endif
