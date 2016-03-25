#ifndef RenderBitmapOp_H
#define RenderBitmapOp_H

#include <memory>
#include "RenderOp.h"
class BMPBox;

class RenderBitmapOp : public RenderOp {
public:
    RenderBitmapOp();
    void Draw(Renderer* renderer);

    double x;				// Absolute screen x position
    double y;				// Absolute screen y position
    double sx;				// Source x1 (left edge)
    double sy;				// Source y1 (top edge)
    double sw;				// Source x2 (right edge)
    double sh;				// Source y2 (bottom edge)

	double dw;				// Destination width(for scaling)
	double dh;				// Destination height

    float  opacity;         // Opacity value
    bool   sceneCoords;     // If true, x and y are scene coordinates
	bool scaling;			// If true, rendered image will be scaled.

    std::shared_ptr<BMPBox> direct_img;
};

#endif
