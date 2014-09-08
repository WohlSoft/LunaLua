#include "CSprite.h"
#include "Rendering.h"

// CTOR
CSprite::CSprite() {
	Init();
}

// INIT
void CSprite::Init() {
	m_ImgResCode = -1;
	m_CollisionCode = -1;
	m_StaticScreenPos = false;
	m_Visible = true;
	m_Birthed = false;
	m_Died = false;
	m_Invalidated = false;
	m_LimitedFrameLife = false;
	m_AnimationSet = false;
	m_FramesLeft = 0;	
	m_DrawPriorityLevel = 1;
	m_Xpos = 0;
	m_Ypos = 0;
	m_Ht = 0;
	m_Wd = 0;
	m_Xspd = 0;
	m_Yspd = 0;	
	m_AnimationPhase = 0;
	m_AnimationFrame = 0;
	m_AnimationTimer = 0;
	m_Hitbox.pParent = this;
}

// CLEAR EXPIRED COMPONENTS
void CSprite::ClearExpiredComponents() {
	std::list<SpriteComponent>::iterator iter = m_BehavComponents.begin();
	std::list<SpriteComponent>::iterator end  = m_BehavComponents.end();

	while(iter != m_BehavComponents.end()) {
		SpriteComponent comp = *iter;
		if((*iter).expired) {			
			iter = m_BehavComponents.erase(iter);
		} else {
			++iter;
		}
	}
}

#pragma region Add functions

// ADD BIRTH -- Add a birth function to sprite
void CSprite::AddBirthComponent(SpriteComponent comp) {
	if(comp.func != NULL)
		m_BirthComponents.push_back(comp);
}

// ADD BEHAVIOR -- Add a behavior component to sprite
void CSprite::AddBehaviorComponent(SpriteComponent comp) {
	if(comp.func != NULL)
		m_BehavComponents.push_back(comp);
}

// ADD DRAW -- Add a draw component to sprite
void CSprite::AddDrawComponent(pfnSprDraw func) {
	if(func != NULL)
	m_DrawFuncs.push_back(func);
}

// ADD DEATH -- Add a death function to sprite
void CSprite::AddDeathComponent(SpriteComponent comp) {
	if(comp.func != NULL)
		m_DeathComponents.push_back(comp);
}

#pragma endregion

// SET IMAGE RESOURCE
void CSprite::SetImageResource(int _resource_code) {
	m_ImgResCode = _resource_code;
}

// MAKE LIMITED LIFETIME
void CSprite::MakeLimitedLifetime(int new_lifetime) {
	this->m_LimitedFrameLife = true;
	this->m_FramesLeft = new_lifetime;
}

// BIRTH -- Run the birth components
void CSprite::Birth() {
	for (std::list<SpriteComponent>::iterator iter = m_BirthComponents.begin(); iter != m_BirthComponents.end(); ++iter) {
		(*iter).func(this, &(*iter));
	}
	m_Birthed = true;
}

// PROCESS -- Call all of the running behavior components
void CSprite::Process() {
	ClearExpiredComponents();

	if(m_Birthed == false)
		Birth();

	for (std::list<SpriteComponent>::iterator iter = m_BehavComponents.begin(); iter != m_BehavComponents.end(); ++iter) {
		(*iter).func(this, &(*iter));
		(*iter).Tick();		
	}

	// Die?
	if(this->m_LimitedFrameLife == true) {
		this->m_FramesLeft--;
		if(m_FramesLeft <= 0) {
			Die();
		}
	}
}

// DRAW -- Call all of the registered draw components
void CSprite::Draw() {
	for (std::list<pfnSprDraw>::iterator iter = m_DrawFuncs.begin(); iter != m_DrawFuncs.end(); ++iter) {
		(*iter)(this);
	}
}

// DIE -- Run the death components
void CSprite::Die() {
	for (std::list<SpriteComponent>::iterator iter = m_DeathComponents.begin(); iter != m_DeathComponents.end(); ++iter) {
		(*iter).func(this, &(*iter));
	}
	m_Died = true;
	m_Invalidated = true;
}
