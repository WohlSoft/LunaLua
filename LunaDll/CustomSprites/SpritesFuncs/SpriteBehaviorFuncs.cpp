#include "SpriteFuncs.h"
#include "../../Globals.h"
#include "../../Rendering/RenderOps/RenderBitmapOp.h"
#include "../../Rendering/RenderOps/RenderRectOp.h"
#include "../../SMBXInternal/PlayerMOB.h"
#include "../../Misc/MiscFuncs.h"
#include "../../CellManager/CellManager.h"
#include "../../SMBXInternal/Blocks.h"
#include <math.h>

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
        if (me->m_Hitbox.Test((int)demo->momentum.x, (int)demo->momentum.y,
            (int)demo->momentum.width, (int)demo->momentum.height)) {
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

// SET SPRITE VAR
void SpriteFunc::SetSpriteVar(CSprite* me, SpriteComponent* obj) {
	if(obj->data5.length() > 0) {		
		me->SetCustomVar(obj->data5, (OPTYPE)(int)obj->data2, obj->data3);
	}
}

// IF SPRITE VAR
void SpriteFunc::IfSpriteVar(CSprite* me, SpriteComponent* obj) {
	if(obj->data5.length() > 0) {
		if(me->CustomVarExists(obj->data5)) {

			double var_val = me->GetCustomVar(obj->data5);
			double check_against = obj->data3;
			double component_to_activate = obj->data4;

			switch((COMPARETYPE)(int)obj->data2) {
			case CMPT_EQUALS:
				if(var_val == check_against)
					Activate((int)component_to_activate, me);
				break;
			case CMPT_GREATER:
				if(var_val > check_against)
					Activate((int)component_to_activate, me);
				break;
			case CMPT_LESS:
				if(var_val < check_against)
					Activate((int)component_to_activate, me);
				break;
			case CMPT_NOTEQ:
				if(var_val != check_against)
					Activate((int)component_to_activate, me);
				break;
			default:
				break;
			}
		}
	}
}

// IF LUNA VAR
void SpriteFunc::IfLunaVar(CSprite* me, SpriteComponent* obj) {
	if(obj->data5.length() > 0) {
		if(gAutoMan.VarExists(obj->data5)) {			
			double var_val = gAutoMan.GetVar(obj->data5);
			double check_against = obj->data3;
			double component_to_activate = obj->data4;

			switch((COMPARETYPE)(int)obj->data2) {
			case CMPT_EQUALS:
				if(var_val == check_against)
					Activate((int)component_to_activate, me);
				break;
			case CMPT_GREATER:
				if(var_val > check_against)
					Activate((int)component_to_activate, me);
				break;
			case CMPT_LESS:
				if(var_val < check_against)
					Activate((int)component_to_activate, me);
				break;
			case CMPT_NOTEQ:
				if(var_val != check_against)
					Activate((int)component_to_activate, me);
				break;
			default:
				break;
			}
		}
	}
}

// RANDOM COMPONENT
void SpriteFunc::RandomComponent(CSprite* me, SpriteComponent* obj) {
	int choice = rand() % 4;
	switch(choice) {
		case 0:
			Activate((int)obj->data1, me);
			break;
		case 1:
			Activate((int)obj->data2, me);
			break;
		case 2:
			Activate((int)obj->data3, me);
			break;
		case 3:
			Activate((int)obj->data4, me);
			break;
		}		
}

// RANDOM COMPONENT RANGE
void SpriteFunc::RandomComponentRange(CSprite* me, SpriteComponent* obj) {
	int val1 = (int)obj->data1;
	int val2 = (int)obj->data2;
	if(val1 < val2) { // rule out bad values
		int diff = val2 - val1;
		int choice = rand() % diff;
		Activate(val1 + choice, me);		
	}
}

// DIE
void SpriteFunc::Die(CSprite* me, SpriteComponent* obj) {
	me->Die();
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
        if (demo->momentum.x < me->m_Xpos) {
			me->m_Xspd -= obj->data1;
		}
		else {
			me->m_Xspd += obj->data1;
		}
        if (demo->momentum.y < me->m_Ypos) {
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

// APPLY VARIABLE GRAVITY
void SpriteFunc::ApplyVariableGravity(CSprite* me, SpriteComponent* obj) {
	double var = gAutoMan.GetVar(obj->data5);		
	if(obj->data2 == 0) {// x
		me->m_Xspd += var;
	}
	else { // y
		me->m_Yspd += var;
	}
}

// ON PLAYER COLLIDE
void SpriteFunc::OnPlayerCollide(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);

	if(demo) {				
		if(obj->data2 == 0) { // player normal hitbox
            if (me->m_Hitbox.Test((int)demo->momentum.x, (int)demo->momentum.y, (int)demo->momentum.width, (int)demo->momentum.height)) {
				Activate((int)obj->data4, me);
			}
		}
		else { // special small circle hitbox
			double extent = obj->data2 / 2;

            double cx = demo->momentum.x + (demo->momentum.width / 2);
            double cy = demo->momentum.y + (demo->momentum.height / 2);
			if(me->m_Hitbox.Test((int)cx, (int)cy, (int)extent)) {		
				Activate((int)obj->data4, me);
			}
		}
	}
}

// ON PLAYER DISTANCE
void SpriteFunc::OnPlayerDistance(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {
        double xdist = abs(demo->momentum.x - me->m_Xpos);
        double ydist = abs(demo->momentum.y - me->m_Ypos);

		// Checking farness or nearness?
		if(obj->data2 == 0) { 
			if(xdist + ydist >= obj->data1)
				Activate((int)obj->data4, me);
		}
		else {
			if(xdist + ydist <= obj->data1)
				Activate((int)obj->data4, me);
		}
	}
}

// PHASE MOVE
void SpriteFunc::PhaseMove(CSprite* me, SpriteComponent* obj) {		
	me->m_Xpos += me->m_Xspd;
	me->m_Ypos += me->m_Yspd;
}

// BUMP MOVE
void SpriteFunc::BumpMove(CSprite* me, SpriteComponent* obj) {
	double energy_loss_mod = (100 - obj->data2) / 100;

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

	// Get all blocks being collided with into collide_list
	list<CellObj> collide_list;	
    for(std::list<CellObj >::const_iterator it = nearby_list.begin();it!=nearby_list.end();it++)
    {
        CellObj cellobj=*it;
		bool collide = false;
		if(cellobj.Type == CLOBJ_SMBXBLOCK) {
			Block* block = (Block*)cellobj.pObj;
			if(!block->IsHidden && !block->IsInvisible) {				
                collide = me->m_Hitbox.Test((int)block->mometum.x, (int)block->mometum.y, (int)block->mometum.width, (int)block->mometum.height);
				if(collide) {
					collide_list.push_back(cellobj);
				}
			}
		}
	}

	// Sort the blocks by distance to find the best one
	gCellMan.SortByNearest(&collide_list, me->m_Hitbox.CenterX(), me->m_Hitbox.CenterY());	
	
	// Force sprite out of block if colliding with block, and reverse speed according to energy_loss_mod
	if(collide_list.size() > 0) {
        for(std::list<CellObj >::const_iterator it = collide_list.begin(); it!=collide_list.end();it++)
        {
            CellObj cellobj = *it;
			if(cellobj.Type == CLOBJ_SMBXBLOCK) {
				Block* block = (Block*)cellobj.pObj;
				if(!block->IsHidden && !block->IsInvisible 
                    && me->m_Hitbox.Test((int)block->mometum.x, (int)block->mometum.y, (int)block->mometum.width, (int)block->mometum.height)) {
					double sprite_bot = me->m_Hitbox.CalcBottom();
					double sprite_right = me->m_Hitbox.CalcRight();
					double sprite_top = me->m_Hitbox.CalcTop();
					double sprite_left = me->m_Hitbox.CalcLeft();

					if(false) { // debugging
						double* pCameraY = (double*)GM_CAMERA_Y;
						double camtop = -pCameraY[1];
						double* pCameraX = (double*)GM_CAMERA_X;
						double camleft = -pCameraX[1];
						//debug_rect.color = COLOR;
						COLOR += 0x55000055;
						//debug_rect.m_FramesLeft = 1;
						//debug_rect.x1 = block->XPos - camleft;
						//debug_rect.y1 = block->YPos - camtop;
						//debug_rect.x2 = (block->XPos + block->W) - camleft;
						//debug_rect.y2 = (block->YPos + block->H) - camtop;
						//debug_rect.Draw(&gLunaRender);
					}

					if(me->m_CollisionCode == -1) { // default solid collision
                        double block_topcol = abs(block->mometum.y - sprite_bot);
                        double block_botcol = abs((block->mometum.y + block->mometum.height) - sprite_top);
                        double block_leftcol = abs(block->mometum.x - sprite_right);
                        double block_rightcol = abs((block->mometum.x + block->mometum.width) - sprite_left);

						// Determine best direction to free sprite
						// Top collision, push sprite up and out
						if(block_topcol <= block_botcol && block_topcol <= block_leftcol && 
							block_topcol <= block_rightcol && !collided_top) {
                            me->m_Ypos = (block->mometum.y - me->m_Hitbox.H) - 1;
							me->m_Yspd = -(me->m_Yspd * energy_loss_mod);
							collided_top = true;
						} 

						// Bot collision, push sprite down
						else if(block_botcol <= block_leftcol && block_botcol <= block_rightcol && !collided_right) {							
                            me->m_Ypos = ((block->mometum.y + block->mometum.height) - me->m_Hitbox.Top_off) + 1;
							me->m_Yspd = -(me->m_Yspd * energy_loss_mod);							
							collided_bot = true;
						}

						// Left collision, push sprite left
						else if(block_leftcol <= block_rightcol && !collided_left) {							
                            me->m_Xpos = (block->mometum.x - me->m_Hitbox.W) - 1;
							me->m_Xspd = -(me->m_Xspd  * energy_loss_mod);
							collided_left = true;
						}

						// Right collision, push sprite right
						else if(!collided_right){							
                            me->m_Xpos = ((block->mometum.x + block->mometum.width) - me->m_Hitbox.Left_off) + 1;
							me->m_Xspd = -(me->m_Xspd  * energy_loss_mod);
							collided_right = true;
						}
					}
				}
			}
		}
	}
}

// CRASH MOVE
void SpriteFunc::CrashMove(CSprite* me, SpriteComponent* obj) {
	me->m_Xpos += me->m_Xspd;
	me->m_Ypos += me->m_Yspd;

	list<CellObj> collide_list;
	gCellMan.GetObjectsOfInterest(&collide_list, me->m_Hitbox.CalcLeft(), 
												me->m_Hitbox.CalcTop(), 
												(int)me->m_Hitbox.W,
												(int)me->m_Hitbox.H);
	if(collide_list.size() > 0) {		
        for(std::list<CellObj >::const_iterator it = collide_list.begin();it!=collide_list.end();it++)
        {
            CellObj cellobj=*it;
			if(cellobj.Type == CLOBJ_SMBXBLOCK) {
				Block* block = (Block*)cellobj.pObj;
				if(!block->IsHidden && !block->IsInvisible 
                    && me->m_Hitbox.Test((int)block->mometum.x, (int)block->mometum.y, (int)block->mometum.width, (int)block->mometum.height)) {
						me->Die();
				}
			}
		}
	}
}

// SET X SPEED
void SpriteFunc::SetXSpeed(CSprite* me, SpriteComponent* obj) {
	me->m_Xspd = obj->data1;	
}

// SET Y SPEED
void SpriteFunc::SetYSpeed(CSprite* me, SpriteComponent* obj) {
	me->m_Yspd = obj->data1;	
}

// SET ALWAYS PROCESS
void SpriteFunc::SetAlwaysProcess(CSprite* me, SpriteComponent* obj) {
	me->m_AlwaysProcess = (bool)obj->data1;
}

// SET VISIBLE
void SpriteFunc::SetVisible(CSprite* me, SpriteComponent* obj) {
	me->m_Visible = (bool)obj->data1;
}

// SET HITBOX
void SpriteFunc::SetHitbox(CSprite* me, SpriteComponent* obj) {
	if(obj->data5.find(L"circle") != std::wstring::npos) {
		me->m_Hitbox.CollisionType = 1;
	}
	else {
		me->m_Hitbox.CollisionType = 0;
	}
	me->m_Hitbox.Left_off = (short)obj->data1;
	me->m_Hitbox.Top_off = (short)obj->data2;
	me->m_Hitbox.W = (short)obj->data3;
	me->m_Hitbox.H = (short)obj->data4;
}

// TELEPORT NEAR PLAYER
void SpriteFunc::TeleportNearPlayer(CSprite* me, SpriteComponent* obj) {
	PlayerMOB* demo = Player::Get(1);
	if(demo) {
        double cx = demo->momentum.x;
        double cy = demo->momentum.y;
		double phase = rand() % 360;
		double xoff = sin(phase) * obj->data1;
		double yoff = cos(phase) * obj->data1;
		me->m_Xpos = cx + xoff;
		me->m_Ypos = cy + yoff;		
	}
}

// TELEPORT TO
void SpriteFunc::TeleportTo(CSprite* me, SpriteComponent* obj) {
	me->m_Xpos = obj->data1;
	me->m_Ypos = obj->data2;
}

// TRIGGER LUNA EVENT -- Trigger a lunadll script event (such as #1000)
void SpriteFunc::TriggerLunaEvent(CSprite* me, SpriteComponent* obj) {	
	if(obj->data1 > 21)
		gAutoMan.ActivateCustomEvents(0, (int)obj->data1);
}

// HARM PLAYER
void SpriteFunc::HarmPlayer(CSprite* me, SpriteComponent* obj) {	
	int player = 1;
	Player::Harm(&player);
}

// GENERATE IN RADIUS
void SpriteFunc::GenerateInRadius(CSprite* me, SpriteComponent* obj) {
	double rand_x; double rand_y;	
	RandomPointInRadius(&rand_x, &rand_y, me->m_Hitbox.CenterX(), me->m_Hitbox.CenterY(), (int)obj->data3);

	CSpriteRequest req;
	req.type = 0;
	req.img_resource_code = (int)obj->data2;
	req.x = (int)rand_x;
	req.y = (int)rand_y;
	req.time = (int)obj->data4;
	req.str = obj->data5;
	req.spawned = true;

	gSpriteMan.InstantiateSprite(&req, false);
}

// GENERATE AT ANGLE
void SpriteFunc::GenerateAtAngle(CSprite* me, SpriteComponent* obj) {
	double angle = me->GetCustomVar(CVAR_GEN_ANGLE);
	double speed = obj->data3;

	double vx = cos(angle) * speed;					// vector x speed
	double vy = sin(angle) * speed;					// vector y speed
	double gx = me->m_Hitbox.CenterX() + (vx * 2);	// generation point
	double gy = me->m_Hitbox.CenterY() + (vy * 2);	// generation point

	CSpriteRequest req;
	req.type = 0;
	req.img_resource_code = (int)obj->data2;
	req.x = (int)gx;
	req.y = (int)gy;
	req.time = (int)obj->data4;
	req.str = obj->data5;

	req.x_speed = vx;
	req.y_speed = vy;
	req.spawned = true;

	gSpriteMan.InstantiateSprite(&req, false);
}

// SPRITE TIMER
void SpriteFunc::SpriteTimer(CSprite* me, SpriteComponent* obj) {
	bool repeat = (bool)obj->data3;
	int timer = (int)obj->run_time;
	if(timer == 1 || timer == 0) {
		if(timer == 1)
			obj->expired = true;
		Activate((int)obj->data4, me);

		if(repeat) {
			obj->expired = false;
			obj->run_time = obj->org_time;
		}
	}
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

// ANIMATE FLOAT
void SpriteFunc::AnimateFloat(CSprite* me, SpriteComponent* obj) {
	double speed = obj->data1;
	double x_mag = obj->data2;
	double y_mag = obj->data3;
	if(speed != 0 && (x_mag != 0 || y_mag != 0)) {
		double frame_val = me->m_FrameCounter / speed;
		if(x_mag != 0) {
			me->m_GfxXOffset = (int)(cos(frame_val) * x_mag);
		}
		if(y_mag != 0) {
			me->m_GfxYOffset = (int)(sin(frame_val) * y_mag);
		}
	}
}

// BLINK
void SpriteFunc::Blink(CSprite* me, SpriteComponent* obj) {
	int mod = (int)obj->run_time % (int)obj->data1;
	if(mod == 0) {
		me->m_Visible = (bool)obj->data2;
	}
	else {
		me->m_Visible = (bool)(obj->data2 == 0 ? 1 : 0);
	}
}

// SPRITE DEBUG
void SpriteFunc::SpriteDebug(CSprite* me, SpriteComponent* obj) {
	gLunaRender.DebugPrint(L"XPOS - ", me->m_Xpos);
	gLunaRender.DebugPrint(L"YPOS - ", me->m_Ypos);
	gLunaRender.DebugPrint(L"XSPD - ", me->m_Xspd);
	gLunaRender.DebugPrint(L"YSPD - ", me->m_Yspd);
	gLunaRender.DebugPrint(L"FRAME - ", me->m_AnimationFrame);
	gLunaRender.DebugPrint(L"VISIBLE - ", me->m_Visible);
	gLunaRender.DebugPrint(L"CVARS - ", me->m_CustomVars.size());
	gLunaRender.DebugPrint(L"BEHAVIORS - ", me->m_BehavComponents.size());
}
