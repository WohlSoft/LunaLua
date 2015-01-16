#include "RenderBitmapOp.h"
#include "../BMPBox.h"
#include "../../Globals.h"
#include "../../Misc/MiscFuncs.h"

// CTOR
RenderBitmapOp::RenderBitmapOp() {
	x = 0;
	y = 0;
	sx1 = 0;
	sy1 = 0;
	sx2 = 0;
	sy2 = 0;
	m_PerCycleOnly = false;
}

// DRAW
void RenderBitmapOp::Draw(Renderer* renderer) {
	BMPBox* bmp = renderer->LoadedImages[img_resource_code];	
	if(bmp != NULL && bmp->m_hdc != NULL) {
		//BitBlt(renderer->m_hScreenDC, (int)x, (int)y, bmp->m_W, bmp->m_H, bmp->m_hdc, 0, 0, SRCCOPY);
		TransparentBlt(renderer->m_hScreenDC, (int)x, (int)y, (int)sx2, (int)sy2, 
						bmp->m_hdc, (int)sx1, (int)sy1, (int)sx2, (int)sy2, bmp->m_TransColor);

		if(false) { //debug
			Render::Print(to_wstring((long long)x), 3, 300,420);
			Render::Print(to_wstring((long long)y), 3, 300,440);
			Render::Print(to_wstring((long long)sx1), 3, 300,460);
			Render::Print(to_wstring((long long)sy1), 3, 300,480);
			Render::Print(to_wstring((long long)sx2), 3, 300,500);
			Render::Print(to_wstring((long long)sy2), 3, 300,520);
		}
	}
}
