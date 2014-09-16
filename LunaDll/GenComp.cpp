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
	case AT_RandomComponent:
		return SpriteFunc::RandomComponent;
	case AT_RandomComponentRange:
		return SpriteFunc::RandomComponentRange;
	case AT_SetSpriteVar:
		return SpriteFunc::SetSpriteVar;
	case AT_IfSpriteVar:
		return SpriteFunc::IfSpriteVar;
	case AT_IfLunaVar:
		return SpriteFunc::IfLunaVar;
	case AT_Die:
		return SpriteFunc::Die;
	case AT_Deccelerate:
		return SpriteFunc::Deccelerate;
	case AT_AccelToPlayer:
		return SpriteFunc::AccelToPlayer;
	case AT_ApplyVariableGravity:
		return SpriteFunc::ApplyVariableGravity;
	case AT_PhaseMove:
		return SpriteFunc::PhaseMove;
	case AT_BumpMove:
		return SpriteFunc::BumpMove;
	case AT_CrashMove:
		return SpriteFunc::CrashMove;
	case AT_SetXSpeed:
		return SpriteFunc::SetXSpeed;
	case AT_SetYSpeed:
		return SpriteFunc::SetYSpeed;
	case AT_SetAlwaysProcess:
		return SpriteFunc::SetAlwaysProcess;
	case AT_SetVisible:
		return SpriteFunc::SetVisible;
	case AT_SetHitbox:
		return SpriteFunc::SetHitbox;
	case AT_TeleportNearPlayer:
		return SpriteFunc::TeleportNearPlayer;
	case AT_TeleportTo:
		return SpriteFunc::TeleportTo;
	case AT_GenerateInRadius:
		return SpriteFunc::GenerateInRadius;
	case AT_GenerateAtAngle:
		return SpriteFunc::GenerateAtAngle;
	case AT_BasicAnimate:
		return SpriteFunc::BasicAnimate;
	case AT_Blink:
		return SpriteFunc::Blink;
	case AT_AnimateFloat:
		return SpriteFunc::AnimateFloat;
	case AT_TriggerLunaEvent:
		return SpriteFunc::TriggerLunaEvent;
	case AT_HarmPlayer:
		return SpriteFunc::HarmPlayer;	
	case AT_SpriteTimer:
		return SpriteFunc::SpriteTimer;
	case AT_SpriteDebug:
		return SpriteFunc::SpriteDebug;
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