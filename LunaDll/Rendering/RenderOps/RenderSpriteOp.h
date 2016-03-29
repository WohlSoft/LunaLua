#ifndef RenderSpriteOp_H
#define RenderSpriteOp_H

#include "RenderOp.h"
class SMBXMaskedImage;

class RenderSpriteOp : public RenderOp {
public:
    RenderSpriteOp();
    void Draw(Renderer* renderer);

    double x;				// x position
    double y;				// y position
    double sx;				// Source x1 (left edge)
    double sy;				// Source y1 (top edge)
    double sw;				// Source width (right edge)
    double sh;				// Source height (bottom edge)

    bool   sceneCoords;     // If true, x and y are scene coordinates

    SMBXMaskedImage* sprite;
};

#endif
