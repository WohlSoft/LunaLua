//*** Globals.h - It has some globals in it ***

#pragma once
#pragma warning(disable: 4731)
#pragma warning(disable: 4800)
#pragma warning(disable: 4996) //localtime
#include "LevelCodes.h"
#include "Rendering.h"
#include "DeathCounter.h"
#include "AutocodeManager.h"
#include "Logging.h"
#include "CSprite.h"
#include "CSpriteManager.h"
#include "CellManager.h"
#include "UserSaving.h"
#include "mciEmulator.h"

#pragma comment (lib, "Msimg32.lib")

struct startUpSettings{
	bool patch;
	bool game;
	bool lvlEditor;
	bool frameskip;
	bool noSound;
	bool debugger;
	bool logger;
	bool newLauncher;
};

/// Global settings
extern bool		gLunaEnabled;
extern bool		gShowDemoCounter;
extern bool		gSkipSMBXHUD;
extern bool		gPrintErrorsToScreen;
extern bool		gIsOverworld;
extern startUpSettings gStartupSettings;

/// General use globals - These are all defined in Globals.cpp ///
extern int		gFrames;

extern int*		gpFreeGlob;

extern int		gLastDownPress;
extern int		gDownTapped;
extern int		gLastUpPress;
extern int		gUpTapped;
extern int		gLastLeftPress;
extern int		gLeftTapped;
extern int		gLastRightPress;
extern int		gRightTapped;

extern int		gLastJumpPress;
extern int		gJumpTapped;
extern int		gLastRunPress;
extern int		gRunTapped;

extern int		gCurrentMainPlayer;

extern HDC		ghMemDC;		// General-use screen-compatible DC
extern HBITMAP	ghGeneralDIB;	// General-use screen-sized DIB
extern DWORD*	gpScreenBits;	// Pointer to screen bits for general use DIB

// Key status
extern short	gNumpad4;
extern short	gNumpad8;
extern short	gNumpad2;
extern short	gNumpad6;

// Level init
extern LvlEnum	gLevelEnum;

// Misc
extern Renderer				gLunaRender;
extern DeathCounter			gDeathCounter;
extern AutocodeManager		gAutoMan;
extern CSpriteManager		gSpriteMan;
extern Logger				gLogger;
extern CellManager			gCellMan;
extern SavedVariableBank	gSavedVarBank;
extern MciEmulator			gMciEmulator;

/// Functions ///
void InitGlobals();
void ResetFreeGlob();
void CleanUp();

/// HELPER FUNCTIONS ///
std::vector<std::wstring> wsplit( std::wstring str, wchar_t delimiter);
std::vector<std::string> split( std::string str, char delimiter);
bool vecStrFind(const std::vector<std::string>& vecStr, const std::string& find);
void resetDefines();
bool is_number(const std::string& s);
HMODULE getModule(std::string moduleName);

/// HELPER MACROS ///
/*
if(!hRunProc){
std::string errMsg = "Failed to load 'run' in the Launcher dll D:!\nIs Lunadll.dll or LunadllNewLauncher.dll different versions?\nError code:";
errMsg += std::to_string((long long)GetLastError());
MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);
FreeLibrary(newLauncherLib);
newLauncherLib = NULL;
return;
}
*/
#define GL_PROC_CHECK(procHandle, procName, moduleHandle, moduleName) \
	if(!procHandle){\
		std::string errMsg = "Failed to load 'procName' in moduleName D:!\nIs Lunadll.dll or moduleName different versions?\nError code:";\
		errMsg += std::to_string((long long)GetLastError());\
		MessageBoxA(NULL, errMsg.c_str(), "Error", NULL);\
		FreeLibrary(moduleHandle);\
		moduleHandle = NULL;\
		return;\
	}