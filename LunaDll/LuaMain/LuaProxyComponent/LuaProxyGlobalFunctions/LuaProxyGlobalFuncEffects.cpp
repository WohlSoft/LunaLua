#include "../../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Rendering/RenderOps/RenderEffectOp.h"

void LuaProxy::Effects::screenGlow(unsigned int color)
{
    RenderEffectOp* op = new RenderEffectOp(RNDEFF_ScreenGlow, BLEND_Additive, color, 100);
    op->m_FramesLeft = 1;
    op->m_PerCycleOnly = true;
    gLunaRender.AddOp(op);
}

void LuaProxy::Effects::screenGlowNegative(unsigned int color)
{
    RenderEffectOp* op = new RenderEffectOp(RNDEFF_ScreenGlow, BLEND_Subtractive, color, 100);
    op->m_FramesLeft = 1;
    op->m_PerCycleOnly = true;
    gLunaRender.AddOp(op);
}
