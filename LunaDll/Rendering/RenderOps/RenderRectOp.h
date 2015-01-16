#ifndef RenderRectOp_Hhhh
#define RenderRectOp_Hhhh

#include "RenderOp.h"

class RenderRectOp : public RenderOp {
public:
	RenderRectOp() : x1(0), y1(0), x2(0), y2(0), color(0xFFFFFFFF) {   };

	void Draw(Renderer* renderer); 

	double x1;
	double y1;
	double x2;
	double y2;
	DWORD color;
};

#endif
