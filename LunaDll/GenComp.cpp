#include "Autocode.h"
#include "SpriteComponent.h"
#include "SpriteFuncs.h"

SpriteComponent Autocode::GenerateComponent(Autocode* obj_to_convert) {
	SpriteComponent comp;
	comp.Init((int)obj_to_convert->Length);
	comp.data1 = obj_to_convert->Target;
	comp.data2 = obj_to_convert->Param1;
	comp.data3 = obj_to_convert->Param2;
	comp.data4 = obj_to_convert->Param3;	
	comp.data5 = obj_to_convert->MyString;
	comp.lookup_code = obj_to_convert->ActiveSection;

	comp.func = Autocode::GetSpriteFunc(obj_to_convert);
	return comp;
}

pfnSprFunc Autocode::GetSpriteFunc(Autocode* pAC) {
	switch(pAC->m_Type) {
	case AT_OnPlayerCollide:
		return SpriteFunc::OnPlayerCollide;
	case AT_OnPlayerDistance:
		return SpriteFunc::OnPlayerDistance;
	case AT_WaitForPlayer:
		return SpriteFunc::WaitForPlayer;
	case AT_PlayerHoldingSprite:
		return SpriteFunc::PlayerHoldingSprite;
	case AT_Deccelerate:
		return SpriteFunc::Deccelerate;
	case AT_AccelToPlayer:
		return SpriteFunc::AccelToPlayer;
	case AT_PhaseMove:
		return SpriteFunc::PhaseMove;
	case AT_BumpMove:
		return SpriteFunc::BumpMove;
	case AT_SetXSpeed:
		return SpriteFunc::SetXSpeed;
	case AT_SetYSpeed:
		return SpriteFunc::SetYSpeed;
	case AT_SetHitbox:
		return SpriteFunc::SetHitbox;
	case AT_TeleportNearPlayer:
		return SpriteFunc::TeleportNearPlayer;
	case AT_BasicAnimate:
		return SpriteFunc::BasicAnimate;
	case AT_HarmPlayer:
		return SpriteFunc::HarmPlayer;
	default:
		return NULL;
	}
}

pfnSprDraw Autocode::GetDrawFunc(Autocode* pAC) {
	switch(pAC->m_Type) {
	case AT_StaticDraw:
		return SpriteFunc::StaticDraw;
	case AT_RelativeDraw:
		return SpriteFunc::RelativeDraw;
	default:
		return NULL;
	}
}