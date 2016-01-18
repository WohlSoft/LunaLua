#include "../../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Rendering/RenderOps/RenderEffectOp.h"

void LuaProxy::Effects::screenGlow(unsigned int color)
{
    RenderEffectOp* op = new RenderEffectOp();
    op->effect_type = RNDEFF_ScreenGlow;
    op->blend_type = BLEND_Additive;
    op->color = color;
    op->m_FramesLeft = 1;
    gLunaRender.AddOp(op);
}

void LuaProxy::Effects::screenGlowNegative(unsigned int color)
{
    RenderEffectOp* op = new RenderEffectOp();
    op->effect_type = RNDEFF_ScreenGlow;
    op->blend_type = BLEND_Subtractive;
    op->color = color;
    op->m_FramesLeft = 1;
    gLunaRender.AddOp(op);
}

void LuaProxy::Effects::flipX()
{
    RenderEffectOp* op = new RenderEffectOp();
    op->effect_type = RNDEFF_Flip;
    op->flip_type = FLIP_TYPE_X;
    op->m_FramesLeft = 1;
    gLunaRender.AddOp(op);
}

void LuaProxy::Effects::flipY()
{
    RenderEffectOp* op = new RenderEffectOp();
    op->effect_type = RNDEFF_Flip;
    op->flip_type = FLIP_TYPE_Y;
    op->m_FramesLeft = 1;
    gLunaRender.AddOp(op);
}

void LuaProxy::Effects::flipXY()
{
    RenderEffectOp* op = new RenderEffectOp();
    op->effect_type = RNDEFF_Flip;
    op->flip_type = FLIP_TYPE_XY;
    op->m_FramesLeft = 1;
    gLunaRender.AddOp(op);
}
