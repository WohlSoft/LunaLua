#include "Main.h"
//#include <windows.h>
#include <string>
#include "Globals.h"
#include "GlobalFuncs.h"
#include "Defines.h"
#include "SMBXInternal/Layer.h"
#include "LevelCodes/LevelCodes.h"
#include "SMBXInternal/PlayerMOB.h"
#include "Rendering/Rendering.h"
#include "SMBXInternal/Level.h"
#include "SMBXInternal/NPCs.h"
#include "SMBXInternal/Blocks.h"
#include "Misc/MiscFuncs.h"
#include "SMBXInternal/Sound.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Input/Input.h"
#include "SMBXInternal/SMBXEvents.h"
#include "LuaMain/LunaLuaMain.h"
#include "Misc/RuntimeHook.h"
#include "SdlMusic/MusicManager.h"
#include "Misc/Playground.h"
#include "Rendering/GLEngine.h"
#include "Rendering/GLInitTest.h"

#define PATCHIT 1

// Standard DLL loader main
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		gHInstance = (HINSTANCE)hinstDLL;
        SetupThunRTMainHook();
        timeBeginPeriod(1);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
        timeEndPeriod(1);
		CleanUp();
		break;
	}
	return TRUE;
}

static DWORD __stdcall GetCurrentProcessorNumberXP(void)
{
    __asm {
        MOV EAX, 1
        CPUID
        SHR EBX, 24
        MOV EAX, EBX
    }
}

// We don't call this directly from DLL_PROCESS_ATTACH because if we do things
// can break when we're loaded via LoadLibrary
// Instead this is called by LunaDLLInitHook, which is set up by
// SetupLunaDLLInitHook that runs from DLL_PROCESS_ATTACH
void LunaDLLInit()
{
    InitGlobals();
#if PATCHIT
    TrySkipPatch();
#endif // PATCHIT

    // Test OpenGL support
    if (LunaDLLTestGLFeatures()) {
        g_GLEngine.Enable();
    } else {
        g_GLEngine.Disable();
    }

    // Set processor affinity for the main thread. Switching cores is bad for stable frame rate
    DWORD curProcessor = GetCurrentProcessorNumberXP();
    SetThreadAffinityMask(GetCurrentThread(), 1 << curProcessor);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
}

// *EXPORT* On Level Load -- Run once as a level is loaded (including title screen level)
int OnLvlLoad() {

	// Restore some code the hook overwrote
	*(DWORD*)0x00B25958 = 0;

    // WIP
    // dumpTypeLibrary((IDispatch*)*(DWORD*)0xB2D7E8, std::wcout);

#ifndef NO_SDL
	if(!episodeStarted)
	{//Load custom sounds
		std::string wldPath = wstr2str(GM_FULLDIR);
		std::string SndRoot = MusicManager::SndRoot();
        std::string custPath = wstr2str(getCustomFolderPath());
        replaceSubStr(wldPath, "\"", "");
		replaceSubStr(wldPath, "\\\\",  "\\");
		replaceSubStr(wldPath, "/",  "\\");

		replaceSubStr(SndRoot, "\"", "");
		replaceSubStr(SndRoot, "\\\\",  "\\");
		replaceSubStr(SndRoot, "/",  "\\");

		if(wldPath!=SndRoot)
		{
            MusicManager::loadCustomSounds(wldPath+"\\", custPath);
			//MessageBoxA(0, std::string(wldPath+"\n"+SndRoot+"\nLevel started").c_str(), "Debug", 0);
		}
	}
#endif
	// Clean up leftovers
	gSkipSMBXHUD = false;
	gIsOverworld = false;
    gOverworldHudControlFlag = WHUD_ALL;
	gLunaRender.ClearAll();
	gSpriteMan.ResetSpriteManager();
	gCellMan.Reset();
	gSavedVarBank.ClearBank();	
	Input::ResetAll();

	// Update renderer stuff
	gLunaRender.ReloadScreenHDC();
    g_GLEngine.ClearSMBXSprites();

	if(gLunaEnabled) {
		// Load autocode
		gAutoMan.Clear(false);		
		gAutoMan.ReadFile((std::wstring)GM_FULLDIR);

		// Try to load world codes		
		gAutoMan.ReadWorld((std::wstring)GM_FULLDIR);

		// Init var bank
		gSavedVarBank.TryLoadWorldVars();
		gSavedVarBank.CheckSaveDeletion();
		gSavedVarBank.CopyBank(&gAutoMan.m_UserVars);

        //  Don't try to call the CLunaLua constructor... It's already
        //  constructed automatically once, and trying to do this will call
        //  the constructor extra times *without* ever calling the destructor,
        //  which can result in a memory leak of the whole Lua state!
		//    gLunaLua = CLunaLua();
		gLunaLua.init(CLunaLua::LUNALUA_LEVEL, (std::wstring)GM_FULLDIR, Level::GetName());

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
#if COMPILE_PLAYGROUND
        Playground::doPlaygroundStuff();
#endif
		gLunaLua.doEvents();

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

	// Overwrite next instruction if we're skipping hud drawing,
    // otherwise make sure the original is restored
	if(gSkipSMBXHUD) {
        // 0096C036 | E9 D5 BB 01 00 | jmp 987C10
        // 0096C03B | 90 | nop
        *((unsigned int*)0x96C036) = 0x01BBD5E9;
        *((unsigned short*)0x96C03a) = 0x9000;
    } else {
        // 0096C036 | 0F 84 D1 8B 00 00 | je 974C0D
        *((unsigned int*)0x96C036) = 0x8BD1840F;
        *((unsigned short*)0x96C03a) = 0x0000;
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

    if (gLunaLua.isValid()) {
        Event inputEvent("onHUDDraw", false);
        inputEvent.setDirectEventName("onHUDDraw");
        inputEvent.setLoopable(false);
        gLunaLua.callEvent(&inputEvent);
    }

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
		demo -> Identity = CHARACTER_MARIO;
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
		Player::Get(1)->Identity = CHARACTER_MARIO;
	}

}






