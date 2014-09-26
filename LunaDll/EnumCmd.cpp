#include "Autocode.h"
#include "MiscFuncs.h"
//#include "Globals.h"

// Commander reader function

AutocodeType Autocode::EnumerizeCommand(wchar_t* wbuf) {
	if(wbuf) {
		wchar_t command[100];
		ZeroMemory(command, 100 * sizeof(wchar_t));
		int success = swscanf(wbuf, L" %[^,] ,", &command);
		if(!success) {
			// Bad or mistyped command?
			wstring line = wstring(wbuf);
			if(line.size() > 10) {
				PrintSyntaxError(line);
			}
			return AT_Invalid;
		}

		if(wcscmp(command, L"FilterToSmall") == 0)
			return AT_FilterToSmall;

		if(wcscmp(command, L"FilterToBig") == 0)
			return AT_FilterToBig;

		if(wcscmp(command, L"FilterToFire") == 0)
			return AT_FilterToFire;

		if(wcscmp(command, L"FilterMount") == 0)
			return AT_FilterMount;

		if(wcscmp(command, L"FilterReservePowerup") == 0)
			return AT_FilterReservePowerup;

		if(wcscmp(command, L"FilterPlayer") == 0)
			return AT_FilterPlayer;


		if(wcscmp(command, L"SetHearts") == 0)
			return AT_SetHearts;

		if(wcscmp(command, L"HeartSystem") == 0)
			return AT_HeartSystem;

		if(wcscmp(command, L"InfiniteFlying") == 0)
			return AT_InfiniteFlying;


		if(wcscmp(command, L"ScreenEdgeBuffer") == 0)
			return AT_ScreenEdgeBuffer;


		if(wcscmp(command, L"ShowText") == 0)			
			return AT_ShowText;

		if(wcscmp(command, L"ShowNPCLifeLeft") == 0)			
			return AT_ShowNPCLifeLeft;


		if(wcscmp(command, L"Trigger") == 0)
			return AT_Trigger;		

		if(wcscmp(command, L"Timer") == 0)
			return AT_Timer;

		if(wcscmp(command, L"IfNPC") == 0)
			return AT_IfNPC;		

		if(wcscmp(command, L"BlockTrigger") == 0)
			return AT_BlockTrigger;

		if(wcscmp(command, L"TriggerRandom") == 0)
			return AT_TriggerRandom; 

		if(wcscmp(command, L"TriggerRandomRange") == 0)
			return AT_TriggerRandomRange; 

		if(wcscmp(command, L"TriggerZone") == 0)
			return AT_TriggerZone; 

		if(wcscmp(command, L"ScreenBorderTrigger") == 0)
			return AT_ScreenBorderTrigger;

		if(wcscmp(command, L"OnInput") == 0)
			return AT_OnInput;

		if(wcscmp(command, L"OnCustomCheat") == 0)
			return AT_OnCustomCheat;

		if(wcscmp(command, L"OnPlayerMem") == 0)
			return AT_OnPlayerMem;

		if(wcscmp(command, L"OnGlobalMem") == 0)
			return AT_OnGlobalMem;


		if(wcscmp(command, L"SetVar") == 0)
			return AT_SetVar;

		if(wcscmp(command, L"LoadPlayerVar") == 0)
			return AT_LoadPlayerVar;

		if(wcscmp(command, L"LoadNPCVar") == 0)
			return AT_LoadNPCVar;

		if(wcscmp(command, L"LoadGlobalVar") == 0)
			return AT_LoadGlobalVar;

		if(wcscmp(command, L"ShowVar") == 0)
			return AT_ShowVar;

		if(wcscmp(command, L"IfVar") == 0)
			return AT_IfVar;

		if(wcscmp(command, L"CompareVar") == 0)
			return AT_CompareVar;

		if(wcscmp(command, L"BankVar") == 0)
			return AT_BankVar;

		if(wcscmp(command, L"WriteBank") == 0)
			return AT_WriteBank;


		if(wcscmp(command, L"LunaControl") == 0)
			return AT_LunaControl;


		if(wcscmp(command, L"DeleteCommand") == 0)
			return AT_DeleteCommand;
		
		if(wcscmp(command, L"ModParam") == 0)
			return AT_ModParam;

		if(wcscmp(command, L"ChangeTime") == 0)
			return AT_ChangeTime;
		

		if(wcscmp(command, L"DeleteEventsFrom") == 0)
			return AT_DeleteEventsFrom;

		if(wcscmp(command, L"ClearInputString") == 0)
			return AT_ClearInputString;


		if(wcscmp(command, L"LayerXSpeed") == 0)
			return AT_LayerXSpeed;

		if(wcscmp(command, L"LayerYSpeed") == 0)
			return AT_LayerYSpeed;

		if(wcscmp(command, L"AccelerateLayerX") == 0)
			return AT_AccelerateLayerX;

		if(wcscmp(command, L"AccelerateLayerY") == 0)
			return AT_AccelerateLayerY;

		if(wcscmp(command, L"DeccelerateLayerX") == 0)
			return AT_DeccelerateLayerX;

		if(wcscmp(command, L"DeccelerateLayerY") == 0)
			return AT_DeccelerateLayerY;

		if(wcscmp(command, L"SetAllBlocksID") == 0)
			return AT_SetAllBlocksID;

		if(wcscmp(command, L"SwapAllBlocks") == 0)
			return AT_SwapAllBlocks;

		if(wcscmp(command, L"ShowAllBlocks") == 0)
			return AT_ShowAllBlocks;

		if(wcscmp(command, L"HideAllBlocks") == 0)
			return AT_HideAllBlocks;


		if(wcscmp(command, L"PushScreenBoundary") == 0)
			return AT_PushScreenBoundary;

		if(wcscmp(command, L"SnapSectionBounds") == 0)
			return AT_SnapSectionBounds;


		if(wcscmp(command, L"CyclePlayerRight") == 0)
			return AT_CyclePlayerRight;

		if(wcscmp(command, L"CyclePlayerLeft") == 0)
			return AT_CyclePlayerLeft;


		if(wcscmp(command, L"SFX") == 0)
			return AT_SFX;

		if(wcscmp(command, L"SetMusic") == 0)
			return AT_SetMusic;

		if(wcscmp(command, L"PlayMusic") == 0)
			return AT_PlayMusic;


		if(wcscmp(command, L"TriggerSMBXEvent") == 0)
			return AT_TriggerSMBXEvent;


		if(wcscmp(command, L"Kill") == 0)
			return AT_Kill;

		if(wcscmp(command, L"Hurt") == 0)
			return AT_Hurt;


		if(wcscmp(command, L"SetHits") == 0)
			return AT_SetHits;

		if(wcscmp(command, L"NPCMemSet") == 0)
			return AT_NPCMemSet;

		if(wcscmp(command, L"PlayerMemSet") == 0)
			return AT_PlayerMemSet;

		if(wcscmp(command, L"ForceFacing") == 0)
			return AT_ForceFacing;

		if(wcscmp(command, L"MemAssign") == 0)
			return AT_MemAssign;

		if(wcscmp(command, L"DebugPrint") == 0)
			return AT_DebugPrint;	

		if(wcscmp(command, L"DebugWindow") == 0)
			return AT_DebugWindow;


		if(wcscmp(command, L"CollisionScan") == 0)
			return AT_CollisionScan;


		if(wcscmp(command, L"LoadImage") == 0)
			return AT_LoadImage;	

		if(wcscmp(command, L"SpriteBlueprint") == 0)
			return AT_SpriteBlueprint;

		if(wcscmp(command, L"Attach") == 0)
			return AT_Attach;

		if(wcscmp(command, L"PlaceSprite") == 0)
			return AT_PlaceSprite;	


		// Sprite Component section
		if(wcscmp(command, L"OnPlayerCollide") == 0)
			return AT_OnPlayerCollide;

		if(wcscmp(command, L"OnPlayerDistance") == 0)
			return AT_OnPlayerDistance;

		if(wcscmp(command, L"WaitForPlayer") == 0)
			return AT_WaitForPlayer;

		if(wcscmp(command, L"PlayerHoldingSprite") == 0)
			return AT_PlayerHoldingSprite;

		if(wcscmp(command, L"RandomComponent") == 0)
			return AT_RandomComponent;

		if(wcscmp(command, L"RandomComponentRange") == 0)
			return AT_RandomComponentRange;

		if(wcscmp(command, L"SetSpriteVar") == 0)
			return AT_SetSpriteVar;

		if(wcscmp(command, L"IfSpriteVar") == 0)
			return AT_IfSpriteVar;

		if(wcscmp(command, L"IfLunaVar") == 0)
			return AT_IfLunaVar;

		if(wcscmp(command, L"Die") == 0)
			return AT_Die;

		if(wcscmp(command, L"Deccelerate") == 0)
			return AT_Deccelerate;

		if(wcscmp(command, L"AccelToPlayer") == 0)
			return AT_AccelToPlayer;

		if(wcscmp(command, L"ApplyVariableGravity") == 0)
			return AT_ApplyVariableGravity;

		if(wcscmp(command, L"PhaseMove") == 0)
			return AT_PhaseMove;

		if(wcscmp(command, L"BumpMove") == 0)
			return AT_BumpMove;

		if(wcscmp(command, L"CrashMove") == 0)
			return AT_CrashMove;

		if(wcscmp(command, L"SetXSpeed") == 0)
			return AT_SetXSpeed;

		if(wcscmp(command, L"SetYSpeed") == 0)
			return AT_SetYSpeed;

		if(wcscmp(command, L"SetAlwaysProcess") == 0)
			return AT_SetAlwaysProcess;

		if(wcscmp(command, L"SetVisible") == 0)
			return AT_SetVisible;

		if(wcscmp(command, L"SetHitbox") == 0)
			return AT_SetHitbox;

		if(wcscmp(command, L"TeleportNearPlayer") == 0)
			return AT_TeleportNearPlayer;

		if(wcscmp(command, L"TeleportTo") == 0)
			return AT_TeleportTo;

		if(wcscmp(command, L"HarmPlayer") == 0)
			return AT_HarmPlayer;

		if(wcscmp(command, L"GenerateInRadius") == 0)
			return AT_GenerateInRadius;

		if(wcscmp(command, L"GenerateAtAngle") == 0)
			return AT_GenerateAtAngle;

		if(wcscmp(command, L"BasicAnimate") == 0)
			return AT_BasicAnimate;

		if(wcscmp(command, L"Blink") == 0)
			return AT_Blink;

		if(wcscmp(command, L"AnimateFloat") == 0)
			return AT_AnimateFloat;

		if(wcscmp(command, L"TriggerLunaEvent") == 0)
			return AT_TriggerLunaEvent;

		if(wcscmp(command, L"HarmPlayer") == 0)
			return AT_HarmPlayer;

		if(wcscmp(command, L"SpriteTimer") == 0)
			return AT_SpriteTimer;

		if(wcscmp(command, L"SpriteDebug") == 0)
			return AT_SpriteDebug;


		if(wcscmp(command, L"StaticDraw") == 0)
			return AT_StaticDraw;

		if(wcscmp(command, L"RelativeDraw") == 0)
			return AT_RelativeDraw;
	}

	if(wbuf) {
		// Nothing matched. Bad or mistyped command?
		wstring line = wstring(wbuf);
		if(line.size() > 10) {
			PrintSyntaxError(line);
		}
	}

	return AT_Invalid;
}