#include "RenderRectOp.h"
#include "../../Globals.h"

// DRAW
void RenderRectOp::Draw(Renderer* renderer) {
	Rectangle(renderer->m_hScreenDC, (int)x1, (int)y1, (int)x2, (int)y2);
}
