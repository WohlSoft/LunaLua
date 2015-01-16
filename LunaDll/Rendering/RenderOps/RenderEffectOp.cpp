#include "RenderEffectOp.h"
#include "../../Globals.h"
#include <mmintrin.h>

// DRAW
RenderEffectOp::RenderEffectOp() : effect_type(RNDEFF_ScreenGlow), blend_type(BLEND_Additive),  color(0x00000000), intensity(0) {}

void RenderEffectOp::Draw(Renderer* g) {
    switch(effect_type) {
    case RNDEFF_ScreenGlow:
        ScreenGlow(g);
        break;
    default:
        break;
	}
}

// SCREEN GLOW
void RenderEffectOp::ScreenGlow(Renderer* g) {
	HDC hScreen = g->m_hScreenDC;

	HBITMAP hOld = (HBITMAP)SelectObject(ghMemDC, ghGeneralDIB);

	if(ghGeneralDIB && ghMemDC && gpScreenBits) {
		BitBlt(ghMemDC, 0, 0, 800, 600, hScreen, 0, 0, SRCCOPY);

        #ifndef __MINGW32__
		// MMX code and loop
		_mm_empty();		
			int nLoops = (800 * 600) / 2;
			__m64 color64 = _mm_set_pi32(color, color);		
			__m64* pDest = (__m64*)gpScreenBits;

		if(blend_type == BLEND_Additive) {
			for(int i = 0; i < nLoops; i++) {				
				//tmp = 
				pDest[i] = _mm_adds_pu8(color64, pDest[i]);

				//pDest[i] = tmp;
			}
		}
		else if(blend_type == BLEND_Subtractive) {
			for(int i = 0; i < nLoops; i++) {
				//tmp =
				pDest[i] = _mm_subs_pu8(color64, pDest[i]);

				//pDest[i] = tmp;
			}
		}

		_mm_empty();
        #endif

        BitBlt(hScreen, 0, 0, 800, 600, ghMemDC, 0, 0, SRCCOPY);

		SelectObject(ghMemDC, hOld);
	}

}
