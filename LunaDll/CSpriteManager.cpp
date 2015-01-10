#include "CSprite.h"
#include "CSpriteManager.h"
#include "SpriteFuncs.h"
#include "Globals.h"
#include "BMPBox.h"
#include "PlayerMOB.h"
#include "MiscFuncs.h"

// RESET SPRITE MANAGER
void CSpriteManager::ResetSpriteManager() {
	ClearAllSprites();
	m_ComponentList.clear();
}

// INSTANTIATE SPRITE -- Instantiate a new sprite
void CSpriteManager::InstantiateSprite(CSpriteRequest* req, bool center_coords) {
	CSprite* spr = NULL;

	// For built in sprites
	if(req != NULL && req->type != (BUILTIN_SPRITE_TYPE)BST_Custom) {
		switch(req->type) {

		// STATIC SPRITE
		case BST_Static: {
			spr = new CSprite;
			BasicInit(spr, req, center_coords);		
			spr->m_StaticScreenPos = true;						
			spr->AddDrawComponent(&SpriteFunc::StaticDraw);			
			break;
						 }

		// NORMAL SPRITE
		case BST_Normal: {
			spr = new CSprite;
			BasicInit(spr, req, center_coords);
			spr->AddDrawComponent(&SpriteFunc::RelativeDraw);			
			break;
					   }

		// ITEM SPRITE
		case BST_Item: {
			spr = new CSprite;
			BasicInit(spr, req, center_coords);
			spr->AddDrawComponent(&SpriteFunc::RelativeDraw);

			// Add collectible event # if applicable
			int str_arg = _wtoi(req->str.c_str());
			if(str_arg > 21) {
				SpriteComponent comp;						// Add collectible component
				comp.func = SpriteFunc::PlayerCollectible;							
				spr->AddBehaviorComponent(comp);

				if(str_arg > 21) {						// Add trigger event on death
					comp.func = SpriteFunc::TriggerLunaEvent;	
					comp.data1 = str_arg;
					spr->AddDeathComponent(comp);
				}

			}
			break;
					   }

		// PROGRESS BAR
		case BST_Bar: {
			//spr = new CSprite;
			//TODO: MAKE IT
			spr = NULL;
			spr->m_Xpos = req->x;
			spr->m_Ypos = req->y;
			break;
					  }

		// PHANTO
		case BST_Phanto: {
			// Need to add phanto to blueprints?
			if(m_SpriteBlueprints.find(L"__DefaultPhanto") == m_SpriteBlueprints.end()) {
				spr = new CSprite;
				SpriteComponent comp;
				BasicInit(spr, req, center_coords);
				spr->AddDrawComponent(&SpriteFunc::RelativeDraw);

				// Always Decelerate
				comp.Init(0);
				comp.func = SpriteFunc::Deccelerate;
				comp.data1 = 0.04;
				comp.data2 = 0.04;
				comp.data3 = 0.00;
				spr->AddBehaviorComponent(comp);				

				// Wait for player to have key -- activate 100
				comp.Init(0);
				comp.func = SpriteFunc::PlayerHoldingSprite;
				comp.data1 = 31; // key ID				
				comp.data4 = 100; // activates 100
				spr->AddBehaviorComponent(comp);

				comp.Init(0);
				comp.func = SpriteFunc::PlayerHoldingSprite;
				comp.data1 = 0x0E; // key ID alternate
				comp.data4 = 100; // activates 100
				spr->AddBehaviorComponent(comp);

				comp.Init(0); // Phase Move
				comp.func = SpriteFunc::PhaseMove;
                //comp.lookup_code;
				spr->AddBehaviorComponent(comp);

				// #100 -- Chase player / active mode
				comp.Init(1);
				comp.func = SpriteFunc::AccelToPlayer;
				comp.data1 = 0.11;
				comp.data2 = 0.11;
				comp.data3 = 5.6;				
				comp.lookup_code = 100;
				gSpriteMan.m_ComponentList.push_back(comp);								

				comp.Init(1); // Check for collision -- activate 101
				comp.func = SpriteFunc::OnPlayerCollide;
				comp.data4 = 101; // harm player
				comp.lookup_code = 100;
				gSpriteMan.m_ComponentList.push_back(comp);

				comp.Init(1); // Check for too much distance from player -- activate 102
				comp.func = SpriteFunc::OnPlayerDistance;
				comp.data1 = 3000;
				comp.data4 = 102; // tele near player
				comp.lookup_code = 100;
				gSpriteMan.m_ComponentList.push_back(comp);

				// #101 -- harm player
				comp.Init(1); // Check for too much distance from player
				comp.func = SpriteFunc::HarmPlayer;				
				comp.lookup_code = 101;
				gSpriteMan.m_ComponentList.push_back(comp);

				// #102 -- teleport near player
				comp.Init(1); // Check for too much distance from player
				comp.func = SpriteFunc::TeleportNearPlayer;
				comp.data1 = 1000;
				comp.lookup_code = 102;
				gSpriteMan.m_ComponentList.push_back(comp);
				m_SpriteBlueprints[L"__DefaultPhanto"] = spr;
			}
            wchar_t* defPhant = L"__DefaultPhanto";
            CSprite* from_bp = CopyFromBlueprint(defPhant);
			from_bp->m_Xpos = req->x;
			from_bp->m_Ypos = req->y;
			from_bp->SetImageResource(req->img_resource_code);
			InitializeDimensions(from_bp, center_coords);	
			if(req->time != 0)
				from_bp->MakeLimitedLifetime(req->time);
			spr = from_bp;
			break;
					  }

		} //< Switch

		// Add the sprite
		if(spr != NULL) {
			AddSprite(spr);
		}
	}
	// Else, instantiate custom sprite?
	else if(req != NULL && req->type == BST_Custom) {
		CSprite* from_bp = CopyFromBlueprint(const_cast<wchar_t*>(req->str.c_str()));
		if(from_bp != NULL) {
			from_bp->m_Xpos = req->x;
			from_bp->m_Ypos = req->y;
			from_bp->m_Xspd = req->x_speed;
			from_bp->m_Yspd = req->y_speed;
			from_bp->SetImageResource(req->img_resource_code);
			InitializeDimensions(from_bp, center_coords);	
			if(req->time != 0)
				from_bp->MakeLimitedLifetime(req->time);
			from_bp->Birth();
			AddSprite(from_bp);
		}
	}
}

