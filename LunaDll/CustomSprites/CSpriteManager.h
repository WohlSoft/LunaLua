#ifndef CSpriteManager_hhhhhh
#define CSpriteManager_hhhhhh

#include "SpriteComponent.h"
#include <list>
#include <map>

////////////////////////////
/// BUILTIN SPRITE TYPES /// ****************************************************************************************************
////////////////////////////
//
// 1 = Static / HUD sprite		1: Type		2: Img code		3: Xpos		4: Ypos		5: Active lifetime	6: 
// 2 = "In level" sprite		1: Type		2: Img code		3: Xpos		4: Ypos		5: Active lifetime	6: 
// 3 = Collectible item			1: Type		2: Img code		3: Xpos		4: Ypos		5: Active lifetime	6: Lundll event to trigger
// 4 = Progress bar				1: Type		2: Img code		3: Xpos		4: Ypos		5: Active lifetime	6: 
// 5 = Phanto					1: Type		2: Img code		3: Xpos		4: Ypos		5: Active lifetime	6: 
//
//////////////////////////// ****************************************************************************************************

enum BUILTIN_SPRITE_TYPE {
	BST_Custom = 0,
	BST_Static = 1,
	BST_Normal = 2,
	BST_Item = 3,
	BST_Bar = 4,
	BST_Phanto
};


struct CSpriteRequest;
class CSprite;

// Basic manager class for sprites
struct CSpriteManager {

	void ResetSpriteManager();											// Will be called on level load to re-init everything

	void BasicInit(CSprite* spr, CSpriteRequest* req, bool center);		// Quickly set frequently-used sprite attributes
	void InitializeDimensions(CSprite* spr, bool center_coords);		// Set hitbox and dimensions to size of image
	void InstantiateSprite(CSpriteRequest* req, bool center_coords);	// Officially place sprite in level (m_SpriteList)

	void AddBlueprint(const wchar_t* blueprint_name, CSprite* spr);
	CSprite* CopyFromBlueprint(wchar_t* blueprint_name);// Generate new sprite based on blueprint
	
	void RunSprites();
	int CountSprites() { return m_SpriteList.size(); }
	int CountBlueprints() { return m_SpriteBlueprints.size(); }
	int CountComponents() { return m_ComponentList.size(); }
	
	void ClearInvalidSprites(); // Don't call while iterating
	void ClearAllSprites();

	void AddSprite(CSprite* spr);

	void GetComponents(int code, std::list<SpriteComponent*>* component_list); // Get components with the given code #

	std::list<CSprite*> m_SpriteList;
	std::map<std::wstring, CSprite*> m_SpriteBlueprints;
	std::list<SpriteComponent> m_ComponentList;		// User components that can be copied (activated) into a sprite's behavior list
};

#endif
