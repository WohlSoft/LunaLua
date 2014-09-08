#include "SpriteFuncs.h"
#include "Globals.h"
#include "RenderBitmapOp.h"
#include "RenderRectOp.h"
#include "PlayerMOB.h"
#include "MiscFuncs.h"
#include "CellManager.h"
#include "Blocks.h"

using namespace std;

// Activate -- Formally trigger a component link in the given sprite (copy all matches into behavior list)
void Activate(int code, CSprite* spr) {
	if(code > 99) {		
		std::list<SpriteComponent*> complist;
		gSpriteMan.GetComponents(code, &complist);		
		while(complist.empty() == false) {
			spr->AddBehaviorComponent(*complist.front());
			complist.pop_front();
		}
	}
}

// PLAYER COLLECTIBLE -- Call sprite's "Die()" when touched by player
void SpriteFunc::PlayerCollectible(CSprite* me, SpriteComponent* comp) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {		
		if(me->m_Hitbox.Test((int)demo->CurXPos, (int)demo->CurYPos, 
			(int)demo->Width, (int)demo->Height)) {		
			me->Die();
		}
	}
}

// WAIT FOR PLAYER -- Activate the linked component when a player condition becomes true
void SpriteFunc::WaitForPlayer(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {
		FIELDTYPE ftype = (FIELDTYPE)_wtoi(obj->data5.c_str());
		byte* ptr = (byte*)demo;
		ptr += (int)obj->data1; // offset
		bool triggered = CheckMem((int)ptr, (int)obj->data2, (COMPARETYPE)(int)obj->data3, ftype);
		if(triggered) {
			//TODO: FINISH IT
			// me->AddBehaviorComponent(
		}
	}
}

// PLAYER HOLDING SPRITE 
void SpriteFunc::PlayerHoldingSprite(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {				
		if(Player::IsHoldingSpriteType(demo, (int)obj->data1)) {						
			Activate((int)obj->data4, me);
		}
	}
}

// DECCELERATE
void SpriteFunc::Deccelerate(CSprite* me, SpriteComponent* obj) {
	double XRate = obj->data1;
	double YRate = obj->data2;
	double Min	= obj->data3;
	double accum = 0;	
	if(me->m_Xspd >= 0 && XRate != 0) {
		accum = me->m_Xspd - XRate;
		if(accum < Min)
			accum = Min;
		me->m_Xspd = accum;
	}
	else if(XRate != 0) {
		accum = me->m_Xspd + XRate;
		if(accum > Min)
			accum = Min;
		me->m_Xspd = accum;
	}

	if(me->m_Yspd >= 0 && YRate != 0) {
		accum = me->m_Yspd - YRate;
		if(accum < Min)
			accum = Min;
		me->m_Yspd = accum;
	}
	else if(YRate != 0) {
		accum = me->m_Yspd + YRate;
		if(accum > Min)
			accum = Min;
		me->m_Yspd = accum;
	}
}

// ACCELERATE
void SpriteFunc::Accelerate(CSprite* me, SpriteComponent* obj) {
	//TODO
}

// ACCEL TO PLAYER
void SpriteFunc::AccelToPlayer(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {				
		double negmax = obj->data3 * -1;
		if(demo->CurXPos < me->m_Xpos) {
			me->m_Xspd -= obj->data1;
		}
		else {
			me->m_Xspd += obj->data1;
		}
		if(demo->CurYPos < me->m_Ypos) {
			me->m_Yspd -= obj->data2;
		}
		else {
			me->m_Yspd += obj->data2;
		}
		if(obj->data3 != 0) {
			if(me->m_Xspd > obj->data3) {
				me->m_Xspd = obj->data3;
			} 
			else if(me->m_Xspd < negmax) {
				me->m_Xspd = negmax;
			}
			if(me->m_Yspd > obj->data3) {
				me->m_Yspd = obj->data3;
			}
			else if(me->m_Yspd < negmax) {
				me->m_Yspd = negmax;
			}
		}		
	}
}

