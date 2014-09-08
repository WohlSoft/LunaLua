#include "SpriteFuncs.h"
#include "Globals.h"
#include "RenderBitmapOp.h"
#include "RenderRectOp.h"
#include "PlayerMOB.h"
#pragma warning(disable: 4018)	// signed comparison

// STATIC DRAW - Simply draw the sprite at its absolute screen coordinates
//				 by registering a new bitmap render operation
void SpriteFunc::StaticDraw(CSprite* me) {
	if(me != NULL && me->m_Visible) {
		if(me->m_AnimationFrame < me->m_GfxRects.size()) { // Frame should be less than size of GfxRect container	
			RenderBitmapOp* op = new RenderBitmapOp();
			op->m_FramesLeft = 1;
			op->x = me->m_Xpos;
			op->y = me->m_Ypos;
			op->sx1 = me->m_GfxRects[me->m_AnimationFrame].left;
			op->sy1 = me->m_GfxRects[me->m_AnimationFrame].top;
			op->sx2 = me->m_GfxRects[me->m_AnimationFrame].right;
			op->sy2 = me->m_GfxRects[me->m_AnimationFrame].bottom;
			op->img_resource_code = me->m_ImgResCode;

			gLunaRender.AddOp(op);
		}
	}
}

// RELATIVE DRAW - Calculate sprite position inside level and draw relative 
//				   to camera position by registering new bitmap render operation
void SpriteFunc::RelativeDraw(CSprite* me) {
	if(me != NULL && me->m_Visible) {
		if(me->m_AnimationFrame < me->m_GfxRects.size()) {
			double cx = 0;				// camera x (top left of screen)
			double cy = 0;				// camera y (top left of screen)
			double sx = me->m_Xpos;		// sprite x position (top left of sprite)
			double sy = me->m_Ypos;		// sprite y position (top left of sprite)

			// Calc screen draw position based on camera position
			Render::CalcCameraPos(&cx, &cy);
			sx = sx - cx;
			sy = sy - cy;

			// Register drawing operation
			RenderBitmapOp* op = new RenderBitmapOp();
			op->m_FramesLeft = 1;
			op->x = sx;
			op->y = sy;
			op->sx1 = me->m_GfxRects[me->m_AnimationFrame].left;
			op->sy1 = me->m_GfxRects[me->m_AnimationFrame].top;
			op->sx2 = me->m_GfxRects[me->m_AnimationFrame].right;
			op->sy2 = me->m_GfxRects[me->m_AnimationFrame].bottom;
			op->img_resource_code = me->m_ImgResCode;

			gLunaRender.AddOp(op);	
			return;
		}
	}
}