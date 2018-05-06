#include <windows.h>
#include <time.h>
#include <cctype>
#include "Globals.h"
#include "LevelCodes/LevelCodes.h"
#include "Shlwapi.h"
#include "UserSaves/UserSaving.h"
#include "GlobalFuncs.h"

HINSTANCE gHInstance;

// Global settings
bool gLunaEnabled;
bool gPrintErrorsToScreen;
bool gIsOverworld;
bool gHook_SkipTestMsgBox;
bool gAutostartRan = false;
bool gIsWindowsVistaOrNewer = false;
SMBXHUDSettings gSMBXHUDSettings = {
    false,    // skip
    WHUD_ALL, // overworldHudControlFlag
    false     // skipStarCount
};
bool gRenderBGOFlag = true;

// Global variables
int	gFrames;

int gLastDownPress;
int gDownTapped;
int gLastUpPress;
int gUpTapped;
int gLastLeftPress;
int gLeftTapped;
int gLastRightPress;
int gRightTapped;

int gLastJumpPress;
int gJumpTapped;
int gLastRunPress;
int gRunTapped;

HDC	ghMemDC;			// General use screen-compatible DC
HBITMAP	ghGeneralDIB;	// General use screen-sized DIB
DWORD* gpScreenBits;	// Pointer to screen bits for general use DIB

LvlEnum gLevelEnum;

KeyMap gRawKeymap[2] = {0};

bool gIsTestModePauseActive;

// Global objs
Renderer gLunaRender;

AutocodeManager gAutoMan;

CSpriteManager gSpriteMan;

CellManager gCellMan;

SavedVariableBank gSavedVarBank;

MciEmulator gMciEmulator;

startUpSettings gStartupSettings;

HardcodedGraphicsManager gGraphManager;

SHMemServer gShMemServer;

FreeImageInit        gFreeImgInit;

GeneralLunaConfig    gGeneralConfig;

CLunaLua gLunaLua;


// Paths
std::string  gAppPathANSI;

std::string  gAppPathUTF8;

std::wstring gAppPathWCHAR;