// ON PLAYER COLLIDE
void SpriteFunc::OnPlayerCollide(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);

	if(demo) {				
		if(me->m_Hitbox.Test((int)demo->CurXPos, (int)demo->CurYPos, (int)demo->Width, (int)demo->Height)) {		
				Activate((int)obj->data4, me);				
		}
	}
}

// ON PLAYER DISTANCE
void SpriteFunc::OnPlayerDistance(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {
		double xdist = abs(demo->CurXPos - me->m_Xpos);
		double ydist = abs(demo->CurYPos - me->m_Ypos);		

		if(xdist + ydist >= obj->data1)
			Activate((int)obj->data4, me);
	}
}

// PHASE MOVE
void SpriteFunc::PhaseMove(CSprite* me, SpriteComponent* obj) {		
	me->m_Xpos += me->m_Xspd;
	me->m_Ypos += me->m_Yspd;
}

// BUMP MOVE
void SpriteFunc::BumpMove(CSprite* me, SpriteComponent* obj) {
	double energy_loss_percent = obj->data2;
	me->m_Xpos += me->m_Xspd;
	me->m_Ypos += me->m_Yspd;

	DWORD COLOR = 0x11111111;

	bool collided_left = false;
	bool collided_right = false;
	bool collided_top = false;
	bool collided_bot = false;	

	list<CellObj> nearby_list;
	gCellMan.GetObjectsOfInterest(&nearby_list, me->m_Hitbox.CalcLeft(), 
												me->m_Hitbox.CalcTop(), 
												(int)me->m_Hitbox.W,
												(int)me->m_Hitbox.H);	

	gCellMan.SortByNearest(&nearby_list, me->m_Hitbox.CenterX(), me->m_Hitbox.CenterY());

	for each(CellObj cellobj in nearby_list) {		
		if(cellobj.Type == CLOBJ_SMBXBLOCK) {
			Block* block = (Block*)cellobj.pObj;
			if(!block->IsHidden && !block->IsInvisible) {
				bool collide = me->m_Hitbox.Test((int)block->XPos, (int)block->YPos, (int)block->W, (int)block->H);				

				// Force sprite out if colliding with block, and reverse speed
				if(collide) {
					double sprite_bot = me->m_Hitbox.CalcBottom();
					double sprite_right = me->m_Hitbox.CalcRight();
					double sprite_top = me->m_Hitbox.CalcTop();
					double sprite_left = me->m_Hitbox.CalcLeft();

					if(false) { // for debugging
						double* pCameraY = (double*)GM_CAMERA_Y;
						double camtop = -pCameraY[1];
						double* pCameraX = (double*)GM_CAMERA_X;
						double camleft = -pCameraX[1];
						RenderRectOp debug_rect;
						debug_rect.color = COLOR;
						COLOR += 0x55000055;
						debug_rect.m_FramesLeft = 1;
						debug_rect.x1 = block->XPos - camleft;
						debug_rect.y1 = block->YPos - camtop;
						debug_rect.x2 = (block->XPos + block->W) - camleft;
						debug_rect.y2 = (block->YPos + block->H) - camtop;
						debug_rect.Draw(&gLunaRender);
					}

					if(me->m_CollisionCode == -1) { // default solid collision
						double block_topcol = abs(block->YPos - sprite_bot);
						double block_botcol = abs((block->YPos + block->H) - sprite_top);
						double block_leftcol = abs(block->XPos - sprite_right);
						double block_rightcol = abs((block->XPos + block->W) - sprite_left);

						// Determine best direction to free sprite
						// Top collision, push sprite up and out
						if(block_topcol <= block_botcol && block_topcol <= block_leftcol && 
							block_topcol <= block_rightcol && !collided_top) {							
							me->m_Ypos = (block->YPos - me->m_Hitbox.H) - 1;							
							me->m_Yspd = -me->m_Yspd;							
							collided_top = true;
						} 
						// Bot collision, push sprite down
						else if(block_botcol <= block_leftcol && block_botcol <= block_rightcol && !collided_right) {							
							me->m_Ypos = ((block->YPos + block->H) - me->m_Hitbox.Top_off) + 1;								
							me->m_Yspd = -me->m_Yspd;							
							collided_bot = true;
						}
						// Left collision, push sprite left
						else if(block_leftcol <= block_rightcol && !collided_left) {							
							me->m_Xpos = (block->XPos - me->m_Hitbox.W) - 1;							
							me->m_Xspd = -me->m_Xspd;							
							collided_left = true;							
						}
						// Right collision, push sprite right
						else if(!collided_right){							
							me->m_Xpos = ((block->XPos + block->W) - me->m_Hitbox.Left_off) + 1;				
							me->m_Xspd = -me->m_Xspd;						
							collided_right = true;
						}
					}
				}
			}
		}
	}
}

