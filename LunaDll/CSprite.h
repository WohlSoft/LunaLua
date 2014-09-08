#pragma once

#include "Defines.h"
#include <list>
#include <vector>
#include <map>
#include "SpriteComponent.h"
#include "Hitbox.h"

class CSprite;
struct CSpriteRequest;

typedef void (*pfnSprFunc)(CSprite*, SpriteComponent* obj);
//typedef void (*pfnSprBehav)(CSprite*);
typedef void (*pfnSprDraw)(CSprite*);
//typedef void (*pfnSprDeath)(CSprite*);


// General custom sprite class
class CSprite {
public:

	/// Functions ///
	CSprite();
	void Init();

	void ClearExpiredComponents();
	
	void AddBirthComponent(SpriteComponent component);
	void AddBehaviorComponent(SpriteComponent component);
	void AddDrawComponent(pfnSprDraw component);
	void AddDeathComponent(SpriteComponent component);

	void SetImageResource(int _resource_code);
	void MakeLimitedLifetime(int new_lifetime);

	void Birth();
	void Process();
	void Draw();
	void Die();

	/// Members///
	int m_ImgResCode;				// Image bank code of image resource the sprite uses
	int m_CollisionCode;			// Collision code for collision blueprint bank (-1 == all blocks collide as solid)

	int m_FramesLeft;				// How many frames are left if dying automatically	
	int m_DrawPriorityLevel;		// 0 = Low  1 = Mid  2 = High (drawn in front)	
	bool m_StaticScreenPos;			// Whether or not sprite uses absolute screen coords for drawing
	bool m_Visible;					// Whether or not this sprite should be drawn
	bool m_Birthed;					// Whether or not sprite's birth funcs have been run yet
	bool m_Died;					// Whether or not sprite's death funcs have been run yet
	bool m_Invalidated;				// Whether or not this sprite will be removed during the next cleanup phase
	bool m_LimitedFrameLife;		// Whether or not the sprite dies automatically after a number of frames
	bool m_AnimationSet;			// Whether or not the animation parameters have been set for this sprite yet	

	double m_Xpos;
	double m_Ypos;
	double m_Ht;					// Of loaded image graphic for backwards compat with native SMBX sprites
	double m_Wd;					// Of loaded image graphic for backwards compat with native SMBX sprites
	double m_Xspd;
	double m_Yspd;	

	Hitbox m_Hitbox;				// Hitbox relative to sprite position

	int m_AnimationPhase;			// Time per animation
	int m_AnimationTimer;			// Current timer for animation (animate when reaching 0)
	int m_AnimationFrame;			// The current frame in m_GfxRects to be drawn (0 == first frame)
	std::vector<RECT> m_GfxRects;	// Spritesheet areas to draw

	std::list<SpriteComponent> m_BirthComponents;	
	std::list<SpriteComponent> m_BehavComponents;	// Currently loaded behavioral components	
	std::list<pfnSprDraw> m_DrawFuncs;
	std::list<SpriteComponent> m_DeathComponents;	

	std::map<std::wstring, double> m_CustomVars;	// User-defined vars	
};


// Obj for interfacing with sprite factory
struct CSpriteRequest {
	CSpriteRequest() : type(0), x(0), y(0), time(0), img_resource_code(0) {		}	
	int type;
	int x;
	int y;
	int time;
	int img_resource_code;
	std::wstring str;
};