#include "Main.h"
//#include <windows.h>
#include <string>
#include "Globals.h"
#include "GlobalFuncs.h"
#include "Defines.h"
#include "Layer/Layer.h"
#include "LevelCodes/LevelCodes.h"
#include "MOBs/PlayerMOB.h"
#include "Rendering/Rendering.h"
#include "Level/Level.h"
#include "MOBs/NPCs.h"
#include "Blocks/Blocks.h"
#include "Misc/MiscFuncs.h"
#include "Sound/Sound.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Input/Input.h"
#include "SMBX_Events/SMBXEvents.h"
#include "LuaMain/LunaLuaMain.h"
#include "Misc/RuntimeHook.h"
#include "SdlMusic/MusicManager.h"

#define PATCHIT 1

// Standard DLL loader main
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		InitGlobals();
#if PATCHIT
		TrySkipPatch();
#endif // PATCHIT
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		CleanUp();
		break;
	}
	return TRUE;
}

// *EXPORT* On Level Load -- Run once as a level is loaded (including title screen level)
int OnLvlLoad() {

	// Restore some code the hook overwrote
	*(DWORD*)0x00B25958 = 0;
    resetDefines();

#ifndef NO_SDL
	if(!episodeStarted)
	{//Load custom sounds
		std::string wldPath = wstr2str(std::wstring((wchar_t*)GM_FULLDIR));
		std::string SndRoot = MusicManager::SndRoot();
		replaceSubStr(wldPath, "\"", "");
		replaceSubStr(wldPath, "\\\\",  "\\");
		replaceSubStr(wldPath, "/",  "\\");

		replaceSubStr(SndRoot, "\"", "");
		replaceSubStr(SndRoot, "\\\\",  "\\");
		replaceSubStr(SndRoot, "/",  "\\");

		if(wldPath!=SndRoot)
		{
			MusicManager::loadCustomSounds(wldPath+"\\");
			//MessageBoxA(0, std::string(wldPath+"\n"+SndRoot+"\nLevel started").c_str(), "Debug", 0);
		}
	}
#endif
	// Clean up leftovers
	gSkipSMBXHUD = false;
	gIsOverworld = false;
	gLunaRender.ClearAll();
	gSpriteMan.ResetSpriteManager();
	gCellMan.Reset();
	gSavedVarBank.ClearBank();	
	Input::ResetAll();

	// Update renderer stuff
	gLunaRender.ReloadScreenHDC();

	if(gLunaEnabled) {
		// Load autocode
		gAutoMan.Clear(false);		
		gAutoMan.ReadFile(std::wstring((wchar_t*)GM_FULLDIR));

		// Try to load world codes		
		gAutoMan.ReadWorld(std::wstring((wchar_t*)GM_FULLDIR));

		// Init var bank
		gSavedVarBank.TryLoadWorldVars();
		gSavedVarBank.CheckSaveDeletion();
		gSavedVarBank.CopyBank(&gAutoMan.m_UserVars);

        LunaLua::init(std::wstring((wchar_t*)GM_FULLDIR));

		// Do some stuff
		gAutoMan.DoEvents(true); // do with init

		// Init some stuff
		InitLevel();	
		gAutoMan.m_Hearts = 2;	

		// Recount deaths
		gDeathCounter.Recount();
	}

    //PGE DBG STUFF
    //readAndWriteNPCSettings();
    //overwriteFunc();

	return 0;
}

// *EXPORT* Test Func -- Run once per gameplay frame
int TestFunc()
{
	
	// Clean up
	gLunaRender.ClearExpired();
	gAutoMan.ClearExpired();
	gSavedVarBank.CheckSaveDeletion();

	// Update inputs
	Input::CheckSpecialCheats();
	Input::UpdateInputTasks();	



	if(gLunaEnabled) {	
        LunaLua::Do();

		// Run autocode
		gAutoMan.DoEvents(false);

		// Update some stuff
		gFrames++;	
		gDeathCounter.UpdateDeaths(true);
		gSavedVarBank.SaveIfNeeded();

		// Run any framecode
		TestFrameCode();
		LevelFrameCode();
	}
	return 0;
}

// *EXPORT* HUD Hook -- Runs each time the HUD is drawn.
int HUDHook()
{

	if(gLunaEnabled) {
		OnHUDDraw();
	}

	// Overwrite return address? (skip hud drawing)
	if(gSkipSMBXHUD) {
        #ifndef __MINGW32__
		__asm {
			MOV DWORD PTR DS: [ESP+8], 0x00987C10
        }
        #else
        asm(".intel_syntax noprefix\n"
        "MOV DWORD PTR DS: [ESP+8], 0x00987C10\n"
        ".att_syntax\n");
        #endif
    }

	// Restore some code the hook overwrote
    #ifndef __MINGW32__
	__asm {
		MOV AX, WORD PTR DS: 0x00B25130
		CMP AX, 5
		MOV BX, 0
	}
    #else
    asm(".intel_syntax noprefix\n"
    "MOV AX, WORD PTR DS: 0x00B25130\n"
    "CMP AX, 5\n"
    "MOV BX, 0\n"
    ".att_syntax\n");
    #endif
	return *(WORD*)0x00B25130;
}