// SET X SPEED
void SpriteFunc::SetXSpeed(CSprite* me, SpriteComponent* obj) {
	me->m_Xspd = obj->data1;
	gLunaRender.SafePrint(L"SETTING X", 3, 400, 300);
}

// SET Y SPEED
void SpriteFunc::SetYSpeed(CSprite* me, SpriteComponent* obj) {
	me->m_Yspd = obj->data1;
	gLunaRender.SafePrint(L"SETTING Y", 3, 400, 320);
}

// SET HITBOX
void SpriteFunc::SetHitbox(CSprite* me, SpriteComponent* obj) {
	me->m_Hitbox.Left_off = (short)obj->data1;
	me->m_Hitbox.Top_off = (short)obj->data2;
	me->m_Hitbox.W = (short)obj->data3;
	me->m_Hitbox.H = (short)obj->data4;
}

// TELEPORT NEAR PLAYER
void SpriteFunc::TeleportNearPlayer(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {
		double cx = demo->CurXPos;
		double cy = demo->CurYPos;
		double phase = rand() % 360;
		double xoff = sin(phase) * obj->data1;
		double yoff = cos(phase) * obj->data1;
		me->m_Xpos = cx + xoff;
		me->m_Ypos = cy + yoff;		
	}
}

// TRIGGER LUNA EVENT -- Trigger a lunadll script event (such as #1000)
void SpriteFunc::TriggerLunaEvent(CSprite* me, SpriteComponent* comp) {
	//Render::Print(L"ABOUT TO TRIGGER", 3, 300,360);
	if(comp->data1 > 21)
		gAutoMan.ActivateCustomEvents(0, (int)comp->data1);
}

// HARM PLAYER
void SpriteFunc::HarmPlayer(CSprite* me, SpriteComponent* obj) {	
	int player = 1;
	Player::Harm(&player);
}

// BASIC ANIMATE
void SpriteFunc::BasicAnimate(CSprite* me, SpriteComponent* obj) {
	int anim_height = (int)obj->data1;
	if(anim_height == 0)
		anim_height = 1;
	int implicit_frames = (int)me->m_Ht / anim_height;	

	// Init animation state if necessary
	if(me->m_AnimationSet == false) {
		me->m_Hitbox.H = anim_height;
		me->m_AnimationFrame = 0;
		me->m_AnimationPhase = (int)obj->data2;
		me->m_AnimationTimer = (int)obj->data2;
		me->m_GfxRects.clear();
		for(int i = 0; i < implicit_frames; i++) {			
			RECT temp;
			temp.left = 0;
			temp.right = (LONG)me->m_Wd;
			temp.top = (LONG)anim_height * i;
			temp.bottom = (LONG)anim_height; // <this is a HEIGHT argument
			me->m_GfxRects.push_back(temp);
		}
		me->m_AnimationSet = true;
	}	

	// Process animation
	me->m_AnimationTimer--;	
	if(me->m_AnimationTimer <= 0) {
		me->m_AnimationTimer = me->m_AnimationPhase;
		me->m_AnimationFrame += 1;		
		if(me->m_AnimationFrame + 1 > implicit_frames) {			
			me->m_AnimationFrame = 0;		
		}
	}
}