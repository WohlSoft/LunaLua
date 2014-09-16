#pragma once

#include "RenderOp.h"

enum RENDER_EFFECT {	
	RNDEFF_ScreenGlow
};

enum BLEND_TYPE {
	BLEND_Additive,
	BLEND_Subtractive
};

class RenderEffectOp : public RenderOp {
public:
	RenderEffectOp() : effect_type(RNDEFF_ScreenGlow), blend_type(BLEND_Additive),  color(0x00000000), intensity(0) {   };
	RenderEffectOp(RENDER_EFFECT effect, BLEND_TYPE blend, COLORREF col, int intensity) {
		effect_type = effect;
		blend_type = blend;
		color = col;
		intensity = intensity;
	}

	void Draw(Renderer* renderer);

	// Effects //
	void ScreenGlow(Renderer* renderer);

	// Members //
	RENDER_EFFECT effect_type;
	BLEND_TYPE blend_type;
	int intensity;
	COLORREF color;
};