void OnHUDDraw() {

	if(gShowDemoCounter)
		gDeathCounter.Draw();

	gSpriteMan.RunSprites();
	gLunaRender.RenderAll();
}


// TEST CODE - This code will run every frame everywhere, making for easy testing
void TestFrameCode() {

	/// DEBUG STUFF//

	//- Uncomment to test variable bank
	//for each(pair<wstring, double> kvp in gSavedVarBank.m_VarBank) {
	//	gLunaRender.DebugPrint(kvp.first, kvp.second);
	//}

	//static double bgX = 0;
	//double** pBGs = (double **)0x00B2B984;
	//pBGs[0][1] = bgX;
	//bgX -= 0.1;

}

// LEVEL FRAME CODE - This will be run every frame of leveltime. Currently a low-cost enumeration switch based on the loaded level
void LevelFrameCode() {

	switch(gLevelEnum)
	{

	case DllTestLvl:
		dlltestlvlCode();
		break;

	case QraestoliaCaverns:
		QraestoliaCavernsCode();
		break;

	case TheFloorIsLava:
		TheFloorisLavaCode();
		break;

	case Calleoca:
		CalleocaCode();
		break;

	case Snowbordin:
		SAJSnowbordin::SnowbordinCode();
		break;

	case Science:
		ScienceBattle::ScienceCode();

	case CurtainTortoise:
		//CurtainTortoiseCode();
		break;

	case AbstractAssault:
		AbstractAssaultCode();
		break;
		
	case DemosBrain:
		DemosBrainCode();
		break;

	case EuroShellRandD:
		//EuroShellRandDCode();
		break;

	case ThouStartsANewVideo:
		KilArmoryCode();
		break;

	case Invalid:
	default:
		break;
	}
}

// INIT LEVEL - This should be called during the level load hook function. It'll also figure out which level is being played.
//				Also put init code in here if you want.
void InitLevel() {

	// Reset some variables
	gFrames = 0;
	gLevelEnum = Invalid;

	std::wstring curlvl = Level::GetName();
	PlayerMOB* demo = Player::Get(1);


	/// INITIALIZATION BLOCKS ///

	// Example init block
	if(curlvl == L"dlltest.lvl") {
		gLevelEnum = DllTestLvl;
		gCurrentMainPlayer = 1; // If you need to init stuff pre-level like this, you can put it in your level's init block		
	}

	// Qraestolia Caverns init block
	else if(curlvl == L"SAJewers-QraestoliaCaverns.lvl") {
		gLevelEnum = QraestoliaCaverns;
	}

	// The Floor is Lava init block
	else if(curlvl == L"Docopoper-TheFloorisLava.lvl") {
		gLevelEnum = TheFloorIsLava;
		demo -> Identity = 1;
	}

	// Curtain Tortoise init block
	else if(curlvl == L"JosephStaleknight-CurtainTortoise.lvl") {
		gLevelEnum = CurtainTortoise;
	}

	// Abtract Assault init block
	else if(curlvl == L"Docopoper-AbstractAssault.lvl") {
		gLevelEnum = AbstractAssault;
	}

	// Snowbordin init block
	else if(curlvl == L"SAJewers-Snowboardin.lvl") {
		gLevelEnum = Snowbordin;
		SAJSnowbordin::SnowbordinInitCode();
	}

	// Calleoca init block
	else if(curlvl == L"Docopoper-Calleoca.lvl") {
		gLevelEnum = Calleoca;
		CalleocaInitCode();
	}

	// Science init block
	else if(curlvl == L"Talkhaus-Science_Final_Battle.lvl") {
		gLevelEnum = Science;
		ScienceBattle::ScienceInitCode();
	}
	
	// EuroShellRandD
	else if(curlvl == L"ztarwuff-EuroShellRandD.lvl") {
		gLevelEnum = EuroShellRandD;
	}

	else if(curlvl == L"Kil-DemosBrain.lvl") {
		gLevelEnum = DemosBrain;
		Player::FilterToSmall(Player::Get(1));
		Player::FilterMount(Player::Get(1));
		Player::FilterReservePowerup(Player::Get(1));
	}

	else if(curlvl == L"LUNA12-thou_starts_a_new_video.lvl") {
		gLevelEnum = ThouStartsANewVideo;
		Player::FilterToBig(Player::Get(1));
		Player::FilterMount(Player::Get(1));
		Player::FilterReservePowerup(Player::Get(1));
		Player::Get(1)->Identity = 1;
	}

}






