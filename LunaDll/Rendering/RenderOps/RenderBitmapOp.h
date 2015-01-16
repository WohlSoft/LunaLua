#ifndef RenderBitmapOp_H
#define RenderBitmapOp_H

#include "RenderOp.h"

class RenderBitmapOp : public RenderOp {
public:
	RenderBitmapOp();
	void Draw(Renderer* renderer);

	double x;				// Absolute screen x position
	double y;				// Absolute screen y position
	double sx1;				// Source x1 (left edge)
	double sy1;				// Source y1 (top edge)
	double sx2;				// Source x1 (right edge)
	double sy2;				// Source x1 (bottom edge)
	
	int img_resource_code;
};

#endif
