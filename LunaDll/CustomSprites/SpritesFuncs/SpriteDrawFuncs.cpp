#include "SpriteFuncs.h"
#include "../../Globals.h"
#include "../../Rendering/RenderOps/RenderBitmapOp.h"
#include "../../Rendering/RenderOps/RenderRectOp.h"
#include "../../SMBXInternal/PlayerMOB.h"
#ifndef __MINGW32__
#pragma warning(disable: 4018)	// signed comparison
#endif

// STATIC DRAW - Simply draw the sprite at its absolute screen coordinates
//				 by registering a new bitmap render operation
void SpriteFunc::StaticDraw(CSprite* me) {	
	if(me != NULL && me->m_Visible) {
        if(me->m_AnimationFrame < (signed)me->m_GfxRects.size()) { // Frame should be less than size of GfxRect container
			RenderBitmapOp* op = new RenderBitmapOp();
			op->m_FramesLeft = 1;
			op->x = me->m_Xpos + me->m_GfxXOffset;
			op->y = me->m_Ypos + me->m_GfxYOffset;
			op->sx = me->m_GfxRects[me->m_AnimationFrame].left;
			op->sy = me->m_GfxRects[me->m_AnimationFrame].top;
			op->sw = me->m_GfxRects[me->m_AnimationFrame].right;
			op->sh = me->m_GfxRects[me->m_AnimationFrame].bottom;
            if (me->m_directImg) {
                op->direct_img = me->m_directImg;
            } else {
                op->direct_img = Renderer::Get().GetImageForResourceCode(me->m_ImgResCode);
            }

			Renderer::Get().AddOp(op);
		}
	}
}

// RELATIVE DRAW - Calculate sprite position inside level and draw relative 
//				   to camera position by registering new bitmap render operation
void SpriteFunc::RelativeDraw(CSprite* me) {
	if(me != NULL && me->m_Visible) {
        if(me->m_AnimationFrame < (signed)me->m_GfxRects.size()) {
			double cx = 0;				// camera x (top left of screen)
			double cy = 0;				// camera y (top left of screen)
			double sx = me->m_Xpos;		// sprite x position (top left of sprite)
			double sy = me->m_Ypos;		// sprite y position (top left of sprite)
			sx +=  me->m_GfxXOffset;
			sy +=  me->m_GfxYOffset;

			// Calc screen draw position based on camera position
			Render::CalcCameraPos(&cx, &cy);
			sx = sx - cx;
			sy = sy - cy;

			// Register drawing operation
			RenderBitmapOp* op = new RenderBitmapOp();
			op->m_FramesLeft = 1;
			op->x = sx;
			op->y = sy;
			op->sx = me->m_GfxRects[me->m_AnimationFrame].left;
			op->sy = me->m_GfxRects[me->m_AnimationFrame].top;
			op->sw = me->m_GfxRects[me->m_AnimationFrame].right;
			op->sh = me->m_GfxRects[me->m_AnimationFrame].bottom;
            if (me->m_directImg) {
                op->direct_img = me->m_directImg;
            }
            else {
                op->direct_img = Renderer::Get().GetImageForResourceCode(me->m_ImgResCode);
            }

			Renderer::Get().AddOp(op);	
			return;
		}
	}
}
