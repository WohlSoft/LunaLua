#include "RenderRectOp.h"
#include "../../Globals.h"

// DRAW
void RenderRectOp::Draw(Renderer* renderer) {
    Rectangle(renderer->GetScreenDC(), (int)x1, (int)y1, (int)x2, (int)y2);
}