// ADD BLUEPRINT
void CSpriteManager::AddBlueprint(const wchar_t* blueprint_name, CSprite* spr) {	
	m_SpriteBlueprints[blueprint_name] = spr;
}

// COPY FROM BLUEPRINT -- Returns an allocated copy of a sprite from blueprint list
CSprite* CSpriteManager::CopyFromBlueprint(wchar_t* blueprint_name) {
	if(m_SpriteBlueprints.find(blueprint_name) != m_SpriteBlueprints.end()) {
		CSprite* newspr = new CSprite;
		*newspr = *m_SpriteBlueprints[blueprint_name];
		return newspr;
	}
	else {
		return NULL;
	}
}

// ADD SPRITE
void CSpriteManager::AddSprite(CSprite* spr) {
	m_SpriteList.push_back(spr);	
}

// RUN SPRITES -- Run each sprite
void CSpriteManager::RunSprites() {
	ClearInvalidSprites();		
	PlayerMOB* demo = Player::Get(1);

	if(demo) {
		// Process each
		if(GM_PAUSE_OPEN == 0) {
			for (std::list<CSprite*>::iterator iter = m_SpriteList.begin();	iter != m_SpriteList.end(); ++iter) {			
				if(!(*iter)->m_Invalidated) {// Don't process invalids					
					if(ComputeLevelSection((int)(*iter)->m_Xpos, (int)(*iter)->m_Ypos) == demo->CurrentSection+1 || 
						(*iter)->m_AlwaysProcess || (*iter)->m_StaticScreenPos) { // Valid level section to process in?
						(*iter)->Process();						
					}
				}
			}
		}

		// Draw each
		for (std::list<CSprite*>::iterator iter = m_SpriteList.begin();	iter != m_SpriteList.end(); ++iter) {			
			if(!(*iter)->m_Invalidated) { 
				if((*iter)->m_StaticScreenPos || Render::IsOnScreen((*iter)->m_Xpos, (*iter)->m_Ypos, (*iter)->m_Wd, (*iter)->m_Ht)) {
					(*iter)->Draw();
				}
			}
		}
	}
}

// CLEAR INVALID SPRITES -- Cleans up any sprites set to be deleted
void CSpriteManager::ClearInvalidSprites() {
	std::list<CSprite*>::iterator iter = m_SpriteList.begin();
	std::list<CSprite*>::iterator end = m_SpriteList.end();

	while(iter != m_SpriteList.end()) {
        //CSprite* spr = *iter;
		if((*iter)->m_Invalidated) {
			delete (*iter);
			iter = m_SpriteList.erase(iter);
		} else {
			++iter;
		}
	}
}

// CLEAR ALL SPRITES
void CSpriteManager::ClearAllSprites() {
	while(m_SpriteList.empty() == false) {
		delete m_SpriteList.back();
		m_SpriteList.pop_back();
	}
}

// BASIC INIT
void CSpriteManager::BasicInit(CSprite* spr, CSpriteRequest* pReq, bool center) {
	spr->m_Xpos = pReq->x;
	spr->m_Ypos = pReq->y;			
	//spr->m_StaticScreenPos = false;
	spr->SetImageResource(pReq->img_resource_code);
	InitializeDimensions(spr, center);	
	if(pReq->time != 0)
		spr->MakeLimitedLifetime(pReq->time);
}

// INITIALIZE DIMENSIONS -- Resets sprite hitbox according to image. Needs img code set and image loaded
void CSpriteManager::InitializeDimensions(CSprite* spr, bool center_coords) {

	BMPBox* box = gLunaRender.LoadedImages[spr->m_ImgResCode];

	if(box != NULL) {		
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = box->m_W;
		rect.bottom = box->m_H;
		spr->m_GfxRects.clear();
		spr->m_GfxRects.push_back(rect);

		spr->m_Ht = (double)rect.bottom;
		spr->m_Wd = (double)rect.right;
		spr->m_AnimationSet = false;		

		spr->m_Hitbox.Left_off = 0;
		spr->m_Hitbox.Top_off = 0;
		spr->m_Hitbox.W = (short)rect.right;
		spr->m_Hitbox.H = (short)rect.bottom;
		spr->m_Hitbox.CollisionType = 0;
		spr->m_Hitbox.pParent = spr;

		if(center_coords) { // Fix to generate with x/y as center instead of minimum left/top
			spr->m_Xpos -= spr->m_Wd / 2;
			spr->m_Ypos -= spr->m_Ht / 2;
		}
	}
}

// GET COMPONENTS
void CSpriteManager::GetComponents(int code, std::list<SpriteComponent*>* component_list) {
	for (std::list<SpriteComponent>::iterator iter = m_ComponentList.begin(), end = m_ComponentList.end(); iter != end; ++iter) {	
		// Lookup components that match the code
		if((*iter).lookup_code == code) {			
			component_list->push_back(&(*iter));
		}
	}
}
